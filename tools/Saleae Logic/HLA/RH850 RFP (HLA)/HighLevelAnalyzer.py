# High Level Analyzer
# For more information and documentation, please go to https://support.saleae.com/extensions/high-level-analyzer-extensions

from saleae.analyzers import HighLevelAnalyzer, AnalyzerFrame, StringSetting, NumberSetting, ChoicesSetting


# High level analyzers must subclass the HighLevelAnalyzer class.
class Hla(HighLevelAnalyzer):
    # List of settings that a user can set for this High Level Analyzer.
    display_debug = ChoicesSetting(label='Debug Informatino', choices=['No', 'Yes'])
    p_search = StringSetting()

    # An optional list of types this analyzer produces, providing a way to customize the way frames are displayed in Logic 2.
    result_types = {
        'byte': {'format': 'DATA: {{data.dd}}' },
        'str': {'format': '{{data.str}}'}
    }

    def print_dbg(s, str):
        if(s.display_debug == 'Yes'):
            print(str)

    def format_data(s, n1, n2):
        v = 0
        n = n2 - n1 + 1
        cmd = s.p_cmd
        if n <= 4:
            for i in range(n):
                v = v * 256 + s.datas[n1+i-1]
            if n == 1:
                if(cmd == 0x2C):
                    return f"{v:02X} ("+{0: "ID auth mode", 0xFF: "CMD protection mode", 0x55: "CMD protection mode(incomplete)"}.get(v, "Unknow") + ")"
                else:
                    return f"H'{v:02X} ({v})"
            elif n == 2:
                return f"H'{v:04X} ({v})"
            elif n == 4:
                return f"H'{v:08X} ({v})"
        elif n == 16:
            s_ascii = ""
            for i in range(n):
                # print(f"[ppp] {s.datas[n1+i-1]}")
                s_ascii += chr(s.datas[n1+i-1])
            return s_ascii
            # print(f"cmd: {s.p_cmd} => {s_ascii}")
        else:
            out_str = ""
            for i in range(n):
                if (i != 0) and (i % 4 == 0):
                    out_str += ' '
                out_str += f'{s.datas[n1+i-1]:02X}'
            return out_str

    def __init__(self):
        '''
        Initialize HLA.

        Settings can be accessed using the same name used above.
        '''
        self.p_start_time = None
        self.p_end_time = None
        self.p_cmd = 0x00
        self.datas = []
        self.sub_areas = []
        self.out_byte = 0
        self.p_length = 0

        self.p_cnt = 0
        self.p_frame_us = None

        print("Settings:", self.p_search, self.display_debug)

    def decode(self, frame: AnalyzerFrame):
        '''
        Process a frame from the input analyzer, and optionally return a single `AnalyzerFrame` or a list of `AnalyzerFrame`s.

        The type and data values in `frame` will depend on the input analyzer.
        '''

        self.p_cnt += 1
        self.p_frame_us = float(frame.end_time - frame.start_time) * 1_000_000
        # self.print_dbg(f"[frame {self.p_cnt:03}, p_frame_us = {self.p_frame_us:.2}us")

        self.st = frame.start_time
        self.ed = frame.end_time
        frames = []
        # Get variable from AnalyzerFrame
        p_bytes_count = frame.data['p_bytes_count']
        data = frame.data['data']

        if p_bytes_count == 1:
            self.datas = []
            self.p_length = 0
        self.datas.append(data)

        # match self.p_cnt:
        #     case 1:
        #         self.p_start_time = frame.start_time
        #     case 2:
        #     case 3:
        #     case 4:
        #         self.p_end_time = frame.end_time
        #         frames.append(AnalyzerFrame('cmd', frame.start_time, frame.end_time, {'cmd': "CMD PACKAGE"}))
        #     case out_byte:
        #     case _:
        #         frames.append(AnalyzerFrame('byte', frame.start_time, frame.end_time, {'data': s_cmd}))

        #+--------+
        #| Output |
        #+--------+
        if (p_bytes_count == 1):
            self.p_start_time = frame.start_time
        elif (p_bytes_count == 2):
            pass
        elif (p_bytes_count == 3):
            pass
        elif (p_bytes_count == 4):
            #+------------+
            #| CMD output |
            #+------------+
            self.p_cmd = data
            self.p_length = self.datas[1] * 256 + self.datas[2]
            self.p_end_time = frame.end_time
            s_cmd = decode_cmd(data)
            frames.append(AnalyzerFrame('str', self.p_start_time, self.p_end_time, {'str': f"{s_cmd} (Len:{self.p_length})"}))
            self.sub_areas = decode_data(self.p_cmd, self.p_length)

        elif (p_bytes_count == 4 + self.p_length):
            #+-----+
            #| SUM |
            #+-----+
            c_sum = cal_sum(self.datas, 4 + self.p_length - 1)
            if c_sum == data:
                s_sum = f'SUM:{c_sum:02X}'
            else:
                s_sum = f'!SUM:{c_sum:02X} (expected)'
            frames.append(AnalyzerFrame('str', frame.start_time, frame.end_time, {'str': f"{s_sum}"}))
            print(f'CMD:{self.p_cmd:02X} Len:{self.p_length:2} {s_sum}')
        elif (p_bytes_count == 4 + self.p_length + 1):
            #+-----+
            #| ETX |
            #+-----+
            frames.append(AnalyzerFrame('str', frame.start_time, frame.end_time, {'str': f"ETX:03"}))
        else:
            #+-----------+
            #| DATA AREA |
            #+-----------+
            for area in self.sub_areas:
                if p_bytes_count == area['st']:
                    self.p_start_time = frame.start_time
                if p_bytes_count == area['ed']:
                    # in case area['st'] = area['ed']
                    self.print_dbg(f"  [D] {area['st']:2}-{area['ed']:2}: {area['name']}")
                    self.p_end_time = frame.end_time
                    s_data = self.format_data(area['st'], area['ed'])
                    frames.append(AnalyzerFrame('str', self.p_start_time, self.p_end_time, {'str': f"{area['name']}:{s_data}"}))
                else:
                    pass
        
        return frames

def decode_cmd(cmd):
    com_list = {
        0x00: {"cmd": "Inquiry"},
        0x10: {"cmd": "Blank Check"},
        0x12: {"cmd": "Erase"},
        0x13: {"cmd": "Write"},
        0x15: {"cmd": "Read"},
        0x16: {"cmd": "Verify"},
        0x17: {"cmd": "ID Verify"},
        0x18: {"cmd": "CRC Sum Check"},
        0x4D: {"cmd": "Addition Sum Check"},
        0x19: {"cmd": "ID Check"},
        0x1C: {"cmd": "Configuration Clear"},

        0x20: {"cmd": "Command protection Set"},
        0x21: {"cmd": "Command protection Get"},
        0x22: {"cmd": "Lock-bit Set"},
        0x23: {"cmd": "Lock-bit Get"},
        0x26: {"cmd": "Option-byte Set"},
        0x27: {"cmd": "Option-byte Get"},
        0x28: {"cmd": "Serial programming ID-code Set"},
        0x29: {"cmd": "Serial programming Disable"},
        0x2A: {"cmd": "ID-code Set"},
        0x2B: {"cmd": "ID-code Get"},
        0x2C: {"cmd": "ID authentication mode Get"},
        0x2D: {"cmd": "OTP Set"},
        0x2E: {"cmd": "OTP Get"},

        0x30: {"cmd": "Serial programming ID-code Check"},
        0x32: {"cmd": "Frequency Set"},
        0x34: {"cmd": "Baud Rate Set"},
        0x38: {"cmd": "Device Type Get"},
        0x3A: {"cmd": "Signature Get"},

        0x74: {"cmd": "ICU-M configuration set"},
        0x75: {"cmd": "ICU-M option byte set"},
        0x76: {"cmd": "ICU-M verify"},
        0x77: {"cmd": "ICU-M mode get"}
    }

    if cmd in com_list:
        cmd_n = com_list[cmd]
        # return f'COM_{cmd:02X}: (Undefined)'
        return f'COM_{cmd:02X}: {cmd_n["cmd"]}'
    # for cmd_n in cmd_list:
        # if cmd == cmd_n[0]:
            # return f'COM_{cmd:02X}: {cmd_n[1]}'
    return f'COM_{cmd:02X}: (Undefined)'

def decode_data(cmd, len):
    ret_areas = []
    if cmd == 0x12: # Erase
        ret_areas.append({'name':'Start address', 'st': 5, 'ed': 8})
    elif cmd == 0x13 or cmd == 0x15 or cmd == 0x16 or cmd == 0x18: # Write / Read / Verify /CRC
        ret_areas.append({'name':'Start address', 'st': 5, 'ed': 8})
        ret_areas.append({'name':'End address', 'st': 9, 'ed':12})
    elif cmd == 0x21:
        ret_areas.append({'name':'Security data', 'st': 5, 'ed': 5})
    elif cmd == 0x23 or cmd == 0x2E:
        tmp_name = {0x23: 'LB', 0x2E: 'OP'}.get(cmd, 'N/A')
        # ret_areas.append({'name':'LB1', 'st': 5, 'ed': 69})     # 65 bytes
        for i in range(65):
            ret_areas.append({'name':f'{tmp_name}1[{i+1}]', 'st': 5+i, 'ed': 5+i})

        # ret_areas.append({'name':'LB2', 'st': 70, 'ed': 101})   # 32 bytes
        for i in range(32):
            ret_areas.append({'name':f'{tmp_name}2[{i+1}]', 'st': 70+i, 'ed': 70+i})
        ret_areas.append({'name':f'{tmp_name}3', 'st': 102, 'ed': 102})
    elif cmd == 0x4D: # Add sun check
        ret_areas.append({'name':'ARE', 'st': 5, 'ed': 5})
    elif cmd == 0x26 or cmd == 0x27:
        ret_areas.append({'name':'OPB1', 'st': 5, 'ed': 8})
        ret_areas.append({'name':'OPB2', 'st': 9, 'ed':12})
        ret_areas.append({'name':'OPB3', 'st': 13, 'ed':16})
        ret_areas.append({'name':'OPB4', 'st': 17, 'ed':20})
        ret_areas.append({'name':'OPB5', 'st': 21, 'ed':24})
        ret_areas.append({'name':'OPB6', 'st': 25, 'ed':28})
        ret_areas.append({'name':'OPB7', 'st': 29, 'ed':32})
        ret_areas.append({'name':'OPB8', 'st': 33, 'ed':36})
    elif cmd == 0x28 or cmd == 0x2A or cmd==0x2B or cmd == 0x30: # serial ID set/ID set/serial ID check
        ret_areas.append({'name':'ID-code1', 'st': 5, 'ed': 8})
        ret_areas.append({'name':'ID-code2', 'st': 9, 'ed':12})
        ret_areas.append({'name':'ID-code3', 'st': 13, 'ed':16})
        ret_areas.append({'name':'ID-code4', 'st': 17, 'ed':20})
    elif cmd == 0x2C:
        if(len==2):
            ret_areas.append({'name':'Mode', 'st': 5, 'ed':5})
    elif cmd == 0x2D:
        ret_areas.append({'name':'Code Flash 1 OTP info', 'st': 5, 'ed':69})
        ret_areas.append({'name':'Code Flash 2 OTP info', 'st': 70, 'ed':101})
        ret_areas.append({'name':'User Boot Area/Extended User Area OTP info', 'st': 102, 'ed':102})
    elif cmd == 0x32:
        ret_areas.append({'name':'SOC clock freq.', 'st': 5, 'ed': 8})
        ret_areas.append({'name':'CPU clock freq.', 'st': 9, 'ed':12})
    elif cmd == 0x34:
        ret_areas.append({'name':'Set Baud', 'st': 5, 'ed': 8})
    elif cmd == 0x38:
        ret_areas.append({'name':'Type code', 'st': 5, 'ed': 12})
        ret_areas.append({'name':'Max. Main-OSC', 'st': 13, 'ed': 16})
        ret_areas.append({'name':'Min. Main-OSC', 'st': 17, 'ed': 20})
        ret_areas.append({'name':'Max. CPU freq', 'st': 21, 'ed': 24})
        ret_areas.append({'name':'Min. CPU freq', 'st': 25, 'ed': 28})
    elif cmd == 0x3A:
        ret_areas.append({'name':'Device name', 'st': 5, 'ed': 20})
        ret_areas.append({'name':'CF1:', 'st': 21, 'ed': 21})
        ret_areas.append({'name':'CodeFlash1 block size:', 'st': 22, 'ed': 25})
        ret_areas.append({'name':'CodeFlash1 blocks:', 'st': 26, 'ed': 27})
        ret_areas.append({'name':'CF2:', 'st': 28, 'ed': 28})
        ret_areas.append({'name':'CodeFlash2 block size:', 'st': 29, 'ed': 32})
        ret_areas.append({'name':'CodeFlash2 blocks:', 'st': 33, 'ed': 34})
        ret_areas.append({'name':'SP1:', 'st': 35, 'ed': 35})
        ret_areas.append({'name':'SS1:', 'st': 36, 'ed': 39})
        ret_areas.append({'name':'SN1:', 'st': 40, 'ed': 41})
        ret_areas.append({'name':'CF3:', 'st': 42, 'ed': 42})
        ret_areas.append({'name':'CodeFlash3 block size:', 'st': 43, 'ed': 46})
        ret_areas.append({'name':'CodeFlash3 blocks:', 'st': 47, 'ed': 48})
        ret_areas.append({'name':'CF4:', 'st': 49, 'ed': 49})
        ret_areas.append({'name':'CodeFlash4 block size:', 'st': 50, 'ed': 53})
        ret_areas.append({'name':'CodeFlash4 blocks:', 'st': 54, 'ed': 55})
        ret_areas.append({'name':'DF1:', 'st': 56, 'ed': 56})
        ret_areas.append({'name':'DS1:', 'st': 57, 'ed': 60})
        ret_areas.append({'name':'DN1:', 'st': 61, 'ed': 62})
    elif cmd == 0x75:
        ret_areas.append({'name':'ICUMOPBT', 'st': 5, 'ed': 8})
    elif cmd == 0x74:
        ret_areas.append({'name':'ION', 'st': 5, 'ed': 5})
        ret_areas.append({'name':'IRC', 'st': 6, 'ed': 9})
        ret_areas.append({'name':'IRD', 'st': 10, 'ed':13})
        ret_areas.append({'name':'IRV', 'st': 14, 'ed':17})
        ret_areas.append({'name':'UP1', 'st': 18, 'ed':18})
        ret_areas.append({'name':'UP2', 'st': 19, 'ed':19})
    elif cmd == 0x76:
        ret_areas.append({'name':'ION', 'st': 5, 'ed': 5})
        ret_areas.append({'name':'IRC', 'st': 6, 'ed': 9})
        ret_areas.append({'name':'IRD', 'st': 10, 'ed':13})
        ret_areas.append({'name':'IRV', 'st': 14, 'ed':17})
        ret_areas.append({'name':'UP1', 'st': 18, 'ed':18})
        ret_areas.append({'name':'UP2', 'st': 19, 'ed':19})
        ret_areas.append({'name':'ICUMOPBT', 'st': 20, 'ed':23})

    return ret_areas

def cal_sum(in_datas, n):
    sum = 0
    for i, v in enumerate(in_datas):
        if (i != 0) and (i < n):
            sum += v
    return (0x100 - (sum % 0x100)) % 0x100
    # return sum
