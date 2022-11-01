#include "Type.h"
#include "Page.h"
#include "ModeSwitch.h"

void kPrintString(int iX, int iY, const char* pcString);
BOOL kIsMemoryEnugh(void);
BOOL kInitializeKernel64Area();
WORD kCopyKernel64ImageTo2Mbyte();

void main(){
    char vcVendorString[13]={0,};
    WORD wCopiedSectorCount;
    char vcCopiedSectorCount[4]={0,};
    kPrintString(0, 3, "C Language Kernel Started..........[Pass]");
    kPrintString(0, 4, "Minimum Memory Size Check..........[    ]");
    if(!kIsMemoryEnugh()){
        kPrintString(36, 4, "Fail");
        while(1);
    }
    kPrintString(36, 4, "Pass");
    kPrintString(0, 5, "Initialize IA-32e Kernel Area......[    ]");
    if(!kInitializeKernel64Area()){
        kPrintString(36, 5, "Fail");
        while(1);
    }
    kPrintString(36, 5, "Pass");
    
    kPrintString(0, 6, "Initialize IA-32e Page Tables......[    ]");
    kInitializePageTables();
    kPrintString(36, 6, "Pass");

    kReadCPUID(vcVendorString);
    kPrintString(0, 7, "CPUID : ");
    kPrintString(8, 7, vcVendorString);

    kPrintString(0, 8, "Checking 64 Bit mode suppport......[    ]");
    if(kCheck64BitMode()){
        kPrintString(36, 8, "Pass");
    }
    else{
        kPrintString(36, 8, "Fail");
        while(1);
    }

    kPrintString(0, 9, "Copy Kernel64 Image to 2Mbyte .....[    ]");
    wCopiedSectorCount=kCopyKernel64ImageTo2Mbyte();
    vcCopiedSectorCount[0]=wCopiedSectorCount/10+'0';
    vcCopiedSectorCount[1]=wCopiedSectorCount%10+'0';
    kPrintString(36, 9, vcCopiedSectorCount);

    kPrintString(0, 10, "Switch to IA-32e Mode");
    kSwitchAndExecute64BitMode();

    while(1);
}

void kPrintString(int iX, int iY, const char* pcString){
    int i;
    kCharStruct* pCharStr;
    pCharStr=(kCharStruct*)(0xB8000+160*iY+iX*2);
    for(int i=0; pcString[i]!=0; i++){
        pCharStr[i].bChar=pcString[i];
    }
}

BOOL kIsMemoryEnugh(void){
    DWORD* pCurentAddress=(DWORD*) 0x100000;
    while((DWORD)pCurentAddress<0x4000000){
        *pCurentAddress=0x12345678;
        if(*pCurentAddress!=0x12345678)
            return FALSE;
        pCurentAddress+=(0x100000)/4;
    }
    return TRUE;
}

BOOL kInitializeKernel64Area(){
    DWORD* pCurrentAddress=(DWORD*) 0x100000;
    while((DWORD)pCurrentAddress<0x600000){
        *pCurrentAddress=0;
        if(*pCurrentAddress!=0){
            return FALSE;
        }
        pCurrentAddress++;
    }
    return TRUE;
}

WORD kCopyKernel64ImageTo2Mbyte(){
    WORD wKernel32SectorCount;
    WORD wTotalSectorCount;
    DWORD* pdwSourceAddress, *pdwTargetAddress;
    int i;

    wTotalSectorCount = *((WORD*)0x7C05);
    wKernel32SectorCount=*((WORD*) 0x7C07);
    
    pdwSourceAddress=(DWORD*) (0x10000+wKernel32SectorCount*512);
    pdwTargetAddress=(DWORD*) (0x200000);
    for(i=0; i<512*(wTotalSectorCount-wKernel32SectorCount)/4; i++){
        *pdwTargetAddress = *pdwSourceAddress;
        pdwTargetAddress++;
        pdwSourceAddress++;
    }

    return wTotalSectorCount-wKernel32SectorCount;
}