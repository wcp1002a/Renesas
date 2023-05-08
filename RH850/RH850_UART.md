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
    - `LINFREQ` - LIN coomunication clock source frequency (CLKSCL_ILIN)
    - `LPRS_SELCLK` - `RLN3nLWBR.LPRS[2:0]` selected clock
    - `BRP01_SETTING` - (`RLN3nLBRP01` + 1)
    - `NSPB_SEL_CNT` - `RLN3nLWBR.NSPB[3:0]` selected count

This sentence uses `$` delimiters to show math inline:  $\sqrt{3x-1}+(1+x)^2$

$\Gamma(z) = \int_0^\infty t^{z-1}e^{-t}dt\,.$