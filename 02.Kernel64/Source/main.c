#include "Type.h"

void kPrintString(int iX, int iY, const char* pcString);

void main(){
    kPrintString(0, 10, "Switch to IA-32e Mode Success~!!");
    kPrintString(0, 11, "IA-32e C Language Kernel Start..............[Pass]");
}

void kPrintString(int iX, int iY, const char* pcString){
    kCharStruct* pstString=(kCharStruct*) 0xB8000;
    int i;

    pstString+=(iY*80)+iX;
    for(i=0; pcString[i]!=0; i++){
        pstString[i].bChar=pcString[i];
    }
}