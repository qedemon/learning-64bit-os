#ifndef TYPE_H
#define TYPE_H

#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long
#define BOOL unsigned char

#define QWORD_MAX 0xFFFFFFFFFFFFFFFF

#define NULL 0

#define TRUE 1
#define FALSE 0

#pragma pack(push, 1)
typedef struct kCharStruct{
    BYTE bChar;
    BYTE bAttrib;
}CHARACTER;
#pragma pack(pop)

#endif