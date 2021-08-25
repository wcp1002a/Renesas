# RX140 EVK
## Pin design
Hardware Connection Layout  

Function    |Signal Name | Pin Name  | RX140 EVK
------------|------------|-----------|-----------
UART        | TXD5       | PC3       | CN2.31
UART        | RXD5       | PC2       | CN2.32
I2C         | SSCL1      | P15       | CN2.19
I2C         | SSDA1      | P16       | CN2.18
GPIO        | Input      | P30       | SW1
GPIO        | Output     | P31       | LED0

### UART
* TXD5  PC3     CN2.31
* RXD5  PC2     CN2.32
 
### I2C
* SSCL1 P15     CN2.19
* SSDA1 P16     CN2.18

### GPIO
* Input     P30     SW1
* Output    P31     LED0


### Debug console
Modify lowlvl.c

    #if 1   /* PRINTF */    // paul20210824
    #define BSP_CFG_USER_CHARGET_ENABLED    (1)
    #define BSP_CFG_USER_CHARGET_FUNCTION     my_sw_charget_function

    #define BSP_CFG_USER_CHARPUT_ENABLED    (1)
    #define BSP_CFG_USER_CHARPUT_FUNCTION     my_sw_charput_function
    #endif  /* PRINTF */

Modify R_Config_SCI5_Start() of Config_SCI5.c

    /* Enable SCI interrupt */
    IEN(SCI5, TXI5) = 0U;   // paul20210824
    // IEN(SCI5, TXI5) = 1U;   // paul20210824

Add below setting in main.c

    R_Config_SCI5_Start();  // Start UART

    // paul20210824
    PORTC.PMR.BYTE |= 0x08U;
    SCI5.SCR.BIT.TE = 1U;
    SCI5.SCR.BIT.TIE = 1U;
    IR(SCI5, TXI5) = 1;


    printf("Renesas RX140 printf\r\n");

Add macro definition
Properties -> C/C++ Build -> Settings -> Tool Settings -> Compiler -> Source -> Macro definition
Add `DEBUG_CONSOLE`


