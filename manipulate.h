#ifndef MANIPULATE_H
#define MANIPULATE_H
#include "struct.h"

void dealloc(void**);
int freeSubfields(Subfield *sub);
void freeFields(Fields* fields, int indFields);
void freeFieldsCompletly(Fields* fields, int indFields);
void freeTokens(char** tokenarray,int begin,int end);

int newField(Fields* field,int isStatic, char* add, char* token,int maxlimit,int groupindex);
Subfield* newSubfield( unsigned int offset, char* v,int isStatic, int* min, int* max,int maxlimit,int groupindex);
int addSubfield(Fields* field, char* token,int maxlimit,int groupindex);

void setAdd(Fields* field,char* add);
void setFieldvalue(Fields* field);
void setFieldBorder(Fields* fields,int min, int max);

int parseVariableFields(char *pAdd, int* m, int* M);
void adjustfield(Fields* field);
int isStatic(char *regex,int var);

void doerrormessage(char *errormsg,int errorcode);

#endif
