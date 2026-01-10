#define PY_SSIZE_T_CLEAN
#include <python.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>




//wrapper function for fit()
static PyObject *kmeansLib_fit(PyObject *self, PyObject *args) {
    int k;
    int iter;
    double eps;
    PyObject *centroids_list;
    int num_of_points;
    int dim;
    PyObject *points_list;

    PyObject *point;
    PyObject *coord;

    int i, j;

    if (!PyArg_ParseTuple(args, "iidOiiO", &k, &iter, &eps, &centroids_list, &num_of_points, &dim, &points_list)) {
        return NULL;
    }

    /* Allocate centroids */
    double **centroids = (double **)malloc(k * sizeof(double *));
    for (i = 0; i < k; i++) {
        centroids[i] = (double *)malloc(dim * sizeof(double));
        point = PyList_GetItem(centroids_list, i);
        for (j = 0; j < dim; j++) {
            coord = PyList_GetItem(point, j);
            centroids[i][j] = PyFloat_AsDouble(coord);
        }
    }

    /* Allocate points */
    double **points = (double **)malloc(num_of_points * sizeof(double *));
    for (i = 0; i < num_of_points; i++) {
        points[i] = (double *)malloc(dim * sizeof(double));
        point = PyList_GetItem(points_list, i);
        for (j = 0; j < dim; j++) {
            coord = PyList_GetItem(point, j);
            points[i][j] = PyFloat_AsDouble(coord);
        }
    }

    /* Call C k-means function (returns new centroids) */
    double **new_centroids = fit(k, iter, eps, centroids, num_of_points, dim, points);

    /* Convert result to Python list */
    PyObject *result = PyList_New(k);
    for (i = 0; i < k; i++) {
        PyObject *centroid = PyList_New(dim);
        for (j = 0; j < dim; j++) {
            PyList_SetItem(
                centroid,
                j,
                PyFloat_FromDouble(new_centroids[i][j])
            );
        }
        PyList_SetItem(result, i, centroid);
    }

    /* Free input centroids */
    for (i = 0; i < k; i++) {
        free(centroids[i]);
    }
    free(centroids);

    /* Free points */
    for (i = 0; i < num_of_points; i++) {
        free(points[i]);
    }
    free(points);

    /* Free centroids returned by fit */
    for (i = 0; i < k; i++) {
        free(new_centroids[i]);
    }
    free(new_centroids);

    return result;
}



// module's function table
static PyMethodDef kmeanssp_FunctionsTable[] = {
    {
        "fit",                 /* name exposed to Python */
        kmeansLib_fit,         /* C wrapper function */
        METH_VARARGS,          /* receives positional arguments */
        "Calculates k-means"   /* documentation */
    },
    {NULL, NULL, 0, NULL}
};



// modules definition
static struct PyModuleDef kmeanssp_Module = {
    PyModuleDef_HEAD_INIT,
    "kmeanssp",   /* module name */
    "kmeans Python wrapper for custom C extension library.",
    -1,
    kmeanssp_FunctionsTable
};

PyMODINIT_FUNC PyInit_kmeanssp(void) {
    return PyModule_Create(&kmeanssp_Module);
}

