import matplotlib.pyplot as plt
import numpy as np
from sklearn.cluster import KMeans
import pandas as pd
from sklearn.datasets import load_iris


def main():
    # Load data
    iris = load_iris()
    iris_df = pd.DataFrame(data=iris.data, columns=iris.feature_names)
    iris_np_array = iris_df.to_numpy()

    # Setup variables
    k_range = range(1, 11)
    inertia_arr = []

    # Fit models
    for k in k_range:
        kmeans = KMeans(n_clusters=k, random_state=0, n_init="auto")
        kmeans.fit(iris_np_array)
        inertia_arr.append(kmeans.inertia_)
    
    # Plot results
    plt.figure(figsize=(8, 5))
    plt.plot(k_range, inertia_arr, marker='o')
    plt.annotate('Elbow Point (k=3)', 
             xy=(3, inertia_arr[2]), 
             xytext=(5, inertia_arr[2] + 100),
             arrowprops=dict(facecolor='black', shrink=0.05, width=2, headwidth=8),
             fontsize=12,
             fontweight='bold',
             color='red')
    plt.title('Elbow Method For Optimal k')
    plt.xlabel('Number of clusters (k)')
    plt.ylabel('Inertia')
    plt.grid(True)
    plt.show()






if __name__ =='__main__':
    main()