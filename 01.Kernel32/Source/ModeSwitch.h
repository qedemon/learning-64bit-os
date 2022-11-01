#ifndef MODESWITCH_H
#define MODESWITCH_H

#include "Type.h"

void kReadCPUID(char* pcCPUID);
DWORD kCheck64BitMode();
void kSwitchAndExecute64BitMode();

#endif