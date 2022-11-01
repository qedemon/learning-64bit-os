#include "Page.h"

void kSetPageEntryData(PTENTRY* pstEntry, DWORD dwUpperBaseAddress, DWORD dwLowerBassAddress, DWORD dwLowerFlags, DWORD dwUpperFlags){
    pstEntry->LowerBaseAddressAndAttrib=dwLowerBassAddress|dwLowerFlags;
    pstEntry->EXPAndUpperBaseAddress=dwUpperFlags|(dwUpperBaseAddress&0xFF);
}

void kInitializePageTables(){
    PML4ENTRY* pstPML4Entry;
    PDPENTRY* pstPDPEntry;
    PDENTRY* pstPDEntry;
    DWORD dwMappingAddress;
    int i;

    pstPML4Entry=(PML4ENTRY*)0x100000;
    pstPDPEntry=(PDPENTRY*)(0x100000+PAGE_TABLESIZE);
    pstPDEntry=(PDENTRY*)(0x100000+PAGE_TABLESIZE*2);
    
    kSetPageEntryData(pstPML4Entry, 0, (DWORD) pstPDPEntry, PAGE_FLAG_DEFAULT, 0);
    for(i=1; i<PAGE_MAXENTRYCOUNT; i++){
        kSetPageEntryData(&(pstPML4Entry[i]), 0, 0, 0, 0);
    }

    for(i=0; i<64; i++){
        kSetPageEntryData(&(pstPDPEntry[i]), 0, (DWORD)pstPDEntry + PAGE_TABLESIZE*i, PAGE_FLAG_DEFAULT, 0);
    }
    for(i=64; i<PAGE_MAXENTRYCOUNT; i++){
        kSetPageEntryData(&(pstPDPEntry[i]), 0, 0, 0, 0);
    }
    dwMappingAddress=0;
    for(i=0; i<PAGE_MAXENTRYCOUNT*64; i++){
        kSetPageEntryData(&(pstPDEntry[i]), (i*(PAGE_DEFAULTSIZE>>20))>>12, dwMappingAddress, PAGE_FLAG_DEFAULT|PAGE_FLAG_PS, 0);
        dwMappingAddress+=PAGE_DEFAULTSIZE;
    }
}