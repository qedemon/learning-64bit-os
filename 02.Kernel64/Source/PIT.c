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
    highByte=kInPortByte(PIT_CONTROL_COUNTER0);
    wCounterValue=highByte;
    wCounterValue<<=8;
    wCounterValue|=lowByte;
    return wCounterValue;
}
void kWaitUsingDirectPIT(WORD wCount){
    WORD wLastCounter, wCurrentCounter;
    kInitializePIT(0, TRUE);
    wLastCounter=kReadPITCounter0();
    while(1){
        wCurrentCounter=kReadPITCounter0();
        if(((wLastCounter-wCurrentCounter)&0xFFFF)>=wCount)
            break;
    }
}
void kWaitms(long lMillisecond){
    while(lMillisecond>30){
        //kprintf("wait 0x%x\n", MSTOCOUNT(30));
        kWaitUsingDirectPIT(MSTOCOUNT(30));
        lMillisecond-=30;
    }

    //kprintf("wait 0x%x\n", MSTOCOUNT(lMillisecond));
    kWaitUsingDirectPIT(MSTOCOUNT(lMillisecond));
}