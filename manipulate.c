#include "manipulate.h"

void dealloc(void **s)
{
	if(*s!=NULL){
		free(*s);
		*s = NULL;
	}

}
char* mError = NULL;
/*
	parseVariableFields
	return value:
		error code:
			-11 : wrong format
			-12 : min field to large or max field to large
			-13 : max < min
			-4 : 
		normal return:
			 0 : everything ok
	default values:
		____________________
		min = 0  	    |
				    |  no constraints
		max = len(message)  |
		____________________|
		
		Constraints : 
			max >= min
			if max == min => fields of len max
		
*/
int parseVariableFields(char *pAdd, int* m, int* M){
	
	int i;	
	int j;
	int min=0;
	int max=*M; //init at messagelength
	int len = 0;
	char buff[MaxLen];
	int ind = 0;
	char format[]=".{0,0}";
	char format2[]=".{0}";
	int ismin = 1;
	int nextseparator = 0;
	//printf("to parse %s\n",pAdd);
	
	for(i=0;i<strlen(format);i++){
		if(pAdd[0]==0)
			break;
		if(format[i]=='0'){
			nextseparator=1;
			len = 0;
			while(isdigit(pAdd[len]) && len < MaxLen){
				len++;
			}
			if(len == MaxLen){
				char message[]="The size of variable field cannot be higher than 5 digits : ";
				char etc[] = "... in ";
				mError = (char* )malloc((strlen(message)+strlen(pAdd-ind)+strlen(etc)+len+1)*sizeof(char));
				memset(mError,0,(strlen(message)+strlen(pAdd-ind)+strlen(etc)+len+1));
				strcat(mError,message);				
				strncat(mError,pAdd,len);
				strcat(mError,etc);
				strcat(mError,pAdd-ind);				
				return -12;
			}			
			if(len>0){
				memcpy (buff, pAdd, (size_t) len);
				for(j=len;j<MaxLen;j++){
					buff[j] = '\x00';
				}

				if(ismin)
					min = atoi(buff);
				else
					max = atoi(buff);
			}
			else{
				if(ismin)
					min = *m;
				else
					max = *M;
			}
			pAdd+=len;
			ind += len;
		}
		else{
			if(pAdd[0]==format[i]){
				if(nextseparator)
					ismin = 0;
				ind += 1;
				pAdd++;
			}
			else if(pAdd[0]==format2[i]){
				ind += 1;
				pAdd++;
				max = min;
				break;
			}
			else{
				char mismatch[]=" expected but found ";
				char in[]=" in ";
				char rem[]=".\nUsage Reminder for variable field: . alone or .{min,max} or .{size} where min<max and min and max are two numbers of 5digits maximum and size is a fixed size for the variable field.";
				mError = (char* )malloc((strlen(pAdd-ind)+strlen(in)+strlen(mismatch)+3+strlen(rem))*sizeof(char));
				memset(mError,0,(strlen(pAdd-ind)+strlen(in)+strlen(mismatch)+3+strlen(rem)));
				mError[0] = format[i];
				strcat(mError,mismatch);
				mError[strlen(mismatch)+1] = pAdd[0];
				strcat(mError,in);
				strcat(mError,pAdd-ind);
				strcat(mError,rem);
				return -11;
			}
		}
	}
	if(max < min){
		char maxmin[]="The maximum cannot be lower than the minimum : ";
		mError = (char* )malloc((strlen(pAdd-ind)+strlen(maxmin)+1)*sizeof(char));
		memset(mError,0,ind+strlen(maxmin));
		strcat(mError,maxmin);
		strcat(mError,pAdd-ind);
		return -13;
	}
	*m = min;
	*M = max;
	return ind;
}

int freeSubfields(Subfield *sub){
	//printf("subfree len %d offset %d\n",sub->len,sub->offset);
	dealloc((void **)&sub->value);
	if(sub->next!=NULL){	
		//if(freeSubfields(sub->next)==0)
		freeSubfields(sub->next);
			//free(sub->next);
	}
	return 0;
}
/*Free all the malloc Fields.value before returning error*/

void freeFields(Fields* fields, int indFields){
	while (indFields>0){
		//printf("\nFREE %d of length %d\n",indFields,fields[indFields-1].len);
		//printf("min %d max %d\n",fields[indFields-1].min,fields[indFields-1].max);
		dealloc((void **)&fields[indFields-1].value);
				//if((&fields[indFields-1])->subfields==NULL)
					//printf("soucis\n");
		if(fields[indFields-1].subfields != NULL)
			freeSubfields((&fields[indFields-1])->subfields);
		indFields--;	
	}
}
/* Free fields and subfields before returning an error code*/
void freeFieldsCompletly(Fields* fields, int indFields){
	freeFields(fields,indFields);
	Subfield* sub;
	while (indFields>0){
		sub = fields[indFields-1].subfields;
		while(sub!=NULL){
			fields[indFields-1].subfields = (fields[indFields-1].subfields)->next;
			dealloc((void **)&sub);
			sub = fields[indFields-1].subfields;
		}
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
			dealloc((void **)&v);
			if(retparse<0){
				dealloc((void **)&subfield);
				return NULL;
			}
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

