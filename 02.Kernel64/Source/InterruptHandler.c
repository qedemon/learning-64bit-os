#include "InterruptHandler.h"
#include "Utility.h"
#include "PIC.h"
#include "Keyboard.h"

void kCommonExceptionHandler(int iVectorNumber, QWORD qwErrorCode){
    char vcBuffer[]="[VEC:  ]";
    vcBuffer[5]=iVectorNumber/10+'0';
    vcBuffer[6]=iVectorNumber%10+'0';
    kPrintString(0, 0, "=======================================");
    kPrintString(0, 1, "Common Exception Handler Execute!!");
    kPrintString(0, 2, vcBuffer);
    kPrintString(0, 3, "=======================================");
    while(1);
}

void kCommonInterruptHandler(int iVectorNumber){
    char vcBuffer[]="[INT:  ,  ]";
    static int g_iCommonInterruptCout=0;
    vcBuffer[5]=iVectorNumber/10+'0';
    vcBuffer[6]=iVectorNumber%10+'0';
    vcBuffer[8]=(g_iCommonInterruptCout>=10)?(g_iCommonInterruptCout/10):' ';
    vcBuffer[9]=g_iCommonInterruptCout%10+'0';
    kPrintString(69, 0, vcBuffer);

    kSendEOIToPIC(iVectorNumber-32);
}

void kTimerInterruptHandler(int iVectorNumber, QWORD qwBP){

    char vcBuffer[]="[tNT:  ,  ]";
    static int g_iCommonInterruptCout=0;

    BYTE scanCode;

    g_iCommonInterruptCout++;
    vcBuffer[5]=iVectorNumber/10+'0';
    vcBuffer[6]=iVectorNumber%10+'0';
    vcBuffer[8]=(qwBP>=10)?(qwBP/10+'0'):' ';
    vcBuffer[9]=qwBP%10+'0';
    kPrintString(0, 0, vcBuffer);

    kSendEOIToPIC(iVectorNumber-32);
}

void kKeyboardInterruptHandler(int iVectorNumber){
#ifdef DEBUG
    char vcBuffer[]="[kNT:  ,  ]";
    static int g_iCommonInterruptCout=0;
#endif
    BYTE scanCode;
#ifdef DEBUG
    g_iCommonInterruptCout++;
    vcBuffer[5]=iVectorNumber/10+'0';
    vcBuffer[6]=iVectorNumber%10+'0';
    vcBuffer[8]=(g_iCommonInterruptCout>=10)?(g_iCommonInterruptCout/10+'0'):' ';
    vcBuffer[9]=g_iCommonInterruptCout%10+'0';
    kPrintString(0, 0, vcBuffer);
#endif
    if(kIsOutputBufferFull()){
        scanCode=kGetKeyBoardScanCode();
        kUpdateKeyBoardManagerAndPutKeyDatatToQueue(scanCode);
    }

    kSendEOIToPIC(iVectorNumber-32);
}