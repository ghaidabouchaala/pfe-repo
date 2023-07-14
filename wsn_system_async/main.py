#!/usr/bin/env python
# coding: utf-8

# In[ ]:


import threading
from data_processing_storage import data_processing_storage
from anomaly_detection import detect_anomalies
import time

def main():

    # Start data acquisition and processing in a separate thread
    data_thread = threading.Thread(target=data_processing_storage, args=(900,))  # Run for 1 hour
    data_thread.start()

    # Wait for some time to let data acquisition to start
    time.sleep(120)  # Wait for 3 minute

    # Start anomaly detection threads for each sensor node
    anomaly_threads = []
    for i in range(1, 5):  # Assuming there are 4 sensor nodes
        sensorTableName = 'sender' + str(i)
        anomaly_thread = threading.Thread(target=detect_anomalies, args=(sensorTableName,))
        anomaly_thread.start()
        anomaly_threads.append(anomaly_thread)

    # Wait for all threads to finish
    data_thread.join()
    for anomaly_thread in anomaly_threads:
        anomaly_thread.join()

    print("All tasks are done.")

if __name__ == "__main__":
    main()

