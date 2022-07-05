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
  
RH850 MPU
==========
## Reference Documents
* MPU 說明
    - HWM 3.2.3.6  MPU Function Registers
    - 0_Delta\General\30_TrainingMaterial\E2x_CPU.pdf (From Page#50)
* MDP related information
    - HWM Table 3.95  Exception Cause List (Page#348)
    - PMAT (Page#325)
    - HWM Table 3.107  Exception Cause Code of Memory Protection Violation (Page#389)

## Personal Note
* 手冊的中斷向量表
    - HWM Table 3.98  Selection of Base Register/Offset Address
* 對應範例程式的中斷向量表
    - r_g4mh.csp.asm #184-188, 會觸發中斷 r_g4mh_isr_fe
* MPU啟用時，所有沒有定義的記憶體存取都會觸發例外處理。
    - 針對保護的記憶體內容，則會視每個MPAT的設定來決定是否允許讀/寫/執行
* RH850 MPU
    - 可以設定最多 24 個區間，透過改變MPIDX設定IDX位址
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
