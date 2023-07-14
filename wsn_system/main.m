% Sequence: data collection and processing and storage for a certain
% period, once it ends, the anomlay detection algorithm will read from the
% correspondant table from the database and produce anomaly detection... 

% data_processing_storage();

% Iterate through all sensors 
for i = 1:4
    % Get the sensor table name
    sensorTableName = ['sender', num2str(i)];

    % Display the sensor name
    disp(sensorTableName);

    % Detect anomalies
    detect_anomalies(sensorTableName);
end


% Parallel: the ability to collect and store data to the db from the wsn through the
% data_processing_storage func, after some time make it run in the background, 
% while I analyse the data at the same time through the anomaly detection algo
% 
% % Start the data acquisition and processing in a new thread
% data_processing_future = parfeval(@data_processing_storage, 1);
% 
% % Pause for some time (e.g., 5 minutes) to allow data accumulation
% pause(2*60);
% 
% output = fetchOutputs(data_processing_future);
% disp(output);
% 
% % Start the anomaly detection in parallel for all senders
% p = gcp('nocreate'); % If no pool, do not create new one.
% if isempty(p)
%     parpool(4); % Start a parallel pool with 4 workers
% end
% parfor i = 1:4
%     detect_anomalies(['sender', num2str(i)], data_processing_future);
% end
% delete(gcp); % Close the parallel pool



% Define a parallel pool, this could be based on your system specifications
% If you do not specify 'local', the default profile is selected
% parpool(2); 
% 
% % Create a `Future` object for data processing and storage 
% future_data_proc = parfeval(@data_processing_storage, 0); 
% 
% % Wait for data processing to finish
% wait(future_data_proc); 
% 
% % Wait 5 minutes to let data accumulate
% pause(300); % Pause for 5 minutes
% 
% future_anomaly_detection = parfeval(@detect_anomalies, 0, 'sender1');
% 
% delete(gcp('nocreate'))

% % Iteratively create `Future` objects for anomaly detection
% future_anomaly_detection = cell(1, 4);
% for i = 1:4
%     % Make sure we wait until data_processing_storage is completed before launching the detect_anomalies function
%     wait(future_data_proc);
% 
%     % Detect anomalies, no need to wait for completion
%     future_anomaly_detection{i} = parfeval(@detect_anomalies, 0, ['sender', num2str(i)]);
% end
% 
% % We may want to wait until all parallel tasks have finished. 
% % Wait for all anomaly detection tasks to finish
% for i = 1:4
%     wait(future_anomaly_detection{i});
% end

% Close the parallel pool
%delete(gcp('nocreate'))


