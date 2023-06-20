clear

file = 'temperature_lm35.xlsx'; % Specify the name of the Excel file
temperature_values = importdata(file, 1);

% extract column 2 
var = temperature_values.data(:,2);

plot(var);
% Number of clusters
k = 2;

% Perform K-means clustering
[idx, centroids] = kmeans(var, k);

% Plot the data points with cluster assignments
figure;
scatter(1:numel(var), var, 25, idx, 'filled');
colormap(lines(k)); % Set colormap for clusters
colorbar; % Display colorbar
xlabel('Data Point Index');
ylabel('Temperature');
title('K-means Clustering with 2 Clusters');

% Plot the cluster centroids
hold on;
scatter(1:k, centroids, 100, 'k', 'filled');
legend('Cluster 1', 'Cluster 2', 'Centroids');
