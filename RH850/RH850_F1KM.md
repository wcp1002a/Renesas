RH850 SYSERR
==========
## Personal Note
* 手冊的中斷向量表
    - Table 7BC.27  Selection of Base Register/Offset Address
    - 模組的中斷向量表在各章節中
        + TAU: Table 31.14 (HWM Page#2790)
        + UART: Table 22.15 (HWM Page#1700)

## SYSERR
* HWM 3BC.2.1.2  Register Set 
    -  FEIC — FE Level Exception Source Register (HWM Page#625)
    -  
* HWM 3BC.2.3.3  System Error Generator Function (SEG) 
    - 控制SEG (SysErrGen) 控制數據訪問發生系統錯誤後的中斷通知和記錄。
    - 多個錯誤發生輸入根據錯誤因素進行分類，並從最高優先級的錯誤因素開始依次處理，生成 FE 級異常 (SYSERR)
    - SEGCONT: 開關不同的錯誤
    - SEGFLAG: 錯誤的 Flag
    - SEGADDR: 發生錯誤的位址

* HWM 7BC.7.1  Exception Source Codes for Different Types of SYSERR Exceptions 
    - 列出了不同類型的 SYSERR 異常的異常源代碼。



