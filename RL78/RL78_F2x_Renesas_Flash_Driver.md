RL78_F2x_Renesas_Flash_Driver
=============================

Document
----------------------------------------
### [r20ts0916ej0100-rl78.pdf](https://www.renesas.com/en/document/tnr/released-web-renesas-flash-driver-rl78-type-02-rl78f23-and-rl78f24-sc-version-application-notes-v100?srsltid=AfmBOoougDtFrSGB4kB0tqB9FefLrfkqvNhVAM9YTmfTIw2WDNSlvplQ), Major application notes  
* RL78 Family Renesas Flash Driver RL78 Type 02 SC Version (Code Flash) 
>Application Notes [R20AN0684] 
* RL78 Family Renesas Flash Driver RL78 Type 02 SC Version (Data Flash) 
>Application Notes [R20AN0685] 
* RL78 Family Renesas Flash Driver RL78 Type 02 SC Version (Extra Area) 
>Application Notes [R20AN0686] 
* RL78 Family Renesas Flash Driver RL78 Type 02 SC Version (Flash Common) 
>Application Notes [R20AN0687]

### `rl78-family-flash-memory-reprogramming-software-introduction.pdf`
RL78/F23 and RL78/F24 Group

* Renesas Flash Driver RL78 Type 02 (Simple version)
* Renesas Flash Driver RL78 Type 02 (SC version)
* EEPROM Emulation Software RL78 Type 02

Note: 主要參考 r20an0684ej0100-rl78.pdf, Section 2.  Creating a Sample Project for Code Flash Reprogramming 

Simple verion Setup step
----------------------------------------
### Create CS+ Project
以下將專案 `RL78_F24_RFD_Simple` 為例
### Source Code
解壓縮 `RENESAS_RFD_RL78_Type02_V1.00.zip` ，執行 `RENESAS_RFD_RL78_Type02_V1.00.exe`，會得到 [`RFDRL78T02`](#rfdrl78t02) 目錄，將 `RFDRL78T02` 複製到專案目錄下
```console
\RL78_F24_RFD_Simple\RFDRL78T02
```
將 `RFDRL78T02` 拉近 CS+ 的 `Project Tree` 中註冊原始碼。
>可修改 `subfolder level to search`: `5`

移除專案中不必要的目錄，例如 `xxx\IAR`  
移除專案中重覆的 `main.c`
```
\RL78_F24_RFD_Simple\RFDRL78T02\sample\RL78_F24\EX_FSW\CCRL\source\main.c
\RL78_F24_RFD_Simple\RFDRL78T02\sample\RL78_F24\DF\CCRL\source\main.c
\RL78_F24_RFD_Simple\RFDRL78T02\sample\RL78_F24\CF\CCRL\source\main.c
```
可依據需要(例如 Code Flash)，將 `main.c` 的程式複製到專案的 `main.c`
>Ex. `RFDRL78T02\sample\RL78_F24\CF\CCRL\source\main.c` -> `\RL78_F24_RFD_Simple\main.c`

### 專案設定修改 Section Setting
* `CC-RL (Build Tool)` -> `link Options` -> `Section`  
取消自動 section 規劃 `Layout section automatically`: `No` ，手動增加以下 section
> ROM sections: `RFD_DATA_n`,`RFD_CMN_f`,`RFD_CF_f`,`SMP_CMN_f`,`SMP_CF_f`  
> RAM sections: `RFD_DATA_nR`,`RFD_CMN_fR`,`RFD_CF_fR`,`SMP_CMN_fR`,`SMP_CF_fR`  
以下為範例: 
```
`.const,.text,.RLIB,.SLIB,.textf,.constf,.data,.sdata,.init_array,RFD_DATA_n,RFD_CMN_f,RFD_CF_f,SMP_CMN_f,SMP_CF_f/05000,.dataR,.bss,RFD_DATA_nR,RFD_CMN_fR,RFD_CF_fR,SMP_CMN_fR,SMP_CF_fR/F9F00,.sdataR,.sbss/FFE20
```
改回自動 section 規劃 `Layout section automatically`: `Yes(-AUTO_SECTION_LAYOUT)`  

* `CC-RL (Build Tool)` -> `link Options` -> `ROM to RAM mapped section`  
```
RFD_DATA_n=RFD_DATA_nR
RFD_CMN_f=RFD_CMN_fR
RFD_CF_f=RFD_CF_fR
SMP_CMN_f=SMP_CMN_fR
SMP_CF_f=SMP_CF_fR
```
* `CC-RL (Build Tool)` -> `link Options` -> `Device` 
    * `Option byte values for OCD`: `A5`
    * `Set security option byte`: `Yes(-SECURITY_OPT_BYTE)`
    * `Security option byte value`: `FE`
    * `Set debug monitor area`: `Yes(Specify address range)(-DEBUG_MONITOR=<Address range>)`
    * `Range of debug monitor area`: `3FE00-3FFFF`
    * `Set user option byte`: `Yes(-USER_OPT_BYTE)`
    * `User option byte value`: `6E6FE8`  
      >`6E6FE8:WDT stop, LVD [reset mode], 40MHz [The example for RL78/F24]`
    * `Control allocation to trace RAM area`: `No`

### 專案設定修改 include paths
處理重覆的檔案，例如
```c
#include "sample_config.h"
```
在以下都有定義。
```
RFDRL78T02\sample\RL78_F24\EX_FSW\CCRL\include
RFDRL78T02\sample\RL78_F24\DF\CCRL\include
RFDRL78T02\sample\RL78_F24\CF\CCRL\include
```
還有以下目錄重覆的檔案：
```
RL78_F24_RFD_Simple\RFDRL78T02\sample\RL78_F24\CF\CCRL\source\
RL78_F24_RFD_Simple\RFDRL78T02\sample\RL78_F24\DF\CCRL\source\
RL78_F24_RFD_Simple\RFDRL78T02\sample\RL78_F24\EX_FSW\CCRL\source\
```

### Link Setting (E2)
`RL78 E2 (Debug Tool)` -> `Connect Settings`

* `Clock`
    * `Main clock frequency [MHz]`: `4.00`
    * `Sub clock frequency[kHz]`: `32.768`
* `Connection with Target Board`
    * `Power target from the emulator.(MAX 200mA)`: `Yes`

測試
----------------------------------------
連線後，`View` -> `Memory` -> `Memory1`
輸入 `0x7000`，可看到

```
      | +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F
------|------------------------------------------------
07000 | FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
07010 | FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
07020 | FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
07030 | FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
```
執行以下函式 Erase/Write 後
```c
l_e_sam_ret_value = Sample_CodeFlashControl(WRITE_ADDRESS, WRITE_DATA_LENGTH, l_au08_write_data);
```
可以得到
```
      | +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F
------|------------------------------------------------
07000 | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
07010 | 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F
07020 | 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F
07030 | 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F
```

### NOTE
寫入位址在 `sample_config.h` 中定義
```c
#define CPU_FREQUENCY     (40u)
/**** Address to write ****/
/* It should be the head address of the flash block. */
#define WRITE_ADDRESS     (0x00007000uL)
/**** Data length to write ****/
/* It must be a multiple of 4. Don't across the boundary of the flash block. */
#define WRITE_DATA_LENGTH (1024u)
```
在除錯時，只要切到 programming mode ，Emulator 就會無回應，而在此模式下，也無法呼叫其它程式或中斷。
```c
    l_e_rfd_ret_status = R_RFD_SetFlashMemoryMode(R_RFD_ENUM_FLASH_MODE_CODE_PROGRAMMING);
```
若要檢查 Erase 的結果，可加入模式切換。
```
    l_e_rfd_ret_status = R_RFD_SetFlashMemoryMode(R_RFD_ENUM_FLASH_MODE_CODE_TO_NONPROGRAMMABLE);
    /* User can add Emulator break point or customer function call */
    l_e_rfd_ret_status = R_RFD_SetFlashMemoryMode(R_RFD_ENUM_FLASH_MODE_CODE_PROGRAMMING);
```
----------------------------------------

SC verion Setup step
----------------------------------------
### Create Project
### Import SmartConfigurator
### Add CF_sapmle
### Section Setting
### ROM to RAM settings
```
.data=.dataR
.sdata=.sdataR
RFD_DATA_n=RFD_DATA_nR
RFD_CMN_f=RFD_CMN_fR
RFD_CF_f=RFD_CF_fR
SMP_CMN_f=SMP_CMN_fR
SMP_CF_f=SMP_CF_fR
```
### Include path
`Common Options` -> `[Frequently Used Options(for Compile)]` -> `[Additional include paths]`
```
src\CF_sample\RL78_F24\config   
src\CF_sample\RL78_F24
src\CF_sample\common\include
```
### Device Item Settings
`Option byte values for OCD`: `A5`
`Set security option byte`: `Yes(-SECURITY_OPT_BYTE)`
`Security option byte value`: `FE`
`Set debug monitor area`: `Yes(Specify address range)(-DEBUG_MONITOR=<Address range>)`
`Range of debug monitor area`: `3FE00-3FFFF`
`Set user option byte`: `Yes(-USER_OPT_BYTE)`
`User option byte value`: `6E6FE8`
    `EFF3E0` -> `6E6FE8`, (WDT stop, LVD [reset mode], 40MHz [The example for RL78/F24])
`Control allocation to trace RAM area`: `No

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
        * 新增 `RS-CANFD lite Module Software Integration System` 模組
    * `Generate Code`
* Debug Tool Setting  
  在 `RL78 Simulator (Debug Tool)` 按右鍵 ->` Using Debug Tool` -> `RL78 E2`
    * Connect Settings
        * Clock
            * `Clock Main clock frequency [MHz]` : `4.00`
            * `Sub clock frequency[kHz]` : `32.768`
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

Appendix
----------------------------------------
## RFDRL78T02
```sh
RENESAS_RFD_RL78_TYPE02_V1.00\RFDRL78T02
│  Release.txt
│  Support.txt
│
├─include
│  │  r_rfd_code_flash_api.h
│  │  r_rfd_common_api.h
│  │  r_rfd_common_control_api.h
│  │  r_rfd_common_extension_api.h
│  │  r_rfd_common_get_api.h
│  │  r_rfd_common_userown.h
│  │  r_rfd_data_flash_api.h
│  │  r_rfd_extra_area_api.h
│  │  r_rfd_extra_area_security_api.h
│  │
│  └─rfd
│          r_rfd.h
│          r_rfd_compiler.h
│          r_rfd_device.h
│          r_rfd_memmap.h
│          r_rfd_types.h
│          r_typedefs.h
│
├─sample
│  ├─common
│  │  ├─include
│  │  │      sample_control_code_flash.h
│  │  │      sample_control_common.h
│  │  │      sample_control_data_flash.h
│  │  │      sample_control_extra_fsw.h
│  │  │      sample_defines.h
│  │  │      sample_memmap.h
│  │  │      sample_types.h
│  │  │
│  │  └─source
│  │      ├─codeflash
│  │      │      sample_control_code_flash.c
│  │      │
│  │      ├─common
│  │      │      sample_control_common.c
│  │      │
│  │      ├─dataflash
│  │      │      sample_control_data_flash.c
│  │      │
│  │      └─extra_fsw
│  │              sample_control_extra_fsw.c
│  │
│  └─RL78_F24
│      ├─CF
│      │  ├─CCRL
│      │  │  ├─include
│      │  │  │      iodefine.h
│      │  │  │      sample_config.h
│      │  │  │
│      │  │  └─source
│      │  │          cstart.asm
│      │  │          f24opt.asm
│      │  │          hdwinit.c
│      │  │          main.c
│      │  │          stkinit.asm
│      │  │
│      │  └─IAR
│      │      ├─include
│      │      │      sample_config.h
│      │      │
│      │      └─source
│      │              low_level_init.c
│      │              main.c
│      │              option_byte.c
│      │              sample_linker_file_CF.icf
│      │
│      ├─DF
│      │  ├─CCRL
│      │  │  ├─include
│      │  │  │      iodefine.h
│      │  │  │      sample_config.h
│      │  │  │
│      │  │  └─source
│      │  │          cstart.asm
│      │  │          f24opt.asm
│      │  │          hdwinit.c
│      │  │          main.c
│      │  │          stkinit.asm
│      │  │
│      │  └─IAR
│      │      ├─include
│      │      │      sample_config.h
│      │      │
│      │      └─source
│      │              low_level_init.c
│      │              main.c
│      │              option_byte.c
│      │              sample_linker_file_DF.icf
│      │
│      └─EX_FSW
│          ├─CCRL
│          │  ├─include
│          │  │      iodefine.h
│          │  │      sample_config.h
│          │  │
│          │  └─source
│          │          cstart.asm
│          │          f24opt.asm
│          │          hdwinit.c
│          │          main.c
│          │          stkinit.asm
│          │
│          └─IAR
│              ├─include
│              │      sample_config.h
│              │
│              └─source
│                      low_level_init.c
│                      main.c
│                      option_byte.c
│                      sample_linker_file_EX_FSW.icf
│
├─source
│  ├─codeflash
│  │      r_rfd_code_flash_api.c
│  │
│  ├─common
│  │      r_rfd_common_api.c
│  │      r_rfd_common_control_api.c
│  │      r_rfd_common_extension_api.c
│  │      r_rfd_common_get_api.c
│  │
│  ├─dataflash
│  │      r_rfd_data_flash_api.c
│  │
│  └─extraarea
│          r_rfd_extra_area_api.c
│          r_rfd_extra_area_security_api.c
│
└─userown
        r_rfd_common_userown.c
```


# TEST

target <div id="mylink" /> 

<div id="tesst" />
# TEST2 