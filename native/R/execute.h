#ifndef ROGER_H__
#define ROGER_H__


int LoadR(int argc, char **argv);

void EndR();

SEXP ParseOneLine(const char *code, SEXP env, ParseStatus *status);

int DefineSEXP(const char *name, SEXP val);

SEXP ExecuteOneLine(char *cmd, SEXP env, int *errorOccured);

__declspec(dllexport) int call(int argc, char **argv);

#endif
