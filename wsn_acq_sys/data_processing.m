% Create a serial port object
port = serialport("/dev/cu.usbmodem11701", 9600);

% Specify the number of senders
numSenders = 4;

% Create cell arrays for storing sender data tables
senderData = cell(1, numSenders);
for i = 1:numSenders
    senderData{i} = table('Size',[0, 4], 'VariableTypes', {'string', 'double', 'string', 'double'}, 'VariableNames', {'Address', 'ID', 'Type', 'Value'});
end

tic;
while toc < 3600
    % Read data from the serial port
    line = readline(port);

    % Convert the data to a string
    line = char(line);
    
    % Remove leading and trailing whitespaces
    line = strtrim(line);

    % Check if the line starts with '[' and ends with ']'
    if startsWith(line, '[') && endsWith(line, ']')
        % Remove the brackets
        line = line(2:end-1);

        % Split the line into parts
        parts = split(line, ':');

        % Extract the sender ID from the line
        senderID = str2double(parts{2});

        % Store the received data in the corresponding table
        row = {parts{1}, str2double(parts{2}), parts{3}, str2double(parts{4})};
        senderData{senderID} = [senderData{senderID}; row];

        % Display the received data
        disp(line);
    else
        % Skip lines that do not start with '['
        fprintf('Skipping line: %s\n', line);
    end
    
    % Display the data for each sender
    for i = 1:numSenders
        disp(['Sender ', num2str(i), ' Data:']);
        disp(senderData{i});
    end
end

% Close the serial port
delete(port);

% Save the collected data for anomaly detection
for i = 1:numSenders
    filename = ['senderData', num2str(i), '.mat'];
    data = senderData{i};
    save(filename, 'data');
end
