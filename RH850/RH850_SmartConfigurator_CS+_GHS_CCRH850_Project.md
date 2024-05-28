Create GHS Project
========================================
## 大網
* 複製範例專案
* 建立專案流程
* 使用 Smart Configurator
* 編譯設定
* Emulator 設定

#### 使用範例專案
範例專案可以在 SmartConfigurator的目錄下找到
    
    C:\Program Files (x86)\Renesas Electronics\SmartConfigurator\RH850\RH850F1KM_SampleProjects\SC_CS+GHSCCRH850

檔案內容

    SC_CS+GHSCCRH850
        dr7f701649_irq.h
        iodefine.h
        SC_CS+GHSCCRH850.mtpj
        sc_intprg.c
        sc_linkerdr.ld
        sc_startup.850

#### 建立專案流程

* 將專案目錄複製一份，更改為自訂的專案名稱
* 執行 CS+
* 讀取 `SC_CS+GHSCCRH850.mtpj` 範例專案
* 修改 MCU
    - 修改為目標 device
    - 取代 sc_intprg.c
    - 編輯 ld 檔 (Linker directive file)
* 啟動 Smart Configurator
    - 在 `Project Tree` 的 `Smart Configurator (Design Tool)` 雙擊滑鼠左鍵
    - 選取 `Components` 分頁
    - 按下 + 圖示 (`Add Component`) -> 選取 `RIIC Master` -> `Finish`
    - 按下上方的 `Generate Code`

* 編譯設定
    + `GHS CCRH850 (Build Tool)` -> `Compile Options` -> `Include Directories`
        + `src\smc_gen\Config_RIIC0`
        + `src\smc_gen\general\`
        + `src\smc_gen\r_pincfg\`
        + `.\`
    - 修改目標處理器
        + `Common Optionts` -> `Output File Type and Path` -> `Target Processor`
            * `RH850G3KH(-cpu=rh850g3kh)`
        + `Common Optionts` -> `Path To Tools` -> `Compiler package folder`
            * `C:\GHS\V800.V2015.1.7\comp_201517`
* Emulator 設定
    + 在RH850 Simulator上按右鍵  
      `Using Debug Tool` -> `RH850 E2
    Clock` -> `Main clock frequency [MHz]` -> `16.00`  
    + `Connection with Target Board` -> `Power target from the emulator.(MAX 200mA)`
    + `Flash` -> `Security ID` -> `FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF`

