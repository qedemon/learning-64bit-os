#include "Type.h"
#include "AssemblyUtility.h"
#include "Utility.h"
#include "Keyboard.h"
#include "TextModeTerminal.h"
#include "Descriptor.h"

void main(){
    int i=0;
    kPrintString(0, 10, "Switch to IA-32e Mode Success~!!");
    kPrintString(0, 11, "IA-32e C Language Kernel Start..............[Pass]");

    kPrintString(0, 12, "GDT Initialize And Switch for IA-32e Mode...[    ]");
    kInitializeGDTTableAndTSS();    
    kLoadGDTR((QWORD) GDTR_STARTADDRESS);
    kPrintString(45, 12, "Pass");

    kPrintString(0, 13, "IDT Initialize..............................[    ]");
    kInitializeIDTTables();
    kLoadIDTR((QWORD) IDTR_STARTADDRESS);
    kPrintString(45, 13, "Pass");

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

    kClearTerminal(0x0A, FALSE);
    kMoveCursorPos(0, 14);
    
    while(1){
        if(kIsOutputBufferFull()){
            char str[2]={0,};
            BYTE scanCode;
            BOOL updateTerminal;
            
            BYTE terminalKey;
            scanCode=kGetKeyBoardScanCode();
            updateTerminal=kUpdateKeyBoardManager(scanCode, &terminalKey);
            if(updateTerminal)
                kDataIntoTerminal(terminalKey);
            if(terminalKey=='0'){
                scanCode=scanCode/0;
            }
        }
    }
}
