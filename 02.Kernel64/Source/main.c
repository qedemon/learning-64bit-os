#include "Type.h"
#include "AssemblyUtility.h"
#include "Utility.h"
#include "Keyboard.h"
#include "TextModeTerminal.h"
#include "Descriptor.h"
#include "PIC.h"
#include "PIT.h"
#include "string.h"
#include "Task.h"

void main(){
    WORD wCursorX, wCursorY;
    kTerminalSetAttribute(TERMINAL_DEFAULT_COLOR);
    kTerminalSetCursorPos(0, 10);
    
    kprintf("%s\n", "Switch to IA-32e Mode Success~!!");
    kprintf("%s\n", "IA-32e C Language Kernel Start..............[Pass]");

    kprintf("%s", "GDT Initialize And Switch for IA-32e Mode...[    ]");
    kInitializeGDTTableAndTSS();    
    kLoadGDTR((QWORD) GDTR_STARTADDRESS);
    kTerminalGetCursorPos(&wCursorX, &wCursorY);
    kTerminalSetCursorPos(45, wCursorY++);
    kprintf("Pass\n");

    kprintf("%s", "TSS segment Load............................[    ]");
    kLoadTSSR(GDT_KERNELTSSEGMENT);
    kTerminalSetCursorPos(45, wCursorY++);
    kprintf("Pass\n");

    kprintf("%s", "IDT Initialize..............................[    ]");
    kInitializeIDTTables();
    kLoadIDTR((QWORD) IDTR_STARTADDRESS);
    kTerminalSetCursorPos(45, wCursorY++);
    kprintf("Pass\n");

    kprintf("%s", "Total RAM size check........................[    ]");
    kCheckTotalRamSize();
    kTerminalSetCursorPos(45, wCursorY++);
    kprintf("Pass], Size = %d MB\n", kGetTotalRamSize());
   

    kprintf("TCB Pool And Scheduler Initialize.................[Pass]\n");
    wCursorY++;
    kInitializeScheduler();
    kInitializePIT(MSTOCOUNT(1), FALSE);

    kprintf("%s", "Start Interrupt.............................[    ]");
    kInitializePIC();
    kMaskPICInterrupt(0);
    kEnableInterrupt();
    kTerminalSetCursorPos(45, wCursorY++);
    kprintf("Pass\n");
    
    kprintf("%s", "Activate Keyboard...........................[Pass]");
    kTerminalSetCursorPos(45, wCursorY++);
    if(kInitializeKeyBoard()){
        kprintf("Pass\n");
    }
    else{
        kprintf("Fail\n");
        while(1);
    }

    /*if(!kUpdateKeyboardLeds()){
        while(1);
    }*/
    
    kTerminalSetCursorPos(0, wCursorY);
    kprintf("MINT64 OS Start\n");
    kStartTerminal();
}
