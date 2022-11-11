#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include "Type.h"
#pragma pack(push, 1)
typedef struct kListLinkStruct{
    void* pvNext;
    QWORD qwID;
}LISTLINK;

typedef struct kListManagerStruct{
    int iItemCount;
    void* pvHead;
    void* pvTail;
}LIST;
#pragma pack(pop)

void kInitializeList(LIST* pstList);
void kAddLinkToTail(LIST* pstList, void* pvItem);
void kAddLinkToHead(LIST* pstList, void* pvItem);
void* kRemoveLink(LIST* pstList, QWORD qwID);
void* kRemoveLinkFromHead(LIST* pstList);
void* kRemoveLinkFromTail(LIST* pstList);
#endif