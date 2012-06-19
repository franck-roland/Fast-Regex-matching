#ifndef REGEXP_H
#define REGEXP_H

#include "manipulate.h"
#define couleur(param) printf("\033[%sm",param)

int matchandalign(char** answer,char* regex,char* tomatch,Fields* fields,int exactlymatch,int cimpl);
int match(char* s,char* tomatch,Fields* fields,int);
void retField(char *string,Fields* field,Subfield* sub,int options);
char* computeAlignement(Fields* fields,int options,int indFields,char* answer,char* message,int cimplement);
void showans(char* message, char* answer);

#endif
