function output = data_processing_storage()
output = '';
try
    % Create a serial port object
    port = serialport("/dev/cu.usbmodem11701", 9600);

    % number of senders
    numSenders = 4;

    % Create cell arrays for storing sender data tables
    senderData = cell(1, numSenders);
    for i = 1:numSenders
        senderData{i} = table('Size',[0, 5], 'VariableTypes', {'string', 'double', 'string', 'double', 'datetime'}, 'VariableNames', {'Address', 'ID', 'Type', 'Value',  'Timestamp'});
    end

    % Set up JDBC data source 
    vendor = "MySQL";
    opts = databaseConnectionOptions("jdbc", vendor);
    opts.JDBCDriverLocation = "/Users/ghaidabouchaala/Downloads/mysql-connector-j-8.0.33/mysql-connector-j-8.0.33.jar";
    opts.DatabaseName = "wsn_data";
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
            createTableSQL = ['CREATE TABLE IF NOT EXISTS ', senderTableName, ' (Address VARCHAR(255), ID INT, Type VARCHAR(255), Value DOUBLE, Timestamp DATETIME)'];
            exec(conn, createTableSQL);
        end
        tic;
        while toc < 1200
            % Read data from the serial port
            line = char(readline(port));

            % Debug: Display the class and size of line
            disp(['Class of line: ', class(line)]);
            disp(['Size of line: ', mat2str(size(line))]);

            % Debugging: Display the raw, unprocessed line
            disp(['Raw line: ', line]);

            % Convert the line to a string and trim leading/trailing whitespaces
            line = char(line);
            line = regexprep(line, '^\s+|\s+$', '');

            % Debugging: Display the trimmed line
            disp(['Trimmed line: ', line]);

            % Display the received line for debugging
            disp(['Received line: ', line]);

            % Check if the line starts with '[' and ends with ']'
            if startsWith(line, '[') && endsWith(line, ']')
                % Remove the brackets
                line = line(2:end-1);

                % Split the line into parts
                parts = split(line, ':');

                % Check the number of parts
                if numel(parts) == 4
                    % Extract the sender ID from the line
                    senderID = str2double(parts{2});

                    % Store the received data in the corresponding table
                    row = {parts{1}, str2double(parts{2}), parts{3}, str2double(parts{4}), datetime('now')};
                    senderData{senderID} = [senderData{senderID}; cell2table(row, 'VariableNames', {'Address', 'ID', 'Type', 'Value', 'Timestamp'})];


                    % Prepare the insert statement for the current line
                    senderTableName = ['sender', num2str(senderID)];
                    insertSQL = sprintf('INSERT INTO %s (Address, ID, Type, Value, Timestamp) VALUES (''%s'', %d, ''%s'', %.2f, ''%s'')', ...
                        senderTableName, parts{1}, str2double(parts{2}), parts{3}, str2double(parts{4}), datestr(now, 'yyyy-mm-dd HH:MM:SS'));


                    % Display the insert statement for debugging
                    disp(['Insert statement: ', insertSQL]);


                    % Execute the insert statement
                    exec(conn, insertSQL);

                    % Display the received data
                    disp(line);
                else
                    % Display an error message for lines with incorrect format
                    disp('Skipping line with incorrect format:');
                    disp(line);
                    disp('Parts:');
                    disp(parts);
                    disp('Number of parts:');
                    disp(numel(parts));
                end
            else
                % Skip lines that do not start with '['
                disp('Skipping line:');
                disp(line);
            end
        end

    catch ex
        % Display any errors that occur during the execution
        disp('Error occurred:');
        disp(ex.message);

    end

    % Close the database connection
    if isopen(conn)
        close(conn);
        disp('Database connection closed.');
    else
        disp('Database connection is already closed.');
    end

    % Delete the serial port object
    if isvalid(port)
        delete(port);
        disp('Serial port connection closed.');
    else
        disp('Serial port connection is already closed or does not exist.');
    end
    % assign a success message to output if everything is fine
    output = 'Success!';
catch ex
    output = ex.message;
    % Log the full exception, including the stack trace
    disp(getReport(ex, 'extended', 'hyperlinks', 'on'));
end
end