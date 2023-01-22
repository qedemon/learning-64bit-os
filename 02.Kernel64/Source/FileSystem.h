#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "Type.h"
#include "Synchronization.h"
#include "HDD.h"

#define FILESYSTEM_SIGNATURE 0x7E38CF10
#define FILESYSTEM_SECTORPERCLUSTER 8

#define FILESYSTEM_LASTCLUSTER 0xFFFFFFFF
#define FILESYSTEM_FREECLUSTER 0x00

#pragma pack(push, 1)

typedef struct kPartitionStruct{
    BYTE bBootableFlag;
    BYTE vbStartingCHSAddress[3];
    BYTE bPartitionType;
    BYTE vbEndingCHSAddress[3];
    DWORD dwStartingLBAAddress;
    DWORD dwSizeInSector;
}PARTITION;

typedef struct kMBRStruct{
    BYTE vbBootCode[430];
    DWORD dwSignature;
    DWORD dwReservedSectorCount;
    DWORD dwClusterLinkSectorCount;
    DWORD dwTotalCluasterCount;

    PARTITION vstPartition[4];
    BYTE vbBootLoaderSignature[2];
}MBR;

#pragma pack(pop)

#define FILESYSTEM_MAXFILENAMELENGTH 24

#pragma pack(push, 1)

typedef struct kDirectoryEntryStruct{
    char vcFileName[FILESYSTEM_MAXFILENAMELENGTH];
    DWORD dwFileSize;
    DWORD dwStartClusterIndex;
}DIRECTORYENTRY;

typedef struct kFileSystemManagerStruct{
    BOOL bMounted;
    DWORD dwReservedSectorCount;
    DWORD dwClusterLinkAreaStartAddress;
    DWORD dwClusterLinkAreaSize;
    DWORD dwDataAreaStartAddress;
    DWORD dwTotalClusterCount;

    DWORD dwLastAllocatedClusterLinkSectorOffset;
    MUTEX stMutex;
}FILESYSTEMMANAGER;

#pragma pack(pop)

typedef BOOL (*fReadHDDInformation)(BOOL bPrimary, BOOL bMaster, HDDINFORMATION* pstHDDInformation);
typedef int (*fReadHDDSector)(BOOL bPrimary, BOOL bMaster, DWORD dwLBA, int iSectorCount, char* pcBuffer);
typedef int (*fWriteHDDSector)(BOOL bPrimary, BOOL bMaster, DWORD dwLBA, int iSectorCount, char* pcBuffer);

BOOL kInitializeFileSystem();
BOOL kMount();
BOOL kFormat();

#endif