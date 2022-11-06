#ifndef ISR_H
#define ISR_H
#include "Type.h"
void kISRDivideZero();
void kISRDebug();
void kISRNMI();
void kISRBreakPoint();
void kISROverflow();
void kISRBoundRangeExceeded();
void kISRInvalidOpcode();
void kISRDeviceNotAvailable();


void kISRDoubleFault();
void kISRCoprocessorSegmentOverrun();
void kISRInvalidTSS();
void kISRSegmentNotPresent();
void kISRStackSegmentFault();
void kISRGeneralProtection();
void kISRPageFault();
void kISR15();

void kISRFPUError();
void kISRAlignmentCheck();
void kISRMachineCheck();
void kISRSIMDError();
void kISRETCException();


void kISRTimer();
void kISRKeyboard();
void kISRSlavePIC();
void kISRSerial2();
void kISRSerial1();
void kISRParallel2();
void kISRFloppy();
void kISRParallel1();
void kISRRTC();

void kISRReserved();
void kISRNotUsed1();
void kISRNotUsed2();
void kISRMouse();
void kISRCoprocessor();
void kISRHDD1();
void kISRHDD2();
void kISRETCInterrupt();


#endif