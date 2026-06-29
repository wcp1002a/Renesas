#ifndef RV40F_PACKAGE
#define RV40F_PACKAGE

// 定義 Frame 類型
enum FrameType {
    FRAME_HEADER = 0,
    FRAME_DATA   = 1,
    FRAME_FOOTER = 2
};

// 定義 Byte 類型
enum ByteType {
    BYTE_SOH = 1,
    BYTE_LNH,
    BYTE_LNL,
    BYTE_COM,
    BYTE_DAT,
    BYTE_SUM,
    BYTE_SUM_NG,
    BYTE_ETX,
    BYTE_P_BAUD,
    BYTE_UNDEFINE
};
#endif  //RV40F_PACKAGE
