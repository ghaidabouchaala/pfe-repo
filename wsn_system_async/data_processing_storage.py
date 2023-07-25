#!/usr/bin/env python
# coding: utf-8

# In[ ]:


import mysql.connector
import serial
import time
from datetime import datetime

def data_processing_storage(stop_event=None,run_duration=None):
    
    start_time = time.time()
    
    
    try:
        # Create a serial port object
        ser = serial.Serial('/dev/cu.usbmodem11701', 9600)

        # number of senders
        numSenders = 4

        # Set up MySQL connection
        mydb = mysql.connector.connect(
          host="localhost",
          user="root",
          password="",
          database="wsn_data"
        )
        mycursor = mydb.cursor()

        # Create tables if they don't exist
        for i in range(numSenders):
            senderTableName = 'sender' + str(i + 1)
            createTableSQL = "CREATE TABLE IF NOT EXISTS {} (Address VARCHAR(255), ID INT, Type VARCHAR(255), Value DOUBLE, Timestamp DATETIME, InsertionTimestamp DATETIME(6))".format(senderTableName)
            mycursor.execute(createTableSQL)

        while (stop_event is None or not stop_event.is_set()) and (run_duration is None or time.time() - start_time < run_duration):
            # Check if run_duration has passed, if specified
            if run_duration is not None and time.time() - start_time > run_duration:
                break

            # Read data from the serial port
            line = ser.readline().decode('utf-8').strip()

            # Display the received line for debugging
            print("Received line:", line)

            # Check if the line starts with '[' and ends with ']'
            if line.startswith('[') and line.endswith(']'):
                # Remove the brackets
                line = line[1:-1]

                # Split the line into parts
                parts = line.split(':')

                # Check the number of parts
                if len(parts) == 4:
                    # Extract the sender ID from the line
                    senderID = int(parts[1])

                    # Prepare the insert statement for the current line
                    senderTableName = 'sender' + str(senderID)
                    insertSQL = "INSERT INTO {} (Address, ID, Type, Value, Timestamp, InsertionTimestamp) VALUES ('{}', {}, '{}', {}, '{}', '{}')".format(senderTableName, parts[0], int(parts[1]), parts[2], float(parts[3]), datetime.now().strftime('%Y-%m-%d %H:%M:%S'), datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f'))

                    # Execute the insert statement
                    mycursor.execute(insertSQL)
                    mydb.commit()

                    # Display the received data
                    print(line)

        return 'Success!'

    except Exception as e:
        print(e)
        return str(e)

    finally:
        # Close the database connection
        mycursor.close()
        mydb.close()
        print('Database connection closed.')

        # Close the serial port connection
        ser.close()
        print('Serial port connection closed.')

