#include "Utility.h"
#include "Descriptor.h"

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
    pstEntry->dwUpperBaseAddress=(qwBaseAddress>>32)&0xFFFFFFFF;
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
    kSetGDTEntry16((GDTENTRY16*)&(pstGDTTableEntry[3]), (QWORD) pstTSS, sizeof(TSSEGMENT), GDT_FLAGS_LOWER_TSS, GDT_FLAGS_UPPER_TSS, GDT_TYPE_TSS);

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