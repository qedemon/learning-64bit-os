#include "string.h"

int ksprintf(char* pcString, const char* pcFormatString, ...){
    int i;
    va_list valist;
    va_start(valist, pcFormatString);
    i=kVSPrintf(pcString, pcFormatString, valist);
    va_end(valist);
    return i;
}

int kVSPrintf(char* pcBuffer, const char* pcFormatString, va_list ap){
    int i;
    int iBufferIndex;
    iBufferIndex=0;
    for(i=0; pcFormatString[i]!=0; i++){
        if(pcFormatString[i]=='%'){
            i++;
            if(pcFormatString[i]==0)
                break;
            switch(pcFormatString[i]){
                case 's':{
                    const char* srcString=(const char*)va_arg(ap, const char*);
                    int j;
                    for(j=0; srcString[j]!=0; j++){
                        pcBuffer[iBufferIndex]=srcString[j];
                        iBufferIndex++;
                    }
                    break;
                }
                case '%':
                    pcBuffer[iBufferIndex]='%';
                    iBufferIndex++;
                    break;
                default:
                    pcBuffer[iBufferIndex]='%';
                    iBufferIndex++;
                    pcBuffer[iBufferIndex]=pcFormatString[i];
                    iBufferIndex++;
                    break;
            }
        }
        else{
            pcBuffer[iBufferIndex]=pcFormatString[i];
            iBufferIndex++;
        }
    }
    pcBuffer[iBufferIndex]=0;
    return iBufferIndex;
}