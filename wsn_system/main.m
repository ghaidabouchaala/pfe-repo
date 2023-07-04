% Start the data acquisition and processing in a new thread
f = parfeval(@run, 0, 'data_processing_storage.m');

% Pause for some time (e.g., 10 minutes) to allow data accumulation
pause(10*60);

% Start the anomaly detection in parallel for all senders
parpool(4); % Start a parallel pool with 4 workers
parfor i = 1:4
    detect_anomalies(['sender', num2str(i)]);
end
delete(gcp); % Close the parallel pool

% After all the detections have finished, calculate metrics
calculate_metrics();

% Check if the data acquisition and processing is still running and cancel it if it is
if ~strcmp(f.State, 'finished')
    cancel(f);
end

% Delete the serial port object
if isvalid(port)
    delete(port);
    disp('Serial port connection closed.');
else
    disp('Serial port connection is already closed or does not exist.');
end
