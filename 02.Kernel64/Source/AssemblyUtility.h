#ifndef ASSEMBLY_UTILITY
#define ASSEMBLY_UTILITY
#include "Type.h"

BYTE kInPortByte(WORD wPort);
void kOutPortByte(WORD wPort, BYTE bData);

#endif