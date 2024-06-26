RH850 CC-RH (RH850)
===================

## Overview
* Manual: r20ut3516ej0109-ccrh.pdf

## IDE 相關設定
#### Section 設置
由路徑 `CC-RH (Build Tool)` -> `Link Options` -> `Section` -> `Section start address`  
以F1KM-S1 (R7F701684 100PIN) 原始預設 Section 如下：  


 Address          | Section
------------------|------------
 %ResetVectorPE1% | RESET
 &nbsp;           | EIINTTBL
 0x00008000       | .const
 &nbsp;           | .INIT_DSEC.const
 &nbsp;           | .INIT_BSEC.const
 &nbsp;           | .text
 &nbsp;           | .data
 0xFEDE8000       | .data.R
 &nbsp;           | .bss
 &nbsp;           | .stack.bss

    RESET,EIINTTBL/%ResetVectorPE1%,.const,.INIT_DSEC.const,.INIT_BSEC.const,.text,.data/00008000,.data.R,.bss,.stack.bss/FEDE8000


#### Output information to map file
由路徑 `CC-RH (Build Tool)` -> `Link Options` -> `List`

* `Output link map file`: `Yes`
* `Output symbol informat`: `Yes`

#### Disable Optimization
由路徑 `CC-RH (Build Tool)` -> `Compile Options` -> `Optimization`

* `Leve of optimization`: `Debug precedence(-Onothing)`

## Section 程式相關設定(使用 `#pragma section`)
設定 sections 如下

 Address          | Section
------------------|------------
 %ResetVectorPE1% | RESET
 &nbsp;           | EIINTTBL
 0x00008000       | .const
 &nbsp;           | .INIT_DSEC.const
 &nbsp;           | .INIT_BSEC.const
 &nbsp;           | .text
 &nbsp;           | .data
 __0x00009000__   | __myconst.const__                  
 __0x00009100__   | __mydata.data__ 
 0xFEDE8000       | .data.R
 &nbsp;           | .bss
 &nbsp;           | .stack.bss
 __0xFEDE8400__   | __mydata.data.R__
 __0xFEDE8500__   | __mydata.bss__

#### 指定常變數至 ROM 區域的指定位址 (ROM only)
```c
#pragma section const "myconst"
const unsigned char myconstdata[8] = { 0x12, 0x34, 0x56, 0x78, 'A','B','C','D'};
/* ROM: myconst.const (0x9000) */

#pragma section default
```

        
#### 指定變數至 RAM 區域的指定位址 (ROM to RAM, or RAM only)
```c
#pragma section mydata
int a = 0x5a99;         /* allocated to mydata.data attribute section */
/* ROM: mydata.data (e.g. 0x9100), RAM: mydata.data.R (e.g. 0xFEDE8400) */

int b;                  /* allocated to mydata.bss attribute section */
/* ROM: N/A, RAM: mydata.bss (e.g. 0xFEDE8500) */

#pragma section default
```

#### 一般不指定的情況
```c
// 0x4567 will copy from ROM to RAM after calling __INITSCT_RH
int abc = 0x4567;   /* ROM: .data, RAM: .data.R */      // 跟 myvardata 一樣
int bcd;            /* ROM: N/A,   RAN: .bss    */
```

#### map file 的位址

        SECTION                            START      END         SIZE   ALIGN
        RESET                             00000000  000001f7       1f8   200
        EIINTTBL                          00000200  00000797       598   200
        .const                            00008000  00008000         0   4
        .INIT_DSEC.const                  00008000  0000800b         c   4
        .INIT_BSEC.const                  0000800c  00008013         8   4
        .text                             00008014  00008b8b       b78   2
        .data                             00008b8c  00008b97         c   4
        myconst.const                     00009000  00009007         8   4
        mydata.data                       00009100  00009103         4   4
        .data.R                           fede8000  fede800b         c   4
        .bss                              fede800c  fede8013         8   4
        .stack.bss                        fede8014  fede8213       200   4
        mydata.data.R                     fede8400  fede8403         4   4
        mydata.bss                        fede8500  fede8503         4   4

        SECTION=.data.R
        FILE=DefaultBuild\r_cg_main.obj
                                          fede8000  fede8007         8
          _myvardata
                                          fede8000         4   data ,g         * 
          _abc
                                          fede8004         4   data ,g         * 

        SECTION=.bss
        FILE=DefaultBuild\r_cg_cgc.obj
                                          fede800c  fede800f         4
          _g_cg_sync_read
                                          fede800c         4   data ,g         * 
        FILE=DefaultBuild\r_cg_main.obj
                                          fede8010  fede8013         4
          _bcd
                                          fede8010         4   data ,g         * 
        
        SECTION=mydata.data.R
        FILE=DefaultBuild\r_cg_main.obj
                                          fede8400  fede8403         4
          _a
                                          fede8400         4   data ,g         * 
        
        SECTION=mydata.bss
        FILE=DefaultBuild\r_cg_main.obj
                                          fede8500  fede8503         4
          _b
                                          fede8500         4   data ,g         * 
        