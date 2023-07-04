% Create a serial port object
port = serialport("/dev/cu.usbmodem111701", 9600);

% Specify the number of senders
numSenders = 4;

% Create cell arrays for storing sender data tables
senderData = cell(1, numSenders);
for i = 1:numSenders
    senderData{i} = table('Size',[0, 4], 'VariableTypes', {'string', 'double', 'string', 'double'}, 'VariableNames', {'Address', 'ID', 'Type', 'Value'});
end

% Set up JDBC data source using command line
vendor = "MySQL";
opts = databaseConnectionOptions("jdbc", vendor);
opts.JDBCDriverLocation = "/Users/ghaidabouchaala/Downloads/mysql-connector-j-8.0.33/mysql-connector-j-8.0.33.jar";
opts.DatabaseName = "sensor_data";
opts.Server = "localhost";
opts.PortNumber = 3306;

username = "root";
password = "";

% Create a connection object
conn = database(opts.DatabaseName, username, password, 'Vendor', vendor, 'Server', opts.Server, 'PortNumber', opts.PortNumber);

% Check if the connection is valid
if isopen(conn)
    disp('Database connection successful.');
else
    disp('Failed to connect to the database.');
    delete(port);
    return;
end

try
    % Create tables if they don't exist
    for i = 1:numSenders
        senderTableName = ['sender', num2str(i)];
        createTableSQL = ['CREATE TABLE IF NOT EXISTS ', senderTableName, ' (Address VARCHAR(255), ID INT, Type VARCHAR(255), Value DOUBLE)'];
        exec(conn, createTableSQL);
    end

    tic;
    while toc < 60
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

    % Insert the collected data into the database
    for i = 1:numSenders
        data = senderData{i};

        % Write the data to the database table
        senderTableName = ['sender', num2str(i)];
        columnNames = data.Properties.VariableNames;
        columnNamesStr = join(columnNames, ', ');

        try
            % Convert numeric values to string representation for the SQL query
            values = data{:,:};
            valuesStr = sprintf('%.4f, ', values');
            valuesStr = valuesStr(1:end-2);

            insertSQL = ['INSERT INTO ', senderTableName, ' (', columnNamesStr, ') VALUES (', valuesStr, ')'];
            disp('Insert statement:');
            disp(insertSQL); % Debug statement to display the insert statement
            exec(conn, insertSQL);

            % Check if the data was inserted successfully by performing a SELECT query
            selectSQL = ['SELECT * FROM ', senderTableName];
            dataInserted = fetch(conn, selectSQL);
            disp(['Data inserted into ', senderTableName, ':']);
            disp(dataInserted);
        catch ex
            disp('Error inserting data into the database:');
            disp(ex.message);
        end
    end

    % Close the serial port
    delete(port);

    % Close the database connection
    close(conn);

catch ex
    disp('Error occurred:');
    disp(ex.message);
end
