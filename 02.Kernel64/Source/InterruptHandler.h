#ifndef INTERRUPTHANDLER_H
#define INTERRUPTHANDLER_H
#include "Type.h"
void kCommonExceptionHandler(int iVectorNumber, QWORD qwErrorCode);
void kCommonInterruptHandler(int iVectorNumber);
void kTimerInterruptHandler(int iVectorNumber, QWORD qwStackBaseAddress);
void kKeyboardInterruptHandler(int iVectorNumber);
#endif