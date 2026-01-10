import pandas as pd
import numpy as np

def calculate_distance(p,q): #p and q are arrays representing points
    sum_of_deltas = 0
    for i in range(len(p)):
        sum_of_deltas += (p[i] - q[i]) ** 2
    return sum_of_deltas**0.5

def kmeansplus(k,points_arr):
    
