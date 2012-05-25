#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <Python.h>
#include "libRegex.h"



PyObject *exception = NULL;
char* mError = NULL;
static PyMethodDef regex_methods[] = {
  {"match", py_match, METH_VARARGS},
  {NULL, NULL}
};
//+---------------------------------------------------------------------------+
//| initlibRegex : Python will use this function to init the module
//+---------------------------------------------------------------------------+
PyMODINIT_FUNC init_libRegex(void) {
	PyObject *d = PyDict_New();
	(void) Py_InitModule("_libRegex", regex_methods);
	#if PYTHON_API_VERSION >= 1007
	    exception = PyErr_NewException("_libRegex.error", NULL, NULL);
	#else
    	exception = Py_BuildValue("s", "_libRegex.error");
	#endif
    	PyDict_SetItemString(d, "error", exception);
}


PyObject* py_match(PyObject* self, PyObject* args) {
	char errormsg[512];
	char* tomatch;
	char* regex;
	int indFields;
	int i;
	int totalfields=0;
	Fields fields[MaxFields];

	for(i = 0; i<MaxFields;i++){
		fields[i].set = 0;
		fields[i].subfields = NULL;
		fields[i].lastfields = NULL;
	}
	PyObject *recordedFields = NULL;
	// Converts the arguments
  	if (!PyArg_ParseTuple(args, "ss", &regex, &tomatch)) {
	    	PyErr_SetString(PyExc_TypeError, "Error while parsing the arguments provided to py_getHighestEquivalentGroup");
    		return NULL;
  	}
	/*if(strcmp(regex,tomatch)==0){
		recordedFields = PyList_New(1);
		PyObject *field = Py_BuildValue("s", regex);
		if (!field) {
			Py_XDECREF(recordedFields);		
		   	return NULL;
       		}
		PyList_SET_ITEM(recordedFields, 0, field);
		return recordedFields;
	}*/
	indFields = match(regex,tomatch,fields,&totalfields);
	freeFields(fields,indFields+1);
	if(indFields>=0)
	{
		int index = 0;
		PyObject *string = PyString_FromString("");
		recordedFields = PyList_New(totalfields);
		for (i=0;i<=indFields;i++){
			adjustfield(&fields[i]);
			while(fields[i].subfields!=NULL){
				if((fields[i].subfields)->groupindex!=index){
					PyList_SET_ITEM(recordedFields, index, string);
					string = PyString_FromString("");
					index++;
				}				
				retField(&string,&fields[i],fields[i].subfields);
			}
		}
		PyList_SET_ITEM(recordedFields, index, string);
		return recordedFields;
	}
		
	Py_XINCREF(exception);

	switch (indFields){
		case -1 :
		case -11 : strcpy(errormsg,"Format Error"); break;
		case -2 : strcpy(errormsg, "Cannot match the string with the regex"); break;
		case -3 : strcpy(errormsg, "Too much different fields"); break;
		case -12 : strcpy(errormsg, "One variable value is to large. 5 digit maximum"); break;
		case -13 : strcpy(errormsg, "One min greater than max"); break;
		default : strcpy(errormsg, "Error"); break;
	}
	if(mError == NULL)
		PyErr_SetObject(exception, Py_BuildValue("s#",errormsg,strlen(errormsg) ));
	else
		PyErr_SetObject(exception, Py_BuildValue("s#s",errormsg,strlen(errormsg), mError));
		free(mError);
	return 	NULL;
}
void retField(PyObject **string,Fields* field,Subfield* sub){
	field->subfields = sub->next;
	PyObject *toRet = Py_BuildValue("s#", field->add+sub->offset, sub->len);
	PyString_ConcatAndDel(string,toRet);
	free(sub);
	sub = NULL;

}



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
		min = 0  	 		|
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
				char etc[]="...";
				mError = (char* )malloc((strlen(etc)+len+1)*sizeof(char));
				strncpy(mError,pAdd,len);
				strncpy(mError+len,etc,strlen(etc));
				mError[strlen(etc)+len]='\x00';
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
				char rem[]=".\nUsage Reminder for variable field: . alone or ._min,max (space) where min<max and min and max are two numbers of 5digits maximum.\nDo not forget the space after the max";
				mError = (char* )malloc((strlen(mismatch)+3+strlen(rem))*sizeof(char));
				mError[0] = format[i];
				strncpy(mError+1,mismatch,strlen(mismatch));
				mError[strlen(mismatch)+1] = pAdd[0];
				strncpy(mError+strlen(mismatch)+2,rem,strlen(rem));
				mError[strlen(mismatch)+strlen(rem)+2] = '\x00';
				return -11;
			}
		}
	}
	if(max < min){
		mError = (char* )malloc(ind*sizeof(char));
		strncpy(mError,pAdd-ind,ind-1);
		mError[ind-1] = '\x00';
		return -13;
	}
	*m = min;
	*M = max;
	return ind;
}

/*
*	parsegroup
*	
*/

char* createtoken(unsigned int size, char* from){
	char* nt;

	nt = (char *) malloc (size*sizeof(char));
	memset(nt,0,size);
	strncpy(nt,from,size-1);
	return nt;

}
int parsegroup(char* token,char ** groups)
{
	int nbdifferentfield=0;
	char* dot;
	char* nextdot;
	char* nextbrack;
	unsigned int ntsize = 0;

	
	dot = strchr(token,'.');
	while(dot != NULL){
		nextbrack = strchr(dot,'}');
		nextdot=strchr(dot+1,'.');
		if(dot != token){
			ntsize = (unsigned int)(dot-token+1);
			groups[nbdifferentfield] = createtoken(ntsize,token);
			nbdifferentfield++;
			token = dot;
		}
		else{
			//There is no more variable fields
			if(nextdot == NULL){
					if(nextbrack==NULL){
						ntsize = 2;
						groups[nbdifferentfield] = createtoken(ntsize,token);
						nbdifferentfield++;
						token = dot+1;
					}
					else{
						ntsize = (unsigned int)(nextbrack-dot+2);
						groups[nbdifferentfield] = createtoken(ntsize,token);
						nbdifferentfield++;
						token = nextbrack+1;
					}
			}
			else{
					if(nextbrack==NULL || (nextbrack >= nextdot)){
						ntsize = 2;
						groups[nbdifferentfield] = createtoken(ntsize,token);
						nbdifferentfield++;
						token = dot+1;
					}				
					else{
						ntsize = (unsigned int)(nextbrack-dot+2);
						groups[nbdifferentfield] = createtoken(ntsize,token);
						nbdifferentfield++;
						token = nextbrack+1;
						
					}
					ntsize = (unsigned int)(nextdot - token + 1);
					groups[nbdifferentfield] = createtoken(ntsize,token);
					nbdifferentfield++;
					token = nextdot;
			}
			dot = nextdot;
		}
		
	}
	if(strlen(token)>0){
		ntsize = (unsigned int)(strlen(token)+ 1);
		groups[nbdifferentfield] = createtoken(ntsize,token);
		nbdifferentfield++;
	}
	return nbdifferentfield;
}

/*
	rollBack
	*********************************************
	shift:	
	ind: 	index of the previous variable field

	.......aaaaaa................bbbbbbb
		^				^
		|				|
	previous		too long
*/
int rollBack(unsigned int shift, int ind,Fields* fields, char* tomatch, int first, int lastvar)
{
	char* nextmatching;
	Fields* stat = (&fields[ind+1]);
	Fields* var = (&fields[ind]);
	int retvalue = 0;
	if(first){
		if(ind >= 2){
			while(1){
                if(!lastvar)
				    shift = (unsigned int)(stat->add - var->add - var->max);
				retvalue = rollBack(shift, ind - 2,fields,tomatch,0,0);
				if( retvalue == 0){
					var->add = fields[ind-1].add + fields[ind-1].len;
                    if(!lastvar){
				    	if(stat->add - var->add >= var->min){
				    		var->len = (unsigned int)(stat->add - var->add);
				    		return 0;
				    	}
				    	else{
				    		nextmatching = strstr(var->add + var->min, stat->value);
				    		if (nextmatching == NULL){
				    			return -1;
				    		}
				    		else{
				    			stat->add = nextmatching;
				    			var->len = (unsigned int)(nextmatching - var->add);
				    			if(var->len<=var->max)
				    				return 0;
				    		}
				    	}
                    }
                    else{
                        if(strlen(stat->add) >= var->min){
				    		var->len = (unsigned int)(strlen(stat->add));
				    		return 0;
				    	}
                        else{
                            return -1;
                        }
                    }
				}
				else
					return -1;
			}
		}
		else{
			return -1;
		}
	}
	else{
		nextmatching = strstr(stat->add + shift, stat->value);
		// Cannot shift the previous static field
		if (nextmatching == NULL){
			return -1;
		}
		// The next matching of aaaaa is such as the len of the variable field is lower than the max authorized
		else if((unsigned int)(nextmatching - var->add)<= var->max){
			stat->add = nextmatching;
			var->len = (unsigned int)(nextmatching - var->add);
			return 0;
		}
		else if(ind >= 2){
			shift = (unsigned int)(nextmatching - var->add - var->max);
			retvalue = rollBack(shift, ind - 2,fields,tomatch,0,0);
			if( retvalue == 0){
				var->add = fields[ind-1].add + fields[ind-1].len;
				if(nextmatching - var->add >= var->min){
					stat->add = nextmatching;
					var->len = (unsigned int)(nextmatching - var->add);
					return 0;
				}
				else{
					nextmatching = strstr(var->add + var->min, stat->value);
					if (nextmatching == NULL){
						return -1;
					}
					else{
						stat->add = nextmatching;
						var->len = (unsigned int)(nextmatching - var->add);
						return 0;
					}
				}
			}
			else
				return -1;
		}
		else
			return -1;
	}
	return 0;
}



/*	
	match
	*********************************************
	return: 
		ind = last field index
		-1 : "format error"
		-2 : "not matching"


*/
int match(char* regex,char* tomatch,Fields* fields,int* groupindex){
	#define isgood(c) isalnum(c)||c==46

	char delim='.';
	char border[]="()";
	int ind=0;
	unsigned int size=0;
	char* posmatch;
	int maxlimit = strlen(tomatch);
	int rollret = 0;
	char* tomatchcopy = tomatch;
	int retvalue =0;
	char* tempgroup;
	char *str1;
	char* token;
	char* groups[MaxFields];
	int nbsubtoken = 0;
	int i;

	for(str1 = regex;;str1 = NULL){
		tempgroup = strtok(str1,border);
		if (tempgroup==NULL)
			break;
		
		nbsubtoken = parsegroup(tempgroup,groups);
		if(nbsubtoken<=0){
			freeFields(fields,ind+1);
			return -1;
		}
		
		for(i=0;i<nbsubtoken;i++){
		    token=groups[i];

    		//Very first field
	    	if(fields[ind].set==0){
		    	retvalue = newField(&fields[ind],isStatic(token,'.'),tomatch,token,maxlimit,*groupindex);
			    if(retvalue<0){
		    		freeFields(fields,ind+1);
		    		return retvalue;
		    	}
		    	size+=strlen(token);
		    }

		
    		//New type of field
    		else if(fields[ind].isStatic != isStatic(token,'.')){
    			if(fields[ind].isStatic){
    				setFieldvalue(&fields[ind]);
    				if(ind==0){
    					posmatch = strstr(tomatch,fields[ind].value);
    					if(posmatch!=tomatch){
    						freeFields(fields,ind+1);
    						return -1;
    					}
    					else{
    						setAdd(&fields[ind],posmatch);
	    					tomatch = posmatch + fields[ind].len;
    					}
    				}
    				else{	
						posmatch = strstr(tomatch+fields[ind-1].min,fields[ind].value);
    					if(posmatch == NULL){
						   	freeFields(fields,ind+1);
							return -2;
						}
						else if((unsigned int)(posmatch-tomatch)>fields[ind-1].max){
							rollret = rollBack(0, ind-1,fields,tomatchcopy, 1,0);
							if(rollret!=0){
								freeFields(fields,ind+1);
								return -2;
							}
							//fields[ind-1].len = (int)(fields[ind].add-fields[ind-1].add);
							tomatch = fields[ind].add+fields[ind].len;
						}
						else{
							fields[ind-1].len = (int)(posmatch-tomatch);
							setAdd(&fields[ind],posmatch);
							tomatch = posmatch + fields[ind].len;
						}
				    }
			    }
		    	size = 0;
		    	ind++;
		    	if(ind>=MaxFields-1){
		    		freeFields(fields,ind);
		    		return -3;
	    		}
		    	retvalue = newField(&fields[ind],isStatic(token,'.'),tomatch,token,maxlimit,*groupindex);
	    		if(retvalue<0){
		    		freeFields(fields,ind+1);
		    		return retvalue;
		    	}
		    	size+=strlen(token);
		    }

	    	//Same type of field => push the subfield in the chainlist 
	    	else{
		    	retvalue = addSubfield(&fields[ind],token,maxlimit,*groupindex);
		    	if(retvalue<0){
				freeFields(fields,ind+1);
		        		return retvalue;
		    	}
		    	size+=strlen(token);
		    }
		}

		++*groupindex;
	}

	//last field
	if(fields[ind].isStatic){
		setFieldvalue(&fields[ind]);
		if(ind==0){
			posmatch = strstr(tomatch,fields[ind].value);
			if(posmatch!=tomatch){
				freeFields(fields,ind+1);
				return -1;
			}
			else{
				setAdd(&fields[ind],posmatch);
				tomatch = posmatch + fields[ind].len;
			}
		}
		else{	
				posmatch = strstr(tomatch+fields[ind-1].min,fields[ind].value);
				if(posmatch == NULL){
					freeFields(fields,ind+1);
					return -2;
				}
                    
                if(strcmp(tomatch+(strlen(tomatch)-strlen(fields[ind].value)), fields[ind].value) != 0){
                    freeFields(fields,ind+1);
					return -2;
                }
                else{
                    posmatch = tomatch+(strlen(tomatch)-strlen(fields[ind].value));
                    setAdd(&fields[ind],posmatch);
                }
                printf("%s\n",tomatch+(strlen(tomatch)-strlen(fields[ind].value)));
				if((unsigned int)(posmatch-tomatch)>fields[ind-1].max){
                    printf("in\n");
					rollret = rollBack(0, ind-1,fields,tomatchcopy, 1,0);
					if(rollret!=0){
						freeFields(fields,ind+1);
						return -2;
					}
					//fields[ind-1].len = (int)(fields[ind].add-fields[ind-1].add);
					tomatch = fields[ind].add+fields[ind].len;
				}
				else{
					fields[ind-1].len = (int)(posmatch-tomatch);
					setAdd(&fields[ind],posmatch);
					tomatch = posmatch + fields[ind].len;
				}
		}
	}
	else{
        fields[ind].len =strlen(tomatch);
		if(strlen(tomatch)>fields[ind].max){
            rollret = rollBack(strlen(tomatch)-fields[ind].max, ind,fields,tomatchcopy, 1,1);
            if(rollret!=0){
               freeFields(fields,ind+1);
				return -2; 
            }
        }

	}
	return ind;
}

/*
int main(int argc, char** argv){
	int indFields;
	char* tomatch;
	char* regex;
	Fields fields[1024];
	if (argc ==3){
		tomatch = argv[2];
		regex = argv[1];
		indFields = match(regex,tomatch,fields);
		if(indFields>=0){
			//printf("GOOD\n");
			while (indFields>=0){
				////printf("%ld\n",(fields[indFields].len)/2);
				////printf("%s\n",fields[indFields].add);
				indFields--;
			}
			////printf("%s\n",tomatch);
		}
		else
			//printf("Echec\n");

	}
	return 0;
}*/
