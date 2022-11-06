#ifndef PIC_H
#define PIC_H
#include "Type.h"

#define PIC_MASTER_PORT1 0x20
#define PIC_MASTER_PORT2 0x21
#define PIC_SLAVE_PORT1 0xA0
#define PIC_SLAVE_PORT2 0xA1

#define PIC_IRQSTARTVECTOR 0x20

void kInitializePIC();
void kMaskPICInterrupt(WORD wIRQBitmask);
void kSendEOIToPIC(int iIRQNumber);

#endif