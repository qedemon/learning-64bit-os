#ifndef STRING_H
#define STRING_H
#include <stdarg.h>

int ksprintf(char* pcString, const char* pcFormatString, ...);
int kVSPrintf(char* pcBuffer, const char* pcFormatString, va_list ap);
#endif