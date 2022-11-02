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