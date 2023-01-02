#ifndef INTERRUPTHANDLER_H
#define INTERRUPTHANDLER_H
#include "Type.h"

void kCommonExceptionHandler(int iVectorNumber, QWORD qwErrorCode);
void kDeviceNotAvailableHandler(int iVectorNumber);
void kCommonInterruptHandler(int iVectorNumber);
void kTimerInterruptHandler(int iVectorNumber, QWORD qwStackStartAddress);
void kKeyboardInterruptHandler(int iVectorNumber);
void kHDDInterruptHandler(int iVectorNumber);
#endif