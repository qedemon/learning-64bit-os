#include "Utility.h"
#include "Descriptor.h"
#include "ISR.h"
#include "TextModeTerminal.h"

void kSetGDTEntry8(GDTENTRY8* pstEntry, DWORD dwBaseAddress, DWORD dwLimit, BYTE bLowerFlags, BYTE bUpperFlags, BYTE bType){
    pstEntry->wLowerLimit=dwLimit&0xFFFF;
    pstEntry->wLowerBaseAddress=dwBaseAddress&0xFFFF;
    pstEntry->bMiddleBaseAddress=(dwBaseAddress>>16)&0xFF;
    pstEntry->bLowerFlagsAndType=bLowerFlags|bType;
    pstEntry->bUpperFlagsAndUpperLimit=((dwLimit>>16)&0xFF)|(bUpperFlags);
    pstEntry->bUpperBaseAddress=(dwBaseAddress>>24)&0xFF;
}

void kSetGDTEntry16(GDTENTRY16* pstEntry, QWORD qwBaseAddress, DWORD dwLimit, BYTE bLowerFlags, BYTE bUpperFlags, BYTE bType){
    pstEntry->wLowerLimit=dwLimit&0xFFFF;
    pstEntry->wLowerBaseAddress=qwBaseAddress&0xFFFF;
    pstEntry->bMiddleBaseAddress0=(dwLimit>>16)&0xFF;
    pstEntry->bLowerFlagsAndType=bType|bLowerFlags;
    pstEntry->bUpperFlagsAndUpperLimit=((dwLimit>>16)&0xFF)|bUpperFlags;
    pstEntry->bMiddleBaseAddress1=(qwBaseAddress>>24)&0xFF;
    pstEntry->dwUpperBaseAddress=(qwBaseAddress>>32);
    pstEntry->dwReserved=0;
}

void kInitializeGDTTableAndTSS(){
    GDTR* pstGDTR;
    GDTENTRY8 *pstGDTTableEntry;
    GDTENTRY16 *pstTSSDescriptor;
    TSSEGMENT *pstTSS;

    pstGDTR=(GDTR*)(GDTR_STARTADDRESS);
    pstGDTTableEntry=(GDTENTRY8*)(GDTR_STARTADDRESS+sizeof(GDTR));
    pstTSS=(TSSEGMENT*)((QWORD)pstGDTTableEntry+GDT_TABLEMAXSIZE);

    kMemSet(pstGDTR, 0, sizeof(GDTR));
    pstGDTR->wLimit=GDT_TABLEMAXSIZE-1;
    pstGDTR->qwBaseAddress=(QWORD) pstGDTTableEntry;

    kSetGDTEntry8(&(pstGDTTableEntry[0]), 0, 0, 0, 0, 0);
    kSetGDTEntry8(&(pstGDTTableEntry[1]), 0, 0xFFFF, GDT_FLAGS_LOWER_KERNELCODE,GDT_FLAGS_UPPER_CODE, GDT_TYPE_CODE);
    kSetGDTEntry8(&(pstGDTTableEntry[2]), 0, 0xFFFF, GDT_FLAGS_LOWER_KERNELDATA,GDT_FLAGS_UPPER_DATA, GDT_TYPE_DATA);
    kSetGDTEntry16((GDTENTRY16*)&(pstGDTTableEntry[3]), (QWORD) pstTSS, sizeof(TSSEGMENT)-2, GDT_FLAGS_LOWER_TSS, GDT_FLAGS_UPPER_TSS, GDT_TYPE_TSS);
    kInitializeTSS(pstTSS);
}
void kInitializeTSS(TSSEGMENT* pstTSS){
    kMemSet(pstTSS, 0, sizeof(TSSEGMENT));
    pstTSS->qwIST[0]=IST_STARTADDRESS+IST_SIZE;
    pstTSS->wIOMapBaseAddress=0xFFFF;
}

void kInitializeIDTTables(){
    IDTR* pstIDTR;
    IDTENTRY* pstEntry;
    int i;

    pstIDTR=(IDTR*) IDTR_STARTADDRESS;
    pstEntry=(IDTENTRY*) IDTR_STARTADDRESS+sizeof(IDTR);
    
    pstIDTR->wLimit=IDT_MAXTABLESIZE-1;
    pstIDTR->qwBaseAddress=(QWORD)pstEntry;

    for(i=0; i<IDT_MAXENTRYCOUNT; i++){
        kSetIDTEntry(&(pstEntry[i]), dummyHandler, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    }
    kSetIDTEntry(&(pstEntry[0]), kISRDivideZero, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[1]), kISRDebug, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[2]), kISRNMI, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[3]), kISRBreakPoint, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[4]), kISROverflow, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[5]), kISRBoundRangeExceeded, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[6]), kISRInvalidOpcode, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[7]), kISRDeviceNotAvailable, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    
    kSetIDTEntry(&(pstEntry[8]), kISRDoubleFault, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[9]), kISRCoprocessorSegmentOverrun, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[10]), kISRInvalidTSS, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[11]), kISRSegmentNotPresent, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[12]), kISRStackSegmentFault, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[13]), kISRGeneralProtection, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[14]), kISRPageFault, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[15]), kISR15, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    
    kSetIDTEntry(&(pstEntry[16]), kISRFPUError, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[17]), kISRAlignmentCheck, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[18]), kISRMachineCheck, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[19]), kISRSIMDError, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    for(i=20; i<32; i++)
        kSetIDTEntry(&(pstEntry[i]), kISRETCException, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);

    kSetIDTEntry(&(pstEntry[32]), kISRTimer, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[33]), kISRKeyboard, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[34]), kISRSlavePIC, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[35]), kISRSerial2, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[36]), kISRSerial1, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[37]), kISRParallel2, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[38]), kISRFloppy, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[39]), kISRParallel1, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    
    kSetIDTEntry(&(pstEntry[40]), kISRRTC, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[41]), kISRReserved, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[42]), kISRNotUsed1, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[43]), kISRNotUsed2, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[44]), kISRMouse, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[45]), kISRCoprocessor, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[46]), kISRHDD1, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    kSetIDTEntry(&(pstEntry[47]), kISRHDD2, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    for(i=48; i<IDT_MAXENTRYCOUNT; i++){
        kSetIDTEntry(&(pstEntry[i]), kISRETCInterrupt, GDT_KERNELCODESEGMENT, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    }
}

void kSetIDTEntry(IDTENTRY* pstEntry, void* pvHandler, WORD wSegmentSelector, BYTE bIST, BYTE bFlags, BYTE bGateType){
    pstEntry->wLowerHandlerAddress=(QWORD)pvHandler&0xFFFF;
    pstEntry->wSegmentSelctor=wSegmentSelector;
    pstEntry->bIST=bIST&0b111;
    pstEntry->bFlagsAndGateType=bFlags|bGateType;
    pstEntry->wMiddleHandlerAddress=((QWORD)pvHandler>>16)&0xFFFF;
    pstEntry->dwUpperHandlerAddress=((QWORD)pvHandler>>32);
    pstEntry->dwReserved=0;
}

void dummyHandler(){
    kPrintString(0, 0, "=======================================");
    kPrintString(0, 1, "Dummy Interrupt Handler Execute!!");
    kPrintString(0, 2, "=======================================");
    while(1);
}