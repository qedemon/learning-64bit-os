#include "LinkedList.h"

void kInitializeTask(LIST* pstList){
    pstList->iItemCount=0;
    pstList->pvHead=NULL;
    pstList->pvTail=NULL;
}

void kAddLinkToTail(LIST* pstList, void* pvItem){
    LISTLINK* pstPreviousTail=(LISTLINK*) pstList->pvTail;
    if(pstPreviousTail!=NULL)
        pstPreviousTail->pvNext=pvItem;
    pstList->pvTail=pvItem;
    ((LISTLINK*)pstList->pvTail)->pvNext=NULL;

    if(pstList->pvHead==NULL){
        pstList->pvHead=pvItem;
    }
    pstList->iItemCount++;
}

void kAddLinkToHead(LIST* pstList, void* pvItem){
    LISTLINK* pstNewItem=(LISTLINK*) pvItem;
    pstNewItem->pvNext=pstList->pvHead;
    pstList->pvHead=pstNewItem;
    if(pstList->pvTail==NULL){
        pstList->pvTail=pvItem;
    }
    pstList->iItemCount++;
}

void* kRemoveLink(LIST* pstList, QWORD qwID){
    LISTLINK* pstLink;
    LISTLINK* pstPreviousLink;
    if(pstList->pvHead==NULL)
        return NULL;
    pstLink=pstList->pvHead;
    pstPreviousLink=NULL;
    while(pstLink!=NULL){
        if(pstLink->qwID==qwID){
            if(pstPreviousLink==NULL){
                pstList->pvHead=pstLink->pvNext;
            }
            else{
                pstPreviousLink->pvNext=pstLink->pvNext;
            }
            if(pstLink->pvNext==NULL){
                pstList->pvTail=pstPreviousLink;
            }
            pstList->iItemCount--;
            break;
        }
        pstPreviousLink=pstLink;
        pstLink=pstLink->pvNext;
    }
    return pstLink;
}

void* kRemoveLinkFromHead(LIST* pstList){
    LISTLINK* pstLink=pstList->pvHead;
    if(pstLink==NULL)
        return NULL;
    if(pstLink==(LISTLINK*)pstList->pvTail){
        pstList->pvTail=NULL;
    }
    pstList->pvHead=pstLink->pvNext;
    pstList->iItemCount--;
    return pstLink;
}
void* kRemoveLinkFromTail(LIST* pstList){
    LISTLINK* pstLink=pstList->pvTail;
    LISTLINK* pstPreviousLink=pstList->pvHead;
    if(pstLink==NULL)
        return NULL;
    if((LISTLINK*)pstList->pvHead==pstLink){
        pstList->pvHead=NULL;
        pstList->pvTail=NULL;
        pstList->iItemCount=0;
        return pstLink;
    }
    while(pstPreviousLink->pvNext!=pstLink){
        pstPreviousLink=pstPreviousLink->pvNext;
    }
    pstPreviousLink->pvNext=NULL;
    pstList->pvTail=pstPreviousLink;
    pstList->iItemCount--;
    return pstLink;
}