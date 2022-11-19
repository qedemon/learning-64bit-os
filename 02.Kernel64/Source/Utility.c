#include "Utility.h"
#include "AssemblyUtility.h"
#include "PIT.h"
#include "string.h"
#include "Task.h"
#include "Synchronization.h"

void kMemSet(void* pDest, BYTE bData, int iSize){
    int i;
    for(i=0; i<iSize; i++){
        ((char*)pDest)[i]=bData;
    }
}
int kMemCpy(void* pDest, const void* pSrc, int iSize){
    int i;
    for(i=0; i<iSize; i++){
        ((char*)pDest)[i]=((char*)pSrc)[i];
    }
    return i;
}

int kMemCmp(const void* pSrcA, const void* pSrcB, int iSize){
    int iReturn=0;
    int i;
    for(i=0; i<iSize; i++){
        if(*((char*)pSrcA)!=*((char*)pSrcB)){
            iReturn=i+1;
            break;
        }
        pSrcA++;
        pSrcB++;
    }
    return iReturn;
}

void kPrintString(int iX, int iY, const char* pcString){
    CHARACTER* pstString=(CHARACTER*) 0xB8000;
    int i;

    pstString+=(iY*80)+iX;
    for(i=0; pcString[i]!=0; i++){
        pstString[i].bChar=pcString[i];
    }
}

BOOL kSetInterruptFlag(BOOL bEnableInterrupt){
    QWORD qwRFLAGS;
    qwRFLAGS = kReadRFLAGS();
    if(bEnableInterrupt){
        kEnableInterrupt();
    }
    else{
        kDisableInterrupt();
    }
    if(kReport("")){
        char vcBuffer[100];
        ksprintf(vcBuffer, "kSetInterruptFlag = %s 0x%q\n", bEnableInterrupt?"TRUE":"FALSE", kGetRunningTask()->stLink.qwID);
        kRequireReport();
        if(kReport(""))
            kRequireReport();
    }
    
    if(qwRFLAGS&0x0200){
        return TRUE;
    }
    return FALSE;
}

static QWORD gs_qwTotalRamSize=0;

void kCheckTotalRamSize(){
    DWORD* pdwCurrentAddress;
    DWORD dwPreviousValue;

    pdwCurrentAddress=(DWORD*)0x4000000;
    while(1){
        dwPreviousValue=*pdwCurrentAddress;
        *pdwCurrentAddress=0x12345678;
        if(*pdwCurrentAddress!=0x12345678)
            break;
        *pdwCurrentAddress=dwPreviousValue;
        pdwCurrentAddress+=(0x400000/sizeof(DWORD));
    }
    gs_qwTotalRamSize=(QWORD)pdwCurrentAddress/0x100000;
}
QWORD kGetTotalRamSize(){
    return gs_qwTotalRamSize;
}

void kWaitms(long lMillisecond){
    BOOL bInterrupt=kSetInterruptFlag(FALSE);
    while(lMillisecond>30){
        kWaitUsingDirectPIT(MSTOCOUNT(30));
        lMillisecond-=30;
    }

    kWaitUsingDirectPIT(MSTOCOUNT(lMillisecond));
    kSetInterruptFlag(bInterrupt);
    kInitializePIT(MSTOCOUNT(1), TRUE);
}

char* kConvertDayOfWeekToString(BYTE bDayOfWeek){
    static char* vpcDayOfWeekString[]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Error"};
    if(bDayOfWeek>8){
        return vpcDayOfWeekString[7];
    }
    return vpcDayOfWeekString[bDayOfWeek];
}

QWORD kMeasureCPUSpeed(){
    int i;
    QWORD qwLastTSC, qwTotalTSC=0;
    BOOL bInterrupt=kSetInterruptFlag(FALSE);
    for(i=0; i<200; i++){
        qwLastTSC=kReadTSC();
        kWaitUsingDirectPIT(MSTOCOUNT(50));
        qwTotalTSC+=kReadTSC()-qwLastTSC;
    }
    kSetInterruptFlag(bInterrupt);
    return qwTotalTSC/10;
}

#include "TextModeTerminal.h"

static BOOL gs_bReport=FALSE;
void kRequireReport(){
    gs_bReport=TRUE;
}
BOOL kReport(const char* vcReportMessage){
    if(gs_bReport==TRUE){
        gs_bReport=FALSE;
        kprintf("%s", vcReportMessage);
        return TRUE;
    }
    return FALSE;
}