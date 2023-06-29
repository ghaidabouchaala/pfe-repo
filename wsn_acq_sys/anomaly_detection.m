% Create parallel pool for parallel execution
parpool;

% Create a serial port object
port = serialport("/dev/cu.usbmodem111701", 9600);

% Specify the number of senders
numSenders = 4;

% Create cell arrays for storing sender data tables and anomaly detection results
senderData = cell(1, numSenders);
anomalyResults = cell(1, numSenders);
for i = 1:numSenders
    senderData{i} = table('Size',[0, 4], 'VariableTypes', {'string', 'double', 'string', 'double'}, 'VariableNames', {'Address', 'ID', 'Type', 'Value'});
end

tic;
while toc < 300
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
        senderData{senderID} = [senderData{senderindaID}; row];

        % Display the received data
        disp(line);
    else
        % Skip lines that do not start with '['
        fprintf('Skipping line: %s\n', line);
    end
end

% Perform anomaly detection on each sender's data
if toc >= 120
    parfor i = 1:numSenders
    % Extract the data for the current sender
    data = senderData{i};

    % Extract the values for anomaly detection
    var = data.Value;

    % Perform anomaly detection using the Kalman filter

    % Rest of your anomaly detection code goes here...
    % (Adapted from the previous code you provided)

    % Time step
    dt = 0.001;

    % Initial state
    x = [var(1); 1];  % [position; velocity]

    % State transition matrix
    A = [1 dt; 0 1];

    % Control input matrix
    B = [0; 0];

    % Measurement matrix
    H = [1 0];

    % Process noise covariance
    Q = [1 0; 0 0.1];

    % Measurement noise covariance
    R = 1;

    % Initial state covariance
    P = eye(2);

    % Generate true position and noisy measurements
    num_steps = numel(var);
    true_position = zeros(num_steps, 1);
    measurements = zeros(num_steps, 1);

    clf

    for k = 1:num_steps
        % Update true position
        x(1) = x(1) + x(2) * dt;

        % Generate measurement with noise
        measurement_noise = 0; %sqrt(R) * randn;
        z = var(k);

        % Save true position and measurement
        true_position(k) = x(1);
        measurements(k) = var(k);

        % Kalman filter prediction step
        x_pred = A * x;
        P_pred = A * P * A' + Q;

        % Kalman filter update step
        y = z - H * x_pred;
        S = H * P_pred * H' + R;
        K = P_pred * H' / S;

        threshold = 3 * P(1);

        % update KF
        if abs(x_pred(1) - z) <= threshold
            x = x_pred + K * y;
            P = (eye(2) - K * H) * P_pred;
        end

        subplot(311)
        hold on
        plot(k, z, 'ko')
        plot(k, x_pred(1), 'b+')
        plot(k, x_pred(1) + threshold, 'r.')
        plot(k, x_pred(1) - threshold, 'r.')
        title('Measurement, Predicted Position, and Threshold')
        xlabel('Sample')
        ylabel('Temperature')
        legend('Measurement', 'Predicted Position', 'Threshold')

        subplot(312)
        hold on
        plot(k, abs(x_pred(1) - z), '.')
        title('Estimation Error')
        xlabel('Sample')
        ylabel('Error')

        subplot(313)
        hold on
        plot(k, abs(x_pred(1) - z) > threshold, '.')
        title('Anomaly Detection')
        xlabel('Step')
        ylabel('Anomaly')

        drawnow
    end

    % Store the anomaly detection results for the current sender
    anomalyResults{i} = [true_position, measurements];

    % Plot true position and filtered estimate
    time = dt * (1:num_steps);
    figure;
    hold on;
    plot(time, true_position, 'b-', 'LineWidth', 1.5);
    plot(time, x(1:num_steps), 'r--', 'LineWidth', 1.5);
    xlabel('Time');
    ylabel('Position');
    legend('True Position', 'Filtered Estimate');
    title('Kalman Filter: 1D Constant Velocity');
    grid on;
    end

end

% Display the data for each sender
for i = 1:numSenders
    disp(['Sender ', num2str(i), ' Data:']);
    disp(senderData{i});
end

% Close the parallel pool
delete(gcp);

% Close the serial port
delete(port);
