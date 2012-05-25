#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <Python.h>
#include "libRegex.h"
//#include "commonLib.h"



PyObject *exception = NULL;
char* mError = NULL;
static PyMethodDef regex_methods[] = {
  {"match", py_match, METH_VARARGS},
  {NULL, NULL}
};
//+---------------------------------------------------------------------------+
//| initlibScoreComputation : Python will use this function to init the module
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
	int indChamps;
	int i;
	Champs champs[5012];

	for(i = 0; i<5012;i++){
		champs[i].set = 0;
	}
	PyObject *recordedFields = NULL;
	//PyObject *exception = NULL;

	//printf("in\n");
	// Converts the arguments
  	if (!PyArg_ParseTuple(args, "ss", &regex, &tomatch)) {
	    	PyErr_SetString(PyExc_TypeError, "Error while parsing the arguments provided to py_getHighestEquivalentGroup");
    		return args;
  	}
	if(strcmp(regex,tomatch)==0){
		recordedFields = PyList_New(1);
		PyObject *field = Py_BuildValue("s", regex);
		if (!field) {
			Py_XDECREF(recordedFields);		
		   	return NULL;
        }
		PyList_SET_ITEM(recordedFields, 0, field);
		return recordedFields;
	}
	//printf("ok %s\n",regex);
	indChamps = match(regex,tomatch,champs);
	freeFields(champs,indChamps);	
	if(indChamps>=0){
		recordedFields = PyList_New(indChamps+1);
		for (i=0;i<=indChamps;i++){
			//printf("%ld\n",(champs[indChamps].len)/2);
			//printf("%s\n",champs[indChamps].add);
			PyObject *field = Py_BuildValue("s#", champs[i].add, champs[i].len);
			if (!field) {
		            Py_XDECREF(recordedFields);		
		            return NULL;
        		}
			PyList_SET_ITEM(recordedFields, i, field);
			//indChamps--;
		}
		return recordedFields;
	}
	Py_XINCREF(exception);

	switch (indChamps){
		case -1 :
		case -11 : strcpy(errormsg,"Format Error"); break;
		case -2 : strcpy(errormsg, "Cannot match the string with the regex"); break;
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

/*Free all the malloc Champ.value before returning error*/

void freeFields(Champs* champs, int indChamps){
	while (indChamps>0){
		if(champs[indChamps].value != NULL){
				free((&champs[indChamps-1])->value);
		}
		indChamps--;	
	}
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
			else if (i==1)
				return 1;
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



void assignField(Champs* champ,int isStatic, char* add,char* value, unsigned long len,int min, int max){
	champ->set = 1;
	champ->isStatic = isStatic;
	champ->add = add;
	champ->value = value;
	champ->len = len;
	champ->min = min;
	champ->max = max;
}


/*
	shift:	
	ind: 	index of the previous variable field

	.......aaaaaa................bbbbbbb
		^				^
		|				|
	previous		too long
*/
int rollBack(unsigned long shift, int ind,Champs* champs, char* tomatch, int first)
{
	char* nextmatching;
	Champs* stat = (&champs[ind+1]);
	Champs* var = (&champs[ind]);
	int retvalue = 0;

	if(first){
		if(ind >= 2){
			while(1){
				shift = (unsigned long)(stat->add - var->add - var->max);
				retvalue = rollBack(shift, ind - 2,champs,tomatch,0);
				if( retvalue == 0){
					var->add = champs[ind-1].add + champs[ind-1].len;
					if(stat->add - var->add >= var->min){
						var->len = (unsigned long)(stat->add - var->add);
						return 0;
					}
					else{
						nextmatching = strstr(var->add + var->min, stat->value);
						if (nextmatching == NULL){
							return -1;
						}
						else{
							stat->add = nextmatching;
							var->len = (unsigned long)(nextmatching - var->add);
							if(var->len<=var->max)
								return 0;
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
		else if((unsigned long)(nextmatching - var->add)<= var->max){
			stat->add = nextmatching;
			var->len = (unsigned long)(nextmatching - var->add);
			return 0;
		}
		else if(ind >= 2){
			shift = (unsigned long)(nextmatching - var->add - var->max);
			retvalue = rollBack(shift, ind - 2,champs,tomatch,0);
			if( retvalue == 0){
				var->add = champs[ind-1].add + champs[ind-1].len;
				if(nextmatching - var->add >= var->min){
					stat->add = nextmatching;
					var->len = (unsigned long)(nextmatching - var->add);
					return 0;
				}
				else{
					nextmatching = strstr(var->add + var->min, stat->value);
					if (nextmatching == NULL){
						return -1;
					}
					else{
						stat->add = nextmatching;
						var->len = (unsigned long)(nextmatching - var->add);
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
	return: 
		ind = last field index
		-1 : "format error"
		-2 : "not matching"


*/
int match(char* s,char* tomatch,Champs* champs){
	#define isgood(c) isalnum(c)||c==46
	int ind=0;
	int i;
	unsigned long size=0;
	char *current;
	char* posmatch;
	int messageLen = strlen(tomatch);
	int premierconst = !(s[0] == 46);
	int m;
	int M;
	int shift = 0;
	int rollret = 0;
	char* tomatchcopy = tomatch;

	if(strlen(s) == 1 && s[0]==46){
		(&champs[ind])->add = tomatch ;
		(&champs[ind])->len = strlen(tomatch);
		return ind;
	}
	for(i=0;i<strlen(s);i++){

		/*The character is a forbidden one*/
		if((isgood(s[i]))==0){
			freeFields(champs,ind);
			char mismatch[]=" not expected";
			mError = (char* )malloc((strlen(mismatch)+2)*sizeof(char));
			mError[0] = s[i];
			strncpy(mError+1,mismatch,strlen(mismatch));
			mError[strlen(mismatch)+1] = '\x00';
			return -1;
		}

		/*********************************************************/
		//Variable field
		if(s[i] == 46){

			//The first field is constant
			if(premierconst){
					assignField(&champs[ind],1, tomatch-size, NULL, size,0,0);
					ind++;	
				size = 0;
			}
			premierconst = 0;

			// Variable field
			if(champs[ind].set == 1 && champs[ind].isStatic == 1){
								
				ind++;
			}
			assignField(&champs[ind],0, tomatch,NULL, 0,0,messageLen);
			shift = parseVariableFields(s+i, &(champs[ind].min), &(champs[ind].max));
			ind++;
			//Compute the min and max of the new variable field
			
			if(shift<0){
				freeFields(champs,ind);
				return shift;
			}
			//len of variable field = min by default;
			(&champs[ind])->len = champs[ind].min;


			
			
			//indicate that the previous field is static
			if(size!=0){
				current = (char *)malloc((size+1)*sizeof(char));
				memcpy (current, s+(i-size), (size_t) size);
				current[size]='\x00';				
				posmatch = strstr(tomatch+champs[ind-2].min,current);
				
				/*Cannot find any substring matching with the constant field*/
				if(posmatch == NULL){
					freeFields(champs,ind);
					return -2;
				}
				else{
					// Static field
					assignField(&champs[ind],1, posmatch, current, size,0,0);
					ind++;
	
					// If the length of the variable field is higher than the max authorized, we need to rollback

					if((unsigned long)(posmatch-tomatch)>champs[ind-1].max){
						rollret = rollBack(0, ind-2,champs,tomatchcopy, 1);
						if(rollret!=0){
							freeFields(champs,ind);
							return -2;
						}
						tomatch = champs[ind-1].add+size;
					}
					else
						tomatch=posmatch+size;
                }
				size = 0;
			}
			else{
				tomatch+=champs[ind].len;
			}			
			i+=shift-1;
			
		}

		/**********************************************************************/
		//Static field
		else {
			//regex begin by a constant
			if(premierconst){
				if((0==strlen(tomatch))){
					freeFields(champs,ind);
					return -2;
				}
				if(!(tomatch[0]==s[i])){
					freeFields(champs,ind);
					return -2;
				}
				tomatch ++;
			}
			size++;
			assignField(&champs[ind],1, NULL, NULL, size,0,0);
		}
	}
	
	//The last field is constant
	if(size!=0){
		if(tomatch[0]==0){
			freeFields(champs,ind);
			return -1;
		}
		int comp;
		current = (char *)malloc((size+1)*sizeof(char));
		memcpy (current, s+(i-size), (size_t) size);
		current[size]='\x00';
		//posmatch = strstr(tomatch+m,current);
		comp = strcmp(&tomatch[strlen(tomatch)-size],current);
		if(comp != 0){
			freeFields(champs,ind);
			return -2;
		}
		//Variable Field
		assignField(&champs[ind],0, tomatch, NULL, (unsigned long)(strlen(tomatch)-size),m,M);
		ind++;
		//Static Field
		assignField(&champs[ind],1, &tomatch[strlen(tomatch)-size], current, size,0,0);
		ind++;
		if((unsigned long)(strlen(tomatch)-size)>M){
			rollret = rollBack(0, ind-2,champs,tomatchcopy, 1);
			if(rollret!=0){
				freeFields(champs,ind);
				return -2;
			}
			tomatch = champs[ind-1].add+size;
		}
/*		else
			tomatch=posmatch+size;*/
		//tomatch=posmatch+size;
	}
	
	//last field is variable
	else{
		if(strlen(tomatch)<m){
			freeFields(champs,ind);
			return -2;
		}
		else if(strlen(tomatch)>M){
			rollret = rollBack(strlen(tomatch)-M, ind-2,champs,tomatchcopy, 0);
			if(rollret != 0){
				freeFields(champs,ind);
				return -2;
			}
			tomatch = champs[ind-1].add+champs[ind-1].len;
		}
		assignField(&champs[ind],0, tomatch, NULL,strlen(tomatch),m,M);
		ind++;	
	}
	#undef isgood
	return --ind;
}


int main(int argc, char** argv){
	int indChamps;
	char* tomatch;
	char* regex;
	Champs champs[1024];
	if (argc ==3){
		tomatch = argv[2];
		regex = argv[1];
		indChamps = match(regex,tomatch,champs);
		if(indChamps>=0){
			printf("GOOD\n");
			while (indChamps>=0){
				//printf("%ld\n",(champs[indChamps].len)/2);
				//printf("%s\n",champs[indChamps].add);
				indChamps--;
			}
			//printf("%s\n",tomatch);
		}
		else
			printf("Echec\n");

	}
	return 0;
}
