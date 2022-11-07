#include "Type.h"
#include "AssemblyUtility.h"
#include "Utility.h"
#include "Keyboard.h"
#include "TextModeTerminal.h"
#include "Descriptor.h"
#include "PIC.h"
#include "String.h"

void main(){
    int i=0;
    KEYDATA stKeyData;
    char str[1024];
    char str2[1024];
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

    kPrintString(0, 14, "Start Interrupt.............................[    ]");
    kInitializePIC();
    kMaskPICInterrupt(0);
    kEnableInterrupt();
    kPrintString(45, 14, "Pass");
    

    kPrintString(0, 15, "Activate Keyboard...........................[Pass]");
    if(kInitializeKeyBoard()){
        kPrintString(45, 15, "Pass");
    }
    else{
        kPrintString(45, 15, "Fail");
        while(1);
    }

    if(!kUpdateKeyboardLeds()){
        while(1);
    }

    kClearTerminal(TERMINAL_DEFAULT_COLOR, FALSE);
    kMoveCursorPos(0, 16);
    kHexToString(1024, str2);
    ksprintf(str, "String Formating test ..........[%d=0x%q]", 16+8, 16+8);
    kPrintString(0, 16, str);
    
    while(1){
        if(kGetKeyFromKeyQueue(&stKeyData)){    
            kDataIntoTerminal(stKeyData.bASCIICode);
            if(stKeyData.bASCIICode=='0'){
                BOOL scanCode;
                scanCode=scanCode/0;
            }
        }
    }
}
