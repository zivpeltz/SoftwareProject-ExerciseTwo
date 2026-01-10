#ifndef KMEANSLOGIC_H
#define KMEANSLOGIC_H

#ifdef __cplusplus
extern "C" {
#endif

    /* Mutates centroids in-place and returns the same pointer (or NULL on failure). */
    double **fit(int k, int iter, double eps, double **centroids, int num_of_points, int dim, double **points);


#ifdef __cplusplus
}
#endif

#endif /* KMEANSLOGIC_H */
