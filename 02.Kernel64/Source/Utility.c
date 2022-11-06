#include "Utility.h"

void kMemSet(void* pDest, BYTE bData, int iSize){
    int i;
    for(i=0; i<iSize; i++){
        ((char*)pDest)[i]=bData;
    }
}
int kMemCpy(void* pDest, const void* pSrc, int iSize){
    int i;
    for(i=0; i<iSize; i++){
        ((char*)pDest)[i]=((char*)pSrc)[i];
    }
    return i;
}

void kPrintString(int iX, int iY, const char* pcString){
    kCharStruct* pstString=(kCharStruct*) 0xB8000;
    int i;

    pstString+=(iY*80)+iX;
    for(i=0; pcString[i]!=0; i++){
        pstString[i].bChar=pcString[i];
    }
}