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
    char* tomatch;
    char* regex;
    int indFields;
    int exactlymatch = 0;
    Fields fields[MaxFields];
    PyObject *recordedFields = NULL;
	char *answer = NULL;
    // Converts the arguments
    if (!PyArg_ParseTuple(args, "ssi", &regex, &tomatch, &exactlymatch)) {
    	PyErr_SetString(PyExc_TypeError, "Usage: _libRegex.match(regex,message,option) where option = 0 or 1");
        return NULL;
    }

    indFields = matchandalign(&answer,regex,tomatch,fields,exactlymatch,0);
    
    if(indFields>=0)
    {
        recordedFields = PyString_FromString(answer);
        free(answer);
        return recordedFields;
    }
    free(answer);
    char errormsg[]="Error append during alignment. See standard error output for more infos";      
    Py_XINCREF(exception);
    PyErr_SetObject(exception, Py_BuildValue("s#",errormsg,strlen(errormsg) ));
    return NULL;
}

