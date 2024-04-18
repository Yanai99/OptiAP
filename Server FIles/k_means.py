from sklearn.cluster import KMeans
import matplotlib.pyplot as plt
import numpy as np


# For connecting (x,y) coordinates with rssi before sending it to k-means
def prepare_single_measure_data(coordinates, measure_data):
    x, y = coordinates
    return [x, y] + [measure[2] for measure in measure_data]


# for extracting the (x,y) from each k-means cluster
def extract_first_two_elements(list_of_lists):
    return [[int(round(sublist[0])), int(round(sublist[1]))] for sublist in list_of_lists]


def k_means_algorithm(data_for_k_means):
    data = np.array(data_for_k_means)

    # Define range of cluster numbers
    k_values = range(1, len(data_for_k_means))

    # Calculate WCSS for each cluster number
    wcss = []
    for k in k_values:
        kmeans = KMeans(n_clusters=k)
        kmeans.fit(data)
        wcss.append(kmeans.inertia_)

    # Plot WCSS against number of clusters
    plt.plot(k_values, wcss, marker='o')
    plt.xlabel('Number of clusters')
    plt.ylabel('Within-cluster sum of squares (WCSS)')
    plt.title('Elbow Method')
    plt.show()

    # Take user input for the number of APs
    chosen_num_of_aps = int(input("Enter the number of APs: "))

    # Instantiate KMeans object
    kmeans = KMeans(n_clusters=chosen_num_of_aps)

    # Fit the model to the data
    kmeans.fit(data)

    # Retrieve cluster centers
    cluster_centers = kmeans.cluster_centers_
    optimal_coordinates = extract_first_two_elements(cluster_centers)
    print("Cluster centers:", optimal_coordinates)

    # Predict cluster assignments
    labels = kmeans.predict(data)
    print("Cluster labels:", labels)

    # remaining tasks:
    '''
     1. making all the measurement be the same dimention using padding
     2. how to define the k?
    '''
