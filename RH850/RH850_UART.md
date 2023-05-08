RH850 UART
==========

#### Hardware Connection Layout

* Board: Y-ASK-RH850F1KM-S4-V3

Function    | Signal Name| Pin Name  | F1KM/S1 EVK
------------|------------|-----------|-----------
UART/TX     | RLIN33TX   | P11_5     | J7.15
UART/RX     | RLIN33RX   | P11_6     | J7.16
GPIO        | P0_5       | P0_5      | J3.11
Ground      | GND        | GND       | J7.21


#### Baud Rate Calculation
* Reference HWM Section 22.10.3  UART Mode
* UART baud rate = `LINFREQ` x `LPRS_SELCLK` / `BRP01_SETTING` / `NSPB_SEL_CNT`
    - `LINFREQ`: LIN coomunication clock source frequency (CLKSCL_ILIN)
    - `LPRS_SELCLK`: `RLN3nLWBR.LPRS[2:0]` selected clock
    - `BRP01_SETTING`: (`RLN3nLBRP01` + 1)
    - `NSPB_SEL_CNT`: `RLN3nLWBR.NSPB[3:0]` selected count


#### 傳輸速度要達到 1Mbps，需要修改 Clock 設定
* 使用 Smart Configurator 原始設定

Source Code (312500 bps)

    RLN33.LWBR = _UART_8_SAMPLING | _UART_PRESCALER_CLOCK_SELECT_16;
    RLN33.LBRP01.UINT16 = _UART3_BAUD_RATE_PRESCALER;

    Baud = 40MHz / 16 / (0+1) / 8 = 312,500

Modified Code (1M bps)

    RLN33.LWBR = _UART_9_SAMPLING | _UART_PRESCALER_CLOCK_SELECT_4;
    RLN33.LBRP01.UINT16 = _UART3_BAUD_RATE_PRESCALER;

    Baud = 40MHz / 4 / (0+1) / 10 = 312,500
![RH850_UART_1_RLIN_CLK](attach/RH850_UART_1_RLIN_CLK.png)
![RH850_UART_2_CPU_CLK](attach/RH850_UART_2_CPU_CLK.png)
![RH850_UART_3_Waveform](attach/RH850_UART_3_Waveform.jpg)
