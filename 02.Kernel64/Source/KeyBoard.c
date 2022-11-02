#include "Keyboard.h"
#include "AssemblyUtility.h"

BOOL kIsInputBufferFull(){
    return (kInPortByte(0x64)&0x02!=0);
}
BOOL kIsOutputBufferFull(){
    return (kInPortByte(0x64)&0x01!=0);
}

BOOL kActivateKeyBoard(){
    int i;
    int j;
    kOutPortByte(0x64, 0xAE);

    for(i=0; i<0xFFFF; i++){
        if(!kIsInputBufferFull()){
            break;
        }
    }
    kOutPortByte(0x60, 0xF4);
    for(j=0; j<100; j++){
        for(i=0; j<0xFFFF; j++){
            if(kIsOutputBufferFull())
                break;
        }
        if(kInPortByte(0x60)==0xFA)
            return TRUE;
    }
    return FALSE;
}

BOOL kChangeKeyboardLeds(BOOL bCapsLockOn, BOOL bNumLockOn, BOOL bScrollLockOn){
    int i;
    int j;
    
    for(i=0; i<0xFFFF; i++){
        if(!kIsInputBufferFull())
            break;
    }
    kOutPortByte(0x60, 0xED);
    for(i=0; i<0xFFFF; i++){
        if(!kIsInputBufferFull())
            break;
    }
    for(j=0; j<100; i++){
        for(i=0; i<0xFFFF; i++){
            if(kIsOutputBufferFull())
                break;
        }
        if(kInPortByte(0x60)==0xFA){
            break;
        }
    }
    if(j>=100)
        return FALSE;
    
    kOutPortByte(0x60, (bCapsLockOn<<2)|(bNumLockOn<<1)|(bScrollLockOn));
    for(i=0; i<0xFFFF; i++){
        if(!kIsInputBufferFull())
            break;
    }
    for(j=0; j<100; i++){
        for(i=0; i<0xFFFF; i++){
            if(kIsOutputBufferFull())
                break;
        }
        if(kInPortByte(0x60)==0xFA){
            break;
        }
    }
    if(j>=100)
        return FALSE;
    return TRUE;
}

static KEYMAPPINGENTRY gs_stKeyMappingTable[KEYMAPPINGTABLEMAXCOUNT] = {
    {KEY_NONE, KEY_NONE},   //0x00
    {KEY_ESC, KEY_ESC},     //0x01
    {'1', '!'},             //0x02
    {'2', '@'},             //0x03
    {'3', '#'},             //0x04
    {'4', '$'},             //0x05
    {'5', '%'},             //0x06
    {'6', '^'},             //0x07
    {'7', '&'},             //0x08
    {'8', '*'},             //0x09
    {'9', '('},             //0x0A
    {'0', ')'},             //0x0B
    {'-', '_'},             //0x0C
    {'=', '+'},             //0x0D
    {KEY_BS, KEY_SPACE},    //0x0E
    {'\t', '\t'},            //0x0F
    {'q', 'Q'},             //0x10
    {'w', 'W'},             //0x11
    {'e', 'E'},             //0x12
    {'r', 'R'},             //0x13
    {'t', 'T'},             //0x14
    {'y', 'Y'},             //0x15
    {'u', 'U'},             //0x16
    {'i', 'I'},             //0x17
    {'o', 'O'},             //0x18
    {'p', 'P'},             //0x19
    {'[', '{'},             //0x1A
    {']', '}'},             //0x1B
    {KEY_ENTER, KEY_ENTER}, //0x1C
    {KEY_LCTRL, KEY_RCTRL}, //0x1D
    {'a', 'A'},             //0x1E
    {'s', 'S'},             //0x1F
    {'d', 'D'},             //0x20
    {'f', 'F'},             //0x21
    {'g', 'G'},             //0x22
    {'h', 'H'},             //0x23
    {'j', 'J'},             //0x24
    {'k', 'K'},             //0x25
    {'l', 'L'},             //0x26
    {';', ':'},             //0x27
    {'\'', '\"'},             //0x28
    {'`', '~'},             //0x29
    {KEY_LSHIFT, KEY_LSHIFT},   //0x2A
    {'\\', '|'},             //0x2B       
    {'z', 'Z'},             //0x2C
    {'x', 'X'},             //0x2D
    {'c', 'C'},             //0x2E
    {'v', 'V'},             //0x2F
    {'b', 'B'},             //0x30
    {'n', 'N'},             //0x31
    {'m', 'M'},             //0x32
    {',', '<'},             //0x33
    {'.', '>'},             //0x34
    {'/', '?'},             //0x35
    {KEY_RSHIFT, KEY_RSHIFT}, //0x36
    {'*', KEY_PTSCR}, //0x37
    {KEY_LALT, KEY_RALT},     //0x38
    {KEY_SPACE, KEY_SPACE}, //0x39
    {KEY_CAPSLOCK, KEY_CAPSLOCK},   //0x3A
    {KEY_F1, KEY_F1},       //0x3B
    {KEY_F2, KEY_F2},       //0x3C
    {KEY_F3, KEY_F3},       //0x3D
    {KEY_F4, KEY_F4},       //0x3E
    {KEY_F5, KEY_F5},       //0x3F
    {KEY_F6, KEY_F6},       //0x40
    {KEY_F7, KEY_F7},       //0x41
    {KEY_F8, KEY_F8},       //0x42
    {KEY_F9, KEY_F9},       //0x43
    {KEY_F10, KEY_F10},     //0x44
    {KEY_NUMLOCK, KEY_NUMLOCK}, //0x45
    {KEY_SCROLL_LOCK, KEY_SCROLL_LOCK}, //0x46
    {KEY_HOME, '7'},        //0x47
    {KEY_UP, '8'},          //0x48
    {KEY_PGUP, '9'},        //0x49
    {'-', '-'},             //0x4A
    {KEY_LEFT, '4'},        //0x4B
    {KEY_CENTER, '5'},      //0x4C
    {KEY_RIGHT, '6'},       //0x4D
    {'+', '+'},             //0x4E
    {KEY_END, '1'},         //0x4F
    {KEY_DOWN, '2'},        //0x50
    {KEY_PGDOWN, '3'},      //0x51
    {KEY_INSERT, '0'},      //0x52
    {KEY_DELETE, '.'},      //0x53
    {KEY_NONE, KEY_NONE},   //0x54
    {KEY_NONE, KEY_NONE},   //0x55
    {KEY_NONE, KEY_NONE},   //0x56
    {KEY_F11, KEY_F11},     //0x57
    {KEY_F12, KEY_F12}    //0x58
};

static KEYBOARDMANAGER gs_KeyBoardManager={0,
};

BYTE kGetKeyBoardScanCode(){
    int i;
    for(i=0; i<0xff; i++){
        if(kIsOutputBufferFull())
           break;
    }
    return kInPortByte(0x60);
}

BOOL kUpdateKeyboardLeds(){
    return kChangeKeyboardLeds(gs_KeyBoardManager.bCapsLockOn, gs_KeyBoardManager.bNumLockOn, gs_KeyBoardManager.bScrollLockOn);
}

BOOL kUpdateKeyBoardManager(BYTE bScanCode, BYTE* pbOutputKeyCode){
    BOOL isUpCode;
    BYTE keyCode;
    BOOL extended;
    KEYMAPPINGENTRY keyMappingEntry;

    extended=gs_KeyBoardManager.bExtendedCodeIn;
    gs_KeyBoardManager.bExtendedCodeIn=FALSE;

    if(gs_KeyBoardManager.iSkipCountForPause>0){
        gs_KeyBoardManager.iSkipCountForPause--;
        return FALSE;
    }
    if(bScanCode==KEYCODE_PAUSE){
        gs_KeyBoardManager.iSkipCountForPause=2;
        return FALSE;
    }
    if(bScanCode==KEYCODE_EXTENDED){
        gs_KeyBoardManager.bExtendedCodeIn=TRUE;
        return FALSE;
    }

    isUpCode=((bScanCode&0x80)==0)?FALSE:TRUE;
    keyCode=bScanCode&(0x7F);
    keyMappingEntry=gs_stKeyMappingTable[keyCode];

    if(!isUpCode){
        switch(keyMappingEntry.bNormalCode){
            case KEY_NUMLOCK:
                gs_KeyBoardManager.bNumLockOn=!gs_KeyBoardManager.bNumLockOn;
                kUpdateKeyboardLeds();
                return FALSE;
                break;
            case KEY_CAPSLOCK:
                gs_KeyBoardManager.bCapsLockOn=!gs_KeyBoardManager.bCapsLockOn;
                kUpdateKeyboardLeds();
                return FALSE;
                break;
            case KEY_SCROLL_LOCK:
                gs_KeyBoardManager.bScrollLockOn=!gs_KeyBoardManager.bScrollLockOn;
                kUpdateKeyboardLeds();
                return FALSE;
                break;
            default:
                break;
        }
        
        if(keyMappingEntry.bNormalCode==KEY_LSHIFT||keyMappingEntry.bNormalCode==KEY_RSHIFT){
            gs_KeyBoardManager.bShiftDown=TRUE;
            return FALSE;
        }

        if(kIsKeyInAlphabet(keyCode)){
            *pbOutputKeyCode=(gs_KeyBoardManager.bShiftDown^gs_KeyBoardManager.bCapsLockOn)?keyMappingEntry.bShiftedCode:keyMappingEntry.bNormalCode;
            return TRUE;
        }
        if(kIsKeyInNuberOrSp(keyCode)){
            *pbOutputKeyCode=(gs_KeyBoardManager.bShiftDown)?keyMappingEntry.bShiftedCode:keyMappingEntry.bNormalCode;
            return TRUE;
        }
        if(kIsKeyInNumberPad(keyCode)&&!extended){
            if(gs_KeyBoardManager.bNumLockOn){
                *pbOutputKeyCode=keyMappingEntry.bShiftedCode;
                return TRUE;
            }
        }
    }
    else{
        if(keyMappingEntry.bNormalCode==KEY_LSHIFT||keyMappingEntry.bNormalCode==KEY_RSHIFT){
            gs_KeyBoardManager.bShiftDown=FALSE;
        }
    }


    return FALSE;
}

BOOL kIsKeyInAlphabet(BYTE keyCode){
    return (('a'<= gs_stKeyMappingTable[keyCode].bNormalCode)&&('z'>= gs_stKeyMappingTable[keyCode].bNormalCode));
}
BOOL kIsKeyInNuberOrSp(BYTE keyCode){
    return ((2<=gs_stKeyMappingTable[keyCode].bNormalCode)&&(53>=gs_stKeyMappingTable[keyCode].bNormalCode)&&(!kIsKeyInAlphabet(keyCode)));
}
BOOL kIsKeyInNumberPad(BYTE keyCode){
    return ((keyCode>=0x47)&&(keyCode<=0x53));
}
