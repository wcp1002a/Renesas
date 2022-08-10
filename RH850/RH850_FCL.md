RH850 FCL
==========
## FCL Sections example
Address           | Section
------------------|------------
 %ResetVectorPE1% | RESET
 &nbsp;           | EIINTTBL
 0x00008000       | .const
                  | .INIT_DSEC.const
                  | .INIT_BSEC.const
                  | .text
                  | R_FCL_CODE_ROM.text
                  | R_FCL_CONST.const
                  | R_FCL_CODE_USRINT.text
                  | R_FCL_CODE_USR.text
                  | R_FCL_CODE_RAM.text
                  | R_FCL_CODE_ROMRAM.text
                  | R_FCL_CODE_RAM_EX_PROT.text
                  | .data
 0xFEBE0000       | FCL_RESERVED.bss
                  | R_FCL_DATA.bss
                  | .data.R
                  | .bss
                  | .stack.bss


## Steps
##### 執行 RENESAS_FCL_RH850_T01_V2.13.exe
產生的檔案結構如下：

        CC-RH_200
        ├─FCL
        │  └─lib
        └─RH850-F1x
            └─Sample

###### 複製 \FCL\CC-RH_200\FCL 至專案目錄下的 FCL
###### 複製 \FCL\CC-RH_200\RH850-F1x\Sample 下 `fcl_` 開頭的檔案及 `target.h` 至專案目錄下

    fcl_cfg.h
    fcl_ctrl.c
    fcl_descriptor.c
    fcl_descriptor.h
    fcl_user.c
    fcl_user.h
    target.h

###### 指定 sections

    RESET,EIINTTBL/%ResetVectorPE1%,.const,.INIT_DSEC.const,.INIT_BSEC.const,.text,R_FCL_CODE_ROM.text,R_FCL_CONST.const,R_FCL_CODE_USRINT.text,R_FCL_CODE_USR.text,R_FCL_CODE_RAM.text,R_FCL_CODE_ROMRAM.text,R_FCL_CODE_RAM_EX_PROT.text,.data/00008000,FCL_RESERVED.bss,R_FCL_DATA.bss,.data.R,.bss,.stack.bss/FEBE0000

###### 設定 Emulator
設定 RH850 E2 (Debug Tool) -> Connect Settings -> Using the code flash self programming 為 Yes

W0561017:The evaluation period of CC-RH V2 has expired. Please consider purchasing the product.
W0561322:Section alignment mismatch : "R_FCL_CODE_RAM.text"
W0561322:Section alignment mismatch : "R_FCL_CODE_ROMRAM.text"
W0561322:Section alignment mismatch : "R_FCL_CODE_USR.text"
W0561120:Section address is not assigned to "R_FCL_CODE_RAM.text"
W0561120:Section address is not assigned to "R_FCL_CODE_ROMRAM.text"
W0561120:Section address is not assigned to "R_FCL_CODE_USRINT.text"
W0561120:Section address is not assigned to "R_FCL_CODE_USR.text"
W0561120:Section address is not assigned to "R_FCL_CODE_RAM_EX_PROT.text"
W0561120:Section address is not assigned to "R_FCL_CODE_ROM.text"
W0561120:Section address is not assigned to "R_FCL_DATA.bss"
W0561120:Section address is not assigned to "R_FCL_CONST.const"
W0561017:The evaluation period of CC-RH V2 has expired. Please consider purchasing the product.
Renesas Optimizing Linker Completed
------ Build ended(Error:0, Warning:13)(rh850_study, DefaultBuild) ------
========== Ended(Success:1 Projects, Failed:0 Projects)(2022年7月29日 11:05:43) ==========


    



RESET,EIINTTBL/%ResetVectorPE1%,.const,.INIT_DSEC.const,.INIT_BSEC.const,.text,R_FCL_CODE_ROM.text,R_FCL_CONST.const,R_FCL_CODE_USRINT.text,R_FCL_CODE_USR.text,R_FCL_CODE_RAM.text,R_FCL_CODE_ROMRAM.text,R_FCL_CODE_RAM_EX_PROT.text,.data/00008000,FCL_RESERVED.bss,R_FCL_DATA.bss,.data.R,.bss,.stack.bss/FEDE8000

RESET,EIINTTBL/%ResetVectorPE1%,.const,.INIT_DSEC.const,.INIT_BSEC.const,.text,R_FCL_CODE_ROM.text,R_FCL_CONST.const,R_FCL_CODE_USRINT.text,R_FCL_CODE_USR.text,R_FCL_CODE_RAM.text,R_FCL_CODE_ROMRAM.text,R_FCL_CODE_RAM_EX_PROT.text,.data/00008000,FCL_RESERVED.bss,R_FCL_DATA.bss,.data.R,.bss,.stack.bss/FEBE0000
