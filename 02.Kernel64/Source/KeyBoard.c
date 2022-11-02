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
    {KEY_TAB, KEY_TAB},     //0x0F
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
    {KEY_ENTER, KEY_ENTER},//0x1C
    {KEY_CTRL, KEY_CTRL},//0x1D
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
    {KEY_SHIFT, KEY_SHIFT},             //0x2A
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
    {KEY_SHIFT, KEY_SHIFT}, //0x36
    {KEY_PTSCR, KEY_PTSCR}, //0x37
    {KEY_ALT, KEY_ALT},     //0x38
    {KEY_SPACE, KEY_SPACE}, //0x39
    {KEY_CPASLOCK, KEY_CAPSLOCK},   //0x3A
    {KEY_F1, KEY_F1},       //0x3B
};