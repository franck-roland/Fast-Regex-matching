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
    char* errormsg;
    char* tomatch;
    char* regex;
    int indFields;
    int totalfields=0;
    int exactlymatch = 0;
    Fields fields[MaxFields];
    PyObject *recordedFields = NULL;

    // Converts the arguments
    if (!PyArg_ParseTuple(args, "ssi", &regex, &tomatch, &exactlymatch)) {
    	PyErr_SetString(PyExc_TypeError, "Usage: _libRegex.match(regex,message,option) where option = 0 or 1");
        return NULL;
    }

    indFields = match(regex,tomatch,fields,&totalfields,exactlymatch);
    
    if(indFields>=0)
    {
	    char* answer = (char*) malloc((strlen(tomatch)*2+1)*sizeof(char));
	    memset(answer,0,(strlen(tomatch)*2+1));
        computeAlignement(fields,exactlymatch,indFields,answer,tomatch,0);
        recordedFields = PyString_FromString(answer);
        free(answer);
        return recordedFields;
    }
    errormsg = (char*) malloc((strlen(regex)+512)*sizeof(char));      
    memset(errormsg,0,(strlen(regex)+512));
    Py_XINCREF(exception);

   	doerrormessage(errormsg,indFields);
    PyErr_SetObject(exception, Py_BuildValue("s#",errormsg,strlen(errormsg) ));
    dealloc((void **)&errormsg);
    return NULL;
}

