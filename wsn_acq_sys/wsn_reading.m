% Create a serial port object
port = serialport("/dev/cu.usbmodem11701", 9600);

% Create tables for storing received data from different senders
sender1Data = table('Size',[0, 4], 'VariableTypes', {'string', 'double', 'string', 'double'}, 'VariableNames', {'Address', 'ID', 'Type', 'Value'});
sender2Data = table('Size',[0, 4], 'VariableTypes', {'string', 'double', 'string', 'double'}, 'VariableNames', {'Address', 'ID', 'Type', 'Value'});
sender3Data = table('Size',[0, 4], 'VariableTypes', {'string', 'double', 'string', 'double'}, 'VariableNames', {'Address', 'ID', 'Type', 'Value'});
sender4Data = table('Size',[0, 4], 'VariableTypes', {'string', 'double', 'string', 'double'}, 'VariableNames', {'Address', 'ID', 'Type', 'Value'});

tic; 
while toc < 15
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
        if senderID == 1
            row = {parts{1}, str2double(parts{2}), parts{3}, str2double(parts{4})};
            sender1Data = [sender1Data; row];
        elseif senderID == 2
            row = {parts{1}, str2double(parts{2}), parts{3}, str2double(parts{4})};
            sender2Data = [sender2Data; row];
        elseif senderID == 3
            row = {parts{1}, str2double(parts{2}), parts{3}, str2double(parts{4})};
            sender3Data = [sender3Data; row];
        elseif senderID == 4
            row = {parts{1}, str2double(parts{2}), parts{3}, str2double(parts{4})};
            sender4Data = [sender4Data; row];
        end

        % Display the received data
        disp(line);
    else
        % Skip lines that do not start with '['
        fprintf('Skipping line: %s\n', line);
    end
    
    % Display the data for each sender
    disp('Sender 1 Data:');
    disp(sender1Data);
    
    disp('Sender 2 Data:');
    disp(sender2Data);
    
    disp('Sender 3 Data:');
    disp(sender3Data);
    
    disp('Sender 4 Data:');
    disp(sender4Data);
end

% Close the serial port
delete(port);
