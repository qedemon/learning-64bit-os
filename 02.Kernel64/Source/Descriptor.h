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

#pragma pack(pop)

void kSetGDTEntry8(GDTENTRY8* pstEntry, DWORD dwBaseAddress, DWORD dwLimit, BYTE bLowerFlags, BYTE bUpperFlags, BYTE bType);
void kSetGDTEntry16(GDTENTRY16* pstEntry, QWORD qwBaseAddress, DWORD dwLimit, BYTE bLowerFlags, BYTE bUpperFlags, BYTE bType);

#endif