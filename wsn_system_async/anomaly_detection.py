#!/usr/bin/env python
# coding: utf-8

# In[ ]:


import mysql.connector
from mysql.connector import errorcode
import numpy as np
import time
from datetime import datetime
import logging

# Set up logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

def detect_anomalies(sensorTableName, stop_event):
    logging.info("Starting anomaly detection for table %s", sensorTableName)

    try:
        cnx = mysql.connector.connect(user='root',
                                      password='',
                                      host='localhost',
                                      database='wsn_data')
        cursor = cnx.cursor()

        # Create a new table for the results of anomaly detection
        resultTableName = sensorTableName + '_results'
        createTableSQL = """CREATE TABLE IF NOT EXISTS {} 
                            (SensorID INT, OriginalTimestamp DATETIME, ProcessedTimestamp DATETIME, 
                            SensorValue DOUBLE, Measurement DOUBLE, EstimatedValue DOUBLE, 
                            Threshold DOUBLE, AnomalyStatus BOOLEAN)""".format(resultTableName)
        cursor.execute(createTableSQL)

        processed_up_to = None
        

        while not stop_event.is_set():
            logging.info("Checking for new data in table %s", sensorTableName)
            # Check for new data in the database
            query_value = "SELECT ID, Value, Timestamp FROM {} WHERE Timestamp BETWEEN CURRENT_TIMESTAMP - INTERVAL 2 MINUTE AND CURRENT_TIMESTAMP ORDER BY Timestamp".format(sensorTableName)
            #query_value = "SELECT ID, Value, Timestamp FROM {}".format(sensorTableName)

            cursor.execute(query_value)

            values = cursor.fetchall()
            if values:                
                var = [value[1] for value in values] 
                sensor_id = values[0][0]
                timestamp = [value[2] for value in values] 
                print("values from: ",sensorTableName," ",var )
                # Time step
                dt = 0.001
                
                # Initial state
                x = np.array([var[0], 1])  # [position; velocity]

                # State transition matrix
                A = np.array([[1, dt], [0, 1]])

                # Control input matrix
                B = np.array([[0], [0]])
                
                # Measurement matrix
                H = np.array([[1, 0]])

                # Process noise covariance
                Q = np.array([[1, 0], [0, 0.1]])

                # Measurement noise covariance
                R = 1

                # Initial state covariance
                P = np.eye(2)
                    
                # Generate true position and noisy measurements
                num_steps = len(values)
                true_position = np.zeros((num_steps, 1))
                measurements = np.zeros((num_steps, 1))

                # Initialize the array to store the estimates
                x_estimates = np.zeros((2, num_steps))


                for k in range(num_steps):
                    # Initialize anomaly variable
                    anomaly = False
                    
                    # Update true position
                    x[0] = x[0] + x[1] * dt

                    # Generate measurement with noise
                    measurement_noise = 0 
                    z = var[k]

                    # Save true position and measurement
                    true_position[k] = x[0]
                    measurements[k] = var[k]

                    # Save the estimate
                    x_estimates[:, k] = x.flatten()

                    # Kalman filter prediction step
                    x_pred = A.dot(x)
                    P_pred = A.dot(P).dot(A.T) + Q

                    # Kalman filter update step
                    y = z - H.dot(x_pred)
                    S = H.dot(P_pred).dot(H.T) + R
                    K = P_pred.dot(H.T) / S
                    
                    threshold = 3 * P[0, 0]

                        
                        
                    # update KF
                    if abs(x_pred[0] - z) <= threshold:
                        x = x_pred + K.dot(y)
                        P = (np.eye(2) - K.dot(H)).dot(P_pred)
                            
                            
                    anomaly = int(abs(x_pred[0] - z) > threshold)
                        
                    if anomaly:
                        logging.info("Anomaly detected in sender %s at time %s", str(sensor_id), timestamp)
                            
                    # Insert the results into the new table
                    anomaly_status = 1 if anomaly else 0
                    insertSQL = """INSERT INTO {} (SensorID, OriginalTimestamp, ProcessedTimestamp, SensorValue, Measurement, EstimatedValue, Threshold, AnomalyStatus)VALUES ({}, '{}', '{}', {}, {}, {}, {}, {})""".format(resultTableName, sensor_id, timestamp[k].strftime('%Y-%m-%d %H:%M:%S'),datetime.now().strftime('%Y-%m-%d %H:%M:%S'), var[k], z, x_pred[0], threshold, anomaly_status)
                    cursor.execute(insertSQL)
                    cnx.commit()
                    

            # Update `processed_up_to`
            if values:
                processed_up_to = max(values, key=lambda x: x[2])[2] if values else processed_up_to
            else:
                processed_up_to = None

            # Sleep for 2 minutes before checking again
            time.sleep(120)

            logging.info("Finished processing new data in table %s", sensorTableName)
            

    except mysql.connector.Error as err:
        if err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
            print("Something is wrong with your user name or password")
        elif err.errno == errorcode.ER_BAD_DB_ERROR:
            print("Database does not exist")
        else:
            print(err)
            
    else:
        cnx.close()

