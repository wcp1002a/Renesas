RH850 使用 C 語言設置中斷向量表
==========================================

## 大網
* 建立專案
* 使用 Smart Configurator 加入 Component
* 修改 `boot.asm` 
* CS+ 配置
    + 加入路徑
    + 複製 `sc_intprg-S1.c`，此檔案定義了中斷函式本體 `eiint0`~`eiint357`
    + 修改 CS+ Section 配置

以下以使用 RH850/F1KM-S1 Starter Kit V3 為例

### 建立專案
* 建立專案  
`File` -> `New` -> `Create New Project...` 填入專案名稱(在此範例使用 test)後，按下 `Create`

* 設定 Emulator
    + 在RH850 Simulator上按右鍵 -> `Using Debug Tool` -> `RH850 E2
    Clock` -> `Main clock frequency [MHz]` -> `16.00`  
    + `Connection with Target Board` -> `Power target from the emulator.(MAX 200mA)`
    + `Flash` -> `Security ID` -> `FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF`

### Smart Configurator
* 加入 Component
    + 在 Components 分頁中加入一個 Interval Timer  
      `Clock setting` -> `Clock source` -> `PCLK/1024` -> `Interval time` -> `1000us`  
      按下 `Generate Code`

### CS+ 配置
#### 一、加入路徑
編譯會找不到 iodefine.h  
`CC-RH (Build Tool)` -> `Compile Options` -> `Additional include paths` 加入 `.\`

#### 二、修改 boot.asm
1. 將使用查表方式的註釋打開  
![rh850_interrupt_1](attach\rh850_interrupt_1.png)

2. 將 `boot.asm` 中跟中斷向量表相關部分內容註釋  
![rh850_interrupt_2](attach\rh850_interrupt_2.png)

3. 修改 `boot.asm` ，將使用 `r_cg_intvector.c` 的中斷向量表  
![rh850_interrupt_3](attach\rh850_interrupt_3.png)

4. 加入 section 設定  
由路徑 `CC-RH (Build Tool)` -> `Link Options` -> `Section`，加入 `EIINTTBL.const` 如下  
![rh850_interrupt_4](attach\rh850_interrupt_4.png)
CC

#### 三、修改boot.asm及加入中斷定義檔 sc_intprg-S1.c

* 編譯會找不到中斷函式 `eiintxx`，將 `sc_intprg-S1.c` 加入專案中
* 在 `r_cg_main.c` 中 (`.\src\smc_gen\general\`) 加入 TAU 啟動函式

```c
void main(void)
{
    R_MAIN_UserInit();
    /* Start user code for main. Do not edit comment generated here */
    R_Config_TAUB0_0_Start();
    while(1);
    /* End user code. Do not edit comment generated here */
}
```

#### 其它
TAU 中斷 142 ，可以計算 0x200 + 142 x 4 = 0x438
![rh850_interrupt_5](attach\rh850_interrupt_5.png)
![rh850_interrupt_6](attach\rh850_interrupt_6.png)
