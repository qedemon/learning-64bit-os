#ifndef TEXTMODETERMINAL_H
#define TEXTMODETERMINAL_H
#include "Type.h"
#include "Keyboard.h"

#define TERMINAL_BACKGROUND_BLACK 0x00
#define TERMINAL_BACKGROUND_BLUE 0x10
#define TERMINAL_BACKGROUND_GREEN 0x20
#define TERMINAL_BACKGROUND_CYAN 0x30
#define TERMINAL_BACKGROUND_RED 0x40
#define TERMINAL_BACKGROUND_MAGENTA 0x50
#define TERMINAL_BACKGROUND_BROWN 0x60
#define TERMINAL_BACKGROUND_WHITE 0x70
#define TERMINAL_BACKGROUND_BLINK 0x80
#define TERMINAL_FOREGROUND_BLACK 0x00
#define TERMINAL_FOREGROUND_BLUE 0x01
#define TERMINAL_FOREGROUND_GREEN 0x02
#define TERMINAL_FOREGROUND_CYAN 0x03
#define TERMINAL_FOREGROUND_RED 0x04
#define TERMINAL_FOREGROUND_MAGENTA 0x05
#define TERMINAL_FOREGROUND_BROWN 0x06
#define TERMINAL_FOREGROUND_WHITE 0x07
#define TERMINAL_FOREGROUND_BRIGHT 0x08

#define TERMINAL_DEFAULT_COLOR TERMINAL_BACKGROUND_BLACK|TERMINAL_FOREGROUND_BRIGHT|TERMINAL_FOREGROUND_GREEN


#define TERMINAL_WIDTH 80
#define TERMINAL_HEIGHT 25
#define TERMINAL_VIDEOMEMORYADDRESS 0xB8000
#define VGA_PORT_INDEX 0x3D4
#define VGA_PORT_DATA 0x3D5
#define VGA_INDEX_UPPERCURSOR 0x0E
#define VGA_INDEX_LOWERCURSOR 0x0F

#define TERMINAL_PREFIX "MINT64>"
#define TERMINAL_TAP_SIZE 8

typedef struct kTextTerminalManager{
    int iOffset;
    BYTE bAttrib;
}TERMINALMANAGER;

void kTerminalClear();
BYTE kTerminalGetAttribute();
void kTerminalSetAttribute(BYTE bAttrib);
void kTerminalSetCursorOffset(int iOffset);
int kTerminalGetCursorOffset();
void kTerminalSetCursorPos(WORD iCursorX, WORD iCursorY);
void kTerminalGetCursorPos(WORD* pwCursorX, WORD* pwCursorY);

void kTerminalMoveCursor(int iStep);
void kUpdateCursorPos();
void kDataIntoTerminal(const KEYDATA* pstKeyData);

int kTerminalPrintString(WORD iCursorX, WORD iCursorY, const char* str);
int kprintf(const char* pcFormatString, ...);
char kGetChar();

void kStartTerminal();

void kTerminalExecuteCommand(const char* pcCommandBuffer);


#endif