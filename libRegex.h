#define MaxLen 5
#define MaxFields 5000

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


PyObject* py_match(PyObject* self, PyObject* args);
int match(char* s,char* tomatch,Fields* fields,int*);
void freeFields(Fields* fields, int indFields);
int parse(char* regex,char* delim);
int parseVariableFields(char *pAdd, int* m, int* M);

Subfield* newSubfield( unsigned int offset, char* v,int isStatic,int* min,int* max,int maxlimit, int groupindex);
int addSubfield(Fields* field, char* token,int maxlimit,int);

void retField(PyObject** string,Fields* field,Subfield* sub);
//int totalfields(Fields* fields,int nbfields);
void adjustfield(Fields* field);


int freeSubfields(Subfield *sub){
	//printf("subfree len %d offset %d\n",sub->len,sub->offset);
	if(sub->value!=NULL){
		free(sub->value);
		sub->value = NULL;
	}
	if(sub->next!=NULL){	
		//if(freeSubfields(sub->next)==0)
		freeSubfields(sub->next);
			//free(sub->next);
	}
	return 0;
}
/*Free all the malloc Champ.value before returning error*/

void freeFields(Fields* fields, int indFields){
	while (indFields>0){
		//printf("\nFREE %d of length %d\n",indFields,fields[indFields-1].len);
		//printf("min %d max %d\n",fields[indFields-1].min,fields[indFields-1].max);
		if(fields[indFields-1].value != NULL){
				free(fields[indFields-1].value);
				//if((&fields[indFields-1])->subfields==NULL)
					//printf("soucis\n");
				
		}
		//if(fields[indFields-1].isStatic)
			freeSubfields((&fields[indFields-1])->subfields);
		indFields--;	
	}
}







int newField(Fields* field,int isStatic, char* add, char* token,int maxlimit,int groupindex){
	field->set = 1;
	field->isStatic = isStatic;
	field->add = add;
	field->value = NULL;
	field->min = 0;
	field->max = 0;
	field->len = 0;
	if(isStatic){
		field->len = strlen(token);
		field->subfields = newSubfield(0, token, isStatic,NULL,NULL,maxlimit,groupindex);
	}
	else{
		field->subfields = newSubfield(0, token, isStatic,&(field->min),&(field->max),maxlimit,groupindex);
		field->len = field->min;
	}
	field->lastfields = field->subfields;
	if(field->subfields == NULL)
		return -1;
	return 0;
}

void setAdd(Fields* field,char* add){
		if(add!=NULL)
			field->add = add;
}

int addSubfield(Fields* field, char* token,int maxlimit,int groupindex){
	if(field->subfields == NULL || field->lastfields == NULL)
		return -1;
	if(field->isStatic){	
		(field->lastfields)->next = newSubfield(field->len, token, field->isStatic,NULL,NULL,maxlimit,groupindex);
		field->len += strlen(token);
	}
	else{
		(field->lastfields)->next = newSubfield(field->min, token, field->isStatic,&(field->min),&(field->max),maxlimit,groupindex);
	}
	if((field->lastfields)->next == NULL)
		return -1;
	field->lastfields = (field->lastfields)->next;
	return 0;

}
void adjustfield(Fields* field){
	if(!(field->isStatic)){
		Subfield* sub = field->subfields;	
		int delta = field->len - field->min;
		int newdelta;
		int newoffset = 0;
		while(sub!=NULL){
			sub->offset += newoffset;
			if(delta>0){
				newdelta = sub->max - sub->len;
				if(newdelta>0 && newdelta<=delta){
					sub->len = sub->max;
					newoffset+=newdelta;
					delta-=newdelta;
				}
				else if(delta<newdelta){
					sub->len+=delta;
					newoffset+=delta;
					delta=0;
				}
			}
			sub = sub->next;
		}
	}

}

Subfield* newSubfield( unsigned int offset, char* v,int isStatic, int* min, int* max,int maxlimit,int groupindex){
	
	Subfield* subfield;
	subfield = (Subfield*) malloc(sizeof(Subfield));
	if(subfield!=NULL){
		subfield->offset = offset;
		subfield->next = NULL;
		subfield->value = NULL;
		subfield->groupindex = groupindex;
		if(isStatic){
			subfield->len = strlen(v);
			subfield->value = v;
		}
		else{
			int retparse;
			subfield->min = 0;
			subfield->max = maxlimit;			
			retparse = parseVariableFields(v,&(subfield->min),&(subfield->max));
			free(v);
			if(retparse<0)
				return NULL;
			subfield->len = subfield->min;
			*min += subfield->min;
			*max += subfield->max;
			if(*max >= maxlimit)
				*max = maxlimit;
		}
		
	}
	return subfield;
}

void setFieldvalue(Fields* field){
	if(field->isStatic){
		Subfield* currentsub = field->subfields;
		char *value;
		unsigned int len = field-> len;
		value = (char*) malloc((len+1)*sizeof(char));
		value[0]=0;
		while(currentsub!=NULL){
			strcat(value,currentsub->value);
			currentsub = currentsub->next;
		}
		value[len]=0;	
		field->len = len;
		field->value = value;
		//printf("valueng %s\n",field->value);
	}

}
void setFieldBorder(Fields* fields,int min, int max){
	fields->min = min;
	fields->max = max;
}



int isStatic(char *regex,int var){
	return !(regex[0] == var);
}
