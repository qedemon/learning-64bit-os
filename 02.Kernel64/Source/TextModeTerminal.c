#include <stdarg.h>
#include "TextModeTerminal.h"
#include "AssemblyUtility.h"

static TERMINALMANAGER gs_stTerminalManager={0,};

void kClearTerminal(BYTE attrib, BOOL bClearChar){
    int i;
    kCharStruct* pVGAData=(kCharStruct*) TERMINAL_VIDEOMEMORYADDRESS;
    for(i=0; i<TERMINAL_WIDTH*TERMINAL_HEIGHT; i++){
        if(bClearChar)
            pVGAData->bChar=0;
        pVGAData->bAttrib=attrib;
        pVGAData++;
    }
    gs_stTerminalManager.bAttrib=attrib;
}

void kDataIntoTerminal(BYTE keyCode){
    kCharStruct* pVGAData=(kCharStruct*) TERMINAL_VIDEOMEMORYADDRESS;
    pVGAData+=gs_stTerminalManager.iCursorX+gs_stTerminalManager.iCursorY*TERMINAL_WIDTH;
    pVGAData->bChar=keyCode;
    gs_stTerminalManager.iCursorX++;
    if(gs_stTerminalManager.iCursorX>=TERMINAL_WIDTH){
        gs_stTerminalManager.iCursorY++;
        gs_stTerminalManager.iCursorX=0;
    }
    if(gs_stTerminalManager.iCursorY>=TERMINAL_HEIGHT){
        gs_stTerminalManager.iCursorY=0;
    }
    kUpdateCursorPos();
}

void kMoveCursorPos(WORD iCursorX, WORD iCursorY){
    gs_stTerminalManager.iCursorX=iCursorX;
    gs_stTerminalManager.iCursorY=iCursorY;
    kUpdateCursorPos();
}
void kUpdateCursorPos(){
    WORD pos=gs_stTerminalManager.iCursorX+gs_stTerminalManager.iCursorY*TERMINAL_WIDTH;
    kOutPortByte(VGA_PORT_INDEX, VGA_INDEX_LOWERCURSOR);
    kOutPortByte(VGA_PORT_DATA, pos&0xFF);
    kOutPortByte(VGA_PORT_INDEX, VGA_INDEX_UPPERCURSOR);
    kOutPortByte(VGA_PORT_DATA, pos>>8);
}