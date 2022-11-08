#include "PIT.h"
#include "AssemblyUtility.h"

void kInitializePIT(WORD wCount, BOOL bPeriodic){
    kOutPortByte(PIT_PORT_CONTROL, bPeriodic?PIT_COUNTER0_PERIODIC:PIT_COUNTER0_ONCE);
    kOutPortByte(PIT_PORT_COUNTER0, wCount);
    kOutPortByte(PIT_PORT_COUNTER0, wCount>>8);
}