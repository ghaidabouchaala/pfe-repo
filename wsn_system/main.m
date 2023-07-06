% Sequence: data collection and processing and storage for a certain
% period, once it ends, the anomlay detection algorithm will read from the
% correspondant table from the database and produce anomaly detection... 

data_processing_storage();

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
