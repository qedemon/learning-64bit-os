#include "Type.h"
#include "Keyboard.h"
#include "AssemblyUtility.h"

void kPrintString(int iX, int iY, const char* pcString);

void main(){
    int i=0;
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

    if(!kUpdateKeyboardLeds()){
        while(1);
    }

    while(1){
        if(kIsOutputBufferFull()){
            char str[2]={0,};
            BYTE scanCode;
            BOOL updateTerminal;
            BYTE terminalKey;
            scanCode=kGetKeyBoardScanCode();
            updateTerminal=kUpdateKeyBoardManager(scanCode, &terminalKey);
            if(updateTerminal){
                str[0]=terminalKey;
                kPrintString(i, 13, str);
                i++;
            }
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