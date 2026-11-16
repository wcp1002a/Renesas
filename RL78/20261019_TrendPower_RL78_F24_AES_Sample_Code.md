20261019_TrendPower_RL78_F24_AES_Sample_Code
============================================

* Board: RL78/F24 Target board 100pin

Function    | Signal Name| Pin Name  | RL78/F24 TB
------------|------------|-----------|-----------
UART/TX     | TXD0       | P15       | CN2.5
UART/RX     | RXD0       | P16       | CN2.7
GPIO        | P11        | P11       | CN2.1
Ground      | GND        | GND       | CN1.20, CN1.21

Code Backup
----------------------------------------
* rl78_f24_aes_sample_v1: test ok
* rl78_f24_aes_sample_v1.2: 移除不使用的檔案及目錄
* rl78_f24_aes_sample_v1.3: 新增 GPIO P11 來驗證時間

Teraterm 輸出
```sh

AESEA ENCRYPTION TEST $Rev: 158 $

AES-128 (FIPS-197 C.1) SetKey : pass
  TIMER :180000nsec
 ECB Encrypt : pass
  TIMER :46400nsec
 ECB Decrypt : pass

AES-192 (FIPS-197 C.2) SetKey : pass
  TIMER :224800nsec
 ECB Encrypt : pass
  TIMER :59200nsec
 ECB Decrypt : pass

AES-256 (FIPS-197 C.3) SetKey : pass
  TIMER :225600nsec
 ECB Encrypt : pass
  TIMER :60000nsec
 ECB Decrypt : pass

AES-128 CBC (SP800-38A F.2.1)
 CBC Encrypt : pass
 CBC Decrypt : pass

AES-192 CBC (SP800-38A F.2.3)
 CBC Encrypt : pass
 CBC Decrypt : pass

ALL PASS

Done.

```

----------------------------------------


#### Documentation
* Security HWM: `REN_r01uh0946ej0110-rl78f23-f24_MAH_20241223.pdf`

----------------------------------------

#### 20261021
* 範例程式: r20an0664ej0130-rl78f23-f24-aesea-security.zip
* 開啟專案時，會出現找不到 Device File，主要是在專案檔 (*.mtpj)指定特別名稱，如下：
```xml
      <DeviceName>R7F124FPJ4xFB(AES)</DeviceName>
      <ProjectInitialDeviceName>R7F124FPJ4xFB(AES)</ProjectInitialDeviceName>
```
* 將(AES)字樣移除即可正常開啟 (<span style="color:red">非標準作法</span>)
* 標準作法為至 RL78/F2x SEC 下載對應的 Device File，並複製到以下目錄：
  `C:\Program Files (x86)\Renesas Electronics\CS+\CC\Device_Custom`
* 使用 Claude Code 將 Code/Data Flash 目關程式碼移除，連線後無法正常跳至 Main。  
  <span style="color:red">Note: 之後無法連線，也無法再清除/燒錄。</span>
* 查看 Option Bytes設定，發現應該是 Option Bytes 000C4=0x2F (SECBTEN=0, Secure Boot for Loader enable)  
  000C0~000C4: CE 22 C8 A7 2F (definded in `cstart_rfd_customized.asm`)
* 修改 000C4=0x3F
```
; cstart_rfd_customized.asm
OB4:    .db 0x3f      ;; 0011_1111 / KSIZE=00, SECBTEN=1 (Secure Boot for Loader disabled), SHEEN=1, IDRDEN=1, AESEN=1
; mot file result
S10800C0CE22C8A73F99
```

```
Cannot rewrite the flash programmer security ID.(E1203101)
```

#### Unknow Issue (可能是 Smart Configuratoer 的錯誤)
由於不知明原因，SmartConfigurator 需依以下設定，才能正常運行

* Trace function setting: `Unused`
* 另外在 Flash serial programming security ID setting 的範圍變成 0 ~ 0xFFFF...FF<span style="color:red">FE</span>
* Flash serial programming security ID setting: `0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF`
* 在 CS+ 手動修改  
    + 在 `RL78 E2 (Debug Tool)` ▶️ `Common Options` ▶️ `Devices`:  
    + `Flash serial programming security ID`: `0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF`

----------------------------------------

RL78 F24 Environment setup
----------------------------------------
#### CS+
* Create New Project:  
  `File` -> `New` -> `Create New Project...` -> `RL78` -> `R7F124FPJ5xFB(100pin)`
* Smart Configurator (Design Tool)
    * 雙擊開啟 Smart Configurator
    * Clock
        * `Base frequency`: `40` (MHz)
        * `High-speed on-chip oscillator`: `40` (MHz)
        * `X1 oscillator`: check
            * `Frequency`: `4` (MHz)
        * `PLL setting`: check
            * Source from X1 oscillator
            * `Frequency`: `80` (MHz)
            * `Lockup wait counter`: `256/fMAIN` 64(us)
            * Output to Divider: select (選擇 `fCLK` 的來源由 `PLL` -> `Divider`)
            * `Divider`: `x1/2`
            * Timer RD Clock: 來源選擇 PLL
        * `XT1 oscillator`: check
            * FSL來源選擇 XT1 oscillator (fSUB)
    * System
        * `On-chip debug operation setting`: `Use emulator`
        * `Emulator setting`: `E2`
    * Components (Example)
        * 新增 `Ports` 模組
            * `PORT1`: check
                * `P12`: Out check
        * 新增 `AESEA Security Driver Module Software Integration System`
    * `Generate Code`
* Debug Tool Setting  
  在 `RL78 Simulator (Debug Tool)` 按右鍵 ->` Using Debug Tool` -> `RL78 E2`
    * Connect Settings
        * Clock
            * `Clock Main clock frequency [MHz]` : `4.00`
            * `Sub clock frequeny[kHz]` : `32.768`
        * Connection with Target Board
            * `Power target from the emulator.(MAX 200mA)`: `Yes`
            * `Suppy voltage [V]`: `5.0`
* E2
    * 使用 E2 來連線 RL78 MCU 時，要將 adapter 的 switch 切至 3 side，否則會發生以下 `E1200416` 錯誤
```
Download failed.

[Direct Error Cause]
No response from the emulation CPU. Please confirm the signal of the CLOCK or RESET,WAIT and so on.(E1200416)
```


