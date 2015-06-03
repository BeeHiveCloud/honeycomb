#ifndef RCONSOLE_H__
#define RCONSOLE_H__

__declspec(dllexport) int readConsole(const char *prompt, char *buf, int len, int addtohistory);

__declspec(dllexport) void writeConsole(const char *buf, int len);

__declspec(dllexport) int execute(int argc, char **argv);

#endif
