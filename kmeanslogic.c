#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int global_num_of_points; /* global variables for total number of points */
int global_dim;           /* global variables for total dimension of points */



static int find_string_length(char *s) {
    int c = 0;
    while (*s++) c++;
    return c;
}



static void print_centroids(int k, int dim, double **centroids) {
    int i, j;
    for (i = 0; i < k; i++) {
        for (j = 0; j < dim; j++) {
            if (j == dim - 1) printf("%.4f", centroids[i][j]);
            else printf("%.4f,", centroids[i][j]);
        }
        printf("\n");
    }
}

static double *update_centroid(int length_of_cluster, int dim, double **cluster) {
    double *updated_centroid = (double *)malloc(sizeof(double) * dim);
    int i, j;
    double sum;

    if (updated_centroid == NULL) return NULL;

    for (i = 0; i < dim; ++i) {
        sum = 0.0;
        for (j = 0; j < length_of_cluster; ++j) {
            sum += cluster[j][i];
        }
        updated_centroid[i] = sum / length_of_cluster;
    }
    return updated_centroid;
}

static double calculate_distance(double p[], double q[], int n) {
    double sum = 0.0;
    int i;
    for (i = 0; i < n; i++) {
        sum += (p[i] - q[i]) * (p[i] - q[i]);
    }
    return sqrt(sum);
}


static int find_closest_cluster(double **centroids, double *point, int cluster_num, int dim) {
    int i, closest_cluster_index = 0;
    double distance, temp_distance;
    distance = calculate_distance(point, centroids[0], dim);
    for (i = 1; i < cluster_num; i++) {
        temp_distance = calculate_distance(centroids[i], point, dim);
        if (temp_distance < distance) {
            distance = temp_distance;
            closest_cluster_index = i;
        }
    }
    return closest_cluster_index;
}

static int *count_cluster_points(int k, double **centroids, double **points, int dim, int num_of_points) {
    int *counter_arr = (int *)calloc(k, sizeof(int));
    int i, curr = 0;
    if (!counter_arr) return NULL;

    for (i = 0; i < num_of_points; i++) {
        curr = find_closest_cluster(centroids, points[i], k, dim);
        counter_arr[curr] += 1;
    }
    return counter_arr;
}

/*
 * Returns 0 on success, 1 on allocation failure (caller prints "An Error Has Occurred" and exits 1).
 */
static int cluster_handle(int k, int iter, int num_of_points, int dim, double **points) {
    int i, j, l, curr, convergence;
    int *counter_arr = NULL, *counter_arr_copy = NULL;
    double **centroids = NULL, **updated_centroids = NULL, ***clusters = NULL;
    double *old = NULL;
    int *owned = NULL; /* owned[j]=1 if centroids[j] must be freed, 0 if it points into points[] */

    int status = 1; /* assume failure until we succeed */

    clusters = (double ***)calloc(k, sizeof(double **));
    if (!clusters) goto cleanup;

    centroids = (double **)calloc(k, sizeof(double *));
    if (!centroids) goto cleanup;

    updated_centroids = (double **)calloc(k, sizeof(double *));
    if (!updated_centroids) goto cleanup;

    counter_arr_copy = (int *)calloc(k, sizeof(int));
    if (!counter_arr_copy) goto cleanup;

    owned = (int *)calloc(k, sizeof(int));
    if (!owned) goto cleanup;

    for (i = 0; i < k; i++) {
        centroids[i] = (double *)malloc(dim * sizeof(double));
        if (!centroids[i]) goto cleanup;
        owned[i] = 1;
        for (l = 0; l < dim; l++) centroids[i][l] = points[i][l];
    }

    for (i = 0; i < iter; i++) {
        counter_arr = count_cluster_points(k, centroids, points, dim, num_of_points);
        if (!counter_arr) goto cleanup;

        for (j = 0; j < k; j++) counter_arr_copy[j] = counter_arr[j];

        /* allocate clusters arrays for this iteration */
        for (j = 0; j < k; j++) {
            clusters[j] = (counter_arr[j] == 0) ? NULL : (double **)malloc(sizeof(double *) * counter_arr[j]);
            if (counter_arr[j] != 0 && !clusters[j]) goto cleanup;
        }

        for (j = 0; j < num_of_points; j++) {
            curr = find_closest_cluster(centroids, points[j], k, dim);
            clusters[curr][counter_arr[curr] - 1] = points[j];
            counter_arr[curr]--;
        }

        convergence = 1;

        for (j = 0; j < k; j++) {
            if (counter_arr_copy[j] == 0) {
                updated_centroids[j] = points[0]; /* unchanged behavior */
            } else {
                updated_centroids[j] = update_centroid(counter_arr_copy[j], dim, clusters[j]);
                if (!updated_centroids[j]) goto cleanup;

                for (l = 0; l < dim; l++) {
                    if (fabs(centroids[j][l] - updated_centroids[j][l]) >= 0.001) {
                        convergence = 0;
                    }
                }
            }

            /* swap + free old only if we own it */
            old = centroids[j];
            {
                int old_owned = owned[j];

                centroids[j] = updated_centroids[j];
                owned[j] = (centroids[j] == points[0]) ? 0 : 1;

                if (old_owned && old != NULL) free(old);
            }
        }

        for (l = 0; l < k; l++) {
            free(clusters[l]);
            clusters[l] = NULL;
        }

        free(counter_arr);
        counter_arr = NULL;

        if (convergence == 1) break;
    }

    print_centroids(k, dim, centroids);
    status = 0;

cleanup:
    if (counter_arr) free(counter_arr);

    if (clusters) {
        for (l = 0; l < k; l++) {
            free(clusters[l]);
        }
        free(clusters);
    }

    if (centroids) {
        if (owned) {
            for (j = 0; j < k; j++) {
                if (owned[j] && centroids[j]) free(centroids[j]);
            }
        } else {
            /* fallback: free only non-NULL pointers (best effort) */
            for (j = 0; j < k; j++) {
                if (centroids[j]) free(centroids[j]);
            }
        }
        free(centroids);
    }

    if (updated_centroids) free(updated_centroids);
    if (counter_arr_copy) free(counter_arr_copy);
    if (owned) free(owned);

    return status;
}

double **fit(int k, int iter, double eps, double **centroids, int num_of_points, int dim, double **points){

}

int main(int argc, char *argv[]) {
   return 1;
}
