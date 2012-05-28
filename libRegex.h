#ifndef lIBREGEX_H
#define LIBREGEX_H
#include <Python.h>
#include "struct.h"
#include "regex.h"
#include "manipulate.h"

/*
*   Initializes the module for python
*
*/
PyMODINIT_FUNC init_libRegex(void);

/*
*   Function which find the alignment corresponding to the regex
*   or raises an exception which need to be caught.
*   args must be a tuple with the regex as first argument and the string to match as second
*/
PyObject* py_match(PyObject* self, PyObject* args);


/* Concatenate the string of fields from the same group
*   string: string representation of the group
*   field: the current field we are analyzing
*   sub: the subfield we add the String representation to the variable field
*/
void retField(PyObject** string,Fields* field,Subfield* sub,int options);

#endif
