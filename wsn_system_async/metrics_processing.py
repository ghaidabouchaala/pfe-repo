#!/usr/bin/env python
# coding: utf-8

# In[ ]:


import mysql.connector
from mysql.connector import errorcode
import time
import logging

# Set up logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

def metrics_processing(sensorTableName, stop_event):
    logging.info("Starting metrics display for table %s", sensorTableName)

    
    try:
        # Establish connection to the database
        cnx = mysql.connector.connect(user='root',
                                      password='',
                                      host='localhost',
                                      database='wsn_data')
        cursor = cnx.cursor()
        
        # Create a new table for the metrics of each sender for anomaly detection
        metricTableName = sensorTableName + '_metrics'
        createTableSQL = """CREATE TABLE IF NOT EXISTS {} 
                            (SensorID INT, AnomalyCount DOUBLE, AnomalyDetectionTime DOUBLE)""".format(metricTableName)
        cursor.execute(createTableSQL)
        
        processed_up_to = None

        # Retrieve data from sender{i}_results tables
        while not stop_event.is_set():
            logging.info("Checking for new data in table %s", sensorTableName)
            # Check for new data in the database
            query_value = "SELECT SensorID, AnomalyStatus, OriginalTimestamp, ProcessedTimestamp FROM {}_results WHERE ProcessedTimestamp BETWEEN CURRENT_TIMESTAMP - INTERVAL 3 MINUTE AND CURRENT_TIMESTAMP ORDER BY ProcessedTimestamp".format(sensorTableName)
            #query_value = "SELECT SensorID, AnomalyStatus, OriginalTimestamp, ProcessedTimestamp FROM {}_results".format(sensorTableName)

            cursor.execute(query_value)
            results = cursor.fetchall()

            if results:
                # Perform metrics calculation
                anomalyCount = 0
                anomalyDetectionTime = 0

                first_result = results[0]
                sensorID = first_result[0]
                
                for result in results:
                    anomalyStatus = result[1]
                    originalTimestamp = result[2]
                    processedTimestamp = result[3]

                    if anomalyStatus == 1:
                        anomalyCount += 1
                        anomalyDetectionTime += (processedTimestamp - originalTimestamp).total_seconds()

                if anomalyCount > 0:
                    anomalyDetectionTime /= anomalyCount

                logging.info("Anomaly results in: %s %s", sensorTableName, anomalyCount)
                logging.info("AnomalyDetectionTime in: %s %s", sensorTableName, anomalyDetectionTime)


                 # Insert the metrics results
                insertMetricsSQL = "INSERT INTO {} (SensorID, AnomalyCount, AnomalyDetectionTime) VALUES (%s, %s, %s)".format(metricTableName)
                cursor.execute(insertMetricsSQL, (sensorID, anomalyCount, anomalyDetectionTime))
                cnx.commit()

            # Update `processed_up_to`
            if results:
                processed_up_to = max(results, key=lambda x: x[3])[3] if results else processed_up_to

            # Sleep for 3 minutes before checking again
            time.sleep(180)

        # Close the database connection
        cnx.close()
        
        
        
    except mysql.connector.Error as err:
        if err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
            print("Something is wrong with your user name or password")
        elif err.errno == errorcode.ER_BAD_DB_ERROR:
            print("Database does not exist")
        else:
            print(err)

