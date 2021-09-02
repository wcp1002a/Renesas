# RX140 EVK
LSI: R5F51403AxFM

## Pin Usage
Hardware Connection Layout  

Function    | Signal Name| Pin Name  | RX140 EVK
------------|------------|-----------|-----------
UART        | TXD5       | PC3       | CN2.31
UART        | RXD5       | PC2       | CN2.32
I2C         | SSCL1      | P15       | CN2.19
I2C         | SSDA1      | P16       | CN2.18
Push Button | P30        | P30       | SW1
LED Control | P31        | P31       | LED0
EN Control  | P17        | P17       | CN2.17
Power       | TGVCC      | MCU_VCC   | CN2.10
Ground      | GND        | GND       | CN2.8

#### UART
For message display and debug console
 
#### I2C
To Control LCD1602 and ToF module

#### GPIO
Input: Push button for flow control  
Output: LED control

### Debug console
To Enable `printf` function in RX140 project  
Modify **r_bsp_config.h**  

    \src\smc_gen\r_config\r_bsp_config.h

Redirect charget/charput to user function

```c
#define BSP_CFG_USER_CHARGET_ENABLED    (1)  // paul20210825
#define BSP_CFG_USER_CHARGET_FUNCTION     my_sw_charget_function

#define BSP_CFG_USER_CHARPUT_ENABLED    (1)  // paul20210825
#define BSP_CFG_USER_CHARPUT_FUNCTION     my_sw_charput_function
```

Modify __Config_SCI5.c__  
Disable SCI interrupt in R_Config_SCI5_Start()
```c
    /* Enable SCI interrupt */
    IEN(SCI5, TXI5) = 0U;   // paul20210824
    // IEN(SCI5, TXI5) = 1U;   // paul20210824
```

Modify __main.c__  
Enable SCI module in main()

```c
    R_Config_SCI5_Start();  // Start UART

    // paul20210824
    PORTC.PMR.BYTE |= 0x08U;    // Enable PC3 (TXD)
    SCI5.SCR.BIT.TE = 1U;       // TX enable
    SCI5.SCR.BIT.TIE = 1U;      // TX interrupt enable
    IR(SCI5, TXI5) = 1;         // Interrput flag
```

    printf("Renesas RX140 printf\r\n");

Add __serial_printf.c__
```c
/******************************************************************************
* Function Name: my_sw_charput_function
* Description  : Outputs a character on a serial port
* Arguments    : character to output
* Return Value : none
******************************************************************************/
void my_sw_charput_function(char output_char)
{   
    //  IR(SCI5, TXI5) = 1;
    /* Wait for transmit buffer to be empty */
    while(IR(SCI5, TXI5) == 0);

    /* Clear TXI IR bit */
    IR(SCI5, TXI5) = 0;

    /* Write the character out */ 
    SCI5.TDR = output_char;
}
/******************************************************************************
End of function  my_sw_charput_function
******************************************************************************/

/******************************************************************************
* Function Name: my_sw_charget_function
* Description  : Gets a character on a serial port
* Arguments    : character to output
* Return Value : none
******************************************************************************/
char my_sw_charget_function(void)
{
    uint8_t temp;
    
    /* Read any 'junk' out of receive buffer */
    temp = SCI5.RDR;
    
    /* Clear flag to receive next byte */
    IR(SCI5, RXI5) = 0;
    
    /* Wait for next receive data */
    while(IR(SCI5, RXI5) == 0);
    
    /* Read data */
    temp = SCI5.RDR;
    
    /* Echo data back out */
    if(temp == 0x0d)
    {
        /* Enter was pressed, output newline */
        charput('\r');
        charput('\n');
    }
    else
    {
        /* Echo back character */
        charput(temp);
    }
    
    /* Receive data acquired, send back up */
    return temp;        
}
```

Add macro definition
Properties -> C/C++ Build -> Settings -> Tool Settings -> Compiler -> Source -> Macro definition
Add `DEBUG_CONSOLE`


