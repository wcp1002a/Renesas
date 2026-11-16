20261106_TrendPower_RL78_F22_Security_Sample_Code
=================================================

Overview
----------------------------------------
* Sample Project: `r01an7264ej0100-rl78f22-f25-aesea1-security.zip`
* Hardware: RL78/F22 Target Board

#### F22 TB
<div style="margin-left: 20px;">

Function    | Signal Name| Pin Name  | RL78/F24 TB
------------|------------|-----------|-----------
UART/TX     | TXD0       | P15       | CN2.16
UART/RX     | RXD0       | P16       | CN2.15
VDD         | VDD        | VDD       | CN1.24
Ground      | GND        | GND       | CN1.23

</div>

#### F25 TB
<div style="margin-left: 20px;">

Function    | Signal Name| Pin Name  | RL78/F24 TB
------------|------------|-----------|-----------
UART/TX     | TXD0       | P15       | CN1.43
UART/RX     | RXD0       | P16       | CN1.45
VDD         | VDD        | VDD       | CN2.47, CN2.46
Ground      | GND        | GND       | CN2.45, CN2.46, CN1.18

</div>

### 1. 使用工具生成 MAC 與 KEY
```
R_KEY:    15180F23C051D019108281810F245050
Key_PW:   00112233445566778899AABBCCDDEEFF
MAC_PW:   0123456789ABCDEFFEDCBA9876543210
Key_data: 2B7E151628AED2A6ABF7158809CF4F3C
```

#### ■ 生成命令
```
gen_secboot.py generate --mot secdr_rl78_sample.mot
```

* 工具會依照 mot 的資料生成 secure boot 在位址 102H, 112H 所需的資料，將生成的組合語言原始碼複製到 `cstart_rfd_customized.asm` 最尾端。  
* 重新編譯後，可以得到正確的 mot 檔案。

#### ■ 驗證命令
```
gen_secboot.py verify --mot secdr_rl78_sample.mot
```
若一致會出現 `VERIFY MATCH` 字樣

### 2. 使用 RFP-CLI 燒錄
由於需要給定 key-password 及 mac-password，所以需要使用 command line 指令來燒錄。

#### ■ Secure Boot 燒錄命令
`rfp-cli.exe -d RL78 -t E2 -if uart1 -vo 5.0 -a -key-password <key> -mac-password <mac> -file <mot>`

範例
```
"C:\Program Files (x86)\Renesas Electronics\Programming Tools\Renesas Flash Programmer V3.23\rfp-cli.exe" -d RL78 -t E2 -if uart1 -vo 5.0 -a -key-password 00112233445566778899AABBCCDDEEFF -mac-password 0123456789ABCDEFFEDCBA9876543210 -file secdr_rl78_sample.mot 
```
```
Enter ID Code (16 Bytes)? FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
```
⚠️ __Note__: `rfp-cli` 路徑需依實際安裝目錄及版本自行修改。  
燒錄成功會印出 <span style="color:green">__Operation successful__</span> 字樣

#### ■ 一般燒錄命令
若要燒錄沒有 secure boot 的 mot，需要移除 key-password 及 mac-password 參數，不然會出現錯誤。
`rfp-cli.exe -d RL78 -t E2 -if uart1 -vo 5.0 -a -file secdr_rl78_sample.mot `

----------------------------------------

Working Log
----------------------------------------

### 20261106 測試
* 專案檔開啟時內建為 F25(`R7F125FPL4xFB(AES`)，修改為 F22 (`R7F122FGG3xFB(AES`)
* 修改 OB1，避免 LVD 的問題
> 修改 OB1: 0x22 => 0xFF  
* 在 **OB4** = **0x2F** 時，無法正常執行，程式執行後會停在位址 0xFFE0A。
* OB4: `0x2F` ➜ `0x3F` 後，可正常執行，可透過 UART 輸出 log。
> 修改 OB4: 0x2F ➜ 0x3F

cstart_rfd_customized.asm
<div style="margin-left: 20px;">
```asm
.SECTION .option_byte, opt_byte
OB0:    .db 0xce
;;OB1:    .db 0x22
OB1:    .db 0xFF
;;OB2:    .db 0xc0    ;; 1100_0000 / RESOUTB=0, FRQSEL43210=00000(32MHz)
OB2:    .db 0xc8      ;; 1100_1000 / RESOUTB=0, FRQSEL43210=01000(40MHz)
OB3:    .db 0xa7      ;;
;secure_option_byte .cseg text
;.org 0xc4
;;OB4:    .db 0x2f    ;; KSIZE=00, SECBTEN=1, IDRDEN=1, AESEN=1
OB4:    .db 0x3f    ;; KSIZE=00, SECBTEN=1, IDRDEN=1, AESEN=1
```
</div>

執行結果
<div style="margin-left: 20px;">

```sh

AESEA ENCRYPTION TEST $Rev: 158 $

GET ID
pass  TIMER :63200nsec
SECST :40
UID   :00000023052d15198046f300004520
MAC   :00000000000000000000000000000000

CHK VERI MAC1
pass  TIMER :66400nsec

CHK VERI MAC2
pass  TIMER :66400nsec

LOAD KEY
Index  1: pass  TIMER :2578400nsec
          M4result value is pass
          M5result value is pass
Index  2: pass  TIMER :2660000nsec
          M4result value is pass
          M5result value is pass
Index  3: pass  TIMER :2661600nsec
          M4result value is pass
          M5result value is pass
Index  4: pass  TIMER :2580800nsec
          M4result value is pass
          M5result value is pass
Index  5: pass  TIMER :2651200nsec
          M4result value is pass
          M5result value is pass
Index  6: pass  TIMER :2651200nsec
          M4result value is pass
          M5result value is pass
Index  7: pass  TIMER :2583200nsec
          M4result value is pass
          M5result value is pass
Index  8: pass  TIMER :2653600nsec
          M4result value is pass
          M5result value is pass
Index  9: pass  TIMER :2653600nsec
          M4result value is fail
          M5result value is fail

ECB ENC/DEC 128bit
pass  TIMER :130400nsec
pass  TIMER :130400nsec
result value is pass

ECB ENC/DEC 192bit
pass  TIMER :165600nsec
pass  TIMER :166400nsec
result value is pass

ECB ENC/DEC 256bit
pass  TIMER :187200nsec
pass  TIMER :188000nsec
result value is pass

ECB ENC/DEC 128bit Expansion Key
pass  TIMER :130400nsec
pass  TIMER :130400nsec
result value is pass

ECB ENC/DEC 192bit Expansion Key
pass  TIMER :165600nsec
pass  TIMER :166400nsec
result value is pass

ECB ENC/DEC 256bit Expansion Key
pass  TIMER :188000nsec
pass  TIMER :188000nsec
result value is pass

CBC ENC/DEC 128bit
pass  TIMER :142400nsec
pass  TIMER :142400nsec
result value is pass

CBC ENC/DEC 192bit
pass  TIMER :178400nsec
pass  TIMER :178400nsec
result value is pass

CBC ENC/DEC 256bit
pass  TIMER :200800nsec
pass  TIMER :200000nsec
result value is pass

CBC ENC/DEC 128bit Expansion Key
pass  TIMER :143200nsec
pass  TIMER :142400nsec
result value is pass

CBC ENC/DEC 192bit Expansion Key
pass  TIMER :178400nsec
pass  TIMER :179200nsec
result value is pass

CBC ENC/DEC 256bit Expansion Key
pass  TIMER :200000nsec
pass  TIMER :200000nsec
result value is pass

UPDATE KEY
Index  1: fail  TIMER :308000nsec
          M4result value is fail
          M5result value is fail
Index  2: fail  TIMER :332800nsec
          M4result value is fail
          M5result value is fail
Index  3: fail  TIMER :332800nsec
          M4result value is fail
          M5result value is fail
Index  4: fail  TIMER :308800nsec
          M4result value is fail
          M5result value is fail
Index  5: fail  TIMER :332800nsec
          M4result value is fail
          M5result value is fail
Index  6: pass  TIMER :2768800nsec
          M4result value is pass
          M5result value is pass

MAC GEN/VERI 128bit
fail  TIMER :82400nsec
fail  TIMER :83200nsec
mac verfy result is fail

MAC GEN/VERI 192bit
fail  TIMER :82400nsec
fail  TIMER :83200nsec
mac verfy result is fail

MAC GEN/VERI 256bit
fail  TIMER :82400nsec
fail  TIMER :84000nsec
mac verfy result is fail

MAC GEN/VERI 128bit Expansion Key
fail  TIMER :82400nsec
fail  TIMER :84000nsec
mac verfy result is fail

MAC GEN/VERI 192bit Expansion Key
fail  TIMER :82400nsec
fail  TIMER :84000nsec
mac verfy result is fail

MAC GEN/VERI 256bit Expansion Key
pass  TIMER :192800nsec
pass  TIMER :215200nsec
mac verfy result is pass

SECURE BOOT
pass  TIMER :3967200nsec

BOOT FAILURE
pass  TIMER :2400nsec

SECURE BOOT
pass  TIMER :3967200nsec

BOOT OK
pass  TIMER :2400nsec

INIT RNG
pass  TIMER :400000nsec

EXTEND SEED
pass  TIMER :136000nsec

RND
pass  TIMER :57600nsec
OUT   :58d521fa56715d6016ac34ca9901e868

GET ID
pass  TIMER :180800nsec
SECST :7e
UID   :00000023052d15198046f300004520
MAC   :f86fab6a4f4ef0f00fc059356d2494fa

Done.

``` 
</div>

### 20261109 Note

#### ■ 針對 OB4=0x3F 的 fail
* 由 log 的資訊可知，出錯的部份在 index 9，加入 debug 資訊，發現後面資料不符合。
```
Index  9: pass  TIMER :2653600nsec
          M4result value is fail
          M5result value is fail
result: 0000000000000000000000000000006187fadad0999a61a751e88d937b59a858
expect: 0000000000000000000000000000006187fadad0999a61a751e88d934ab4492b
```

> `security_driver_test.c` 的 `void test_chk_veri_mac(void)`
* 經分析可知，在 `test_load_key()`後，index 9 的資料由以下 tv[8] 讀取，其中 M4 為 `M4_ID_16`
```c
load_key_tv_t tv[8] = { M1_ID_x6, M2_ID_16, M3_ID_16, M4_ID_16, M5_ID_16, 0x01, 0x03};
```
* M4_ID_16 的定義如下：
```
static uint8_t M4_ID_16[32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x61,
                               0x87, 0xfa, 0xda, 0xd0, 0x99, 0x9a, 0x61, 0xa7,
                               0x51, 0xe8, 0x8d, 0x93, 0x4a, 0xb4, 0x49, 0x2b};
```
* 在記憶體 tv[8].m4 的最後幾個位元資料 (7b59a858)，與上面原始碼的定義不同，原因為記憶體被覆蓋掉了，錯誤的位址為 0xFD300。
* 由手冊可知，0xFD300 被 on-chip trace 用掉了
> When using the on-chip trace function of the on-chip debugger, do not use the area from FD300H to FD4FFH.

__解決方法__：修改 section 的 RAM (.dataR) 位址： `0xFCF00` ➜ `0xFD500`

#### ■ Secure Boot

* 範例專案說明文件：<span style="color:lightgrey">r01an7264ej0100-rl78f22-f25-aesea1-security\Document\</span>`r01an7264ej0100-rl78f22-f25-security.pdf`
* 專案會依照所選取的晶片型號，執行 `gen_ram_header.py`來產生標題檔 `r_ram_range.h`
* 針對 **OB4** = **0x2F** 時，無法正常執行，原因如下：
> Loader 的 MAC 是寫死的。 cstart_rfd_customized.asm:277-287 在 0x100 放長度、0x102 放 MAC、0x112 放測試資料，MAC
> 是針對特定內容預先算好的。專案改過 OB1/OB2，也加了 port.c 等檔案，Loader 區的實際內容已經不同，MAC 對不上。
* 在 .asm 檔可以看到以下 section 定義
```asm
boot_len .cseg text
.org 0x100
.db 0x00,0x10

boot_loader_mac .cseg text
.org 0x102
.db 0x0E,0x08,0x6A,0xC7,0xD4,0xB3,0xE5,0x11,0xC4,0x57,0xE0,0x13,0x3F,0x0A,0x1A,0xEE

boot_loader .cseg text
.org 0x112
.db 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xD9,0xB8,0x84,0x17,0x02,0xB5,0x0E,0x9B,0x5E,0xD5,0x0A,0x14,0x94,0xDF,0xF0,0xE2
```
* 在 .map 檔可以看到以下 section
```
boot_len_AT100
                                  00000100  00000101         2   1
boot_loader_mac_AT102
                                  00000102  00000111        10   1
boot_loader_AT112
                                  00000112  00000131        20   1
```

20261110
----------------------------------------

* 若使用 rfp-cli 燒錄沒開啟 secure boot 的 mot，但是設定了 key-password 以及 mac-password，則會出錯。

使用工具

```
R_KEY:    15180F23C051D019108281810F245050
Key_PW:   00112233445566778899AABBCCDDEEFF
MAC_PW:   0123456789ABCDEFFEDCBA9876543210
Key_data: 2B7E151628AED2A6ABF7158809CF4F3C
```

Normal
```
"C:\Program Files (x86)\Renesas Electronics\Programming Tools\Renesas Flash Programmer V3.23\rfp-cli.exe" -d RL78 -t E2 -if uart1 -vo 5.0 -a -file p_3F.mot 
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
```
Secure Boot
```
"C:\Program Files (x86)\Renesas Electronics\Programming Tools\Renesas Flash Programmer V3.23\rfp-cli.exe" -d RL78 -t E2 -if uart1 -vo 5.0 -a -key-password 00112233445566778899AABBCCDDEEFF -mac-password 0123456789ABCDEFFEDCBA9876543210 -file p_2F.mot
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
```

```
"C:\Program Files (x86)\Renesas Electronics\Programming Tools\Renesas Flash Programmer V3.23\rfp-cli.exe" -d RL78 -t E2 -if uart1 -vo 5.0 -a -key-password 00112233445566778899AABBCCDDEEFF -mac-password 0123456789ABCDEFFEDCBA9876543210 -file secdr_rl78_sample.mot
Renesas Flash Programmer CLI V1.16
 Module Version: V3.23.00.000
 Package Version: V3.23.00 [1 Apr 2026]
Load: "C:\temp\20261110\tools\secdr_rl78_sample.mot" (Size=82722, CRC=C4538148)

Connecting the tool (E2 emulator)
Tool: E2 emulator (0CS003148C)
Interface: 1 wire UART
Tool Firmware Version: V1.05.00.000

Emulator power supply: 5.0V
Connecting the target device
Enter ID Code (16 Bytes)? FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
Speed: 1,000,000 bps
Connected to R7F125FPL

Erasing the target device
  [Data Flash 1]       000F1000 - 000F4FFF
  [Code Flash 1]       00000000 - 0007FFFF
  [Code Flash 1]       00000000 - 000007FF
  [Code Flash 1]       00005000 - 0000C7FF
  [Code Flash 1]       0007F800 - 0007FFFF
Writing data to the target device
  [Code Flash 1]       00000000 - 000003FF
  [Code Flash 1]       00005000 - 0000C3FF
  [Code Flash 1]       0007FC00 - 0007FFFF
Verifying data on the target device
  [Code Flash 1]       00000000 - 000003FF
  [Code Flash 1]       00005000 - 0000C3FF
  [Code Flash 1]       0007FC00 - 0007FFFF


Disconnecting the tool

Operation successful

C:\temp\20261110\tools>
```


```

AESEA ENCRYPTION TEST $Rev: 158 $

GET ID
pass  TIMER :63200nsec
SECST :00
UID   :00000001073c1b19803cf300005651
MAC   :00000000000000000000000000000000

CHK VERI MAC1
pass  TIMER :65600nsec

CHK VERI MAC2
pass  TIMER :65600nsec

LOAD KEY
Index  1: pass  TIMER :2578400nsec
          M4result value is pass
          M5result value is pass
Index  2: pass  TIMER :2660000nsec
          M4result value is pass
          M5result value is pass
Index  3: pass  TIMER :2660800nsec
          M4result value is pass
          M5result value is pass
Index  4: pass  TIMER :2580000nsec
          M4result value is pass
          M5result value is pass
Index  5: pass  TIMER :2652000nsec
          M4result value is pass
          M5result value is pass
Index  6: pass  TIMER :2650400nsec
          M4result value is pass
          M5result value is pass
Index  7: pass  TIMER :2582400nsec
          M4result value is pass
          M5result value is pass
Index  8: pass  TIMER :2653600nsec
          M4result value is pass
          M5result value is pass
Index  9: pass  TIMER :2653600nsec
          M4result value is pass
          M5result value is pass

ECB ENC/DEC 128bit
pass  TIMER :130400nsec
pass  TIMER :129600nsec
result value is pass

ECB ENC/DEC 192bit
pass  TIMER :165600nsec
pass  TIMER :165600nsec
result value is pass

ECB ENC/DEC 256bit
pass  TIMER :188000nsec
pass  TIMER :188000nsec
result value is pass

ECB ENC/DEC 128bit Expansion Key
pass  TIMER :130400nsec
pass  TIMER :130400nsec
result value is pass

ECB ENC/DEC 192bit Expansion Key
pass  TIMER :165600nsec
pass  TIMER :166400nsec
result value is pass

ECB ENC/DEC 256bit Expansion Key
pass  TIMER :188000nsec
pass  TIMER :188000nsec
result value is pass

CBC ENC/DEC 128bit
pass  TIMER :142400nsec
pass  TIMER :142400nsec
result value is pass

CBC ENC/DEC 192bit
pass  TIMER :177600nsec
pass  TIMER :178400nsec
result value is pass

CBC ENC/DEC 256bit
pass  TIMER :200000nsec
pass  TIMER :200000nsec
result value is pass

CBC ENC/DEC 128bit Expansion Key
pass  TIMER :142400nsec
pass  TIMER :142400nsec
result value is pass

CBC ENC/DEC 192bit Expansion Key
pass  TIMER :178400nsec
pass  TIMER :178400nsec
result value is pass

CBC ENC/DEC 256bit Expansion Key
pass  TIMER :200000nsec
pass  TIMER :200000nsec
result value is pass

UPDATE KEY
Index  1: pass  TIMER :2700000nsec
          M4result value is pass
          M5result value is pass
Index  2: pass  TIMER :2770400nsec
          M4result value is pass
          M5result value is pass
Index  3: pass  TIMER :2770400nsec
          M4result value is pass
          M5result value is pass
Index  4: pass  TIMER :2701600nsec
          M4result value is pass
          M5result value is pass
Index  5: pass  TIMER :2772800nsec
          M4result value is pass
          M5result value is pass
Index  6: pass  TIMER :2772800nsec
          M4result value is pass
          M5result value is pass

MAC GEN/VERI 128bit
pass  TIMER :148800nsec
pass  TIMER :152800nsec
mac verfy result is pass

MAC GEN/VERI 192bit
pass  TIMER :177600nsec
pass  TIMER :190400nsec
mac verfy result is pass

MAC GEN/VERI 256bit
pass  TIMER :192000nsec
pass  TIMER :214400nsec
mac verfy result is pass

MAC GEN/VERI 128bit Expansion Key
pass  TIMER :148800nsec
pass  TIMER :170400nsec
mac verfy result is pass

MAC GEN/VERI 192bit Expansion Key
pass  TIMER :177600nsec
pass  TIMER :200000nsec
mac verfy result is pass

MAC GEN/VERI 256bit Expansion Key
pass  TIMER :192800nsec
pass  TIMER :215200nsec
mac verfy result is pass

SECURE BOOT
pass  TIMER :3967200nsec

BOOT FAILURE
pass  TIMER :2400nsec

SECURE BOOT
pass  TIMER :3967200nsec

BOOT OK
pass  TIMER :1600nsec

INIT RNG
pass  TIMER :372800nsec

EXTEND SEED
pass  TIMER :135200nsec

RND
pass  TIMER :57600nsec
OUT   :ff91a205d3294de81673a660e9cca3de

GET ID
pass  TIMER :180800nsec
SECST :3e
UID   :00000001073c1b19803cf300005651
MAC   :6260d6da8303885a78a6887f82b28f31

Done.

```
