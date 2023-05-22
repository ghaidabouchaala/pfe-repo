% Create a serial port object
port = serialport("/dev/cu.usbmodem1101", 9600);
configureTerminator(port, "LF");

% Initialize arrays to store the data
lm35_data = [];
dallas_data = [];
dht11_temp_data = [];
dht11_hum_data = [];
ultrasonic_data = [];
time = [];
% Start a timer
tic;

counter = 1;
% Read data for 60 seconds
% Read data for 60 seconds
while toc < 100
    % Read a line from the serial port
    line = readline(port);
    
    % Convert the line to a double
    value = str2double(line);
    fprintf(line);
    % If the line is not a number, skip to the next iteration
    if isnan(value)
        fprintf("Invalid data: %s\n", line);
        continue
    end

    % Append the value to the appropriate data array based on the counter
    if counter == 1
        lm35_data = [lm35_data; value];
        time = [time; datetime('now')];
    elseif counter == 2
        dallas_data = [dallas_data; value];
    elseif counter == 3
        dht11_hum_data = [dht11_hum_data; value];
    elseif counter == 4
        dht11_temp_data = [dht11_temp_data; value];
    elseif counter == 5
        ultrasonic_data = [ultrasonic_data; value];
    end

    % Update the counter
    counter = mod(counter, 5) + 1;
end

if (numel(time) > numel(dallas_data)) || (numel(lm35_data) > numel(dallas_data))
    time(end) = [];
    lm35_data(end) = [];
end

% Close the serial port
delete(port);

% Create a table and write it to a file
T = table(time, lm35_data, dallas_data, dht11_hum_data, dht11_temp_data, ultrasonic_data);
try
    writetable(T, "sensors_readings.xlsx");
catch ME
    fprintf("Error writing to file: %s\n", ME.message);
end


