RH850 RIIC
==========

#### Hardware Connection Layout

* Board: Y-ASK-RH850-F1KM-S1-V3

Function    | Signal Name| Pin Name  | F1KM/S1 EVK
------------|------------|-----------|-----------
I2C         | RIIC0SCL   | P0_12     | J3.14
I2C         | RIIC0SDA   | P0_11     | J3.13
Ground      | GND        | GND       | J3.21

#### RIIC clock source
CKSCLK_IIIC 40MHz   

``` c
//#define _RIIC0_BITRATE_LOW_LEVEL_PERIOD           (0x0000001FUL) /* Bit Rate Low-Level Period */
#define _RIIC0_BITRATE_LOW_LEVEL_PERIOD           (0x00000005UL) /* Bit Rate Low-Level Period */
#define _RIIC0_BITRATE_HIGH_LEVEL_PERIOD          (0x00000002UL) /* Bit Rate High-Level Period */

// 1MHz setting
//#define _RIIC0_BITRATE_LOW_LEVEL_PERIOD           (0x00000005UL) /* Bit Rate Low-Level Period */
//#define _RIIC0_BITRATE_HIGH_LEVEL_PERIOD          (0x00000003UL) /* Bit Rate High-Level Period */
```
## Issue 傳輸過程會 Hang 住
### 2023/03/14
* 發現使用 TAU0 的 Timer 產生 1ms 中斷來當計數器時才會發生。
* 若不執行 R_Config_TAUB0_0_Start() 則不會影響 RIIC 傳輸

### 2023/03/15
#### 目前 Smart Configurator 產生的程式中斷運作特點如下：
* 使用變數 g_riic0_mode_flag 來控制流程
* IER 暫存器：啟用/停用中斷控制暫存器
* SR2 暫存器：狀態暫存器，包含中斷旗標
* CR2 暫存器：控制暫存器，用來產生 START, STOP 等狀態
* EE 中斷 error/event (vector=77)
* TI 中斷 Transmit data empty(vector=76)
* TEI 中斷 Transmit end (vector=79)
* 
#### 正常流程
* 產生 EE(vector=77) 中斷，SR2.TDRE (Transmit Data Empty) 與 START 中斷旗標被設定，先進入 EE 中斷副程式
    - 關閉 START 中斷，啟用 STOP 中斷
* 產生 TI(vector=76) 中斷
    - 設定將 Slave Address (DRT = Slave Address) 送出，g_riic0_mode_flag 設為 RIIC_MASTER_SENDS_DATA
    - 送出資料後持續觸發 TI 中斷，直到最後一筆資料時，將 g_riic0_mode_flag 設為 RIIC_MASTER_SENDS_END
    - 將 g_riic0_mode_flag 設為 RIIC_MASTER_SENDS_DATA
* 產生 TEI(vector=79) 中斷
    - 在 CR2 設置 STOP 請求，g_riic0_mode_flag 狀態設為 RIIC_MASTER_SENDS_STOP
* 產生 EE 中斷，STOP 旗標

#### 異常流程
* 在 TAU0 啟動的狀態，有時候會先觸發 TI(vector=76) 中斷，而不是 EE(vector=77)。
* 但是最後無法觸發 TEI 中斷，沒有送出 STOP 請求，造成傳輸流程卡住

#### 解決方案
* 1. (不推薦)修改 TI 中斷內的流程，加上結束產生 STOP 的程式
* 2. (不推薦)先不啟用 TI 中斷，待 EE 中斷產生後，在 EE 中斷副程式內啟用 TI 中斷
* 3. (推薦)將 EE 的中斷優先權提高
* 4. (推薦)另外的方式是將 TEI 的優先權提高也可行。(不會一直卡在 TI 中斷)
```
    INTC2.ICRIIC0EE.UINT16 &= _INT_PRIORITY_LEVEL10;    // 設置為較 TI 中斷來得高即可
```

