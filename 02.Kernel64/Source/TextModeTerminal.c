#include <stdarg.h>
#include "TextModeTerminal.h"
#include "AssemblyUtility.h"
#include "Keyboard.h"
#include "string.h"
#include "Utility.h"

static TERMINALMANAGER gs_stTerminalManager={0,};
static TERMINALCOMMANDENTRY gs_stCommansList[]={
    {"help", "Show Help", kTerminalCommandHelp},
    {"cls", "Clear Screen", kTerminalCommandClear},
    {"totalram", "Show Total RAM Size", kTerminalCommandShowTotalRamSize},
    {"strtod", "String to Decimal/Hex Convert", kTerminalCommandStringToDeciHexConvert},
    {"shutdown", "Reboot PC", kTerminalCommandShutdown},
};

void kTerminalClear(BYTE attrib, BOOL bClearChar){
    int i;
    kCharStruct* pVGAData=(kCharStruct*) TERMINAL_VIDEOMEMORYADDRESS;
    for(i=0; i<TERMINAL_WIDTH*TERMINAL_HEIGHT; i++){
        if(bClearChar)
            pVGAData->bChar=0;
        pVGAData->bAttrib=attrib;
        pVGAData++;
    }
    if(bClearChar){
        gs_stTerminalManager.iOffset=0;
    }
    gs_stTerminalManager.bAttrib=attrib;
}

static char gs_vcCommandBuffer[300]={0,};
static int gs_iCommandBufferIndex=0;
void kDataIntoTerminal(const KEYDATA* pstKeyData){
    BYTE bASCIICode=pstKeyData->bASCIICode;
    if(bASCIICode==KEY_ENTER){
        kprintf("\n");
        if(gs_iCommandBufferIndex>0){
            gs_vcCommandBuffer[gs_iCommandBufferIndex]=0;
            kTerminalExecuteCommand(gs_vcCommandBuffer);
        }
        kMemSet(gs_vcCommandBuffer, 0, gs_iCommandBufferIndex);
        gs_iCommandBufferIndex=0;
        kprintf("%s", TERMINAL_PREFIX);
    }
    else if(bASCIICode==KEY_BS){
        if(gs_iCommandBufferIndex>0){
            WORD iCursorX, iCursorY;
            kTerminalGetCursorPos(&iCursorX, &iCursorY);
            kTerminalPrintString(iCursorX-1, iCursorY, " ");
            kTerminalSetCursorPos(iCursorX-1, iCursorY);
            gs_iCommandBufferIndex--;
        }
    }
    else if(bASCIICode==KEY_LSHIFT||bASCIICode==KEY_RSHIFT||bASCIICode==KEY_LCTRL||bASCIICode==KEY_RCTRL||bASCIICode==KEY_LALT||bASCIICode==KEY_RALT||
        bASCIICode==KEY_NUMLOCK||bASCIICode==KEY_SCROLL_LOCK||bASCIICode==KEY_CAPSLOCK){

    }
    else{
        if(bASCIICode==KEY_TAP){
            bASCIICode=' ';
        }
        gs_vcCommandBuffer[gs_iCommandBufferIndex]=bASCIICode;
        gs_iCommandBufferIndex++;
        kprintf("%c", bASCIICode);
    }
}

void kTerminalSetCursorPos(WORD wCursorX, WORD wCursorY){
    gs_stTerminalManager.iOffset=wCursorX+wCursorY*TERMINAL_WIDTH;
    kUpdateCursorPos();
}

void kTerminalGetCursorPos(WORD* pwCursorX, WORD* pwCursorY){
    *pwCursorX=gs_stTerminalManager.iOffset%TERMINAL_WIDTH;
    *pwCursorY=gs_stTerminalManager.iOffset/TERMINAL_WIDTH;
}

void kTerminalSetCursorOffset(int iOffset){
    gs_stTerminalManager.iOffset=iOffset;
    kUpdateCursorPos();
}

int kTerminalGetCursorOffset(){
    return gs_stTerminalManager.iOffset;
}

void kTerminalMoveCursor(int iStep){
    gs_stTerminalManager.iOffset+=iStep;
    kUpdateCursorPos();
}

void kUpdateCursorPos(){
    WORD pos=(WORD) gs_stTerminalManager.iOffset;
    kOutPortByte(VGA_PORT_INDEX, VGA_INDEX_LOWERCURSOR);
    kOutPortByte(VGA_PORT_DATA, pos&0xFF);
    kOutPortByte(VGA_PORT_INDEX, VGA_INDEX_UPPERCURSOR);
    kOutPortByte(VGA_PORT_DATA, pos>>8);
}

void kTerminalScrollUp(){
    int i;
    kCharStruct* pVGAData=(kCharStruct*)TERMINAL_VIDEOMEMORYADDRESS;
    for(i=1; i<TERMINAL_HEIGHT; i++){
        kMemCpy(pVGAData, ((char*) pVGAData)+TERMINAL_WIDTH*sizeof(kCharStruct), TERMINAL_WIDTH*sizeof(kCharStruct));
        pVGAData+=TERMINAL_WIDTH;
    }
    for(i=0; i<TERMINAL_WIDTH; i++){
        pVGAData->bChar=0;
        pVGAData++;
    }
}

int kTerminalPrintString(WORD iCursorX, WORD iCursorY, const char* str){
    int i, iOffset;
    kCharStruct* pVGAData=(kCharStruct*)TERMINAL_VIDEOMEMORYADDRESS;
    iOffset=iCursorX+iCursorY*TERMINAL_WIDTH;
    for(i=0; str[i]!=0; i++){
        if(str[i]=='\n'){
            iOffset+=TERMINAL_WIDTH-(iOffset%TERMINAL_WIDTH);
        }
        else if(str[i]=='\t'){
            iOffset+=TERMINAL_TAP_SIZE-(iOffset%TERMINAL_TAP_SIZE);
        }
        else{
            pVGAData[iOffset].bChar=str[i];
            iOffset++;
        }
        while(iOffset>=TERMINAL_HEIGHT*TERMINAL_WIDTH){
            kTerminalScrollUp();
            iOffset-=TERMINAL_WIDTH;
        }
    }
    return iOffset;
}

int kprintf(const char* pcFormatString, ...){
    int iReturn;
    char vcBuffer[100];
    int iOffset;
    WORD wCursorX, wCursorY;
    va_list valist;
    va_start(valist, pcFormatString);
    iReturn=kVSPrintf(vcBuffer, pcFormatString, valist);
    va_end(valist);
    kTerminalGetCursorPos(&wCursorX, &wCursorY);
    iOffset=kTerminalPrintString(wCursorX, wCursorY, vcBuffer);
    kTerminalSetCursorOffset(iOffset);
    return iReturn;
}


void kInitializeArgumentList(ARGUMENTLIST* pstArgumentList, const char* pcBuffer){
    pstArgumentList->iCurrentIndex=0;
    pstArgumentList->iLength=kstrlen(pcBuffer);
    pstArgumentList->pcBuffer=pcBuffer;
}
int kGetNextArgumnet(ARGUMENTLIST* pstArgumentList, char* pcArgument){
    int iLen=0;
    int i;
    if(pstArgumentList->iCurrentIndex>=pstArgumentList->iLength)
        return 0;
    for(; pstArgumentList->iCurrentIndex<pstArgumentList->iLength; pstArgumentList->iCurrentIndex++){
        if(pstArgumentList->pcBuffer[pstArgumentList->iCurrentIndex]!=' ')
            break;
    }
    if(pstArgumentList->iCurrentIndex>=pstArgumentList->iLength)
        return 0;
    for(; pstArgumentList->iCurrentIndex<pstArgumentList->iLength; pstArgumentList->iCurrentIndex++){
        if(pstArgumentList->pcBuffer[pstArgumentList->iCurrentIndex]==' ')
            break;
        pcArgument[iLen]=pstArgumentList->pcBuffer[pstArgumentList->iCurrentIndex];
        iLen++;
    }
    pcArgument[iLen]=0;
    return iLen;
}

void kStartTerminal(){
    kprintf(TERMINAL_PREFIX);
    while(1){
        KEYDATA stKeyData;
        if(kGetKeyFromKeyQueue(&stKeyData)){    
            kDataIntoTerminal(&stKeyData);
        }
    }
}

void kTerminalSearchCommandEntryAndSpaceIndex(const char* pcCommandBuffer, TERMINALCOMMANDENTRY** ppstTerminalCmd, int* piSpaceIndex){
    int i, iSpaceIndex, iCount;
    int iCommandBufferLen=kstrlen(pcCommandBuffer);
    *ppstTerminalCmd=NULL;
    *piSpaceIndex=0;
    for(iSpaceIndex=0; iSpaceIndex<iCommandBufferLen; iSpaceIndex++){
        if(pcCommandBuffer[iSpaceIndex]==' ')
            break;
    }
    iCount=sizeof(gs_stCommansList)/sizeof(TERMINALCOMMANDENTRY);
    for(i=0; i<iCount; i++){
        if(kstrlen(gs_stCommansList[i].pcCommand)==iSpaceIndex){
            if(kMemCmp(gs_stCommansList[i].pcCommand, pcCommandBuffer, iSpaceIndex)==0){
                if(ppstTerminalCmd!=NULL)
                    *ppstTerminalCmd = &(gs_stCommansList[i]);
                if(piSpaceIndex!=NULL)
                    *piSpaceIndex=iSpaceIndex;
            }
        }
    }
}

void kTerminalExecuteCommand(const char* pcCommandBuffer){
    TERMINALCOMMANDENTRY* pstCommandEntry;
    int iSpaceIndex;
    kTerminalSearchCommandEntryAndSpaceIndex(pcCommandBuffer, &pstCommandEntry, &iSpaceIndex);
    if(pstCommandEntry!=NULL){
        pstCommandEntry->pfFunction(pcCommandBuffer+iSpaceIndex+1);
    }
    else
        kprintf("%s is not found.\n", pcCommandBuffer);
}

void kTerminalCommandHelp(const char* pcArgument){
    TERMINALCOMMANDENTRY* pTerminalCmd;
    int iSpaceIndex;
    kTerminalSearchCommandEntryAndSpaceIndex(pcArgument, &pTerminalCmd, &iSpaceIndex);
    if(pTerminalCmd!=NULL){
        kprintf("%s\n", pTerminalCmd->pcHelp);
    }
    else{
        kprintf("%s is not found\n", pcArgument);
    }
}
void kTerminalCommandClear(const char* pcArgument){
    kTerminalClear(gs_stTerminalManager.bAttrib, TRUE);
    kTerminalSetCursorOffset(0);
    kprintf("%s", TERMINAL_PREFIX);
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