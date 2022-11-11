#ifndef TERMINALCOMMAND_H
#define TERMINALCOMMNAD_H

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