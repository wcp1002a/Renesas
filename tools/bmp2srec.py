import cv2
import numpy as np
import sys
import os
########################################################################################################################
DISPLAY_W = 1920
DISPLAY_H = 205
START_ADDRESS = '7C333600'
srec_header= 'S01B0000000000000000000000000000000000000000000000000000E4'
srec_address_termination = 'S70500000000FA'
########################################################################################################################
# Funciton List

get_path = lambda x: '\\'.join(x.split("\\")[:-1])
get_ext  = lambda x:os.path.splitext(x)[1]
get_name  = lambda x:get_path_name(x).split("\\")[-1]   # filename
get_path_name  = lambda x:os.path.splitext(x)[0]        # C:\folder\filename

def gen_output_filename(src_filename, prefix='', suffix='', ext=''):
    ''' 由輸入名產生輸出檔名 
            Usage: gen_output_filename('C:\\folder\\filename.ext', suffix='_p', ext='.out')
                   c:\folder\filenae_p.out
    '''
    full_path_name = src_filename
    full_path = "\\".join(full_path_name.split("\\")[:-1])
    filename = get_name(full_path_name)
    file_ext = get_ext(full_path_name)

    if ext == '':
        ext = file_ext

    if ext[0] != '.':
        ext = '.' + ext

    out_filename = prefix + filename + suffix + ext
    if full_path != '':
        out_filename = full_path + "\\" + out_filename
    return out_filename

def parse_line_data(srec_string):
# parse data from srec record
# S3157C333630 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF E5
    datas = []
    if srec_string[0] == 'S':
        rec_type = srec_string[0:2]
        address_bits = 0
        if rec_type == 'S0':
            pass
            # print('skip')
        elif rec_type == 'S1':
            address_bits = 16
        elif rec_type == 'S2':
            address_bits = 24
        elif rec_type == 'S3':
            address_bits = 32
        if address_bits > 0:
            offset = 2 + 2 + int(address_bits/4) # S-type(2) + ByteCount(2) + Address fieled(n)
            data_size = int(srec_string[2:4], 16) - int(address_bits/8) - 1 # address: 4 bytes, checksum: 1 bytes
            for i in range(data_size):
                datas.append(int(srec_string[offset+i*2:offset+i*2+2], 16))
    return datas

def read_srec_file(filename):
    img_datas = []
    with open(filename) as file_object:
        for line in file_object:
            datas = parse_line_data(line)
            if len(datas) > 0:
                img_datas += datas
    return img_datas

def check_srec_line(line):
    line = line.strip()
    s_type = line[0:2]
    s_byte_count = int(line[2:4], 16)
    s_datas = line[4:]

    if s_byte_count * 2 != len(s_datas):
        print("Length Error:")
        print("    len(s_datas): {}".format(len(s_datas)))
        print("    s_byte_count: {} x 2 = {}".format(s_byte_count, 2*s_byte_count))
        if len(s_datas) < s_byte_count*2:
            line = line + 'A'*int((s_byte_count*2-len(s_datas)))
            s_datas = line[4:]
            print("    [Fix] {}".format(line))
        else:
            line = line[:(s_byte_count+1+1)*2]    # Total bytes = byte_count + S_type(1) + ByteCount(1)
            s_datas = line[4:]
            print("    [Fix] {}".format(line))


    if s_byte_count * 2 != len(s_datas):
        print("   Format Error!")
    else:
        chk_sum = 0
        for i in range(1, s_byte_count+1):
            chk_sum += int(line[2*i:2*i+2], 16)
        chk_sum = chk_sum % 256
        chk_sum = 255 - chk_sum
        line_out = line[0:s_byte_count*2+2] + "{:02X}".format(chk_sum)
        # print("chk_sum: {}".format(chk_sum))
    return line_out



# S3157C333600FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF15
# S3157C333610FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF05
# S3157C333620FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF5


def gen_srec_line(rgb_data, address):
    s_type = 'S3'
    s_byte_count = 4 + 16 + 1   # Address(4) + Data(16) + Checksum(1)
    line = s_type + "{:02X}".format(s_byte_count) + "{:8X}".format(address)
    for points in rgb_data:
        for c in points:
            line += "{:02X}".format(c)
        line += 'FF'    # Add Alpha

    # Calculate checksum
    chk_sum = 0
    for i in range(1, s_byte_count+1):
        chk_sum += int(line[2*i:2*i+2], 16)
    chk_sum = chk_sum % 256
    chk_sum = 255 - chk_sum

    line_out = line[0:s_byte_count*2+2] + "{:02X}".format(chk_sum)
    return line_out

########################################################################################################################

# 取得所在目錄，並取得引數
cur_path = os.getcwd()
print("cur_path: {}".format(cur_path))
arguments = len(sys.argv)
src_filename = ''
tar_filename = ''

if arguments == 1:
    # 沒有引數，列出所有的 BMP 檔案以供選擇
    files = os.listdir(cur_path)
    bmp_files = []
    for file in files:
        if get_ext(file) == ".bmp":
            bmp_files.append(file)
    # 使用者選擇檔案
    input_filename = ''

    input_filename = 'r-car.bmp'
    while input_filename == '':
        print()
        print("Please select your file:")
        for idx, file in enumerate(bmp_files):
            print("{}. {}".format(idx+1, file))
        print("q. Exit")
        r = input("Select mot file:")
        if r.lower() == 'q':
            print("Quit!")
            exit()
        if int(r) >= 1 and int(r) <= len(bmp_files):
            input_filename = bmp_files[int(r)-1]
    src_filename = cur_path + '\\' + input_filename
    src_filename = input_filename
    tar_filename = gen_output_filename(src_filename, ext = '.srec')
elif arguments == 2:
    src_filename = sys.argv[1]
elif arguments == 3:
    src_filename = sys.argv[1]
    tar_filename = sys.argv[2]
else:
    print()
    exit()

print()
if os.path.isfile(tar_filename):
    tar_filename = 'p_output.srec'

print("src_filename: {}".format(src_filename))
print("tar_filename: {}".format(tar_filename))


# 讀取 BITMAP 檔案
img = cv2.imread(src_filename)
# cv2.imshow('img', img)
# cv2.waitKey(0)

# (w, h, d) = img.shape
# print(img[0,0:4])

DISPLAY_H, DISPLAY_W, d = img.shape
print("DISPLAY_W: {}".format(DISPLAY_W))
# exit()
# 開啟輸出檔

address = int(START_ADDRESS, 16)
with open(tar_filename, 'w') as fp:
    fp.write(srec_header + '\n')
    for h in range(DISPLAY_H):
        for w in range(int(DISPLAY_W/4)):   # for points a line
            datas = img[h,w*4:(w+1)*4]
            line = gen_srec_line(datas, address)
            fp.write(line+'\n')
            address += 16
            # if address >= int('7C3336D0', 16):
                # break
    fp.write(srec_address_termination + '\n')

print()

# 讀取 SREC 檔案
img = read_srec_file(tar_filename)

print("SREC檔案資料: {} bytes".format(len(img)))

tmp_img = []
n = 0
for i in range(len(img)):
    if i % 4 != 3:
        tmp_img.append(img[i])
    else:
        n = n + 1
        if(img[i] != 0xFF):
            print('n={}'.format(n))
            print(i, hex(img[i]))

print("去除ARGB中的A資料: {} bytes".format(len(tmp_img)))

# 建立輸出資料
img = np.array(tmp_img, np.uint8)
img = np.reshape(img, (DISPLAY_H, DISPLAY_W, 3))
cv2.imshow('img', img)
cv2.waitKey(0)

exit()

