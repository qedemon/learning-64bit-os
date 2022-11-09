#ifndef TERMINALCOMMAND_H
#define TERMINALCOMMNAD_H

void kTerminalCommandHelp(const char* pcArgument);
void kTerminalCommandClear(const char* pcArgument);
void kTerminalCommandShowTotalRamSize(const char* pcArgumnet);
void kTerminalCommandStringToDeciHexConvert(const char* pcArgument);
void kTerminalCommandShutdown(const char* pcArgument);
void kTerminalCommandWait(const char* pcArgument);
void kTerminalShowDateAndTime(const char* pcArgument);
void kTermianlMeasureCPUSpeed(const char* pcArgument);
void kTerminalStartTimer(const char* pcArgument);

#endif