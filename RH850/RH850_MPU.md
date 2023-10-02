RH850 MPU
==========
## Overview
### Reference Documents
* HWM 3.2.3.6  MPU Function Registers
* 0_Delta\General\30_TrainingMaterial\E2x_CPU.pdf (From Page#50)
##### MDP related information
* HWM Table 3.95  Exception Cause List (Page#348)
* PMAT (Page#325)
* HWM Table 3.107  Exception Cause Code of Memory Protection Violation (Page#389)

##### MPU Registers
HWM 3.2.3.6 MPU Function Registers 中可找到 MPU 相關暫存器說明。
3.2.2.5  Registers (5)
(5)  Supervisor Lock Setting 
If the SVLOCK.SVL bit is set (1), the following system registers cannot be updated even in supervisor mode: 
SPID, MPM, MPLA, MPUA, MPAT, and MPIDn 

### Hardware
使用 Embark E2 Eval Board V2 來測試
* 版子上的晶片為 R7F702Z15 (600pin)
* CS+中選 R7F702Z12A (BGA373)

## MPU 的暫存器及運作說明
* MPM, MPCFG
* MCA, MCS, MCC, CMR, MCI, MPIDX
* MPLA, MPUA, MPAT
* MPID0~MPID6

主要功能暫存器說明如下：

* MPM: 用於切換 Supervisior Mode(SV) 及啟用 MPU 功能
* MCA: 記憶體保護檢查位址
* MCS: 記憶體保護檢查大小
* MCC: 記憶體保護檢查命令
* MCR: 記憶體保護檢查結果
* MCI: 記憶體保護檢查 SPID (Bit4-0)
* MPIDX: 記憶保護索引 (Bit4-0, 有效數值為 0~23，共 24 組設定)
* MPLA: 記憶體保護最小位址
* MPUA: 記憶體保護最大位址
* MPAT: 記憶體保護區間屬性設定
* MPIDn: n 可為 0~6 ，共七個設定，用來指定每個設定的 SPID。

實際使用上，使用這些暫存器來指定特定記憶體區間的存取設定(沒指定到的視為禁止存取)，最多總共有 24 個保護區間可以設定，每個區間有每個區間的專屬設定。要設定不同的區間，可在 MPIDX 暫存器寫入編號 (0~23)，則會切換到對應的區間的暫存器設定。

保護區間的位址由 MPLA 跟 MPUA 來決定，屬性在 MPAT 設定。 MPAT包含了許多屬性，針對使用者模式(User Mode)及監督模式(SV Mode)對該區域的存取權(讀、寫或執行)，在 UR/UW/UX 及 SR/SW/SX 設定。

Bit7 的 E 用來設定是否啟用該區域的保護。WG/RG 用來決定是否啟用更細部的讀寫設定，若不啟用，則所有的 SPID 都可以存取 (WG=1 或 RG=1)，若啟用了，則可針對不同 SPID 來設定不同的存取權。SPID 的編號可以在 HWM 40.5.2.1 中找到。要設定細部的讀寫設定，將要設定的 Bus Master 編號，寫入 MPID0~MPID6，例如要指定 CPU0 的行為，就寫入 0，若 RHSIF0，則寫入 19，共有七組可以設定，之後，再使用 MPAT 的 MWPID7-0 (Bit31-24) 來設定是否允許寫入，以及 MPAT 的 RMPID7-0 (Bit23-16) 來設定是否允許讀取及執行(WG=0 或 RG=0)。

## Personal Note
* 手冊的中斷向量表
    - HWM Table 3.98  Selection of Base Register/Offset Address
* 對應範例程式的中斷向量表
    - r_g4mh.csp.asm #184-188, 會觸發中斷 r_g4mh_isr_fe
* MPU啟用時，所有沒有定義的記憶體存取都會觸發例外處理。
    - 針對保護的記憶體內容，則會視每個MPAT的設定來決定是否允許讀/寫/執行
* RH850 MPU
    - 可以設定最多 24 個區間，透過改變MPIDX設定IDX位址
* SVLOCK, HWM Section 3.2.2.5  Registers
    - 在 (5) Supervisor Lock Setting 說明，SVLOCK 可以禁止 CPU 系統暫存器被更新
    - If the SVLOCK.SVL bit is set (1), the following system registers cannot be updated even in supervisor mode: SPID, MPM, MPLA, MPUA, MPAT, and MPIDn
## Firmware
* 要存取 MPU 相關暫存器，需使用ldsr
* 使用 inline_asm 來存取

函數的引數會放在 R6，舉例來說，MPM 為 R0,5，故將 R6 寫到 R0, 5

    #pragma inline_asm      asm_write_MPM
    static int      asm_write_MPM(unsigned int data){
        ldsr  r6, 0, 5
    }
    #pragma inline_asm      asm_write_MPCFG
    static int      asm_write_MPCFG(unsigned int data){
        ldsr  r6, 2, 5
    }
    #pragma inline_asm      asm_write_MPIDX
    static int      asm_write_MPIDX(unsigned int data){
        ldsr  r6, 16, 5
    }
    #pragma inline_asm      asm_write_MPLA
    static int      asm_write_MPLA(unsigned int data){
        ldsr  r6, 20, 5
    }
    #pragma inline_asm      asm_write_MPUA
    static int      asm_write_MPUA(unsigned int data){
        ldsr  r6, 21, 5
    }
    #pragma inline_asm      asm_write_MPAT
    static int      asm_write_MPAT(unsigned int data){
        ldsr  r6, 22, 5
    }
    #pragma inline_asm      asm_write_MPID0
    static int      asm_write_MPID0(unsigned int data){
        ldsr  r6, 24, 5
    }
    #pragma inline_asm      asm_write_MPID1
    static int      asm_write_MPID1(unsigned int data){
        ldsr  r6, 25, 5
    }
    #pragma inline_asm      asm_write_MPID2
    static int      asm_write_MPID2(unsigned int data){
        ldsr  r6, 26, 5
    }
    #pragma inline_asm      asm_write_MPID3
    static int      asm_write_MPID3(unsigned int data){
        ldsr  r6, 27, 5
    }

##### Delta 目前的設定
實驗結果， 0~0BFFFC, 0FDE00000~0xFDE07FFC 這兩個區間都為必要設定

    typedef struct
    {
        uint32 min_addr;
        uint32 max_addr;
        uint32 mpat_setting;
    } mpu_region_cfg_t;
    
    static mpu_region_cfg_t regions_bank0_pe0[] = {
        {   0x0,        0xBFFFFC,   0xC0AD },   // 0
        // {   0x10000000, 0x2FFFFFFC, 0xC01B },   // 1
        // {   0xFDC00000, 0xFDC07FFC, 0xC09B },   // 2
        {   0xFDE00000, 0xFDE07FFC, 0xC09B },   // 3
        // {   0xFE000000, 0xFE0FFFFC, 0xC09B },   // 4
        // {   0xFF000000, 0xFFFBEFFC, 0xC09B },   // 5
        // {   0xFFFC0000, 0xFFFD3FFC, 0xC09B },   // 6
        // {   0xF9010000, 0xF901FFFC, 0xC09B },   // 7
        // {   0xFA000000, 0xFA017FFC, 0xC0BF },   // 8
        // {   0xFB000000, 0xFB1FFFFC, 0xC0BF },   // 9
        // {   0xFB400000, 0xFB5FFFFC, 0xC0BF },   // 10
        // {   0x0C980000, 0x0CA3FFFC, 0xC0BF },   // 11
        // {   0x0CA80000, 0x0CA8FFFC, 0xC0FF },   // 12
        // {   0x0BFF0000, 0x0BFFFFFF, 0xC0BF },   // 13
    };
    
##### 初始化
撰寫初始化函數來填暫存器

    int c_mpu_init(void){
        int i;
        uint8 n = sizeof(regions_bank0_pe0)/sizeof(regions_bank0_pe0[0]);
    
        asm_write_MPM(0x0);
        asm_write_MPID0(0);
        asm_write_MPID1(1);
        asm_write_MPID2(2);
        asm_write_MPID3(3);
        for(i=0; i<n; i++){
            asm_write_MPIDX(i);
            asm_write_MPLA(regions_bank0_pe0[i].min_addr);
            asm_write_MPUA(regions_bank0_pe0[i].max_addr);
            asm_write_MPAT(regions_bank0_pe0[i].mpat_setting);
        }
        asm_write_MPM(0x03);
        asm_write_MPIDX(0);
    }
