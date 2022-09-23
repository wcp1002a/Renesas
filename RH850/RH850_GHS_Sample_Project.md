Create GHS Project
========================================
## 大網
* 建立專案
* 使用 Smart Configurator 加入 Component
* 編譯路徑設定
* 其它修正

#### 建立專案
* 建立新專案  
  `File` -> `New` -> `Create New Project...`
* Create Project 使用以下設定  
  Kind of project: `Empty Application(GHS CCRH850)`  
  Project name: `RH850_GHS_Project`

* 設定 Emulator
    + 在RH850 Simulator上按右鍵  
      `Using Debug Tool` -> `RH850 E2
    Clock` -> `Main clock frequency [MHz]` -> `16.00`  
    + `Connection with Target Board` -> `Power target from the emulator.(MAX 200mA)`
    + `Flash` -> `Security ID` -> `FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF`

#### Smart Configurator
* 在 `Project Tree` 的 `Smart Configurator (Design Tool)` 雙擊滑鼠左鍵
* 選取 `Components` 分頁
* 按下 + 圖示 (`Add Component`) -> 選取 `RIIC Master` -> `Finish`
* 按下上方的 `Generate Code`

#### 編譯設定
* `GHS CCRH850 (Build Tool)` -> `Compile Options` -> `Include Directories`
    - `src\smc_gen\Config_RIIC0`
    - `src\smc_gen\general\`
    - `src\smc_gen\r_pincfg\`
    - `.\`

#### 其它修正
* 編譯會找不到中斷函式 `eiintxx`，將 `sc_intprg-S1.c` 加入專案中
* 加入 ld 檔案，並設定 `sc_intprg-S1,c` 的 `EIINTTBL` section 位址  
  `EIINTTBL 0x200 :>.`

```c
    /* 以上省略 */
    SECTIONS
     {
    
    /* Start of internal ROM area (iROM_0) */
    
      .intvect                              :>iROM_0 /* start of interrupt vector */
      EIINTTBL 0x200                        :>.      /* 設定 EIINTTBL 的位址為 0x200 */
      .intvect_end  0x0798                  :>.      /* end   of interrupt vector */
      .rozdata                              :>.      /* constant datas in ZDA area */
      .robase   align(4)                    :>.      /* initialize textpointer TP for SDA addressing */
      .rosdata  align(4)                    :>.      /* constant datas in SDA area */
      .rodata   align(4)                    :>.      /* constant datas in normal area */
      /* 以下省略 */
     }
```