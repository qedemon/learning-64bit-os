#include "Type.h"
#include "Keyboard.h"

void kPrintString(int iX, int iY, const char* pcString);

void main(){
    int i;
    BOOL a[4]={TRUE, FALSE, FALSE, FALSE};
    kPrintString(0, 10, "Switch to IA-32e Mode Success~!!");
    kPrintString(0, 11, "IA-32e C Language Kernel Start..............[Pass]");
    kPrintString(0, 11, "Activate Keyboard...........................[Pass]");
    if(kActivateKeyBoard()){
        kPrintString(45, 11, "Pass");
    }
    else{
        kPrintString(45, 11, "Fail");
        while(1);
    }

    i=0;
    while(1){
        if(kIsOutputBufferFull()){
            kInPortByte(0x60);
            kChangeKeyboardLeds(a[0], a[1], a[2]);
            kPrintString(i, 12, "A");
            i++;
            a[3]=a[2];
            a[2]=a[1];
            a[1]=a[0];
            a[0]=a[3];
        }
    }
}

void kPrintString(int iX, int iY, const char* pcString){
    kCharStruct* pstString=(kCharStruct*) 0xB8000;
    int i;

    pstString+=(iY*80)+iX;
    for(i=0; pcString[i]!=0; i++){
        pstString[i].bChar=pcString[i];
    }
}