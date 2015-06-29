#ifndef RCONSOLE_H__
#define RCONSOLE_H__

int readConsole(const char *prompt, char *buf, int len, int addtohistory);

void writeConsole(const char *buf, int len);

int execute(int argc, char **argv);

#endif
