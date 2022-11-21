#include "TextModeTerminal.h"
#include "TerminalCommand.h"
#include "Utility.h"
#include "string.h"
#include "PIT.h"
#include "RTC.h"
#include "Task.h"
#include "Type.h"
#include "Synchronization.h"
#include "AssemblyUtility.h"
#include "DynamicMemory.h"

static TERMINALCOMMANDENTRY gs_stCommandList[]={
    {"help", "Show Help", kTerminalCommandHelp},
    {"cls", "Clear Screen", kTerminalCommandClear},
    {"totalram", "Show Total RAM Size", kTerminalCommandShowTotalRamSize},
    {"strtod", "String to Decimal/Hex Convert", kTerminalCommandStringToDeciHexConvert},
    {"shutdown", "Reboot PC", kTerminalCommandShutdown},
    {"wait", "wait milliseconds", kTerminalCommandWait},
    {"time", "show date and time", kTerminalCommandShowDateAndTime},
    {"cpuspeed", "measure cpu speed", kTermianlCommandMeasureCPUSpeed},
    {"settimer", "settimer 100(ms) 1(periodic)", kTerminalCommandStartTimer},
    {"createtask", "Create Task", kTerminalCommandCreateTask},
    {"stoptask", "Stop Task", kTerminalCommandStopOtherTasks},
    {"showtask", "Show task list", kTerminalCommandShowTaskList},
    {"killtask", "End Task ex) killtask 2(ID)", kTerminalCommandKillTask},
    {"testLink", "testLink", kTerminalCommandTestLinkedList},
    {"cpuload", "Get CPU Processing Load", kTerminalCommandGetProcessorLoad},
    {"chpri", "chpri 0x30002(ID) 3(priority)", kTerminalCommandChangePriority},
    {"testfloat", "test float caculation", kTerminalCommandTestFPU},
    {"testalloc", "test dynamic memory allocation", kTerminalCommandTestDynamicMemory},
};

void kTerminalSearchCommandEntryAndSpaceIndex(const char* pcCommandBuffer, TERMINALCOMMANDENTRY** ppstTerminalCmd, int* piSpaceIndex){
    int i, iSpaceIndex, iCount;
    int iCommandBufferLen=kstrlen(pcCommandBuffer);
    *ppstTerminalCmd=NULL;
    *piSpaceIndex=0;
    for(iSpaceIndex=0; iSpaceIndex<iCommandBufferLen; iSpaceIndex++){
        if(pcCommandBuffer[iSpaceIndex]==' ')
            break;
    }
    iCount=sizeof(gs_stCommandList)/sizeof(TERMINALCOMMANDENTRY);
    for(i=0; i<iCount; i++){
        if(kstrlen(gs_stCommandList[i].pcCommand)==iSpaceIndex){
            if(kMemCmp(gs_stCommandList[i].pcCommand, pcCommandBuffer, iSpaceIndex)==0){
                if(ppstTerminalCmd!=NULL)
                    *ppstTerminalCmd = &(gs_stCommandList[i]);
                if(piSpaceIndex!=NULL)
                    *piSpaceIndex=iSpaceIndex;
            }
        }
    }
}


TERMINALCOMMANDENTRY* kTerminalGetCMDEntry(int iIndex){
    if(iIndex>=sizeof(gs_stCommandList)/sizeof(TERMINALCOMMANDENTRY)){
        return NULL;
    }
    return &(gs_stCommandList[iIndex]);
}

void kTerminalCommandHelp(const char* pcArgument){
    TERMINALCOMMANDENTRY* pTerminalCmd;
    int i;
    for(i=0; (pTerminalCmd=kTerminalGetCMDEntry(i))!=NULL; i++){
        kprintf("%s\t", pTerminalCmd->pcCommand);
        if(kstrlen(pTerminalCmd->pcCommand)<8)
            kprintf("%c", '\t');
        kprintf("%s\n", pTerminalCmd->pcHelp);
    }
}
void kTerminalCommandClear(const char* pcArgument){
    kTerminalClear();
    kTerminalSetAttribute(TERMINAL_DEFAULT_COLOR);
    kTerminalSetCursorOffset(0);
}
void kTerminalCommandShowTotalRamSize(const char* pcArgumnet){
    kprintf("Total RAM size = %d MB\n", kGetTotalRamSize());
}
void kTerminalCommandStringToDeciHexConvert(const char* pcArgument){
    char vcArgument[100];
    int i=0;
    ARGUMENTLIST aList;
    kInitializeArgumentList(&aList, pcArgument);
    while(1){
        if(kGetNextArgumnet(&aList, vcArgument)==0){
            break;
        }
        kprintf("Argument%d = %s, ", i+1, vcArgument);
        i++;
        if(kMemCmp(vcArgument, "0x", 2)==0){
            long lValue=katoi(vcArgument+2, 16);
            kprintf("HEX value = 0x%q, Decimal value = %d\n", lValue, lValue);
        }
        else{
            long lValue=katoi(vcArgument, 10);
            kprintf("Decimal value = %d, Hex value = 0x%q\n", lValue, lValue);
        }
    }
}

void kTerminalCommandShutdown(const char* pcArgument){
    kprintf("System Shutdown Start\n");
    kprintf("Press Any Key To Reboot PC");
    while(1){
        KEYDATA keyData;
        if(kGetKeyFromKeyQueue(&keyData)){
            break;
        }
    }
    kReboot();
}

void kTerminalCommandWait(const char* pcArgument){
    ARGUMENTLIST argumentList;
    char pcBuffer[100];
    kInitializeArgumentList(&argumentList, pcArgument);
    if(kGetNextArgumnet(&argumentList, pcBuffer)!=0){
        int iWait=katoi(pcBuffer, 10);
        kprintf("wait %d ms.\n", iWait);
        kWaitms(iWait);
    }
    else{
        kprintf("input the time for wait\n");
    }

}


void kTerminalCommandShowDateAndTime(const char* pcArgument){
    BYTE bHour, bMinute, bSecond;
    BYTE bMonth, bDayOfMonth, bDayOfWeek;
    WORD wYear;
    kReadRTCTime(&bHour, &bMinute, &bSecond);
    kReadRTCDate(&wYear, &bMonth, &bDayOfMonth, &bDayOfWeek);
    kprintf("Date : %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth, kConvertDayOfWeekToString(bDayOfWeek));
    kprintf("Time : %d:%d:%d\n", bHour, bMinute, bSecond);
}


void kTermianlCommandMeasureCPUSpeed(const char* pcArgument){
    QWORD qwCPUCount;
    kprintf("Measureing CPU Speed......\n");
    qwCPUCount=kMeasureCPUSpeed();
    kprintf("CPU Frequency : %dMHz\n", qwCPUCount/1000/1000);
}

void kTerminalCommandStartTimer(const char* pcArgument){
    ARGUMENTLIST argumentList;
    char pcBuffer[100];
    int millisecond;
    BOOL bPeriodic;
    kInitializeArgumentList(&argumentList, pcArgument);
    if(kGetNextArgumnet(&argumentList, pcBuffer)!=0){
        millisecond=katoi(pcBuffer, 10);
        bPeriodic=FALSE;
        if(kGetNextArgumnet(&argumentList, pcBuffer)!=0)
            bPeriodic=katoi(pcBuffer, 10);
        kInitializePIT(MSTOCOUNT(millisecond), bPeriodic);
    }
}

static TCB gs_stTaskControlBlock[2]={0,};
static QWORD gs_vstStack[1024]={0,};

void kTestTask1(){
    BYTE bData;
    int i=0, iX=0, iY=0, iMargin;
    int j;
    CHARACTER* pstScreen=(CHARACTER*) TERMINAL_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;
    pstRunningTask=kGetRunningTask();
    iMargin=((pstRunningTask->stLink.qwID&0xFFFFFFFF)-1)%10;
    bData=pstRunningTask->stLink.qwID;
    iX=iMargin-1;
    iY=iMargin;
    //kprintf("offset = %d \n", iMargin);
    for(j=0; j<20000; j++){
        switch(i){
            case 0:
                iX++;
                if(iX>=(TERMINAL_WIDTH-1-iMargin)){
                    i=1;
                }
                break;
            case 1:
                iY++;
                if(iY>=(TERMINAL_HEIGHT-1-iMargin)){
                    i=2;
                }
                break;
            case 2:
                iX--;
                if(iX<=iMargin){
                    i=3;
                }
                break;
            case 3:
                iY--;
                if(iY<=iMargin){
                    i=0;
                }
                break;
        }
        pstScreen[iY*TERMINAL_WIDTH+iX].bChar=bData;
        pstScreen[iY*TERMINAL_WIDTH+iX].bAttrib=bData&0x0F;
        bData++;
        //kSchedule();
    }
    //kExitTask();
}

void kTestTask2(){
    int i, iOffset;
    int j;
    TCB* pstRunningTask;
    char vcData[4]={'/', '-', '\\', '|'};
    CHARACTER* pstScreen=(CHARACTER*) TERMINAL_VIDEOMEMORYADDRESS;
    pstRunningTask=kGetRunningTask();
    iOffset=(pstRunningTask->stLink.qwID&0xFFFFFFFF)-2;
    iOffset=TERMINAL_WIDTH*TERMINAL_HEIGHT-(iOffset%(TERMINAL_WIDTH*TERMINAL_HEIGHT))-1;
    for(j=0; j<20000; j++){
        pstScreen[iOffset].bChar=vcData[i%4];
        pstScreen[iOffset].bAttrib=iOffset%15+1;
        i++;
        //kSchedule();
    }
    //kExitTask();
}

void kTerminalCommandCreateTask(const char* pcArgument){
    ARGUMENTLIST argumentList;
    char vcBuffer[100];
    int itaskNumber, itaskCount, i;
    QWORD qwTaskAddress;
    kInitializeArgumentList(&argumentList, pcArgument);
    if(kGetNextArgumnet(&argumentList, vcBuffer)==0){
        kprintf("createtask 1(taskNo) 3(taskCount)\n");
        return;
    }
    itaskNumber=katoi(vcBuffer, 10);
    switch(itaskNumber){
        case 1:
            qwTaskAddress=(QWORD)kTestTask1;
            break;
        case 2:
        default:
            qwTaskAddress=(QWORD)kTestTask2;
        break;
    }
    if(kGetNextArgumnet(&argumentList, vcBuffer)==0){
        kprintf("createtask 1(taskNo) 3(taskCount)\n");
        return;
    }
    itaskCount=katoi(vcBuffer, 10);
    for(i=0; i<itaskCount; i++){
        kCreateTask(TASK_FLAG_LOW|TASK_FLAG_THREAD, qwTaskAddress, 0, QWORD_MAX);
    }
}

void kTerminalCommandStopOtherTasks(const char* pcArgument){
    kClearOtherTask();
    kprintf("stop interrupt\n");
}

void kTerminalCommandShowTaskList(const char* pcArgument){
    int i;
    TCB* pstTCB;
    int iCount=0;
    for(i=0; i<TASK_MAXCOUNT; i++){
        pstTCB=kGetTCBFromTCBPool(i);
        if(ISTASKALLOCATED(pstTCB->stLink.qwID)){
            if((iCount!=0)&&(iCount%10==0)){
                kprintf("Press any key to continue....('q' is exit) : ");
                if(kGetChar()=='q'){
                    kprintf("\n");
                    break;
                }
                kprintf("\n");
            }
            kprintf("[%d] Task ID[0x%q], Priority[%d], Flags[0x%q]\n", 1+iCount, pstTCB->stLink.qwID, GETPRIORITY(pstTCB->qwFlags), pstTCB->qwFlags);
            iCount++;
        }
    }
}

void kTerminalCommandTestLinkedList(const char* pcArgument){
    LIST stLinkedList={0,};
    LISTLINK links[5]={0,};
    LISTLINK* pstLink;
    int i;
    stLinkedList.pvHead=0;
    stLinkedList.pvTail=0;
    stLinkedList.iItemCount=0;
    for(i=0; i<5; i++){
        links[i].qwID=i+1;
        links[i].pvNext=NULL;
        kAddLinkToTail(&stLinkedList, &links[i]);
    }
    kprintf("Links are added\n");
    for(pstLink=stLinkedList.pvHead; pstLink!=NULL; pstLink=pstLink->pvNext){
        kprintf("List ID : %d\n", pstLink->qwID);
    }
    kRemoveLink(&stLinkedList, 3);
    kprintf("Links are removed\n");
    while(1){
        pstLink=kRemoveLinkFromHead(&stLinkedList);
        if(pstLink==NULL)
            break;
        kprintf("List ID : %d\n", pstLink->qwID);
    }
}

void kTerminalCommandGetProcessorLoad(const char* pcArgument){
    kprintf("CPU Load : %d%%\n", kGetProcessorLoad());
}

void kTerminalCommandChangePriority(const char* pcArgument){
    ARGUMENTLIST argumentList;
    QWORD qwID;
    BYTE bPriority, bPreviousPriority;
    char vcBuffer[100];
    TCB* pstTargetTask;
    kInitializeArgumentList(&argumentList, pcArgument);
    if(kGetNextArgumnet(&argumentList, vcBuffer)==0){
        kprintf("chpri 0x30002(ID) 3(priority)\n");
        return;
    }
    if(kMemCmp(vcBuffer, "0x", 2)==0){
        qwID=katoi(vcBuffer+2, 16);
    }
    else{
        qwID=katoi(vcBuffer, 10);
    }
    pstTargetTask=kGetTCBFromTCBPool(GETTCBOFFSET(qwID));
    if((pstTargetTask==NULL)){
        kprintf("Cannot find task[ID=0x%q]\n", qwID);
        return;
    }
    if(kGetNextArgumnet(&argumentList, vcBuffer)==0){
        kprintf("chpri 0x300029(ID) 3(priority)");
        return;
    }
    if(kMemCmp(vcBuffer, "0x", 2)==0){
        bPriority=katoi(vcBuffer+2, 16);
    }
    else{
        bPriority=katoi(vcBuffer, 10);
    }
    bPreviousPriority=GETPRIORITY(pstTargetTask->qwFlags);
    if(kChangePriority(pstTargetTask->stLink.qwID, bPriority)){
        kprintf("Task[ID 0x%q] priority [%d->%d]\n", pstTargetTask->stLink.qwID, bPreviousPriority, bPriority);
    }
    else{
        kprintf("Task[ID 0x%q] priority [%d->%d] failed.\n", pstTargetTask->stLink.qwID, bPreviousPriority, bPriority);
    }
}

void kTerminalCommandKillTask(const char* pcArgument){
    ARGUMENTLIST argumentList;
    QWORD qwID;
    char vcBuffer[100];
    TCB* pstTargetTask;
    kInitializeArgumentList(&argumentList, pcArgument);
    if(kGetNextArgumnet(&argumentList, vcBuffer)==0){
        kprintf("killtask 0x30002(ID)\n");
        return;
    }
    if(kMemCmp(vcBuffer, "0x", 2)==0){
        qwID=katoi(vcBuffer+2, 16);
    }
    else{
        qwID=katoi(vcBuffer, 10);
    }
    if(qwID!=0xFFFFFFFF){
        pstTargetTask=kGetTCBFromTCBPool(GETTCBOFFSET(qwID));
        if((pstTargetTask==NULL)){
            kprintf("Cannot find task[ID=0x%q]\n", qwID);
            return;
        }
        if(kEndTask(pstTargetTask->stLink.qwID)){
            kprintf("Task[ID 0x%q] stopped\n", pstTargetTask->stLink.qwID);
        }
        else{
            kprintf("Task[ID 0x%q] stopped abnormaly.\n", pstTargetTask->stLink.qwID);
        }
    }
    else{
        int i;
        for(i=0; i<TASK_MAXCOUNT; i++){
            pstTargetTask=kGetTCBFromTCBPool(i);
            if(ISTASKALLOCATED(pstTargetTask->stLink.qwID)){
                if(pstTargetTask->qwFlags&TASK_FLAG_SYSTEM)
                    continue;
                kprintf("Task[ID 0x%q] stoppoed ", pstTargetTask->stLink.qwID);
                if(kEndTask(pstTargetTask->stLink.qwID)){
                    kprintf("\n");
                }
                else{
                    kprintf("abnormaly\n");
                }
            }
        }
    }
}

void kTerminalCommandTestFPU(const char* pcArgument){
    double a, b, c;
    a=0.1;
    b=1.1;
    c=(a+b);
    kprintf("0.1 + 1.1 = %f\n", VAFLOAT(c));
}


void kTerminalCommandTestDynamicMemory(const char* pcArgument){
    char vcBuffer[1024];
    int iLen=0;
    char* vcMemory=kAllocateDynamicMemory(1024);
    kFreeDynamicMemory(vcMemory);
    vcMemory=kAllocateDynamicMemory(1024*10);
    iLen=kGetDynamicMemoryInfo(NULL, 0);
    kGetDynamicMemoryInfo(vcBuffer, iLen<sizeof(vcBuffer)?iLen:sizeof(vcBuffer));
    kprintf("%s", vcBuffer);
    kprintf("Allocated at 0x%q\n", (QWORD)vcMemory);
}