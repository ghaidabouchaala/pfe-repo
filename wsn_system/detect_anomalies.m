function detect_anomalies(sensorTableName)
% Vendor and JDBC Driver details
vendor = "MySQL";
opts = databaseConnectionOptions("jdbc", vendor);
opts.JDBCDriverLocation = "/Users/ghaidabouchaala/Downloads/mysql-connector-j-8.0.33/mysql-connector-j-8.0.33.jar";
opts.DatabaseName = "wsn_data";
opts.Server = "localhost";
opts.PortNumber = 3306;

username = "root";
password = "";

% Establishing connection inside worker
conn = database(opts.DatabaseName, username, password, 'Vendor', vendor, 'Server', opts.Server, 'PortNumber', opts.PortNumber);

if ~isopen(conn)
    error('Failed to connect to the database.');
end

% Query the database to get the 'Value' column data from the given sensor table
values = select(conn, sprintf('SELECT Value FROM %s', sensorTableName));

% Query the database to get the 'Type' column data from the given sensor table, limit 1
type = select(conn, sprintf('SELECT Type FROM %s LIMIT 1', sensorTableName));


% Extract sensor value
var = values.Value;

% Extract sensor type
sensorType = type.Type;


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

% Initialize the array to store the estimates
x_estimates = zeros(2, num_steps);

for k = 1:num_steps
    % Update true position
    x(1) = x(1) + x(2) * dt;

    % Generate measurement with noise
    measurement_noise = 0; %sqrt(R) * randn;
    z = var(k);

    % Save true position and measurement
    true_position(k) = x(1);
    measurements(k) = var(k);

    % Save the estimate
    x_estimates(:, k) = x;

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


    %clf
    subplot(311)
    hold on
    plot(k,z,'ko')
    plot(k,x_pred(1),'b+')
    plot(k,x_pred(1) + threshold, 'r.')
    plot(k,x_pred(1) - threshold, 'r.')
    title('Measurement, Predicted Position, and Threshold for', sensorType)
    xlabel('Sample')
    ylabel([sensorType,' Value '])
    legend('Measurement', 'Predicted Position', 'Threshold')


    %pause(.1)
    % Plot estimation error
    subplot(312)
    hold on
    plot(k,abs(x_pred(1) - z),'.')
    title('Estimation Error')
    xlabel('Sample')
    ylabel('Error')

    % Plot anomaly detection
    subplot(313)
    hold on
    plot(k,abs(x_pred(1) - z) > threshold,'.')
    title('Anomaly Detection')
    xlabel('Step')
    ylabel('Anomaly')

    drawnow
end

% Plot true position and filtered estimate
time = dt * (1:num_steps);
figure;
hold on;
plot(time, true_position, 'b-', 'LineWidth', 1.5);
plot(time, x_estimates(1,:), 'r--', 'LineWidth', 1.5);
xlabel('Time');
ylabel('Position');
legend('True Position', 'Filtered Estimate');
title('Kalman Filter: 1D Constant Velocity');
grid on;


% Remember to close the connection when you're finished
if isopen(conn)
    close(conn);
    disp('Database connection closed.');
else
    disp('Database connection is already closed.');
end

close(conn);
end
