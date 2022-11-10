#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include "Type.h"
typedef struct kListLinkStruct{
    void* pvNext;
    QWORD qwID;
}LISTLINK;

typedef struct kListManagerStruct{
    int iItemCount;
    void* pvHead;
    void* pvTail;
}LIST;

#endif