#ifndef TERMINALCOMMAND_H
#define TERMINALCOMMNAD_H

typedef void(*TERMINALCOMMANDFUNCTION)(const char* pcArgument);
typedef struct kTerminalCommandEntryStructure{
    char* pcCommand;
    char* pcHelp;
    TERMINALCOMMANDFUNCTION pfFunction;
}TERMINALCOMMANDENTRY;

typedef struct kArgumentListStruct{
    const char* pcBuffer;
    int iLength;
    int iCurrentIndex;
}ARGUMENTLIST;

void kInitializeArgumentList(ARGUMENTLIST* pstArgumentList, const char* pcBuffer);
int kGetNextArgumnet(ARGUMENTLIST* pstArgumentList, char* pcArgument);

void kTerminalSearchCommandEntryAndSpaceIndex(const char* pcCommandBuffer, TERMINALCOMMANDENTRY** pstTerminalCmd, int* piSpaceIndex);


void kTerminalCommandHelp(const char* pcArgument);
void kTerminalCommandClear(const char* pcArgument);
void kTerminalCommandShowTotalRamSize(const char* pcArgumnet);
void kTerminalCommandStringToDeciHexConvert(const char* pcArgument);
void kTerminalCommandShutdown(const char* pcArgument);
void kTerminalCommandWait(const char* pcArgument);
void kTerminalCommandShowDateAndTime(const char* pcArgument);
void kTermianlCommandMeasureCPUSpeed(const char* pcArgument);
void kTerminalCommandStartTimer(const char* pcArgument);
void kTerminalCommandCreateTask(const char* pcArgument);
void kTerminalCommandTestLinkedList(const char* pcArgument);

#endif