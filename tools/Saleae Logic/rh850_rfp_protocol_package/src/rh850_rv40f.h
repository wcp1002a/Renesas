#ifndef RV40F_PACKAGE
#define RV40F_PACKAGE

// 定義 Frame 類型
enum FrameType {
    FRAME_HEADER = 0,
    FRAME_DATA   = 1,
    FRAME_FOOTER = 2,
    FRAME_P_BAUDRATE,
    FRAME_UNDEFINE
};

#endif  //RV40F_PACKAGE
