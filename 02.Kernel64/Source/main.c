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
#include "DynamicMemory.h"
#include "HDD.h"

void main(){
    WORD wCursorX, wCursorY;
    kTerminalSetAttribute(TERMINAL_DEFAULT_COLOR);
    kTerminalSetCursorPos(0, 10);
    
    kprintf("%s\n", "Switch to IA-32e Mode Success~!!");
    kprintf("%s\n", "IA-32e C Language Kernel Start..............[Pass]");

    kprintf("%s", "GDT Initialize And Switch for IA-32e Mode...[    ]");
    kInitializeGDTTableAndTSS();    
    kLoadGDTR((QWORD) GDTR_STARTADDRESS);
    kLoadTSSR(GDT_KERNELTSSEGMENT);
    kTerminalGetCursorPos(&wCursorX, &wCursorY);
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
   
    kprintf("%s", "Start Interrupt.............................[    ]");
    kInitializePIC();
    kMaskPICInterrupt(0);
    kEnableInterrupt();
    kTerminalSetCursorPos(45, wCursorY++);
    kprintf("Pass\n");
    kprintf("TCB Pool And Scheduler Initialize.................[Pass]\n");
    wCursorY++;
    kInitializeScheduler();
    kInitializePIT(MSTOCOUNT(1), TRUE);
    
    kprintf("%s", "Activate Keyboard...........................[Pass]");
    kTerminalSetCursorPos(45, wCursorY++);
    if(kInitializeKeyBoard()){
        kprintf("Pass\n");
    }
    else{
        kprintf("Fail\n");
        while(1);
    }

    kprintf("%s", "Initialize Dynamic Memory Blocks............[    ]");
    kInitializeDynamicMemory();
    kTerminalSetCursorPos(45, wCursorY++);
    kprintf("Pass\n");

    kprintf("%s", "Initialize HDD Driver.......................[    ]");
    kTerminalSetCursorPos(45, wCursorY++);
    if(kInitializeHDD()){
        kprintf("Pass\n");
    }
    else{
        kprintf("Fail\n");
    }

    /*if(!kUpdateKeyboardLeds()){
        while(1);
    }*/
    
    //kTerminalSetCursorPos(0, wCursorY);
    kprintf("MINT64 OS Start\n");
    kCreateTask(TASK_FLAG_LOWEST|TASK_FLAG_IDLE|TASK_FLAG_SYSTEM|TASK_FLAG_THREAD, (QWORD) kIdleTask, 0, QWORD_MAX);
    kStartTerminal();
}
