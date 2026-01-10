import pandas as pd
import numpy as np
import sys

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

def kmeans(k, points_arr):
    x = np.random.choice(points_arr)
    centers=[x]
    for i in range (k-1):
        distance_arr = [calculate_min_distance(centers, p) for p in points_arr]
        sum = sum(distance_arr)
        prob_arr = [distance_arr[n]/sum for n in range(len(points_arr))]
        centers.append(np.random.choice(points_arr, p=prob_arr))
    return centers
    
        


        

        






def main():
    np.random.seed(1234)
    #reading user CMD arguments
    if len(sys.argv) == 1 or len(sys.argv) > 6:
        print("An Error Has Occurred")
        raise SystemExit(1)

    k = sys.argv[1]
    if not k.isdigit():
        print("Incorrect number of clusters!")
        raise SystemExit(1)

    k = int(k)

    if len(sys.argv)==5:
        iter = sys.argv[2]
        if not iter.isdigit():
            print("Incorrect maximum iteration!")
            raise SystemExit(1)
        iter = int(iter)
        if iter < 2 or iter > 799:
            print("Incorrect maximum iteration!")
            raise SystemExit(1)
    else:
        iter = 400
    eps = sys.argv[3]


if __name__=="__main__":
    main()