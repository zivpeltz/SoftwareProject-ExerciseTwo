#define PY_SSIZE_T_CLEAN
#include <python.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>




//wrapper function for fit()
static PyObject* kmeansLib_fit(PyObject *self, PyObject *args){
    int k;
    int iter;
    int eps;
    PyObject **centroids;
    int num_of_points;
    int dim;
    PyObject **points;


}



// module's function table
static PyMethodDef kmeansLib_FunctionsTable[] = {
    {
        "fit", // name exposed to Python
        kmeansLib_fit, // C wrapper function
        METH_VARARGS, // received variable args
        "Calculates kmeans" // documentation
    },  {
        NULL, NULL, 0, NULL
    }
};


// modules definition
static struct PyModuleDef DemoLib_Module = {
    PyModuleDef_HEAD_INIT,
    "kmeanssp",     // name of module exposed to Python
    "kmeans Python wrapper for custom C extension library.", // module documentation
    -1,
    DemoLib_FunctionsTable
};