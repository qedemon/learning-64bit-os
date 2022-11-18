#include "DynamicMemory.h"
#include "Utility.h"
#include "TextModeTerminal.h"
#include "Synchronization.h"

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
    int i, j, iOffset, iBlockCountOfCurrentLevel;
    BYTE* pbBitmapOfCurrentLevel;
    
    qwTotalDynamicMemorySize=kCalculateDynamicMemorySize();
    iMetaBlockCount=kCalculateMetaBlockCount(qwTotalDynamicMemorySize);
    gs_stDynamicMemory.iBlockCountOfSmallestBlock=((int)(qwTotalDynamicMemorySize/DYNAMICMEMORY_MIN_SIZE)-iMetaBlockCount);

    gs_stDynamicMemory.pbAllocatedBlockListIndex=(BYTE*)DYNAMICMEMORY_START_ADDRESS;
    for(i=0; i<gs_stDynamicMemory.iBlockCountOfSmallestBlock; i++){
        gs_stDynamicMemory.pbAllocatedBlockListIndex[i]=0xFF;
    }

    for(i=0; (gs_stDynamicMemory.iBlockCountOfSmallestBlock>>i)>0; i++){
    }
    gs_stDynamicMemory.iMaxLevelCount=i;
    gs_stDynamicMemory.pstBitMapOfLevel=(BITMAP*)((QWORD)(gs_stDynamicMemory.pbAllocatedBlockListIndex)+sizeof(BYTE)*gs_stDynamicMemory.iBlockCountOfSmallestBlock);
    pbBitmapOfCurrentLevel=(BYTE*)(((QWORD)gs_stDynamicMemory.pstBitMapOfLevel)+sizeof(BITMAP)*gs_stDynamicMemory.iMaxLevelCount);
    for(i=0; i<gs_stDynamicMemory.iMaxLevelCount; i++){
        gs_stDynamicMemory.pstBitMapOfLevel[i].pbBitmap=pbBitmapOfCurrentLevel;
        gs_stDynamicMemory.pstBitMapOfLevel[i].qwExistBitCount=0;
        iBlockCountOfCurrentLevel=(gs_stDynamicMemory.iBlockCountOfSmallestBlock>>i);
        for(j=0; j<iBlockCountOfCurrentLevel/8; j++){
            *pbBitmapOfCurrentLevel=0;
            pbBitmapOfCurrentLevel++;
        }
        iOffset=iBlockCountOfCurrentLevel%8;
        if(iOffset>0){
            *pbBitmapOfCurrentLevel=0;
            if(iOffset%2==1){
                *pbBitmapOfCurrentLevel|=(1<<iOffset);
                gs_stDynamicMemory.pstBitMapOfLevel[i].qwExistBitCount=1;
            }
            pbBitmapOfCurrentLevel++;
        }
    }
    gs_stDynamicMemory.qwUsedSize=0;
    gs_stDynamicMemory.qwStartAddress=DYNAMICMEMORY_START_ADDRESS+iMetaBlockCount*DYNAMICMEMORY_MIN_SIZE;
    gs_stDynamicMemory.qwEndAddress=gs_stDynamicMemory.qwStartAddress+((QWORD)gs_stDynamicMemory.iBlockCountOfSmallestBlock)*DYNAMICMEMORY_MIN_SIZE;

    kprintf("Initilaize Dynamic Memory\n");
    kprintf("Allocating 0x%q -> 0x%q\n", gs_stDynamicMemory.qwStartAddress, gs_stDynamicMemory.qwEndAddress);
    kprintf("Level : %d\n", gs_stDynamicMemory.iMaxLevelCount);
    kprintf("Smallest Block Count %d\n", gs_stDynamicMemory.iBlockCountOfSmallestBlock);
}

static int kFindBlockLevelForRquiredMemorySize(QWORD qwMemorySize){
    int iLevel;
    for(iLevel=0; iLevel<gs_stDynamicMemory.iMaxLevelCount; iLevel++){
        if(qwMemorySize<=(gs_stDynamicMemory.iBlockCountOfSmallestBlock<<iLevel)){
            break;
        }
    }
    if(iLevel==gs_stDynamicMemory.iMaxLevelCount){
        return -1;
    }
    return iLevel;
}

static BOOL kSetBitMap(BITMAP* pstTargetBitmap, int iOffset, BOOL bExist){
    if(bExist==DYNAMICMEMORY_EXIST){
        if((pstTargetBitmap->pbBitmap[iOffset/8]&(DYNAMICMEMORY_EXIST<<(iOffset%8)))==0){
            pstTargetBitmap->qwExistBitCount++;
            pstTargetBitmap->pbBitmap[iOffset/8]|=(DYNAMICMEMORY_EXIST<<(iOffset%8));
            return TRUE;
        }
        return FALSE;
    }
    else{
        if((pstTargetBitmap->pbBitmap[iOffset/8]&(DYNAMICMEMORY_EXIST<<(iOffset%8)))){
            pstTargetBitmap->qwExistBitCount--;
            pstTargetBitmap->pbBitmap[iOffset/8]&=~(DYNAMICMEMORY_EXIST<<(iOffset%8));
            return TRUE;
        }
        return FALSE;
    }
}

static BYTE kGetBitMap(BITMAP* pstBitmap, int iOffset){
    if(pstBitmap->qwExistBitCount==0)
        return DYNAMICMEMORY_EMPTY;
    if(pstBitmap->pbBitmap[iOffset/8]&(DYNAMICMEMORY_EXIST<<(iOffset%8))){
        return DYNAMICMEMORY_EXIST;
    }
    else{
        return DYNAMICMEMORY_EMPTY;
    }
}

void* kAllocateDynamicMemory(QWORD qwRequiredSize){
    int i, iRequiredLevel, iExistLevel;
    QWORD* pqwBitmapData;
    QWORD qwOffset;
    BYTE bLockInfo;

    bLockInfo=kLockForSystemData();
    if(qwRequiredSize>(gs_stDynamicMemory.qwEndAddress-gs_stDynamicMemory.qwStartAddress-gs_stDynamicMemory.qwUsedSize)){
        return NULL;
    }
    iRequiredLevel=kFindBlockLevelForRquiredMemorySize(qwRequiredSize);
    if(iRequiredLevel<0){
        return NULL;
    }
    for(i=iRequiredLevel; i<gs_stDynamicMemory.iMaxLevelCount; i++){
        if(gs_stDynamicMemory.pstBitMapOfLevel[i].qwExistBitCount>0){
            pqwBitmapData=(QWORD*) &gs_stDynamicMemory.pstBitMapOfLevel[i].pbBitmap;
            qwOffset=0;
            while(*pqwBitmapData==0){
                qwOffset+=64;
                pqwBitmapData++;
            }
            while(((*pqwBitmapData)&(DYNAMICMEMORY_EXIST<<(qwOffset%64)))==0){
                qwOffset++;
            }
            break;
        }
    }
    if(i==gs_stDynamicMemory.iMaxLevelCount)
        return NULL;
    
    iExistLevel=i;
    kSetBitMap(&gs_stDynamicMemory.pstBitMapOfLevel[iExistLevel], qwOffset, DYNAMICMEMORY_EMPTY);
    while(iExistLevel>iRequiredLevel){
        iExistLevel--;
        qwOffset<<=1;
        //kSetBitMap(&gs_stDynamicMemory.pstBitMapOfLevel[iExistLevel], qwOffset, DYNAMICMEMORY_EMPTY);
        kSetBitMap(&gs_stDynamicMemory.pstBitMapOfLevel[iExistLevel], qwOffset+1, DYNAMICMEMORY_EXIST);
    }
    gs_stDynamicMemory.pbAllocatedBlockListIndex[qwOffset<<iRequiredLevel]=iRequiredLevel;
    gs_stDynamicMemory.qwUsedSize+=(DYNAMICMEMORY_MIN_SIZE<<iRequiredLevel);
    kUnlockForSystemData(bLockInfo);
    return (void*)(gs_stDynamicMemory.qwStartAddress+(qwOffset<<iRequiredLevel)*DYNAMICMEMORY_MIN_SIZE);
}

BOOL kFreeDynamicMemory(void* vpMemoryAddress){
    int iLevel, i;
    QWORD qwOffset;
    BYTE bLockInfo;
    qwOffset=(QWORD)vpMemoryAddress/DYNAMICMEMORY_MIN_SIZE;
    bLockInfo=kLockForSystemData();
    iLevel=gs_stDynamicMemory.pbAllocatedBlockListIndex[qwOffset];
    if(iLevel>=gs_stDynamicMemory.iMaxLevelCount){
        kUnlockForSystemData(bLockInfo);
        return FALSE;
    }
    qwOffset>>=iLevel;
    for(i=iLevel; i<gs_stDynamicMemory.iMaxLevelCount; i++){
        kSetBitMap(&gs_stDynamicMemory.pstBitMapOfLevel[i], qwOffset, DYNAMICMEMORY_EXIST);
        if(i==gs_stDynamicMemory.iMaxLevelCount-1){
            break;
        }
        if(qwOffset%2==0){
            if(kGetBitMap(&gs_stDynamicMemory.pstBitMapOfLevel[i], qwOffset+1)==DYNAMICMEMORY_EMPTY){
                break;
            }
            kSetBitMap(&gs_stDynamicMemory.pstBitMapOfLevel[i], qwOffset, DYNAMICMEMORY_EMPTY);
            kSetBitMap(&gs_stDynamicMemory.pstBitMapOfLevel[i], qwOffset+1, DYNAMICMEMORY_EMPTY);
        }
        else{
            if(kGetBitMap(&gs_stDynamicMemory.pstBitMapOfLevel[i], qwOffset-1)==DYNAMICMEMORY_EMPTY){
                break;
            }
            kSetBitMap(&gs_stDynamicMemory.pstBitMapOfLevel[i], qwOffset, DYNAMICMEMORY_EMPTY);
            kSetBitMap(&gs_stDynamicMemory.pstBitMapOfLevel[i], qwOffset-1, DYNAMICMEMORY_EMPTY);
        }
        qwOffset>>1;
    }
    kUnlockForSystemData(bLockInfo);
    return TRUE;
}