#ifndef KEYBORD_H
#define KEYBORD_H
#include "Type.h"

#define KEY_NONE 0
#define KEY_ENTER '\n'
#define KEY_TAP '\t'
#define KEY_ESC 0x1B
#define KEY_BS 0x08
#define KEY_SPACE 0x20

#define KEY_LCTRL 0x81
#define KEY_LSHIFT 0x82
#define KEY_LALT 0x83
#define KEY_RCTRL 0x84
#define KEY_RSHIFT 0x85
#define KEY_RALT 0x86
#define KEY_PTSCR 0x87

#define KEY_F1 0x88
#define KEY_F2 0x89
#define KEY_F3 0x8A
#define KEY_F4 0x8B
#define KEY_F5 0x8C
#define KEY_F6 0x8D
#define KEY_F7 0x8E
#define KEY_F8 0x8F
#define KEY_F9 0x90
#define KEY_F10 0x91
#define KEY_F11 0x92
#define KEY_F12 0x93

#define KEY_CAPSLOCK 0x94
#define KEY_NUMLOCK 0x95
#define KEY_SCROLL_LOCK 0x96

#define KEY_UP  0x97
#define KEY_DOWN 0x98
#define KEY_LEFT 0x99
#define KEY_RIGHT 0x9A
#define KEY_CENTER 0x9B

#define KEY_INSERT 0x9C
#define KEY_DELETE 0x9D
#define KEY_HOME 0x9E
#define KEY_END 0x9F
#define KEY_PGUP 0xA0
#define KEY_PGDOWN 0xA1

#define KEY_PAUSE 0xA2

#define KEYCODE_EXTENDED 0xE0
#define KEYCODE_PAUSE 0xE1

typedef struct kKeyMappingEntryStruct{
    BYTE bNormalCode;
    BYTE bShiftedCode;
}KEYMAPPINGENTRY;
#define KEYMAPPINGTABLEMAXCOUNT 0x59

typedef struct kKeyBoradManagerStruct{
    BOOL bShiftDown;
    BOOL bCapsLockOn;
    BOOL bNumLockOn;
    BOOL bScrollLockOn;

    BOOL bExtendedCodeIn;
    int iSkipCountForPause;
}KEYBOARDMANAGER;

typedef struct kKeyDataStruct{
    BYTE bScanCode;
    BYTE bASCIICode;
    BYTE bFlags;
}KEYDATA;

BOOL kIsInputBufferFull();
BOOL kIsOutputBufferFull();

BOOL kIsKeyInAlphabet(BYTE keyCode);
BOOL kIsKeyInNuberOrSp(BYTE keyCode);
BOOL kIsKeyInNumberPad(BYTE keyCode);

BYTE kGetKeyBoardScanCode();

BOOL kActivateKeyBoard();
BOOL kChangeKeyboardLeds(BOOL bCapsLockOn, BOOL bNumLockOn, BOOL bScrollLockOn);
BOOL kUpdateKeyboardLeds();
BOOL kUpdateKeyBoardManager(BYTE bScanCode, BYTE* pbOutputKeyCode);

BOOL kInitializeKeyBoard();
BOOL kUpdateKeyBoardManagerAndPutKeyDatatToQueue(BYTE bScanCode);
BOOL kGetKeyFromKeyQueue(KEYDATA* pstOutput);

BOOL kWaitForACKAndPutOtherScanCode();


void kReboot();

#endif