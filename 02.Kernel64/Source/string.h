#ifndef STRING_H
#define STRING_H
#include <stdarg.h>
#include "Type.h"

int kstrlen(const char* str);
long katoi(const char* str, int iRadix);
int kitoa(long lValue, char* pcBuffer, int iRadix);
int ksprintf(char* pcString, const char* pcFormatString, ...);
int kVSPrintf(char* pcBuffer, const char* pcFormatString, va_list ap);

long kDecimalStringToLong(const char* str);
QWORD kHexStringToQWORD(const char* str);
int kDecimalToString(long lValue, char* pcDest);
int kHexToString(unsigned long lValue, char* pcDest);
int kDoubleToString(double dValue, char* pcDest);
int kReverseString(char* str);
#endif