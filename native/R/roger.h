#ifndef ROGER_H__
#define ROGER_H__

#define Win32
#include <windows.h>
#include <stdio.h>
#undef ERROR
#include <Rversion.h>
#include <Rembedded.h>
#include <R_ext/RStartup.h>
#include <Rinternals.h>
#include <Rdefines.h>
#include <R_ext/Parse.h>

int LoadR(int argc, char **argv);

void EndR();

SEXP ParseOneLine(const char *code, SEXP env, ParseStatus *status);

int DefineSEXP(const char *name, SEXP val);

SEXP ExecuteOneLine(char *cmd, SEXP env, int *errorOccured);

#endif
