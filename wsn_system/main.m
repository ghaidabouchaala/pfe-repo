clear all;

% Start the data acquisition and processing in a new thread
data_processing_future = parfeval(@data_processing_storage, 1);

% Pause for some time (e.g., 5 minutes) to allow data accumulation
pause(2*60);

output = fetchOutputs(data_processing_future);
disp(output);

% Start the anomaly detection in parallel for all senders
p = gcp('nocreate'); % If no pool, do not create new one.
if isempty(p)
    parpool(4); % Start a parallel pool with 4 workers
end
parfor i = 1:4
    detect_anomalies(['sender', num2str(i)], data_processing_future);
end
delete(gcp); % Close the parallel pool
