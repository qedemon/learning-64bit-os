#include "PIT.h"
#include "AssemblyUtility.h"
#include "TextModeTerminal.h"

void kInitializePIT(WORD wCount, BOOL bPeriodic){
    kOutPortByte(PIT_PORT_CONTROL, PIT_COUNTER0_ONCE);
    if(bPeriodic)
        kOutPortByte(PIT_PORT_CONTROL, PIT_COUNTER0_PERIODIC);
    kOutPortByte(PIT_PORT_COUNTER0, wCount);
    kOutPortByte(PIT_PORT_COUNTER0, wCount>>8);
}

WORD kReadPITCounter0(){
    BYTE highByte, lowByte;
    WORD wCounterValue;
    kOutPortByte(PIT_PORT_CONTROL, PIT_CONTROL_LATCH);
    lowByte=kInPortByte(PIT_PORT_COUNTER0);
    highByte=kInPortByte(PIT_PORT_COUNTER0);
    wCounterValue=highByte;
    wCounterValue<<=8;
    wCounterValue|=lowByte;
    return wCounterValue;
}
void kWaitUsingDirectPIT(WORD wCount){
    WORD wLastCounter, wCurrentCounter;
    kInitializePIT(0xFFFF, TRUE);
    wLastCounter=kReadPITCounter0();
    while(1){
        wCurrentCounter=kReadPITCounter0();
        if(((wLastCounter-wCurrentCounter)&0xFFFF)>=wCount)
            break;
    }
}

volatile QWORD g_qwTickCount=0;

QWORD kGetTickCount(){
    return g_qwTickCount;
}

void kAddTickCount(QWORD kValue){
    g_qwTickCount+=kValue;
}