% Create a serial port object
port = serialport("/dev/cu.usbmodem1101", 9600);
configureTerminator(port, "LF");

% Initialize arrays to store the data
temp = [];
time = [];
type = [];
% Start a timer
tic;

counter = 0;
% Read data for 60 seconds
while toc < 3600
    % Read a line from the serial port
    line = readline(port);
    
    % Convert the line to a double
    temperature = str2double(line);
    
    % If the line is not a number, skip to the next iteration
    if isnan(temperature)
        fprintf("Invalid data: %s\n", line);
        continue
    end
    
    if mod(counter,2)==0
        sensor_type = 'lm_35';
    else
        sensor_type = 'ds18b20'
    end

    % Append the temperature and time to the data arrays
    temp = [temp; temperature];
    time = [time; datetime('now')];
    type = [type;string(sensor_type)];

    counter = counter + 1;

end

% Close the serial port
delete(port);

% Create a table and write it to a file
T = table(time, temp, type);
try
    writetable(T, "temp_with_anomalies.xlsx");
catch ME
    fprintf("Error writing to file: %s\n", ME.message);
end

% Display some debugging information
%fprintf("Received %d data points\n", length(data));
%disp(T);

% Plot the data
plot(time, temp);

% Set the axis limits
xlim([time(1), time(end)]);
ylim([min(temp)-1, max(temp)+1]);

% Add labels to the plot
xlabel("Time");
ylabel("Temperature (C)");
title("Real-Time Temperature Data");
