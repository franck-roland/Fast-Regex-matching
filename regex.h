#ifndef REGEXP_H
#define REGEXP_H

#include "struct.h"
#include "manipulate.h"



int match(char* s,char* tomatch,Fields* fields,int*,int);
void retField(char *string,Fields* field,Subfield* sub,int options);
char* computeAlignement(Fields* fields,int options,int indFields,char* answer,char* message,int cimplement);

#endif
