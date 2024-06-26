RH850 CPU
==========
## CPU 運作模式文件
* Table 3.8 Basic System Registers (#275)
    - 表格最後一欄 Access Permission 標示出可存取的模式
* 以下敍述節錄於 HWM 3.2.2.1  CPU Operating Modes (#240)

## CPU Operating Mode
CPU 有兩種操作模式：超級用戶模式 (SV) 和用戶模式 (UM)。
系統是處於管理員模式還是用戶模式由 PSW 寄存器中的 UM 位元指示。

* 主管模式 (SV) PSW.UM = 0
    - 所有硬件功能都可以在此模式下管理或使用。
    - 系統在復位後總是以超級用戶模式啟動。
* 用戶模式 (UM) PSW.UM = 1
    - 此操作模式與管理員模式組成一對。
    - 在用戶模式下，僅有使用超級用戶模式允許存取的位址空間，和系統定義為使用者資源的暫存器才可以被訪問。
    - 管理員特權指令無法被執行並且會觸發例外程式。

## CPU 操作模式的轉換

* 因觸發中斷的變化：
    - 在用戶模式下確認異常時，CPU 運行模式更改為特權模式。
    - 當在管理員模式下確認異常時，CPU 操作模式保持原樣。
* 因 Return 指令而發生的變更：
    - 執行返回指令時，恢復 PSW 值根據備份到 EIPSW 的相應位的值或 FEPSW。
* 因系統暫存器指令引起的變化：
    - 使用 LDSR 指令時 CPU 工作模式發生變化直接覆蓋 PSW 操作模式位。

  
RH850 SYSERR
==============
## Exception Level
* SW user manual (e.g. REN_r01us0165ej0120-rh850g3kh_MAS_20171026.pdf)
* Exception的成因通成分為兩個層級 EI level 跟 FE level
* EI 一般是用於處理一般使用者的流程、中斷服務、OS 處理
* FE 用處理緊急的系統例外，或是記憶體管理 (可以中斷 EI 等級的 exception)
* FE級中斷通常用於無法保證完整持續的系統的錯誤。在這種情況下，建議的操作是 RESET。
## SYSERR 相關章節
* 3BC.2.3.3  System Error Generator Function (SEG) 
    - SEGCONT: 控制暫存器，決定哪些錯誤要反應到 SysErr
    - SEGFLAG: 旗標，儲存哪些錯誤發生了
    - SEGADDR: 位址，儲存了錯誤發生的位址
* 7BC.7  Operation when Acknowledging an Interrupt 
    - 中斷觸發流程
* 7BC.7.1  Exception Source Codes for Different Types of SYSERR Exceptions
    - 針對不同的錯誤發生源(SEGFLAG暫存器)的例外碼
* Table 7BC.27  Selection of Base Register/Offset Address 
    - 中斷向量位址 010h
* 7BC.10 Exception Handler Address
    - 中斷處理位址

## Interrupt Reference Pages
* (HWM #873) 中斷表 Table 7BC.27  Selection of Base Register/Offset Address 
* (HWM #828) 中斷向量表 Table 7BC.6  EI Level Maskable Interrupt Sources (RH850/F1KM-S1) 
* (HWM #553) PSW
* (HWM #874) Table Refernce Method
## Ritwin case
* Register values at SYSERR
    - SEGCONT: 0x0000
    - SEGFLAG: 0x0010
    - SEGADDR: 0x98B31E5C
    - FEPC: 0x00001FFE
    - FEPSW: 0x00010022
    - FEIC: 0x00000011

* SEGCONT 設為 0x0000，代表沒有啟用額外的錯誤，
* SEGFLAG 代表 VCIF 旗標被確認
* 錯誤位址為 0x98B31E5C
* 出錯的程式位址為 1FFE
* FEPSW: EBV, SAT, Z
* FEIC: 0x0011 中斷

----------------------------------------
RH850 MPU
==========
[RH850 MPU](RH850_MPU.md)
