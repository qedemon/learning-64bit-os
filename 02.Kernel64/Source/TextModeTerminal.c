#include <stdarg.h>
#include "TextModeTerminal.h"
#include "AssemblyUtility.h"
#include "Keyboard.h"
#include "string.h"
#include "Utility.h"
#include "TerminalCommand.h"

static TERMINALMANAGER gs_stTerminalManager={0,};
static TERMINALCOMMANDENTRY gs_stCommansList[]={
    {"help", "Show Help", kTerminalCommandHelp},
    {"cls", "Clear Screen", kTerminalCommandClear},
    {"totalram", "Show Total RAM Size", kTerminalCommandShowTotalRamSize},
    {"strtod", "String to Decimal/Hex Convert", kTerminalCommandStringToDeciHexConvert},
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

void kDataIntoTerminal(const KEYDATA* pstKeyData){
    BYTE bASCIICode=pstKeyData->bASCIICode;
    if(bASCIICode==KEY_ENTER){
        kprintf("\n%s",TERMINAL_PREFIX);
    }
    else if(bASCIICode==KEY_BS){
        WORD iCursorX, iCursorY;
        kTerminalGetCursorPos(&iCursorX, &iCursorY);
        if(iCursorX>=sizeof(TERMINAL_PREFIX)){
            kTerminalPrintString(iCursorX-1, iCursorY, " ");
            kTerminalSetCursorPos(iCursorX-1, iCursorY);
        }
    }
    else
        kprintf("%c", pstKeyData->bASCIICode);
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

void kStartTerminal(){
    kprintf(TERMINAL_PREFIX);
    while(1){
        KEYDATA stKeyData;
        if(kGetKeyFromKeyQueue(&stKeyData)){    
            kDataIntoTerminal(&stKeyData);
        }
    }
}