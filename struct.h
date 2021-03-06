#ifndef Struct_H
#define Struct_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define MaxLen 5
#define MaxFields 5000

extern char* mError;

typedef struct Subfield Subfield;
struct Subfield{
	Subfield *next;
	char* value;
	unsigned int offset;
	unsigned int len;
	int min;
	int max;
	int groupindex;
};

typedef struct Fields Fields;
struct Fields{
	int set;
	Subfield* subfields;
	Subfield* lastfields;
	int subfieldsSize;
	int isStatic;
	char* add;
	char* value;
	unsigned int len;
	int min;
	int max;
};
#endif
