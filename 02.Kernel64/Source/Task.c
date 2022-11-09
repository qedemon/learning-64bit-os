#include "Task.h"
#include "Utility.h"

void kInitializeTask(TCB* pstTCB, QWORD qwID, QWORD qwFlags, QWORD qwEntryPointAddress, void* pvStackAddress, QWORD qwStackSize){
    kMemSet(pstTCB->stContext.vqRegister, 0, sizeof(pstTCB->stContext.vqRegister));
}