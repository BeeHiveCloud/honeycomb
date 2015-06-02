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

#include "execute.h"

int LoadR(int argc, char **argv){
  char Rversion[25];
  sprintf(Rversion, "%s.%s", R_MAJOR, R_MINOR);

  if(strcmp(getDLLVersion(), Rversion) != 0) {
        fprintf(stderr, "Error: R.DLL version does not match\n");
        return -1;
  }

  return Rf_initEmbeddedR(argc, argv);
}

void EndR(){
	Rf_endEmbeddedR(0);
}

int DefineSEXP(const char *name, SEXP val){
  ParseStatus status;
  char *c = name;
  SEXP sym = ParseOneLine(c, R_GlobalEnv,&status);

  if(status!=1)
    return -1;

  if( TYPEOF(sym) == EXPRSXP && LENGTH(sym)>0 )
    sym = VECTOR_ELT(sym, 0);

  defineVar((sym)?sym:install(c),val,R_GlobalEnv);
  return 0;
}

SEXP ParseOneLine(const char *code, SEXP env, ParseStatus *status){
  SEXP cmd, expr;

  int errorOccured=1, retval=1;
  PROTECT(cmd = allocVector(STRSXP, 1));
  SET_STRING_ELT(cmd, 0, mkChar(code));
  PROTECT(expr = R_ParseVector(cmd, -1, status, R_NilValue));
  UNPROTECT(2);

  return expr;
}

SEXP ExecuteOneLine(char *cmd, SEXP env, int *errorOccured){
	SEXP val, expr;
	ParseStatus status;
	expr = ParseOneLine(cmd, env, &status);
	val = R_tryEval(VECTOR_ELT(expr, 0), env, errorOccured);
	return val;
}

int call(int argc, char **argv){
	char *c = "hello";
	int errorOccured;
	LoadR(argc,argv);

	SEXP val1;
	PROTECT(val1=allocVector(INTSXP, 10));
	for(int i=0; i<10 ;i++)
		INTEGER(val1)[i] = i + 1;
	UNPROTECT(1);

	DefineSEXP(c, val1);
	printf("print value: \n");

	for (int i = 0; i < LENGTH(val1); i++)
		printf("%d value is %d \n", i, INTEGER(val1)[i]);

	ExecuteOneLine("windows()", R_GlobalEnv, &errorOccured);
	ExecuteOneLine("plot(hello)", R_GlobalEnv, &errorOccured);

	//EndR();

	return 0;
}
