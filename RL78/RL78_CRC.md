RL78 CRC
====================
Summary
--------------------
詳細使用位於 HWM 28.3  Operation of Functional Safety   

* 使用 CRC-16-CCITT 算法 (初值 `0000H`)
* 在 on-chip debug 時，CRC 的結果會因為斷點的不同而改變。
* 在 on-chip debug 時，CRC 的結果會和一般的程式不同，因為 Code Flash 的內容包含了 debug 的程式。
* `high-speed CRC` 和 `general CRC` 的結果會不同，因為 `general CRC` 使用 LSB ，要先做 bit reverse。

程式流程
--------------------
* 設定 FEA5 到 FEA0 位元 (指定程式大小，但會少 4 個位元，用來存放 CRC 數值以供比對)
* 將 HALT 和 RET 指令搬到 RAM。
* 停用中斷
* `CRC0CTL.CRC0EN = 1` 啟用 CRC high-speed operation
* 呼叫在 RAM 的副程式 (包含 HALt 和 RET 指令)
* `CRC0CTL.CRC0EN = 0` 停用 CRC high-speed operation
* 由暫存器 `PGCRC` 讀出 CRC 數值

範例程式
--------------------
用 C 語編寫 `crc_operation()` 及 `crc_operation_end()`，其 `crc_operation_end()` 是用於抓取 `crc_operation()` 束位址用的。   
另外，編寫 copy to ram 的函式，範例如下：

    #define RAM_ADDR (0xFA900)

    void copy_crc_func_to_ram(void){
        __far uint8_t *src = &crc_operation;
        __far uint8_t *src_ed = &crc_operation_end;
        uint8_t *tar_adr;
    
        int32_t cnt;
    
        cnt = (uint32_t)src_ed - (uint32_t)src;
        tar_adr = (uint8_t *)RAM_ADDR;
    
        while(cnt-- > 0){
            *tar_adr++ = *src++;
        }
    }

在 CRC 於 RAM 的執行部份，宣告 function pointer，將其位址指向目標記憶體位址。

    void (*crc_operation_ram)(void);
    crc_operation_ram = RAM_ADDR;

主程式的部份，依照流程來編寫。

    // * 將 HALT 和 RET 指令搬到 RAM。
    copy_crc_func_to_ram();

    // 設定 FEA5 到 FEA0 位元
    CRC0CTL_bit.no0 = 0;
    CRC0CTL_bit.no1 = 0;
    CRC0CTL_bit.no2 = 0;
    CRC0CTL_bit.no3 = 0;
    CRC0CTL_bit.no4 = 0;
    CRC0CTL_bit.no5 = 0;

    // 停用中斷
    DI();

    // 啟用 CRC high-speed operation
    CRC0CTL_bit.no7 = 1;

    // 呼叫在 RAM 的副程式 (包含 HALt 和 RET 指令)
    crc_operation_ram();

    // NOP x 10
    NOP(); NOP(); NOP(); NOP(); NOP();
    NOP(); NOP(); NOP(); NOP(); NOP();

    // 停用 CRC high-speed operation
    CRC0CTL_bit.no7 = 0;

CS+
--------------------
以利用 HWM 28.3.1  Flash Memory CRC Operation Function (high-speed CRC) 的計算來驗證程式碼 CRC 為例。   
假設使用空間為 64KB ，即 CRC0CTL 暫存器 FEA5~FEA0 都設定為零為情況， 00000H to 3FFBH (16 Kbytes - 4 bytes) 。   
在 CS+ 編譯工具的選項找到 CRC 相關設定：   
`CC-RL (Build Tool)` -> `Hex Output Options` -> `CRC Operation`

* Outputs the calculation result of CRC: `Yes(-CRc)`
* Output address: `3FFC`
* Target range: `0000-3FFB`
* Type of CRC: `CRC-CCITT(MSB,LITTLE,4 bytes) type(High-speed CRC)`
* Initial value: `0000`
* Endian: `Little endian`
* output size: `4`
* Displays the result of CRC calculation and output address: `Yes(-VERBOSE=CRC)`

編譯後可以在訊息視窗看到位址及計算結果：

    *** CRC Code ***
    CODE    : 0000e58e
    ADDRESS : 00003ffc

### CRC 計算方式
在記憶體的資料若為 12H 34H 56H 78H (其 12H 為最小位址，78H 為最大位址)，編譯器的計算結果如下：

    B42C        CRC-CCITT(MSB) type
    D0FA        CRC-CCITT(MSB,LITTLE,4 bytes) type(High-speed CRC)
    70D8        CRC-CCITT(MSB,LITTLE,2 bytes) type
    67F0        CRC-CCITT(LSB) type(General-purpose CRC)

Python 驗證結果：

    CRC-CCITT(MSB) type:
        Memory: ['0x12', '0x34', '0x56', '0x78']
        CRC16: B42CH
    
    CRC-CCITT(MSB,LITTLE,4 bytes) type(High-speed CRC)
        Memory: ['0x78', '0x56', '0x34', '0x12']
        CRC16: D0FAH
    
    CRC-CCITT(MSB,LITTLE,2 bytes) type
        Memory: ['0x34', '0x12', '0x78', '0x56']
        CRC16: 70D8H
    
    CRC-CCITT(LSB) type(General-purpose CRC)
        Memory: ['0x48', '0x2c', '0x6a', '0x1e']
        CRC16: 0FE6H
        CRC16 bit reverse: 67F0H

    HWM 28.3.2  CRC Operation Function (general-purpose CRC) 
        Memory: ['0x1e', '0x6a', '0x2c', '0x48']
        CRC16: 6F10H
        CRC16 bit reverse: 08F6H    

其中，比較特別的地方是 General-purpose CRC 的計算方式，在 HWM 28.3.2  CRC Operation Function (general-purpose CRC) 有詳細說明。   
其重點就是資料會以 32bit 為單位，先將資料 bit reverse 之後，再計算 CRC ，而計算的結果，也要做一次 bit reverse。所以 General-purpose CRC 的資料如下：    
`1EH` (`78H`), `6AH` (`56H`), `2CH` (`23H`), `48H` (`12H`)   
Python 程式原始碼請參考附錄。

Appendix 
====================
以 Python 為例，計算 CRC 範例程式：

##### `CS+_CRC_Test.py`

    import crcmod
    
    def r(x):
        return int('{:08b}'.format(x)[::-1], 2)
    
    
    # 經測試，RL78 的 CRC 計算採用 CRC-16-CCITT ，其中的初值為 0x0000 ，所以也可使用 predefined xmodem
    # crc16 = crcmod.predefined.Crc('xmodem')
    # r_crc16 = crc16_crcmod(bytearray(v))
    # crc16.update(bytearray(v))
    # print(hex(crc16.crcValue))
    
    crc16_crcmod = crcmod.mkCrcFun(poly=0x11021, rev=False, initCrc=0x0000, xorOut=0x0000)
    #+----------+
    #| CRC TEST |
    #+----------+
    print('CRC-CCITT(MSB) type:')
    v = [0x12, 0x34, 0x56, 0x78]
    print('    Memory: {}'.format([hex(d) for d in v]))
    r_crc16 = crc16_crcmod(bytearray(v))
    print('    CRC16: {:04X}H'.format(r_crc16))
    print()
    
    print('CRC-CCITT(MSB,LITTLE,4 bytes) type(High-speed CRC)')
    v = [0x78, 0x56, 0x34, 0x12]
    r_crc16 = crc16_crcmod(bytearray(v))
    print('    Memory: {}'.format([hex(d) for d in v]))
    print('    CRC16: {:04X}H'.format(r_crc16))
    print()
    
    print('CRC-CCITT(MSB,LITTLE,2 bytes) type')
    v = [0x34, 0x12, 0x78, 0x56]
    r_crc16 = crc16_crcmod(bytearray(v))
    print('    Memory: {}'.format([hex(d) for d in v]))
    print('    CRC16: {:04X}H'.format(r_crc16))
    print()
    
    print('CRC-CCITT(LSB) type(General-purpose CRC)')
    v = [r(0x12), r(0x34), r(0x56), r(0x78)]
    r_crc16 = crc16_crcmod(bytearray(v))
    print('    Memory: {}'.format([hex(d) for d in v]))
    print('    CRC16: {:04X}H'.format(r_crc16))
    print('    CRC16 bit reverse: {:02X}{:02X}H'.format(r(r_crc16&0xFF), r((r_crc16>>8)&0xFF)))
    print()
    
    print('HWM 28.3.2  CRC Operation Function (general-purpose CRC) ')
    v = [r(0x78), r(0x56), r(0x34), r(0x12)]
    r_crc16 = crc16_crcmod(bytearray(v))
    print('    Memory: {}'.format([hex(d) for d in v]))
    print('    CRC16: {:04X}H'.format(r_crc16))
    print('    CRC16 bit reverse: {:02X}{:02X}H'.format(r(r_crc16&0xFF), r((r_crc16>>8)&0xFF)))
    print()
    
    # 20230812
    # =====================================================
    # MEMORY: 12 34 56 78
    # Complier
    #       B42C        CRC-CCITT(MSB) type
    #       D0FA        CRC-CCITT(MSB,LITTLE,4 bytes) type(High-speed CRC)
    #       70D8        CRC-CCITT(MSB,LITTLE,2 bytes) type
    #       67F0        CRC-CCITT(LSB) type(General-purpose CRC)
    
    # CRC16     crc16_crcmod = crcmod.mkCrcFun(poly=0x11021, rev=False, initCrc=0x0000, xorOut=0x0000)
    #       B42C        12 34 56 78
    #       D0FA        78 56 34 12
    #       70D8        34 12 78 56
    #       0FE6(67F0)  48 2C 6A 1E     R(12 34 56 78)
    
    #       6F10        1E 6A 2C 48     R(78 56 34 12)
    #       21E0        2C 48 1D 6A     R(34 12 78 56)
    
