20240610_Retronix_RH850_F1KM-S4_CAN_BUS_Issue
=============================================

2024/08/16
----------------------------------------
* 在 EVB 上使用 RCFDC0.CFDC3CTR 及 RCFDC0.CFDC3STS 驗證沒問題
* 使用 Jimmy 的專案，會有一些問題
    * int32_t 及 uint32_t 重覆定義問題
    > 原因為 r_cg_microdriver.h 及 stdint.h 都定義了
    > 解決方案：在 public.h 中 remark stdint.h `// #include <stdint.h>`
* 實際在運行時
    * 原本的流程是在 parser() 內接收特定資料後觸發 R_RSCAN0FD_loopback_Init7();
    * 改為進入 parser() 後直接觸發 R_RSCAN0FD_loopback_Init7();
    * 程式如果用 Step 的方式進行，會在 set channel reset mode 後卡在 STS 檢查迴圈，若在之後設斷點使用 Go 則不會
    * 如 Jimmy 回報一樣，在切到 self-test mode 後再切到 Channel communication mode，無法成功，卡在 STS 檢查
* 問題
    * 沒有對 Channel 初始化，加上以下程式碼即可
```c
    RCFDC0.CFDC7NCFG.UINT32 = 0x133a9800; //NTS2=20; NTS1=59; NSJW=20; NBRP=0 // paul20240816
```
----------------------------------------
2024/08/15
----------------------------------------
WT/Jimmy
```
想請教您 ,客戶用RH850 F1KM-S4 (R7F7016493AFP)
CANFD一般通訊下沒問題。但在切換成Self-test mode 時一直切不過去。
想問一下,是否有該系列或其他系列的範例,可以參考或要注意的地方,謝謝。

目前Self test 是參考 RH850/F1KH  User’s Manual: Hardware  裡面的流程
Figure 24.35, Self-Test Mode Setting Procedure 


切換成 halt mode 已經沒問題( 藍1)。目前卡在藍色2的地方。
主要是設定完CTME  CTMS
RCFDC0.CFDC7CTR.UINT32 |=0x0500002;
RCFDC0.CFDC7CTR.UINT32 &=0xfffffffc;

其RCFDC0.CFDC7STS  我在EVM上看到是0x00000002
預期要變成0; 才會過while 迴圈。
```

Environment Setup
----------------------------------------
* Board: `Y-ASK-RH850F1KM-S4-V3`
    * Debug Console Setting
        * Jumper Setting: Remove J18 jumpers
        * USB2UART TX: J18.1 (UARTRX printed on PCB)
        * USB2UART RX: J18.3 (UARTTX printed on PCB)
        * Baud Rate: 9600
        * Output example
```
*****************************************
*                RENESAS                *
*      RH850/F1x Starter Kit Demo!      *
*****************************************

Begin with self test...
CAN FD test...Okay
LIN test...Error!
Please check if all jumpers of
J17 are closed and switches 3-6 of S10 are off.

Ethernet loopback test...Error!
Please check if all switches of ETH1-ETH3 are on.

No display detected.
RGB LED test...failed

>>Enter Mode 1
>RGB LED error: green LED
>RGB LED error: blue LED
>>30s without action - enter DeepSTOP

```

    * CANFD Setting
        * J7: Short `1-2`, `3-4`, 5-6, 7-8
        * Switch S4: S4.1 ON, S4.2 ON (Enable on board termination resistor)
        * Switch S4: S4.3 ON, S4.4 ON (Connect on board CAN BUS)

* PCAN-View
    - CAN Setup 要照以下設定才可運作 (與F1KM-S4 程式設定相同)
        + `Mode`: `ISO CAN FD`
        + `Bit Rate Preset`: `None`
        + `Nominal Bit Rate`
            * `Database Entry`: `None`
            * `Bit Rage [kbit/s]`: `500`
            * `Saple Point [%]`: `75` (80, 59, 20, 2)
        + `Data bit Rate`
            * `Database Entry`: `N`
            * `Bit Rage [kbit/s]`: `5000`
            * `Saple Point [%]`: `75.0` (8, 5, 2, 2)
    - `Options`: check `Listen-only mode`

* Software: `Y-ASK-RH850F1KM-S4-V3_sample_V101`


Reserved
----------------------------------------
* 2024/08/16 10:00 20240816_1000_bk
r_canfd.c
```c
r_Error_t R_CANFD_Test(void)

{
    uint32_t i, data_number;
       
#if 1   /* CAN-FD, paul20240815 */
    R_RSCAN0FD_C3TrmByTxBuf(0,&CANFD_TraStandData); //CANFD sending by CH 3

    for(i=0; i<100000; i++)
    {
       __nop();
    }

    // Bit1: Channel Halt Status Flag 
    PrintText("\n\r");
    PrintText("CAN-FD self-mode test:\n\r");

    ctr = RCFDC0CFDC3CTR;
    sprintf(tx_buf, "    RCFDC0CFDC3CTR: %08X\n\r", ctr);
    PrintText_delay(tx_buf, 10);

    sts = RCFDC0CFDC3STS;
    sprintf(tx_buf, "    RCFDC0CFDC3STS: %08X\n\r", sts);
    PrintText_delay(tx_buf, 10);

    //+-----------------------+
    //| Set Channel halt mode |
    //+-----------------------+
    PrintText("Set Channel halt mode:\n\r");
    // CTR[2]       CSLPR = 1
    // CTR[1:0]     CHMDC[1:0]  = 10B
    ctr = RCFDC0CFDC3CTR & 0xfffffffc;
    ctr |= 0x00000002;
    RCFDC0CFDC3CTR = ctr ;    //Switch to Channel halt mode

    // wait CHLTSTS = 1 (STS[1])
    sts = RCFDC0CFDC3STS;
    while((sts & 0x02) != 0x02 ){
        sprintf(tx_buf, "    RCFDC0CFDC3STS: %08X\n\r", sts);
        PrintText_delay(tx_buf, 10);
        sts = RCFDC0CFDC3STS;
    }

    //+--------------------+
    //| Set Self-test mode |
    //+--------------------+
    PrintText_delay("Set Self-test mode:\n\r", 10);

    // Communication test mode is enabled.

    // Switch to Channel halt mode
    // CTR[24]      CTME = 1
    ctr = (RCFDC0CFDC3CTR & 0xfeffffff);
    ctr |= 0x01000000;
    RCFDC0CFDC3CTR = ctr;
    // Self-test mode 0
    // Set CTMS[1:0] = 10B (CTR[26:25])
    ctr = (RCFDC0CFDC3CTR & 0xf9ffffff);
    ctr |= 0x04000000;
    RCFDC0CFDC3CTR = ctr;

    // ctr = (RCFDC0CFDC3CTR & 0xf8ffffff);
    // ctr |= 0x01000000;          // Communication test mode is enabled.
    // // RCFDC0CFDC3CTR = ctr;
    // // Self-test mode 0 (10 B )
    // ctr |= 0x04000000;          Self-test mode 0 (10 B )
    // RCFDC0CFDC3CTR = ctr;

    //+----------------------------+
    //| Channel communication mode |
    //+----------------------------+
    ctr &= 0xfffffffc;
    RCFDC0CFDC3CTR = ctr;    //Switch to Channel halt mode

    // Check STS:
    // STS[2]: CSLPSTS = 0
    // STS[1]: CHLTSTS = 0
    // STS[0]: CRSTSTS = 0
    sts = RCFDC0CFDC3STS;
    while((sts & 0x07) != 0x00 ){
        sprintf(tx_buf, "    RCFDC0CFDC3STS: %08X\n\r", sts);
        PrintText_delay(tx_buf, 10);
        sts = RCFDC0CFDC3STS;
    }
    sprintf(tx_buf, "    RCFDC0CFDC3STS: %08X\n\r", sts);
    PrintText_delay(tx_buf, 10);

    while(1){
        ;
    }


    sprintf(tx_buf, "    RCFDC0CFDC3CTR: %08X\n\r", sts);
    PrintText_delay(tx_buf, 10);

    for(i=0; i<1000000; i++)
    {
       __nop();
    }
#endif  /* CAN-FD */

    R_RSCAN0FD_C3TrmByTxBuf(0,&CANFD_TraStandData); //CANFD sending by CH 3

    for(i=0; i<100000; i++)
    {
       __nop();
    }
    ; //delay for CANFD receiving

    R_RSCANFD_ReadRxFIFOBuffer(&CANFDRxFIFOData); //CANFD CH4 receiving

    for(data_number=0; data_number<64; data_number++)
    {
    if(CANFDRxFIFOData.DB[data_number] != CANFD_TraStandData.DB[data_number])
    {
        return R_ERROR;
    }
    }
    return R_OK;
 
}

```
```
*****************************************
*                RENESAS                *
*      RH850/F1x Starter Kit Demo!      *
*****************************************

Begin with self test...
CAN FD test...
CAN-FD self-mode test:
    RCFDC0CFDC3CTR: 00000000
    RCFDC0CFDC3STS: 00000080
Set Channel halt mode:
    RCFDC0CFDC3STS: 00000080
Set Self-test mode:
    RCFDC0CFDC3STS: 00000002
    RCFDC0CFDC3STS: 00000080
```

#### 2024/08/15 code backup (Paul code)
```c
#if 0   /* CAN-FD, paul20240815 */
uint8_t tx_buf[100];
uint32_t sts;
#endif  /* CAN-FD */
/*****************************************************************************
** Function:    BoardCheck
** Description: This function checks the LED, CAN, and LIN functionality and 
**              prints the welcome screen and status via UART.
** Parameter:   None
** Return:      None
******************************************************************************/
void BoardCheck(void)
{
    uint32_t    i, 
                j = 0;
        
    R_CANFD_Init();
    R_LIN_Init();
    
    /* Send Welcome Message via UART */
    PrintText("\r*****************************************\n\r");
    PrintText("*                RENESAS                *\n\r");
    PrintText("*      RH850/F1x Starter Kit Demo!      *\n\r");
    PrintText("*****************************************\n\r");
    PrintText("\n\rBegin with self test...\n\r");
    
#if 0   /* CAN-FD, paul20240815 */
    R_RSCAN0FD_C3TrmByTxBuf(0,&CANFD_TraStandData); //CANFD sending by CH 3

    for(i=0; i<100000; i++)
    {
       __nop();
    }

    // Bit1: Channel Halt Status Flag 
    // sts = RCFDC0CFDC3STS;
    // sprintf(tx_buf, "%x", sts);

    // RCFDC0CFDC3CTR &= 0xfffffffb;    //Switch to Channel halt mode
    // RCFDC0CFDC3CTR &= 0xfffffffc;    //Switch to communication mode
#endif  /* CAN-FD */
```

#### 2024/08/16 CAN_FAE_test (fixed)
```c
void R_RSCAN0FD_loopback_Init7(void)
{
    /* Wait while CAN RAM initialization is ongoing */
    while((RCFDC0.CFDGSTS.UINT32 & 0x00000008)) ;//until GRAMINIT=0
   /* switch mode to CANFD mode,reset the main registers */
//    RCFDC0.CFDGCTR.UINT32 = 0x00000005;   //P2062:Global stop mode, Global Reset Mode

   /* Switch to global/channel reset mode */   

    RCFDC0.CFDGCTR.UINT32 &= 0xfffffffb;    //set the 3rd bit to 0 -- clear CSLPR to 0  from STOP MODE to RESET mode
    RCFDC0.CFDGCTR.UINT32 |= 0x00000001u;  //GMDC[1:0]BIT = 01B  ---ensure from global operation mode to reset mode
    while(RCFDC0.CFDGSTS.UINT8[LL]!=0x01); //GRSTSTS Global Reset Status Flag; in global reset mode
    RCFDC0.CFDGCTR.UINT32 &= 0xfffffff8;    //set the 3rd bit to 0 -- clear CSLPR to 0  from STOP MODE to RESET mode
    RCFDC0.CFDGCTR.UINT32 |= 0x00000002u;     
    while(RCFDC0.CFDGSTS.UINT8[LL]!=0x02);  
//----
    RCFDC0.CFDGCTR.UINT32 = 0x00; // Global operating mode
    while(RCFDC0.CFDGSTS.UINT8[LL]!=0x00);  
 //RCFDCnCFDCmCTR ¡X Channel Control Register (m = 0 to 7)   

    RCFDC0.CFDC7CTR.UINT32 &= 0xfffffffb;
    RCFDC0.CFDC7CTR.UINT32 &= 0xf8fffff8;   //CSLPR=0; Other than channel stop mode
    RCFDC0.CFDC7CTR.UINT32 |= 0x00000001u;  //set channel reset mode
    while(1){
        if((RCFDC0.CFDC7STS.UINT8[LL] & 0x01)==0x01)//Waiting the reset mode 
            break;
    }    
    RCFDC0.CFDC7NCFG.UINT32 = 0x133a9800; //NTS2=20; NTS1=59; NSJW=20; NBRP=0 // paul20240816

    RCFDC0.CFDC7CTR.UINT32 &= 0xf8fffff8;
 
    RCFDC0.CFDC7CTR.UINT32 = 0x00000002;    //set channel halt mode


    while(1){
        if((RCFDC0.CFDC7STS.UINT8[LL] & 0x02)==0x02)//Waiting the halt mode 
            break;
    }
    RCFDC0.CFDC7CTR.UINT32 |= 0x05000000;   //CTME=1: communication test mode is enable; external self loopback mode
                        //set Self-test mode 0 (EXternal loopback mode)
    RCFDC0.CFDC7CTR.UINT32 &= 0xfffffffc;   //Set CHMDC[1:0]=00; communication mode
#if 1    
    while(1){
     if((RCFDC0.CFDC7STS.UINT8[LL] & 0x07)==0x00)//Waiting CSLPSTS,CHLTSTS, CRSTSTS all 0
        break;
    }
#else
    R_SYSTEM_usDelay(5000);
#endif    
 

   /*ID priority,DLC check is disabled,clkc as CAN clock--PPLLCLK, overflow stored, */
    //RCFDCnCFDGCFG ¡X Global Configuration Register
    ///P2058:
    RCFDC0.CFDGCFG.UINT32 = 0x00001020;   //0B 0001 0000 0010 0000
    //TSSS=1:nominal bit time clock 
   //clkc=PPLLCLK=40MHz
    /*Bitrate = fCAN/(BRP+1)/(1+TSEG1+TSEG2) = 40M/1/80 = 0.5Mbps(500Kbps) */
    RCFDC0.CFDC7NCFG.UINT32 = 0x133a9800; //NTSEG2=20; NTSEG1=59; NSJW=20; NBRP=0  
//data Bitrate = fCAN/(BRP+1)/(1+TSEG1+TSEG2) = 40M/4/10 = 1Mbps
    RCFDC0.CFDC7DCFG.UINT32 = 0x01260003;            //DSJW=2; DTS2=8; DTS1=16; DBRP=3  

    R_RSCAN7_SetRxRule();
/* ==== buffer setting ==== */ 
    RCFDC0.CFDRMNB.UINT16[L] = 0x0310; //P2089:20 bytes data size, 16 receive buffers

    /*Receive FIFO buffer setting*/
    RCFDC0.CFDRFCC0.UINT32=0x0000F770; //8 buffer depth, 64 data length, FIFO 0 interrupt is disabled, FIFO enabled only in operation mode
    
    /* Set CMPOFIE,THLEIE,MEIE,DEIE disabled*/
    RCFDC0.CFDGCTR.UINT32 &= 0xfffff0ff;

    /* If GlobalChannel in halt or reset mode */
    if (RCFDC0.CFDGSTS.UINT8[LL] & 0x03) 
    {
        RCFDC0.CFDGCTR.UINT32 &= 0xfffffffc; //Switch to communication mode
        while ((RCFDC0.CFDGSTS.UINT8[LL] & 0x02) == 2) {
            /* While halt mode */
        }
        while ((RCFDC0.CFDGSTS.UINT8[LL] & 0x01) == 1) {
            /* While reset mode */
        }
    }
    //----CANFD only
    RCFDC0.CFDC7FDCFG.UINT32 &= ~0x40000000; //CLOE=0
    RCFDC0.CFDC7FDCFG.UINT32 |= 0x10000000; //FDOE=1
    RCFDC0.CFDC7FDCFG.UINT32 |= 0x00050200; //TDC enable, 5 offset with measurement
 
    /* If Channel7 in halt or reset mode */
    if (RCFDC0.CFDC7STS.UINT8[LL] & 0x03) 
    {
        RCFDC0.CFDC7CTR.UINT32 &= 0xfffffffc;    //Switch to communication mode
        while ((RCFDC0.CFDC7STS.UINT8[LL] & 0x02) == 2) {
            /* While halt mode */
        }
        while ((RCFDC0.CFDC7STS.UINT8[LL] & 0x01) == 1) {
            /* While reset mode */
        }
    }

    RCFDC0.CFDRFCC0.UINT8[LL] |= 0x01; //set RxFIFO0 to operation mode      
}
```