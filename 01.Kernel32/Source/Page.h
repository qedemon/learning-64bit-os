#ifndef PAGE_H
#define PAGE_H
#include "Type.h"

#define PAGE_FLAG_P 0x01
#define PAGE_FLAG_RW 0x02
#define PAGE_FLAG_US 0x04
#define PAGE_FLAG_PWT 0x08
#define PAGE_FLAG_PCD 0x10
#define PAGE_FLAG_A 0x20

#define PAGE_FLAG_D 0x40
#define PAGE_FLAG_PS 0x80
#define PAGE_FLAG_G 0x100
#define PAGE_FLAG_EXB 0x80000000

#define PAGE_FLAG_DEFAULT (PAGE_FLAG_RW|PAGE_FLAG_P)

#define PAGE_TABLESIZE 0x1000
#define PAGE_MAXENTRYCOUNT 512
#define PAGE_DEFAULTSIZE 0x200000

#pragma pack(push, 1)
typedef struct PageTableEntry{
    DWORD LowerBaseAddressAndAttrib;
    DWORD EXPAndUpperBaseAddress;
}PML4ENTRY, PDPENTRY, PDENTRY, PTENTRY;
#pragma pack(pop)

void kSetPageEntryData(PTENTRY* pstEntry, DWORD dwUpperBaseAddress, DWORD dwLowerBassAddress, DWORD dwLowerFlags, DWORD dwUpperFlags);
void kInitializePageTables();

#endif