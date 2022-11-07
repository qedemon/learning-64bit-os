#include "string.h"


int kVSPrintf(const char* pcBuffer, const char* pcFormatString, va_list a){
    int i;
    int j;
    j=0;
    for(i=0; pcFormatString[i]!=0; i++){
        if(pcFormatString[i]=='%'){
            i++;
            switch(pcFormatString[i]){
                default:
                    pcBuffer[j]
            }
        }
    }
    return j;
}