#ifndef TEXTMODETERMINAL_H
#define TEXTMODETERMINAL_H
#include "Type.h"

#define TERMINAL_WIDTH 80
#define TERMINAL_HEIGHT 25

typedef struct kTextTerminalManager{
    WORD iCursorX;
    WORD iCursorY;
    BYTE bAttrib;
}TERMINALMANAGER;

void kClearTerminal(BYTE attrib, BOOL bClearChar);
void kMoveCursorPos(WORD iCursorX, WORD iCursorY);
void kUpdateCursorPos();
void kDataIntoTerminal(BYTE keyCode);

#endif