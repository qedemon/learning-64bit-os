#include "Descriptor.h"

void kSetGDTEntry8(GDTENTRY8* pstEntry, DWORD dwBaseAddress, DWORD dwLimit, BYTE bLowerFlags, BYTE bUpperFlags, BYTE bType){
    pstEntry->wLowerLimit=dwLimit&0xFFFF;
    pstEntry->wLowerBaseAddress=dwBaseAddress&0xFFFF;
    pstEntry->bMiddleBaseAddress=(dwBaseAddress>>16)&0xFF;
    pstEntry->bLowerFlagsAndType=bLowerFlags|bType;
    pstEntry->bUpperFlagsAndUpperLimit=((dwLimit>>16)&0xFF)|(bUpperFlags);
    pstEntry->bUpperBaseAddress=(dwBaseAddress>>24)&0xFF;
}

void kSetGDTEntry16(GDTENTRY16* pstEntry, QWORD qwBaseAddress, DWORD dwLimit, BYTE bLowerFlags, BYTE bUpperFlags, BYTE bType){
    pstEntry->wLowerLimit=dwLimit&0xFFFF;
    pstEntry->wLowerBaseAddress=qwBaseAddress&0xFFFF;
    pstEntry->bMiddleBaseAddress0=(dwLimit>>16)&0xFF;
    pstEntry->bLowerFlagsAndType=bType|bLowerFlags;
    pstEntry->bUpperFlagsAndUpperLimit=((dwLimit>>16)&0xFF)|bUpperFlags;
    pstEntry->bMiddleBaseAddress1=(qwBaseAddress>>24)&0xFF;
    pstEntry->dwUpperBaseAddress=(qwBaseAddress>>32)&0xFFFFFFFF;
    pstEntry->dwReserved=0;
}