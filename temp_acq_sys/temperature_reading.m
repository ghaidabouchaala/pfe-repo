% Create a serial port object
port = serialport("/dev/cu.usbmodem11101", 9600);
configureTerminator(port, "LF");

% Initialize arrays to store the data
temp_lm35 = [];
temp_ds18b20 = [];
time_lm35 = [];
time_ds18b20 = [];

% Start a timer
tic;

counter = 0;
% Read data for 60 minutes
while toc < 3600
    % Read a line from the serial port
    line = readline(port);

    % Convert the line to a double
    temperature = str2double(line);

    % If the line is not a number, skip to the next iteration
    if isnan(temperature)
        fprintf("Invalid data: %s\n", line);
        continue;
    end

    % Depending on the counter, assign temperature to different sensor
    if mod(counter,2)==0
        temp_lm35 = [temperature_value; temperature];
        time_lm35 = [time_lm35; datetime('now')];
    else
        temp_ds18b20 = [temperature_value; temperature];
        time_ds18b20 = [time_ds18b20; datetime('now')];
    end

    counter = counter + 1;
end

% Close the serial port
delete(port);

% Create tables and write them to separate files
T_lm35 = table(time_lm35, temp_lm35);
T_ds18b20 = table(time_ds18b20, temp_ds18b20);

try
    writetable(T_lm35, "temperature_lm35.xlsx");
    writetable(T_ds18b20, "temperature_ds18b20.xlsx");
catch ME
    fprintf("Error writing to file: %s\n", ME.message);
end

% Display some debugging information
%fprintf("Received %d data points\n", length(data));
%disp(T);

% Plot the data
% plot(time, temp);

% Set the axis limits
% xlim([time(1), time(end)]);
% ylim([min(temp)-1, max(temp)+1]);

% Add labels to the plot
% xlabel("Time");
% ylabel("Temperature (C)");
% title("Real-Time Temperature Data");
