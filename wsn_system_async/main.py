#!/usr/bin/env python
# coding: utf-8

# In[ ]:


import threading
from data_processing_storage import data_processing_storage
from anomaly_detection import detect_anomalies
from metrics_processing import metrics_processing
import time

def main():
    # Create an event to signal other threads to stop
    stop_event = threading.Event()

    # Start data acquisition and processing in a separate thread
    data_thread = threading.Thread(target=data_processing_storage, args=(stop_event, 900))  # Run for 15 minutes
    data_thread.start()

    # Wait for some time to let data acquisition to start
    time.sleep(120)  # Wait for 2 minutes

    # Start anomaly detection threads for each sensor node
    anomaly_threads = []
    for i in range(1, 5):  # Assuming there are 4 sensor nodes
        sensorTableName = 'sender' + str(i)
        anomaly_thread = threading.Thread(target=detect_anomalies, args=(sensorTableName, stop_event))
        anomaly_thread.start()
        anomaly_threads.append(anomaly_thread)

    # Wait for some time to let anomaly detection start
    time.sleep(10)  # Wait for 1 minute

    # Call metrics_processing for each sensor node
    metrics_threads = []
    for i in range(1, 5):  # Assuming there are 4 sensor nodes
        sensorTableName = 'sender' + str(i)
        metrics_thread = threading.Thread(target=metrics_processing, args=(sensorTableName, stop_event))
        metrics_thread.start()
        metrics_threads.append(metrics_thread)

    # Wait for the data thread to finish
    data_thread.join()
    
    # Set the stop event to signal anomaly detection and metrics processing threads to stop after 3 minutes
    stop_event.set()
    
    # Add a grace period for threads to complete their tasks
    grace_period = 300  # Wait for 1 minute
    time.sleep(grace_period)

    # Stop the anomaly detection threads after 3 minutes
    for anomaly_thread in anomaly_threads:
        anomaly_thread.join()

    # Wait for the metrics processing threads to finish
    for metrics_thread in metrics_threads:
        metrics_thread.join()
        
    print("All tasks are done.")

    # Calculate General the metrics

if __name__ == "__main__":
    main()

