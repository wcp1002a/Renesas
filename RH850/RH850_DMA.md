RH850 DMA
==========

## Reference Documents
* HWM Section 8 DMA Controller
### Overview
* DMA 存取空間為 4GB (32-bit address)
* PEID
    - 001B: CPU1 (PE1)
    - 010B: CPU1 (PE2)
* SPID
    - 0: H-BUS
    - 1: CPU1
    - 2: CPU2
    - 3: external master?
* Caution (Need to check later)
    -  PEGSP
    -  PEGGnMK
    -  PEGGnBA
    -  PDMAnDMyiCM: 0x14 (ok)
    -  
* 選擇硬體觸發 8.9.2.12  PDMAnDTFRm — DTFR Setting Register 
    - REQSEL[6:0] (DTFR[7:1])：用來選擇來源
    - 可由 8.1.4  DMA Trigger Factors 表格 8.13 來決定
    - 以 TAUB0REQSEL0 為例，DMACTRG[33]，因此 REQSEL[6:0] = 0x21
    - PDMA0DTFR0 = 0x43 (REQSEL[6:0]=0x21, REQNEN = 0x1)
    - 而TAUB0REQSEL0 的來源可以 Table 8.55  DMA Trigger Factor Selection of TAUD0REQSELj/TAUB0REQSELj (RH850/F1KM-S1) 選擇
* 傳輸模式 (`PDMAnDTCTm.TRM[1:0]`)
* Single Transfer: 單次傳輸，每觸發一次只會傳送一筆
* Block transfer 1: 每觸發一次，會傳指定數量的資料 (`PDMA0.DTC0`)。
* Block transfer 2: 每觸發一次，會傳指定位址更新數量的資料 (`PDMAnDTCm.ARC[15:0]`)。

### ● Debug
* 若來源位址在RAM (例如 0xFEDE800C)，則由暫存器PDMA0DCST0會發現Read Error發生 (ERWR=0)
* 若來源位址改在ROM (例如 0x00000000)，則由暫存器PDMA0DCST0會發現Write Error發生 (ERWR=1)

主要原因為沒有開啟 PE Buard Function
### PEG 設定
* `PEGMK = 0x1F000`: Open up 128kB (96KB-LRAM + 32KB-RRAM)
* `PEGG0BA  = 0xFEBE0027`: Base address 0xFEBE0000, SPID1, W, R, Enable
* `PEGSP = 0x1`: Eanble PEG buard

以下範例程式碼可以允許 RH850/F1KM-S1 1M 版本的 MCU 的所有 12KB RAM (96 LRAM + 32 RRAM)被讀寫

```c
    // PEG settings
    PEG.G0MK.UINT32 = 0x1F000;      /* Open up 128kB (96KB-LRAM + 32KB-RRAM) */
    PEG.G0BA.UINT32 = (0xFEBE0000ul)    |   /* Base address of PE Guard protection Area (start of Local RAM) */
                    (0x1U << 5U)        |   /* Enable Access for SPID 1 */
                    (0x1U<<2U)          |   /* Enable Write Access */
                    (0x1U<<1U)          |   /* Enable Read Access */
                    (0x1U<<0U);             /* Enable Access Condition Settings */
    PEG.SP.UINT32 = 0x00000001;     /* Enable PEG guard */
```

