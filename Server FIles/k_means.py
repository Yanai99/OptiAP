from sklearn.cluster import KMeans
import matplotlib.pyplot as plt
import numpy as np
from kneed import KneeLocator


# For connecting (x,y) coordinates with rssi before sending it to k-means
def prepare_single_measure_data(coordinates, measure_data, coordinates_dic):
    x, y = coordinates

    # Extract beacon IDs from the measurement data
    current_beacons = {data[0] for data in measure_data}

    # Ensure all beacons are included, add missing ones with RSSI -100
    for beacon_id in coordinates_dic:
        if beacon_id not in current_beacons:
            # Insert missing beacon data with a default RSSI of -100
            measure_data.append([beacon_id, 0, -100])  # Distance set to 0 as placeholder

    # Sort measure_data by beacon ID
    measure_data_sorted = sorted(measure_data, key=lambda b: b[0])

    # Return the prepared data including coordinates and sorted RSSI values
    return [x, y] + [item[2] for item in measure_data_sorted]


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

    # Identify the elbow point
    kn = KneeLocator(list(k_values), wcss, curve='convex', direction='decreasing')
    chosen_num_of_aps = kn.elbow

    if chosen_num_of_aps is None:
        chosen_num_of_aps = 1
    print("Optimal number of clusters:", chosen_num_of_aps)

    # Plot WCSS against number of clusters with the elbow point
    plt.plot(k_values, wcss, marker='o')
    plt.xlabel('Number of clusters')
    plt.ylabel('Within-cluster sum of squares (WCSS)')
    plt.title('Elbow Method')
    plt.scatter(chosen_num_of_aps, wcss[chosen_num_of_aps-1], color='red') # highlight the elbow point

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
     1. making all the measurement be the same dimension using padding
     2. how to define the k? - kneed library
    '''
