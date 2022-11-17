#include "DynamicMemory.h"
#include "Utility.h"

static int kCalculateMetaBlockCount(QWORD qwDynamicRamSize){
    QWORD qwBlockCountOfSmallestBlock;
    DWORD dwSizeOfAllocatedBlockIndex;
    DWORD dwSizeofBitmap;
    int i;

    qwBlockCountOfSmallestBlock=qwDynamicRamSize/DYNAMICMEMORY_MIN_SIZE;
    dwSizeOfAllocatedBlockIndex=sizeof(BYTE)*qwBlockCountOfSmallestBlock;
    dwSizeofBitmap=0;
    for(i=0; (qwBlockCountOfSmallestBlock>>i)>0; i++){
        dwSizeofBitmap+=sizeof(BITMAP);
        dwSizeofBitmap+=((qwBlockCountOfSmallestBlock>>i)+7)/8;
    }

    return (qwBlockCountOfSmallestBlock+dwSizeOfAllocatedBlockIndex+dwSizeofBitmap+DYNAMICMEMORY_MIN_SIZE-1)/DYNAMICMEMORY_MIN_SIZE;
}


static QWORD kCalculateDynamicMemorySize(){
    QWORD qwRAMSize;
    qwRAMSize=kGetTotalRamSize()<<20;
    if(qwRAMSize>(((QWORD)3)<<30)){
        qwRAMSize=((QWORD)3)<<30;
    }
    return qwRAMSize-DYNAMICMEMORY_START_ADDRESS;
}

static DYNAMICMEMORY gs_stDynamicMemory;

void kInitializeDynamicMemory(){
    QWORD qwDynamicMemorySize;
    int iMetaBlockCount, i, j;
    BYTE* pbCurrentBitmapPointer;

    qwDynamicMemorySize=kCalculateDynamicMemorySize();
    iMetaBlockCount=kCalculateMetaBlockCount(qwDynamicMemorySize);
    gs_stDynamicMemory.iBlockCountOfSmallestBlock=qwDynamicMemorySize/DYNAMICMEMORY_MIN_SIZE-iMetaBlockCount;

    for(i=0; (gs_stDynamicMemory.iBlockCountOfSmallestBlock>>i)>0; i++){

    }
    gs_stDynamicMemory.iMaxLevelCount=i;
    gs_stDynamicMemory.pbAllocatedBlockListIndex=(BYTE*) DYNAMICMEMORY_START_ADDRESS;
    for(i=0; i<gs_stDynamicMemory.iBlockCountOfSmallestBlock; i++){
        gs_stDynamicMemory.pbAllocatedBlockListIndex[i]=0xFF;
    }

    gs_stDynamicMemory.pstBitMapOfLevel=(BITMAP*)(DYNAMICMEMORY_START_ADDRESS+sizeof(BYTE)*gs_stDynamicMemory.iBlockCountOfSmallestBlock);
    pbCurrentBitmapPointer=(BYTE*)((QWORD)gs_stDynamicMemory.pstBitMapOfLevel+sizeof(BITMAP)*gs_stDynamicMemory.iMaxLevelCount);

    for(j=0; j<gs_stDynamicMemory.iMaxLevelCount; j++){
        int iBlockCountOfLevel;
        gs_stDynamicMemory.pstBitMapOfLevel[j].pbBitmap=pbCurrentBitmapPointer;
        gs_stDynamicMemory.pstBitMapOfLevel[j].qwExistBitCount=0;
        iBlockCountOfLevel=gs_stDynamicMemory.iBlockCountOfSmallestBlock>>j;
        for(i=0; i<iBlockCountOfLevel/8; i++){
            *pbCurrentBitmapPointer=0;
            pbCurrentBitmapPointer++;
        }
        if((iBlockCountOfLevel%8)!=0){
            *pbCurrentBitmapPointer=0;
            i=iBlockCountOfLevel%8;
            if((i%2)==0){
                *pbCurrentBitmapPointer|=(DYNAMICMEMORY_EXIST<<(i-1));
                gs_stDynamicMemory.pstBitMapOfLevel[j].qwExistBitCount=1;
            }
            pbCurrentBitmapPointer++;
        }
    }

    gs_stDynamicMemory.qwStartAddress=DYNAMICMEMORY_START_ADDRESS+iMetaBlockCount*DYNAMICMEMORY_MIN_SIZE;
    gs_stDynamicMemory.qwEndAddress=qwDynamicMemorySize+DYNAMICMEMORY_START_ADDRESS;
    gs_stDynamicMemory.qwUsedSize=0;
}