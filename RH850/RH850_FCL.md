RH850 FCL
==========
## FCL Sections example

Address           | Section
------------------|------------
 %ResetVectorPE1% | RESET
 &nbsp;           | EIINTTBL
 0x00008000       | .const
 &nbsp;           | .INIT_DSEC.const
 &nbsp;           | .INIT_BSEC.const
 &nbsp;           | .text
 &nbsp;           | R_FCL_CODE_ROM.text
 &nbsp;           | R_FCL_CONST.const
 &nbsp;           | R_FCL_CODE_USRINT.text
 &nbsp;           | R_FCL_CODE_USR.text
 &nbsp;           | R_FCL_CODE_RAM.text
 &nbsp;           | R_FCL_CODE_ROMRAM.text
 &nbsp;           | R_FCL_CODE_RAM_EX_PROT.text
 &nbsp;           | .data
 0xFEBE0000       | FCL_RESERVED.bss
 &nbsp;           | R_FCL_DATA.bss
 &nbsp;           | .data.R
 &nbsp;           | .bss
 &nbsp;           | .stack.bss


## Steps
###### 複製 FCL\ 下 `fcl_` 開頭的檔案及 `target.h` 至專案目錄下

    fcl_cfg.h
    fcl_descriptor.c
    fcl_descriptor.h
    fcl_user.c
    fcl_user.h
    target.h


###### 加入路徑
`CC-RH (Build Tool)` -> `Compile Options` -> `Additional include paths`

    FCL\lib
    FCL

###### 修改 cstart.asm
將 STACKSIZE 改大

    STACKSIZE   .set    0x800

###### 主程式加入以下程式碼

    /***********************************************************************************************************************
    Global variables and functions
    ***********************************************************************************************************************/
    /* Start user code for global. Do not edit comment generated here */
        #include "fcl_cfg.h"
    #include "r_fcl_types.h"
    #include "r_fcl.h"
    #include "target.h"
    #include "fcl_descriptor.h"
    #include "fcl_user.h"
    
    #define FCL_RAM_EXECUTION_AREA_SIZE 0x8000
    #if R_FCL_COMPILER == R_FCL_COMP_GHS
        #pragma ghs startdata
        #pragma ghs section bss = ".FCL_RESERVED"
        #define R_FCL_NOINIT
    #elif R_FCL_COMPILER == R_FCL_COMP_IAR
        #pragma dataseg = R_FCL_Data
        #define R_FCL_NOINIT __no_init
    #elif R_FCL_COMPILER == R_FCL_COMP_REC
        #pragma section r0_disp32 "FCL_RESERVED"
        #define R_FCL_NOINIT
    #endif
    
    uint8_t FCL_Copy_area[FCL_RAM_EXECUTION_AREA_SIZE];
    
    #if R_FCL_COMPILER == R_FCL_COMP_GHS
        #pragma ghs section bss = default
        #pragma ghs enddata
    #elif R_FCL_COMPILER == R_FCL_COMP_IAR
        #pragma dataseg = default
    #elif R_FCL_COMPILER == R_FCL_COMP_REC
        #pragma section default
    #endif
    
    #define FLMD0_PROTECTION_OFF    (0x01u)
    #define FLMD0_PROTECTION_ON     (0x00u)
    #define REINITIALIZE_BUFFER     for (i = 0; i < sizeof (writeBuffer_u08); i++)          \
                                    {                                                       \
                                        writeBuffer_u08[i] = i;                             \
                                    }                                                       \
                                    for (i = 0; i < (sizeof (readBuffer_u32) >> 2); i++)    \
                                    {                                                       \
                                        readBuffer_u32[i]  = 0xaa;                          \
                                    }
    r_fcl_request_t     myRequest;
                                    
    void FCL_Ctrl (void);
    void Error_Handle(void)
    {
        /* Error treatment ... */
        while(1);
    }
    /* End user code. Do not edit comment generated here */



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

###### `FCL_Ctrl()`主程式
    
    /* Start user code for adding. Do not edit comment generated here */
    void FCL_Ctrl (void)
    {
        //r_fcl_request_t     myRequest;
    #if R_FCL_COMMAND_EXECUTION_MODE == R_FCL_HANDLER_CALL_USER
            r_fcl_request_t mySecRequest;
    #endif
        uint8_t             writeBuffer_u08[512];
        uint32_t            readBuffer_u32[8];
        uint16_t            i;

        REINITIALIZE_BUFFER;

        FCLUser_Open ();

        /* prepare environment */
        myRequest.command_enu = R_FCL_CMD_PREPARE_ENV;
        R_FCL_Execute (&myRequest);
        #if R_FCL_COMMAND_EXECUTION_MODE == R_FCL_HANDLER_CALL_USER
            while (R_FCL_BUSY == myRequest.status_enu)
            {
                R_FCL_Handler ();
            }
        #endif
        if (R_FCL_OK != myRequest.status_enu)
        {
            Error_Handle();
        }

        /* get block count */
        myRequest.command_enu   = R_FCL_CMD_GET_BLOCK_CNT;
        myRequest.bufferAdd_u32 = (uint32_t)&readBuffer_u32[0];
        R_FCL_Execute (&myRequest);
        if (R_FCL_OK != myRequest.status_enu)
        {
            Error_Handle();
        }

        /* get block end address of block 2 */
        REINITIALIZE_BUFFER;
        myRequest.command_enu   = R_FCL_CMD_GET_BLOCK_END_ADDR;
        myRequest.bufferAdd_u32 = (uint32_t)&readBuffer_u32[0];
        myRequest.idx_u32       = 0x2;                   /* result: 0x5fff */
        R_FCL_Execute (&myRequest);
        if (R_FCL_OK != myRequest.status_enu)
        {
            Error_Handle();
        }

        myRequest.command_enu   =  R_FCL_CMD_DISABLE_LOCKBITS;
        R_FCL_Execute (&myRequest);
        if (R_FCL_OK != myRequest.status_enu)
        {
            Error_Handle();
        }
        
        /* erase block 2 and 3 */
        myRequest.command_enu = R_FCL_CMD_ERASE;
        myRequest.idx_u32     = 2;                   /* erased range = 0x4000 to 0x7fff */
        myRequest.cnt_u16     = 1;
        R_FCL_Execute (&myRequest);
        #if R_FCL_COMMAND_EXECUTION_MODE == R_FCL_HANDLER_CALL_USER
            while (R_FCL_BUSY == myRequest.status_enu)
            {
                R_FCL_Handler ();
            }
        #endif
        if (R_FCL_OK != myRequest.status_enu)
        {
            Error_Handle();
        }

        /* write 512 bytes to address 0x4000 (start of block 2) */
        REINITIALIZE_BUFFER;
        myRequest.command_enu = R_FCL_CMD_WRITE;
        myRequest.bufferAdd_u32 = (uint32_t)&writeBuffer_u08[0];
        myRequest.idx_u32       = 0x4000;
        myRequest.cnt_u16       = 1;                    /* written bytes = 256 * cnt_u16 */
        R_FCL_Execute (&myRequest);
        #if R_FCL_COMMAND_EXECUTION_MODE == R_FCL_HANDLER_CALL_USER
            while (R_FCL_BUSY == myRequest.status_enu)
            {
                R_FCL_Handler ();
            }
        #endif
        if (R_FCL_OK != myRequest.status_enu)
        {
            Error_Handle();
        }
        
        FCLUser_Close ();
    } 
    /* End user code. Do not edit comment generated here */
