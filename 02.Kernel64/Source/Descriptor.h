#ifndef DESCRIPTOR_H
#define DESCRIOTOR_H
#include "Type.h"

#define GDT_TYPE_CODE 0X0A
#define GDT_TYPE_DATA 0X02
#define GDT_TYPE_TSS 0x09

#define GDT_FLAGS_LOWER_S 0x10
#define GDT_FLAGS_LOWER_DPL_0 0x00
#define GDT_FLAGS_LOWER_DPL_1 0x20
#define GDT_FLAGS_LOEWR_DPL_2 0x40
#define GDT_FLAGS_LOWER_DPL_3 0x60
#define GDT_FLAGS_LOWER_P 0x80
#define GDT_FLAGS_UPPER_L 0x20
#define GDT_FLAGS_UPPER_DB 0x40
#define GDT_FLAGS_UPPER_G 0x80

#define GDT_FLAGS_LOWER_KERNELCODE (GDT_FLAGS_LOWER_S|GDT_FLAGS_LOWER_DPL_0|GDT_FLAGS_LOWER_P)
#define GDT_FLAGS_LOWER_KERNELDATA (GDT_FLAGS_LOWER_S|GDT_FLAGS_LOWER_DPL_0|GDT_FLAGS_LOWER_P)
#define GDT_FLAGS_LOWER_TSS (GDT_FLAGS_LOWER_DPL_0|GDT_FLAGS_LOWER_P)
#define GDT_FLAGS_LOWER_USERCODE (GDT_FLAGS_LOWER_S|GDT_FLAGS_LOWER_DPL_3|GDT_FLAGS_LOWER_P)
#define GDT_FLAGS_LOWER_USERDATA (GDT_FLAGS_LOWER_S|GDT_FLAGS_LOWER_DPL_3|GDT_FLAGS_LOWER_P)


#define GDT_FLAGS_UPPER_CODE (GDT_FLAGS_UPPER_G|GDT_FLAGS_UPPER_L)
#define GDT_FLAGS_UPPER_DATA (GDT_FLAGS_UPPER_G|GDT_FLAGS_UPPER_L)
#define GDT_FLAGS_UPPER_TSS (GDT_FLAGS_UPPER_G)

#define GDTR_STARTADDRESS 0x142000
#define GDT_TABLEMAXSIZE (sizeof(GDTENTRY8)*3+sizeof(GDTENTRY16))
#define GDT_KERNELCODESEGMENT 0x08
#define GDT_KERNELDATASEGMENT 0x10
#define GDT_KERNELTSSEGMENT 0x18

#define IDT_TYPE_INTERRUPT 0x0E
#define IDT_TYPE_TRAP 0x0F
#define IDT_FLAGS_DPL0 0x00
#define IDT_FLAGS_DPL1 0x02
#define IDT_FLAGS_DPL2 0x04
#define IDT_FLAGS_DPL3 0x06
#define IDT_FLAGS_P 0x80
#define IDT_FLAGS_IST0 0
#define IDT_FLAGS_IST1 0

#define IDT_FLAGS_KERNEL (IDT_FLAGS_DPL0|IDT_FLAGS_P)
#define IDT_FLAGS_USER (IDT_FLAGS_DPL3|IDT_FLAGS_P)

#define IDT_MAXENTRYCOUNT 100
#define IDTR_STARTADDRESS (GDTR_STARTADDRESS+sizeof(GDTR)+GDT_TABLEMAXSIZE+sizeof(TSSEGMENT))
#define IDT_MAXTABLESIZE (IDT_MAXENTRYCOUNT*sizeof(IDTENTRY))

#define IST_STARTADDRESS 0x700000
#define IST_SIZE 0x100000

#pragma pack(push, 1)

typedef struct kGDTR{
    WORD wLimit;
    QWORD qwBaseAddress;
    WORD wPadding;
    DWORD dwPadding;
}GDTR, IDTR;

typedef struct kGDTEntry8Struct{
    WORD wLowerLimit;
    WORD wLowerBaseAddress;
    BYTE bMiddleBaseAddress;
    BYTE bLowerFlagsAndType;
    BYTE bUpperFlagsAndUpperLimit;
    BYTE bUpperBaseAddress;
}GDTENTRY8;

typedef struct kGDTEntry16Struct{
    WORD wLowerLimit;
    WORD wLowerBaseAddress;
    BYTE bMiddleBaseAddress0;
    BYTE bLowerFlagsAndType;
    BYTE bUpperFlagsAndUpperLimit;
    BYTE bMiddleBaseAddress1;
    DWORD dwUpperBaseAddress;
    DWORD dwReserved;    
}GDTENTRY16;

typedef struct kTSSDataStruct{
    DWORD dwReserved0;
    QWORD qwRsp[3];
    QWORD qwReserved1;
    QWORD qwIST[7];
    QWORD qwReserved2;
    WORD wReserved;
    WORD wIOMapBaseAddress;
}TSSEGMENT;

typedef struct IDTGateDescriptor{
    WORD wLowerHandlerAddress;
    WORD wSegmentSelctor;
    BYTE bIST;
    BYTE bFlagsAndGateType;
    WORD wMiddleHandlerAddress;
    DWORD dwUpperHandlerAddress;
    DWORD dwReserved;
}IDTENTRY;

#pragma pack(pop)

void kSetGDTEntry8(GDTENTRY8* pstEntry, DWORD dwBaseAddress, DWORD dwLimit, BYTE bLowerFlags, BYTE bUpperFlags, BYTE bType);
void kSetGDTEntry16(GDTENTRY16* pstEntry, QWORD qwBaseAddress, DWORD dwLimit, BYTE bLowerFlags, BYTE bUpperFlags, BYTE bType);

void kInitializeGDTTableAndTSS();
void kInitializeTSS(TSSEGMENT* pstTSS);

void kInitializeIDTTables();
void kSetIDTEntry(IDTENTRY* pstEntry, void* pvHander, WORD wSegmentSelector, BYTE bIST, BYTE bFlags, BYTE bGateType);

void dummyHandler();

#endif