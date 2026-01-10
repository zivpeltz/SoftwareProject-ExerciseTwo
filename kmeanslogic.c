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


static double **cluster_handle(int k, int iter, double eps, double **centroids, int num_of_points, int dim, double **points) {
    int i, j, l, curr, convergence;
    int *counter_arr = NULL, *counter_arr_copy = NULL;
    double ***clusters = NULL;
    int ok = 1; /* flag in case of failure*/

    clusters = (double ***)calloc(k, sizeof(double **));
    if (!clusters) { ok = 0; goto cleanup; }

    counter_arr_copy = (int *)calloc(k, sizeof(int));
    if (!counter_arr_copy) { ok = 0; goto cleanup; }

    for (i = 0; i < iter; i++) {
        counter_arr = count_cluster_points(k, centroids, points, dim, num_of_points);
        if (!counter_arr) { ok = 0; goto cleanup; }

        for (j = 0; j < k; j++) counter_arr_copy[j] = counter_arr[j];

        /* allocate cluster pointer arrays */
        for (j = 0; j < k; j++) {
            clusters[j] = (counter_arr[j] == 0) ? NULL
                        : (double **)malloc(sizeof(double *) * counter_arr[j]);
            if (counter_arr[j] != 0 && !clusters[j]) { ok = 0; goto cleanup; }
        }

        /* fill clusters */
        for (j = 0; j < num_of_points; j++) {
            curr = find_closest_cluster(centroids, points[j], k, dim);
            if (counter_arr[curr] <= 0) { ok = 0; goto cleanup; }  // mismatch = bug
            clusters[curr][counter_arr[curr] - 1] = points[j];
            counter_arr[curr]--;

        }

        convergence = 1;

        /* update centroids in place */
        for (j = 0; j < k; j++) {
            if (counter_arr_copy[j] == 0) {
                /* empty cluster rule: centroid becomes points[0] (by VALUE) */
                if (calculate_distance(centroids[j], points[0], dim) >= eps) convergence = 0;
                for (l = 0; l < dim; l++) {
                    /*if (fabs(centroids[j][l] - points[0][l]) >= eps) convergence = 0;*/
                    centroids[j][l] = points[0][l];
                }
            } else {
                double *tmp = update_centroid(counter_arr_copy[j], dim, clusters[j]);
                if (!tmp) { ok = 0; goto cleanup; }

                if (calculate_distance(centroids[j], tmp, dim) >= eps) convergence = 0;
                for (l = 0; l < dim; l++) {
                    /*if (fabs(centroids[j][l] - tmp[l]) >= eps) convergence = 0;*/
                    centroids[j][l] = tmp[l];
                }
                free(tmp);
            }
        }

        /* free per-iteration cluster arrays */
        for (l = 0; l < k; l++) {
            free(clusters[l]);
            clusters[l] = NULL;
        }

        free(counter_arr);
        counter_arr = NULL;

        if (convergence) break;
    }

cleanup:
    free(counter_arr); /* ok if NULL */

    if (clusters) {
        for (l = 0; l < k; l++) free(clusters[l]);
        free(clusters);
    }

    free(counter_arr_copy);

    return ok ? centroids : NULL;
}

double **fit(int k, int iter, double eps, double **centroids, int num_of_points, int dim, double **points){
    return cluster_handle(k, iter, eps, centroids, num_of_points, dim, points);
}

int main(int argc, char *argv[]) {
   return 1;
}
