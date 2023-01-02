#ifndef UTILITY_H
#define UTILITY_H
#include "Type.h"
#include <stdarg.h>

void kPrintString(int iX, int iY, const char* pcString);
void kMemSet(void* pDest, BYTE bData, int iSize);
int kMemCpy(void* pDest, const void* pSrc, int iSize);
int kMemCmp(const void* pSrcA, const void* pSrcB, int iSize);

BOOL kSetInterruptFlag(BOOL bEnableInterrupt);

void kCheckTotalRamSize();
QWORD kGetTotalRamSize();

void kWaitms(long lMillisecond);
char* kConvertDayOfWeekToString(BYTE bDayOfWeek);
QWORD kMeasureCPUSpeed();

void kRequireReport();
void kReport(const char* vcReportMessage);

void kSleep(QWORD millisecond);

#endif