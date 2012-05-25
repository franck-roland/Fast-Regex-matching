#ifndef REGEXP_H
#define REGEXP_H

#include "struct.h"
#include "manipulate.h"
extern char* mError;



int match(char* s,char* tomatch,Fields* fields,int*);
int parse(char* regex,char* delim);


#endif
