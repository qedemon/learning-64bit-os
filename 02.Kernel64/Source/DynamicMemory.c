#include "DynamicMemory.h"
#include "Utility.h"
#include "TextModeTerminal.h"

static QWORD kCalculateDynamicMemorySize(){
    QWORD qwTotalRamSize;
    qwTotalRamSize=kGetTotalRamSize()<<20;
    if(qwTotalRamSize>(((QWORD)3)<<30)){
        qwTotalRamSize=(((QWORD)3)<<30);
    }
    return qwTotalRamSize-DYNAMICMEMORY_START_ADDRESS;
}

static int kCalculateMetaBlockCount(QWORD qwDynamicRamSize){
    QWORD qwSmallestBlockCount, qwBitmapCount;
    QWORD qwLevelCount;
    QWORD qwBitmapCountOfCurrentLevel;

    qwSmallestBlockCount=qwDynamicRamSize/DYNAMICMEMORY_MIN_SIZE;
    qwBitmapCount=0;
    for(qwLevelCount=0; (qwBitmapCountOfCurrentLevel=(qwSmallestBlockCount>>qwLevelCount))>0; qwLevelCount++){
        qwBitmapCount+=(qwBitmapCountOfCurrentLevel+7)/8;
    }
    return (sizeof(BYTE)*qwSmallestBlockCount+sizeof(BITMAP)*qwLevelCount+sizeof(BYTE)*qwBitmapCount+DYNAMICMEMORY_MIN_SIZE-1)/DYNAMICMEMORY_MIN_SIZE;
}



static DYNAMICMEMORY gs_stDynamicMemory;

void kInitializeDynamicMemory(){
    QWORD qwTotalDynamicMemorySize;
    int iMetaBlockCount;
    int i, iBlockCountOfCurrentLevel;
    BYTE* pbBitmapOfCurrentLevel;
    
    qwTotalDynamicMemorySize=kCalculateDynamicMemorySize();
    iMetaBlockCount=kCalculateMetaBlockCount(qwTotalDynamicMemorySize);
    gs_stDynamicMemory.iBlockCountOfSmallestBlock=((int)(qwTotalDynamicMemorySize/DYNAMICMEMORY_MIN_SIZE)-iMetaBlockCount);

    gs_stDynamicMemory.pbAllocatedBlockListIndex=(BYTE*)DYNAMICMEMORY_START_ADDRESS;
    for(i=0; i<gs_stDynamicMemory.iBlockCountOfSmallestBlock; i++){
        gs_stDynamicMemory.pbAllocatedBlockListIndex[i]=0xFF;
    }

    for(i=0; (gs_stDynamicMemory.iBlockCountOfSmallestBlock<<i)>0; i++){
    }
    gs_stDynamicMemory.iMaxLevelCount=i;
    gs_stDynamicMemory.pstBitMapOfLevel=(BITMAP*)((QWORD)(gs_stDynamicMemory.pbAllocatedBlockListIndex)+sizeof(BYTE)*gs_stDynamicMemory.iBlockCountOfSmallestBlock);
    pbBitmapOfCurrentLevel=(BYTE*)(((QWORD)gs_stDynamicMemory.pstBitMapOfLevel)+sizeof(BITMAP)*gs_stDynamicMemory.iMaxLevelCount);
    
}
