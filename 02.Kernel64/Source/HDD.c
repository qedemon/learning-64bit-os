#include "HDD.h"
#include "Synchronization.h"
#include "AssemblyUtility.h"
#include "PIT.h"
#include "Utility.h"
#include "TextModeTerminal.h"

static HDDMANAGER gs_stHDDManager;

BOOL kInitializeHDD(){
    kInitializeMutex(&gs_stHDDManager.stMutex);

    gs_stHDDManager.bPrimaryInterruptOccur=FALSE;
    gs_stHDDManager.bSecondaryInterruptOccur=FALSE;

    kOutPortByte(HDD_PORT_PRIMARYBASE+HDD_PORT_INDEX_DIGITALOUTPUT, 0);
    kOutPortByte(HDD_PORT_SECONDARYBASE+HDD_PORT_INDEX_DIGITALOUTPUT, 0);

    if(kReadHDDInformation(TRUE, TRUE, &gs_stHDDManager.stHDDInformation)==FALSE){
        gs_stHDDManager.bHDDDetected=FALSE;
        gs_stHDDManager.bCanWrite=FALSE;
        return FALSE;
    }
    gs_stHDDManager.bHDDDetected=TRUE;
    if(kMemCmp(&gs_stHDDManager.stHDDInformation.vwModelNumber, "VBOX", 4)==0)
        gs_stHDDManager.bCanWrite=TRUE;
    else
        gs_stHDDManager.bCanWrite=FALSE;
    return TRUE;
}

static BYTE kReadHDDStatus(BOOL bPrimary){
    return kInPortByte((bPrimary?HDD_PORT_PRIMARYBASE:HDD_PORT_SECONDARYBASE)+HDD_PORT_INDEX_STATUS);
}

static BOOL kWaitForHDDNoBusy(BOOL bPrimary){
    QWORD qwStartTick;
    qwStartTick=kGetTickCount();
    while((kGetTickCount()-qwStartTick)<=HDD_WAITTIME){
        BYTE bHDDStatus=kReadHDDStatus(bPrimary);
        if((bHDDStatus&&HDD_STATUS_BUSY)!=HDD_STATUS_BUSY){
            return TRUE;
        }
        kSleep(1);
    }
    return FALSE;
}

static BOOL kWaitForHDDReady(BOOL bPrimary){
    QWORD qwStartTick;
    qwStartTick=kGetTickCount();
    while((kGetTickCount()-qwStartTick)<=HDD_WAITTIME){
        BYTE bHDDStatus=kReadHDDStatus(bPrimary);
        if(bHDDStatus&&HDD_STATUS_DEVICEREADY){
            return TRUE;
        }
        kSleep(1);
    }
    return FALSE;
}

static BOOL kWaitForHDDInterrupt(BOOL bPrimary){
    QWORD qwTickCount;
    qwTickCount=kGetTickCount();
    while((kGetTickCount()-qwTickCount)<=HDD_WAITTIME){
        if(bPrimary){
            if(gs_stHDDManager.bPrimaryInterruptOccur){
                return TRUE;
            }
        }
        else{
            if(gs_stHDDManager.bSecondaryInterruptOccur){
                return TRUE;
            }
        }
        kSleep(1);
    }
    return FALSE;
}

static void kSwapByteInWord(WORD* pwData, int iWordCount){
    int i;
    for(i=0; i<iWordCount; i++){
        pwData[i]=(pwData[i]>>8)|(pwData[i]<<8);
    }
}

BOOL kReadHDDInformation(BOOL bPrimary, BOOL bMaster, HDDINFORMATION* pstHDDInformation){
    WORD wPortBase;
    BYTE bDriveFlag, bWaitResult, bStatus;
    int i;
    wPortBase=bPrimary?HDD_PORT_PRIMARYBASE:HDD_PORT_SECONDARYBASE;
    kLockMutex(&gs_stHDDManager.stMutex);
    if(kWaitForHDDNoBusy(TRUE)==FALSE){
        kUnlockMutex(&gs_stHDDManager.stMutex);
        return FALSE;
    }
    bDriveFlag=bMaster?HDD_DRIVEHEAD_LBA:(HDD_DRIVEHEAD_LBA|HDD_DRIVEHEAD_SLAVE);
    kOutPortByte(wPortBase+HDD_PORT_INDEX_DRIVEHEADER, bDriveFlag);
    if(kWaitForHDDReady(bPrimary)==FALSE){
        kUnlockMutex(&gs_stHDDManager.stMutex);
        return FALSE;
    }
    kSetHDDInterrupt(bPrimary, FALSE);
    kOutPortByte(wPortBase+HDD_PORT_INDEX_COMMAND, HDD_COMMAND_IDENTIFY);
    bWaitResult=kWaitForHDDInterrupt(bPrimary);
    bStatus=kReadHDDStatus(bPrimary);
    if((bWaitResult==FALSE)||((bStatus&HDD_STATUS_ERR)==HDD_STATUS_ERR)){
        kUnlockMutex(&gs_stHDDManager.stMutex);
        return FALSE;
    }
    for(i=0; i<512/2; i++){
        WORD wData;
        wData=kInPortWord(wPortBase+HDD_PORT_INDEX_DATA);
        ((WORD*) pstHDDInformation)[i]=wData;
    }
    kSwapByteInWord(pstHDDInformation->vwModelNumber, sizeof(pstHDDInformation->vwModelNumber)/2);
    kSwapByteInWord(pstHDDInformation->vwSerialNumber, sizeof(pstHDDInformation->vwSerialNumber)/2);
    kUnlockMutex(&gs_stHDDManager.stMutex);
    return TRUE;
}

void kSetHDDInterrupt(BOOL bPrimary, BOOL bFlag){
    if(bPrimary){
        gs_stHDDManager.bPrimaryInterruptOccur=bFlag;
    }
    else{
        gs_stHDDManager.bSecondaryInterruptOccur=bFlag;
    }
}

int kCopyHDDSerialNumber(char* vcDest){
    if(vcDest==NULL){
        return 0;
    }
    return kMemCpy(vcDest, gs_stHDDManager.stHDDInformation.vwSerialNumber, sizeof(gs_stHDDManager.stHDDInformation.vwSerialNumber));
}
int kCopyHDDModelNumber(char* vcDest){
    if(vcDest==NULL){
        return 0;
    }
    return kMemCpy(vcDest, gs_stHDDManager.stHDDInformation.vwModelNumber, sizeof(gs_stHDDManager.stHDDInformation.vwModelNumber));
}