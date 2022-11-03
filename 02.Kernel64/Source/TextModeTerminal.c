#include "TextModeTerminal.h"
#include "AssemblyUtility.h"

static TERMINALMANAGER gs_stTerminalManager={0,};

void kClearTerminal(BYTE attrib, BOOL bClearChar){
    int i;
    kCharStruct* pVGAData=(kCharStruct*) 0xB8000;
    for(i=0; i<TERMINAL_WIDTH*TERMINAL_HEIGHT; i++){
        if(bClearChar)
            pVGAData->bChar=0;
        pVGAData->bAttrib=attrib;
        pVGAData++;
    }
    gs_stTerminalManager.bAttrib=attrib;
}

void kDataIntoTerminal(BYTE keyCode){
    kCharStruct* pVGAData=(kCharStruct*) 0xB8000;
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
    kOutPortByte(0x3D4, 0x0F);
    kOutPortByte(0x3D5, pos&0xFF);
    kOutPortByte(0x3D4, 0x0E);
    kOutPortByte(0x3D5, pos>>8);
}