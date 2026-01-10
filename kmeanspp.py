import pandas as pd
import numpy as np
import sys
import mykmeanssp

def calculate_distance(p,q): #p and q are arrays representing points
    sum_of_deltas = 0
    for i in range(len(p)):
        sum_of_deltas += (p[i] - q[i]) ** 2
    return sum_of_deltas**0.5

def calculate_min_distance(centers, p):
    min = calculate_distance(centers[0], p)
    for i in range(1,len(centers)):
        temp = calculate_distance(centers[i], p)
        if temp < min:
            min = temp
    return min



def kmeansplus_Init(k, points_arr):

    pts = np.asarray(points_arr, dtype=float)
    n = len(pts)

    idx = np.random.randint(n)
    centers = [pts[idx]]

    for _ in range(k - 1):
        dist = np.array([calculate_min_distance(centers, p) for p in pts], dtype=float)
        total = dist.sum()

        # If all distances are 0 (e.g., identical points), fall back to uniform choice
        if total == 0.0:
            next_idx = np.random.randint(n)
        else:
            prob_arr = dist / total
            next_idx = np.random.choice(n, p=prob_arr)

        centers.append(pts[next_idx])

    # Convert each centroid to a plain Python list
    return [c.tolist() for c in centers]




def is_float(value):
    try:
        float(value)
        return True
    except ValueError:
        return False


def parse_points(file_A, file_B):
    points_A = pd.read_csv(file_A, header=None)
    points_B = pd.read_csv(file_B, header=None)

    merged_points = pd.merge(points_A, points_B, how='inner' , on=0)
    merged_points = merged_points.sort_values(merged_points.columns[0])

    num_of_points = merged_points.shape[0]
    dim = merged_points.shape[1]
    points_arr = []
    for i in range(num_of_points):
        temp_point = []
        for j in range(1 , dim): #start from 1 since key is included
            temp_data = merged_points.iloc[i,j]
            if not is_float(temp_data):
                print("An Error Has Occurred")
                raise SystemExit(1)
            temp_point.append(float(temp_data))
        points_arr.append(temp_point)

    return points_arr , merged_points


def print_centroids(centroids):
    for point in centroids:
        s = ""
        for cord in point:
            s = s + f"{cord:.4f}" + ","
        s = s[0:-1]
        print(s)

def same_point(p,q):
    for i in range(len(p)):
        if(p[i] != q[i]):
            return False
    return True

def print_indices(centroids, points_arr):
    s = ""
    for point in centroids:
        for i in range(len(points_arr)):
            if same_point(point, points_arr[i]):
                s = s + i + ","
    s = s[0:-1]
    print(s)


def main():
    #reading user CMD arguments
    np.random.seed(1234)
    provided_iter = 1 #in the case when iter is provided, the fields change index by 1
    if len(sys.argv) == 1 or len(sys.argv) > 6:
        print("An Error Has Occurred")
        raise SystemExit(1)

    k = sys.argv[1]
    if not k.isdigit():
        print("Incorrect number of clusters!")
        raise SystemExit(1)

    k = int(k)

    if len(sys.argv) == 6:
        iter = sys.argv[2]
        if not iter.isdigit():
            print("Incorrect maximum iteration!")
            raise SystemExit(1)
        iter = int(iter)
        if iter < 2 or iter > 799:
            print("Incorrect maximum iteration!")
            raise SystemExit(1)
    else:
        iter = 300
        provided_iter = 0


    eps = sys.argv[2 + provided_iter]
    if not is_float(eps):
        print("Incorrect epsilon!")
        raise SystemExit(1)

    eps = float(eps)
    if eps < 0:
        print("Incorrect epsilon!")
        raise SystemExit(1)

    file_name_a = sys.argv[3 + provided_iter] #we assume these are valid
    file_name_b = sys.argv[4 + provided_iter]

    points_arr , points_dataframe = parse_points(file_name_a, file_name_b)

    N = len(points_arr)

    if N <= 1:
        print("An Error Has Occurred")
        raise SystemExit(1)

    if k < 2 or k > N - 1: #final check on k, depends on the length of the points array
        print("Incorrect number of clusters!")
        raise SystemExit(1)
    
    centroids = kmeansplus_Init(k, points_arr)

    new_centroids = mykmeanssp.fit(k, iter, eps, centroids, len (points_arr), len(points_arr[0]), points_arr)

    print_centroids(new_centroids)


if __name__=="__main__":
    main()