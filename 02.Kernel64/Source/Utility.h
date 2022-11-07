#ifndef UTILITY_H
#define UTILITY_H
#include "Type.h"

void kPrintString(int iX, int iY, const char* pcString);
void kMemSet(void* pDest, BYTE bData, int iSize);
int kMemCpy(void* pDest, const void* pSrc, int iSize);

BOOL kSetInterruptFlag(BOOL bEnableInterrupt);

#endif