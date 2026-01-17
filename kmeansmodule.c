#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "kmeanslogic.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* helper: free a partially/fully allocated double** matrix */
static void free_matrix(double **m, int rows_allocated) {
    int i;
    if (!m) return;
    for (i = 0; i < rows_allocated; i++) {
        free(m[i]);
    }
    free(m);
}

/* wrapper function for fit() */
static PyObject *kmeansLib_fit(PyObject *self, PyObject *args) {
	(void)self;
    int k;
    int iter;
    double eps;
    PyObject *centroids_list;
    int num_of_points;
    int dim;
    PyObject *points_list;

    PyObject *point = NULL;
    PyObject *coord = NULL;

    int i, j;

    double **centroids = NULL;
    double **points = NULL;
    double **new_centroids = NULL;

    int centroids_rows_allocated = 0;
    int points_rows_allocated = 0;

    PyObject *result = NULL;

    if (!PyArg_ParseTuple(args, "iidOiiO", &k, &iter, &eps, &centroids_list, &num_of_points, &dim, &points_list)) {
        return NULL;
    }

    /* Allocate centroids */
    centroids = (double **)malloc((size_t)k * sizeof(double *));
    if (!centroids) {
        PyErr_NoMemory();
        return NULL;
    }
    for (i = 0; i < k; i++) centroids[i] = NULL; /* so partial free is safe */

    for (i = 0; i < k; i++) {
        centroids[i] = (double *)malloc((size_t)dim * sizeof(double));
        if (!centroids[i]) {
            PyErr_NoMemory();
            goto cleanup;
        }
        centroids_rows_allocated++;

        point = PyList_GetItem(centroids_list, i); /* borrowed ref */
        for (j = 0; j < dim; j++) {
            coord = PyList_GetItem(point, j);       /* borrowed ref */
            centroids[i][j] = PyFloat_AsDouble(coord);
            /* Keeping your logic: no extra type/error checks beyond this */
        }
    }

    /* Allocate points */
    points = (double **)malloc((size_t)num_of_points * sizeof(double *));
    if (!points) {
        PyErr_NoMemory();
        goto cleanup;
    }
    for (i = 0; i < num_of_points; i++) points[i] = NULL;

    for (i = 0; i < num_of_points; i++) {
        points[i] = (double *)malloc((size_t)dim * sizeof(double));
        if (!points[i]) {
            PyErr_NoMemory();
            goto cleanup;
        }
        points_rows_allocated++;

        point = PyList_GetItem(points_list, i); /* borrowed ref */
        for (j = 0; j < dim; j++) {
            coord = PyList_GetItem(point, j);   /* borrowed ref */
            points[i][j] = PyFloat_AsDouble(coord);

        }
    }

    /* Call C k-means function (returns new centroids) */
    new_centroids = fit(k, iter, eps, centroids, num_of_points, dim, points);
    if (new_centroids == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "kmeans fit failed");
        goto cleanup;
    }

    /* Convert result to Python list */
    result = PyList_New(k);
    if (!result) {
        /* PyErr already set by CPython */
        goto cleanup;
    }

    for (i = 0; i < k; i++) {
        PyObject *centroid = PyList_New(dim);
        if (!centroid) {
            /* PyErr already set */
            goto cleanup;
        }

        for (j = 0; j < dim; j++) {
            PyObject *val = PyFloat_FromDouble(new_centroids[i][j]);
            if (!val) {
                Py_DECREF(centroid);
                goto cleanup;
            }
            /* steals ref to val */
            PyList_SetItem(centroid, j, val);
        }

        /* steals ref to centroid */
        PyList_SetItem(result, i, centroid);
    }

cleanup:
    /* Free C allocations (safe for partial alloc) */
    free_matrix(points, points_rows_allocated);
    free_matrix(centroids, centroids_rows_allocated);

    /* If we failed after creating result, decref it.
       If success, result is returned and we must NOT decref it. */
    if (result && PyErr_Occurred()) {
        Py_DECREF(result);
        result = NULL;
    }

    return result;
}

/* module's function table */
static PyMethodDef mykmeanssp_FunctionsTable[] = {
    {
        "fit",                 /* name exposed to Python */
        kmeansLib_fit,         /* C wrapper function */
        METH_VARARGS,          /* receives positional arguments */
        PyDoc_STR("calculates kmeans, receives k, iter, epsilon, list of centroids (calculated by kmean++), num of points, dim, points list")  /* documentation */
    },
    {NULL, NULL, 0, NULL}
};

/* module definition */
static struct PyModuleDef mykmeanssp_Module = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp",   /* module name */
    "kmeans Python wrapper for custom C extension library.",
    -1,
    mykmeanssp_FunctionsTable
};

PyMODINIT_FUNC PyInit_mykmeanssp(void) {
    return PyModule_Create(&mykmeanssp_Module);
}
