#ifndef HDD_H
#define HDD_H
#include "Type.h"
#include "Synchronization.h"

#define HDD_PORT_PRIMARYBASE 0x1F0
#define HDD_PORT_SECONDARYBASE 0x170

#define HDD_PORT_INDEX_DATA 0x00
#define HDD_PORT_INDEX_SECTORCOUNT 0X02
#define HDD_PORT_INDEX_SECTORNUMBER 0x03
#define HDD_PORT_INDEX_CYLINDERLSB 0x04
#define HDD_PORT_INDEX_CYLINDERMSB 0x05
#define HDD_PORT_INDEX_DRIVEHEADER 0x06
#define HDD_PORT_INDEX_STATUS 0x07
#define HDD_PORT_INDEX_COMMAND 0x07
#define HDD_PORT_INDEX_DIGITALOUTPUT 0x206

#define HDD_COMMAND_READ 0x20
#define HDD_COMMAND_WRITE 0x30
#define HDD_COMMAND_IDENTIFY 0xEC

#define HDD_STATUS_ERR 0x01
#define HDD_STATUS_IDEX 0x02
#define HDD_STATUS_CORRECTEDDATA 0x04
#define HDD_STATUS_DATAREQUEST 0x08
#define HDD_STATUS_SEEKCOMPLETE 0x10
#define HDD_STATUS_DEVICEFAULT 0x20
#define HDD_STATUS_DEVICEREADY 0x40
#define HDD_STATUS_BUSY 0x80

#define HDD_DRIVEHEAD_LBA 0xE0
#define HDD_DRIVEHEAD_SLAVE 0xB0

#define HDD_DIGITALOUTPUT_RESET 0x04
#define HDD_DIGITALOUTPUT_INTERRUPTDISABLE 0x20

#define HDD_WAITTIME 500
#define HDD_MAXBULKSECTORCOUNT 256

#pragma pack(push, 1)
typedef struct kHDDInformationStruct{
    WORD wConfiguration;
    
    WORD wNumberOfCylinder;
    WORD wReserved1;

    WORD wNumberOfHead;
    WORD wUnformattedBytesPerTrack;
    WORD wUnformattedBytesPerSector;

    WORD wNumberOfSectorPerCylinder;
    WORD wInterSectorGap;
    WORD wBytesInPhaseLock;
    WORD wNumberOfVendorUniqueStatusWord;

    WORD vwSerialNumber[10];
    WORD wControllerType;
    WORD wBufferSize;
    WORD wNumberOfECCBytes;
    WORD vwFirmwareRevision[4];

    WORD vwModelNumber[20];
    WORD vwReserved2[13];

    DWORD dwTotalSectors;
    WORD vwReserved3[196];
}HDDINFORMATION;
#pragma pack(pop)

typedef struct kHDDManagerStruct{
    BOOL bHDDDetected;
    BOOL bCanWrite;

    volatile BOOL bPrimaryInterruptOccur;
    volatile BOOL bSecondaryInterruptOccur;
    MUTEX stMutex;

    HDDINFORMATION stHDDInformation;
}HDDMANAGER;

BOOL kInitializeHDD();
BOOL kReadHDDInformation(BOOL bPrimary, BOOL bMaster, HDDINFORMATION* pstHDDInformation);

void kSetHDDInterrupt(BOOL bPrimary, BOOL bFlag);

int kCopyHDDSerialNumber(char* vcDest);
int kCopyHDDModelNumber(char* vcDest);

#endif