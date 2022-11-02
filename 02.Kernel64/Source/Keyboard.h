#ifndef KEYBORD_H
#define KEYBORD_H
#include "Type.h"

#define KEY_NONE 0
#define KEY_ENTER '\n'
#define KEY_TAP '\t'
#define KEY_ESC 0x1B
#define KEY_BS 0x08
#define KEY_SPACE 0x20
#define KEY_CTRL 0x81
#define KEY_SHIFT 0x82
#define KEY_PTSCR 0x83
#define KEY_ALT 0x84
#define KEY_CPASLOCK 0x85

typedef struct kKeyMappingEntryStruct{
    BYTE bNarmalCode;
    BYTE bShiftedCode;
}KEYMAPPINGENTRY;
#define KEYMAPPINGTABLEMAXCOUNT 0x59

BOOL kIsInputBufferFull();
BOOL kIsOutputBufferFull();

BOOL kActivateKeyBoard();
BOOL kChangeKeyboardLeds(BOOL bCapsLockOn, BOOL bNumLockOn, BOOL bScrollLockOn);

#endif