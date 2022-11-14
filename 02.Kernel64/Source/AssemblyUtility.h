#ifndef ASSEMBLY_UTILITY_H
#define ASSEMBLY_UTILITY_H
#include "Type.h"

BYTE kInPortByte(WORD wPort);
void kOutPortByte(WORD wPort, BYTE bData);
void kLoadGDTR(QWORD qwGDTRAddress);
void kLoadTSSR(WORD wSegmentOffset);
void kLoadIDTR(QWORD qwIDTRAddress);

void kEnableInterrupt();
void kDisableInterrupt();
QWORD kReadRFLAGS(void);
QWORD kReadTSC();

void kHlt();


#endif