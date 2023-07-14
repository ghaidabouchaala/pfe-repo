#!/usr/bin/env python
# coding: utf-8

# In[1]:


import mysql.connector
from mysql.connector import errorcode
import numpy as np
import time
from datetime import datetime
import logging

# Set up logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

def detect_anomalies(sensorTableName):
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
                            SensorValue DOUBLE, EstimatedValue DOUBLE, Threshold DOUBLE, 
                            AnomalyStatus BOOLEAN)""".format(resultTableName)
        cursor.execute(createTableSQL)

        processed_up_to = None
        
        while True:
            logging.info("Checking for new data in table %s", sensorTableName)
            # Check for new data in the database
            query_value = "SELECT ID, Value, Timestamp FROM {} WHERE Timestamp BETWEEN CURRENT_TIMESTAMP - INTERVAL 2 MINUTE AND CURRENT_TIMESTAMP ORDER BY Timestamp".format(sensorTableName)
            logging.info("SQL query: %s", query_value)
            cursor.execute(query_value)
            values = cursor.fetchall()
            logging.info("Fetched values: %s", values)


            
            if values:
                for value in values:
                    sensor_id, var, timestamp = value
                    query_type = "SELECT Type FROM {} LIMIT 1".format(sensorTableName)
                    cursor.execute(query_type)
                    type = cursor.fetchone()
                    sensorType = type[0]
                    dt = 0.001

                    x = np.array([var, 1])
                    A = np.array([[1, dt], [0, 1]])
                    H = np.array([1, 0])
                    Q = np.array([[1, 0], [0, 0.1]])
                    R = 1
                    P = np.eye(2)

                    x[0] = x[0] + x[1] * dt
                    z = var
                    x_pred = A.dot(x)
                    P_pred = A.dot(P).dot(A.T) + Q
                    y = z - H.dot(x_pred)
                    S = H.dot(P_pred).dot(H.T) + R
                    K = P_pred.dot(H.T) / S
                    threshold = 3 * P[0, 0]

                    # check whether anomaly or not
                    anomaly = abs(x_pred[0] - z) > threshold

                    if not anomaly:
                        x = x_pred + K * y
                        P = (np.eye(2) - np.outer(K, H)).dot(P_pred)

                    # Insert the results into the new table
                    insertSQL = """INSERT INTO {} (SensorID, OriginalTimestamp, ProcessedTimestamp, 
                                                    SensorValue, EstimatedValue, Threshold, AnomalyStatus)
                                   VALUES ({}, '{}', '{}', {}, {}, {}, {})""".format(
                        resultTableName, sensor_id, timestamp.strftime('%Y-%m-%d %H:%M:%S'), 
                        datetime.now().strftime('%Y-%m-%d %H:%M:%S'), z, x_pred[0], threshold, anomaly)
                    cursor.execute(insertSQL)
                    cnx.commit()

            # Update `processed_up_to`
            if values:
                processed_up_to = max(values, key=lambda x: x[2])[2] if values else processed_up_to
            else:
                processed_up_to = None

            # Sleep for 3 minutes before checking again
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

