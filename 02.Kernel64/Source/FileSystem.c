#include "FileSystem.h"
#include "Utility.h"

fReadHDDInformation gs_pfReadHDDInformation=NULL;
fReadHDDSector gs_pfReadHDDSector=NULL;
fWriteHDDSector gs_pfWriteHDDSector=NULL;

static FILESYSTEMMANAGER gs_stFileManager;

BOOL kInitializeFileSystem(){
    kMemSet(&gs_stFileManager, 0, sizeof(FILESYSTEMMANAGER));
    kInitializeMutex(&gs_stFileManager.stMutex);
    if(kInitializeHDD()==TRUE){
        gs_pfReadHDDInformation=kReadHDDInformation;
        gs_pfReadHDDSector=kReadHDDSector;
        gs_pfWriteHDDSector=kWriteHDDSector;
    }
    else{
        return FALSE;
    }
    if(kMount()==FALSE){
        return FALSE;
    }
    return TRUE;
}

static BYTE gs_vbTempBuffer[FILESYSTEM_SECTORPERCLUSTER*512];

BOOL kMount(){
    MBR* pstMBR;
    kLockMutex(&gs_stFileManager.stMutex);
    if(gs_pfReadHDDSector(TRUE, TRUE, 0, 1, gs_vbTempBuffer)==0){
        kUnlockMutex(&gs_stFileManager.stMutex);
        return FALSE;
    }
    pstMBR=(MBR*)gs_vbTempBuffer;
    if(pstMBR->dwSignature!=FILESYSTEM_SIGNATURE){
        kUnlockMutex(&gs_stFileManager.stMutex);
        return FALSE;
    }
    gs_stFileManager.bMounted=TRUE;
    gs_stFileManager.dwReservedSectorCount=pstMBR->dwReservedSectorCount;
    gs_stFileManager.dwClusterLinkAreaStartAddress=pstMBR->dwReservedSectorCount+1;
    gs_stFileManager.dwClusterLinkAreaSize=pstMBR->dwClusterLinkSectorCount;
    gs_stFileManager.dwDataAreaStartAddress=pstMBR->dwReservedSectorCount+pstMBR->dwClusterLinkSectorCount+1;
    gs_stFileManager.dwTotalClusterCount=pstMBR->dwTotalCluasterCount;

    kUnlockMutex(&gs_stFileManager.stMutex);
    return TRUE;
}

BOOL kFormat(){
    HDDINFORMATION* pstHDD;
    MBR* pstMBR;
    DWORD dwTotalSectorCount, dwRemainSectorCount;
    DWORD dwMaxClusterCount, dwClusterCount;
    DWORD dwClusterLinkSectorCount;
    DWORD i;

    kLock(&gs_stFileManager.stMutex);
    pstHDD=(HDDINFORMATION*)gs_vbTempBuffer;
    if(gs_pfReadHDDInformation(TRUE, TRUE, pstHDD)==FALSE){
        kUnlockMutex(&gs_stFileManager.stMutex);
        return FALSE;
    }
    dwTotalSectorCount=pstHDD->dwTotalSectors;
    dwMaxClusterCount=dwTotalSectorCount/FILESYSTEM_SECTORPERCLUSTER;
    dwClusterLinkSectorCount=(dwMaxClusterCount+127)/128;
    dwRemainSectorCount=dwTotalSectorCount-dwClusterLinkSectorCount-1;
    dwClusterCount=dwRemainSectorCount/FILESYSTEM_SECTORPERCLUSTER;

    if(gs_pfReadHDDSector(TRUE, TRUE, 0, 1, gs_vbTempBuffer)==FALSE){
        kUnlockMutex(&gs_stFileManager.stMutex);
        return FALSE;
    }
    pstMBR=(MBR*)gs_vbTempBuffer;
    kMemSet(pstMBR->vstPartition, 0, sizeof(pstMBR->vstPartition));
    pstMBR->dwSignature=FILESYSTEM_SIGNATURE;
    pstMBR->dwReservedSectorCount=0;
    pstMBR->dwClusterLinkSectorCount=dwClusterLinkSectorCount;
    pstMBR->dwTotalCluasterCount=dwClusterCount;

    if(gs_pfWriteHDDSector(TRUE, TRUE, 0, 1, pstMBR)==FALSE){
        kUnlockMutex(&gs_stFileManager.stMutex);
        return FALSE;
    }
}