#include "string.h"
#include "Utility.h"

int kstrlen(const char* str){
    int i=0;
    for(i=0; str[i]!=0; i++);
    return i;
}

long katoi(const char* str, int iRadix){
    switch(iRadix){
        case 16:
            return kHexStringToQWORD(str);
        case 10:
        default:
            return kDecimalStringToLong(str);
            break;
    }
    return -1;
}


int kitoa(long lValue, char* pcBuffer, int iRadix){
    switch(iRadix){
        case 16:
            return kHexToString(lValue, pcBuffer);
            break;
        case 10:
        default:
            return kDecimalToString(lValue, pcBuffer);
            break;
    }
}

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
                    int len=kstrlen(srcString);
                    kMemCpy(pcBuffer+iBufferIndex, srcString, len);
                    iBufferIndex+=len;
                    break;
                }
                case 'c':{
                    char ch=(char)va_arg(ap, int);
                    pcBuffer[iBufferIndex]=ch;
                    iBufferIndex++;
                }
                case 'd':
                case 'i':{
                    int iValue=(int)va_arg(ap, int);
                    iBufferIndex+=kitoa(iValue, pcBuffer+iBufferIndex, 10);
                    break;
                }
                case 'x':
                case 'X':{
                    DWORD dwValue=(DWORD)va_arg(ap, DWORD);
                    iBufferIndex+=kitoa(dwValue, pcBuffer+iBufferIndex, 16);
                    break;
                }
                case 'q':
                case 'Q':
                case 'p':{
                    QWORD qwValue=(QWORD)va_arg(ap, QWORD);
                    iBufferIndex+=kitoa(qwValue, pcBuffer+iBufferIndex, 16);
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

long kDecimalStringToLong(const char* str){
    int index;
    long lReturnValue=0;
    if(str[0]=='-'){
        index=1;
    }
    else{
        index=0;
    }
    while(1){
        if(str[index]==0)
            break;
        lReturnValue*=10;
        lReturnValue+=str[index]-'0';
        index++;
    }
    if(str[0]=='-')
        lReturnValue=-lReturnValue;
    return lReturnValue;
}

QWORD kHexStringToQWORD(const char* str){
    QWORD qwReturn=0;
    int i=0;
    while(1){
        if(str[i]==0)
            break;
        qwReturn<<=4;
        if(str[i]>='0'&&str[i]<='9')
            qwReturn|=str[i]-'0';
        if(str[i]>='a'&&str[i]<='f')
            qwReturn|=str[i]-'a'+10;
        if(str[i]>='A'&&str[i]<='F')
            qwReturn|=str[i]-'A'+10;
        i++;
    }
    return qwReturn;
}

int kDecimalToString(long lValue, char* pcDest){
    int i=0;
    if(lValue<0){
        pcDest[0]='-';
        lValue=-lValue;
        i=1;
    }
    for(; lValue!=0; i++){
        pcDest[i]=lValue%10+'0';
        lValue=lValue/10;
    }
    pcDest[i]=0;
    if(pcDest[0]=='-')
        kReverseString(pcDest+1);
    else
        kReverseString(pcDest);
    return i;
}

int kHexToString(long lValue, char* pcDest){
    int i=0;
    if(lValue<0){
        pcDest[0]='-';
        lValue=-lValue;
        i=1;
    }
    for(; lValue!=0; i++){
        char ch=lValue&0x0f;
        pcDest[i]=(ch>=10)?(ch+'A'-10):(ch+'0');
        lValue=lValue>>4;
    }
    pcDest[i]=0;
    if(pcDest[0]=='-')
        kReverseString(pcDest+1);
    else
        kReverseString(pcDest);
    return i;
}

int kReverseString(char* str){
    int len=kstrlen(str);
    int i;
    for(i=0; i<(len>>1); i++){
        char ch=str[i];
        str[i]=str[len-1-i];
        str[len-1-i]=ch;
    }
    return len;
}