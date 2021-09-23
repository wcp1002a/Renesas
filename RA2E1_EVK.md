# RA2E1 EVK
Board: EK-RA2E1
LSI: R7FA2E1A92DFM

## Pin Usage
Hardware Connection Layout  

Function    | Signal Name| Pin Name  | RA2E1 EVK
------------|------------|-----------|-----------
I2C         | SCL0       | P407      | J2.9
I2C         | SDA0       | P408      | J2.8
UART        | TXD0       | P205      | J2.16
UART        | RXD0       | P206      | J2.15
LED Control | P915       | P915      | LED1
Push Button | P004       | P004      | S1
Ground      | GND        | GND       | J2.40
Power       | 3.3V       | 3.3V      | J2.1

EN Control  | P17        | P17       | CN2.17

#### UART
For message display and debug console
 
#### I2C
To Control LCD1602 and ToF module

#### GPIO
Input: Push button for flow control  
Output: LED control

## Sample Code
#### GPIO
``` C
    // GPIO control example
    R_BSP_PinAccessEnable();
    R_BSP_PinWrite(BSP_IO_PORT_09_PIN_13, BSP_IO_LEVEL_LOW);
    R_BSP_PinWrite(BSP_IO_PORT_09_PIN_13, BSP_IO_LEVEL_HIGH);
    R_BSP_PinWrite(BSP_IO_PORT_09_PIN_14, BSP_IO_LEVEL_LOW);
    R_BSP_PinWrite(BSP_IO_PORT_09_PIN_14, BSP_IO_LEVEL_HIGH);
    R_BSP_PinWrite(BSP_IO_PORT_09_PIN_15, BSP_IO_LEVEL_LOW);
    R_BSP_PinWrite(BSP_IO_PORT_09_PIN_15, BSP_IO_LEVEL_HIGH);
    R_BSP_PinAccessDisable();
```

``` C
    sw1 = R_BSP_PinRead(BSP_IO_PORT_00_PIN_04);
    if(sw1 == 1){
        printf("button up\r\n");
    }else{
        printf("button down\r\n");
    }
```

#### AGT
``` C
    fsp_err_t err = FSP_SUCCESS;
    volatile uint32_t tmp;

    /* Initializes the module. */
    err = R_AGT_Open(&g_timer0_ctrl, &g_timer0_cfg);
    /* Handle any errors. This function should be defined by the user. */
    assert(FSP_SUCCESS == err);
    /* Start the timer. */
    (void) R_AGT_Start(&g_timer0_ctrl);

```

#### UART
``` C
#define TRANSFER_LENGTH 40
uint8_t  g_src[TRANSFER_LENGTH];
uint8_t  g_out_of_band_received[TRANSFER_LENGTH];
volatile uint32_t g_transfer_complete = 0;
uint32_t g_receive_complete  = 0;
uint32_t g_out_of_band_index = 0;

void r_sci_uart_basic_example (void)
{
    /* Initialize p_src to known data */
    for (uint32_t i = 0; i < TRANSFER_LENGTH; i++)
    {
        g_src[i] = (uint8_t) ('A' + (i % 26));
    }
    /* Open the transfer instance with initial configuration. */
    fsp_err_t err = R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
    assert(FSP_SUCCESS == err);
    // err = R_SCI_UART_Read(&g_uart0_ctrl, g_dest, TRANSFER_LENGTH);
    // assert(FSP_SUCCESS == err);
    err = R_SCI_UART_Write(&g_uart0_ctrl, g_src, TRANSFER_LENGTH);
    assert(FSP_SUCCESS == err);
    while (!g_transfer_complete)
    {
    }
    // while (!g_receive_complete)
    // {
    // }
}
```
