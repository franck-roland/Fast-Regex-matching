#include "libRegex.h"



PyObject *exception = NULL;

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
		case -4 : strcpy(errormsg, "The regex is empty"); break;
		case -5 : strcpy(errormsg, "The chain to match is empty"); break;
		case -12 : strcpy(errormsg, "One variable value is to large. 5 digit maximum"); break;
		case -13 : strcpy(errormsg, "One min greater than max"); break;
		default : strcpy(errormsg, "Error"); break;
	}
	if(mError == NULL)
		PyErr_SetObject(exception, Py_BuildValue("s#",errormsg,strlen(errormsg) ));
	else{
		PyErr_SetObject(exception, Py_BuildValue("s#s",errormsg,strlen(errormsg), mError));
		dealloc((void **)&mError);
	}
	return 	NULL;
}
void retField(PyObject **string,Fields* field,Subfield* sub){
	field->subfields = sub->next;
	PyObject *toRet = Py_BuildValue("s#", field->add+sub->offset, sub->len);
	PyString_ConcatAndDel(string,toRet);
	dealloc((void **)&sub);

}

