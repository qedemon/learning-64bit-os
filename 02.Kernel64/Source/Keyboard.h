#ifndef KEYBORD_H
#define KEYBORD_H
#include "Type.h"

#define KEY_VALUE_NONE 0

BOOL kIsInputBufferFull();
BOOL kIsOutputBufferFull();

BOOL kActivateKeyBoard();
BOOL kChangeKeyboardLeds(BOOL bCapsLockOn, BOOL bNumLockOn, BOOL bScrollLockOn);

#endif