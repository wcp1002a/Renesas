RH850_GHS_CCRH850_Project_with_SmartConfigurator
================================================
## 大網
* 建立專案流程
    * 範例專案
    * 建立專案流程(複製範例專案)
    * 自行建立專案
* 使用 Smart Configurator
* 編譯設定
* Emulator 設定
* 其它修正

----------------------------------------
### 建立專案流程
#### 範例專案
範例專案可以在 SmartConfigurator 的目錄下找到
    
    C:\Program Files (x86)\Renesas Electronics\SmartConfigurator\RH850\RH850F1KM_SampleProjects\SC_CS+GHSCCRH850

檔案內容
```
    SC_CS+GHSCCRH850
        dr7f701649_irq.h
        iodefine.h
        SC_CS+GHSCCRH850.mtpj
        sc_intprg.c
        sc_linkerdr.ld
        sc_startup.850
```

#### 建立專案(複製範例專案)

* 將專案目錄複製一份，更改為自訂的專案名稱
* 執行 CS+
* 讀取 `SC_CS+GHSCCRH850.mtpj` 範例專案
* 修改 MCU
    - 修改為目標 device
    - 取代 sc_intprg.c
    - 編輯 ld 檔 (Linker directive file)

#### 自行建立專案
* 建立新專案  
  `File` -> `New` -> `Create New Project...`
* Create Project 使用以下設定  
  Kind of project: `Empty Application(GHS CCRH850)`  
  Project name: `RH850_GHS_Project`

----------------------------------------

#### Smart Configurator
* 啟動 Smart Configurator
    - 在 `Project Tree` 的 `Smart Configurator (Design Tool)` 雙擊滑鼠左鍵
    - 選取 `Components` 分頁
    - 按下 + 圖示 (`Add Component`) -> 選取 `RIIC Master` -> `Finish`
    - 按下上方的 `Generate Code`

----------------------------------------
#### 編譯設定
* `GHS CCRH850 (Build Tool)` -> `Compile Options` -> `Include Directories`

```plantext
    src\smc_gen\Config_RIIC0
    src\smc_gen\general\
    src\smc_gen\r_pincfg\
    .\
```

* 修改目標處理器
    + `Common Optionts` -> `Output File Type and Path` -> `Target Processor`  
      `RH850G3KH(-cpu=rh850g3kh)`
    + `Common Optionts` -> `Path To Tools` -> `Compiler package folder`  
      `C:\GHS\V800.V2015.1.7\comp_201517`

----------------------------------------
#### Emulator 設定
* 在RH850 Simulator上按右鍵  
    * `Using Debug Tool` -> `RH850 E2` -> `Clock`  
      `Main clock frequency [MHz]`: `16.00`  
    * `Connection with Target Board`  
      `Power target from the emulator.(MAX 200mA)`: `Yes`
    * `Flash`  
      `Security ID`: `FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF`

----------------------------------------
#### 其它修正
* 編譯會找不到中斷函式 `eiintxx`，將 `sc_intprg-S1.c` 加入專案中
* 加入 ld 檔案，並設定 `sc_intprg-S1.c` 的 `EIINTTBL` section 位址  
  `EIINTTBL 0x200 :>.`

`sc_intprg-S1.c`可以在以下路徑找到

    C:\Program Files (x86)\Renesas Electronics\SmartConfigurator\RH850\RH850F1KM_SampleProjects\intprg\sc_intprg-S1.c

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