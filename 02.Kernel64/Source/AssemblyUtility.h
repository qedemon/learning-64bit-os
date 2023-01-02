#ifndef ASSEMBLY_UTILITY_H
#define ASSEMBLY_UTILITY_H
#include "Type.h"

BYTE kInPortByte(WORD wPort);
void kOutPortByte(WORD wPort, BYTE bData);
WORD kInPortWord(WORD wPort);
void kOutPortWord(WORD wPort, WORD wData);
void kLoadGDTR(QWORD qwGDTRAddress);
void kLoadTSSR(WORD wSegmentOffset);
void kLoadIDTR(QWORD qwIDTRAddress);

void kEnableInterrupt();
void kDisableInterrupt();
QWORD kReadRFLAGS(void);
QWORD kReadTSC();

void kHlt();

BOOL kTestAndSet(volatile BYTE* pbDest, BYTE bCompare, BYTE bSource);

void kInitializeFPU();
void kSaveFPUContext(void* vpFPUContext);
void kLoadFPUContext(void* vpFPUContext);
void kSetTSBit();
void kClearTSBit();

#endif