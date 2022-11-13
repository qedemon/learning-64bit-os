#include <stdarg.h>
#include "TextModeTerminal.h"
#include "AssemblyUtility.h"
#include "Keyboard.h"
#include "PIT.h"
#include "string.h"
#include "Utility.h"
#include "TerminalCommand.h"

static TERMINALMANAGER gs_stTerminalManager={0,};

void kTerminalClear(){
    int i;
    CHARACTER* pVGAData=(CHARACTER*) TERMINAL_VIDEOMEMORYADDRESS;
    for(i=0; i<TERMINAL_WIDTH*TERMINAL_HEIGHT; i++){
        pVGAData->bChar=0;
        pVGAData++;
    }
    gs_stTerminalManager.iOffset=0;
}


BYTE kTerminalGetAttribute(){
    return gs_stTerminalManager.bAttrib;
}
void kTerminalSetAttribute(BYTE bAttrib){
    int i;
    gs_stTerminalManager.bAttrib=bAttrib;
    CHARACTER* pVGAData=(CHARACTER*) TERMINAL_VIDEOMEMORYADDRESS;
    for(i=0; i<TERMINAL_WIDTH*TERMINAL_HEIGHT; i++){
        pVGAData->bAttrib=bAttrib;
        pVGAData++;
    }
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
    CHARACTER* pVGAData=(CHARACTER*)TERMINAL_VIDEOMEMORYADDRESS;
    for(i=1; i<TERMINAL_HEIGHT; i++){
        kMemCpy(pVGAData, ((char*) pVGAData)+TERMINAL_WIDTH*sizeof(CHARACTER), TERMINAL_WIDTH*sizeof(CHARACTER));
        pVGAData+=TERMINAL_WIDTH;
    }
    for(i=0; i<TERMINAL_WIDTH; i++){
        pVGAData->bChar=0;
        pVGAData++;
    }
}

int kTerminalPrintString(WORD iCursorX, WORD iCursorY, const char* str){
    int i, iOffset;
    CHARACTER* pVGAData=(CHARACTER*)TERMINAL_VIDEOMEMORYADDRESS;
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
    pVGAData[iOffset].bChar=0;
    return iOffset;
}

int kprintf(const char* pcFormatString, ...){
    int iReturn;
    char vcBuffer[128];
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

char kGetChar(){
    KEYDATA stKeyData;
    while(1){
        if(kGetKeyFromKeyQueue(&stKeyData))
            break;
    }
    return stKeyData.bASCIICode;
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
