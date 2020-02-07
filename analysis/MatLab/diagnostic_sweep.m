FILENAME = 'marshall-sweep.csv';

% Load the data
movements = csvread(FILENAME, 1, 0);
COUNT = 3; SOURCE = 4; DESTINATION = 5;
distances = squareDistances('reference/bf_district_dist_centeroid.csv');
population = csvread('reference/bf_population_50_scaling.csv', 1, 0);

% Get the unique filters
percentages = unique(movements(:, 2));

% Generate plots of the total trips by distance for each year
% spi = 1;
% for percentage = transpose(percentages)
%     
%     % Set the sub plot
%     subplot(5, 5, spi);
%     
%     % Get the movements for the sweep value
%     batch = movements(movements(:, 2) == percentage, :);
%     rows = size(batch, 1);
%     data = zeros(rows, 2);
%     
%     for row = 1:rows
%         dist = distances(batch(row, SOURCE), batch(row, DESTINATION));
%         data(row, 1) = dist;
%         data(row, 2) = log10(batch(row, COUNT));
%     end
%         
%     % Generate the plots
%     plot(data(:,1), data(:,2), 'ko', 'MarkerFaceColor', 'b', 'MarkerSize', 5 ); hold on;
%     xlabel('DISTANCE IN KILOMETERS');
%     ylabel('log10(COUNT OF TRIPS)');
%     title(sprintf('Circulation: %d%%', round(percentage * 100)));
%     
%     % Move to the next plot
%     spi = spi + 1;
% end

% Note the parameters
ALPHA = 1.27;
RHO = 10 ^ 0.54;
TAU = 1.342;

% Generate plots of the total trips by distance for each year
spi = 1;
for percentage = transpose(percentages)
    
    % Set the sub plot
    subplot(5, 5, spi);
    
    % Isolate the data we care about
    batch = movements(movements(:, 2) == percentage, :);
    rows = size(batch, 1);
    data = zeros(rows, 2);
    
    for row = 1:rows
        % Gravity model with kernel
        kernel = (1 + (distances(batch(row, SOURCE), batch(row, DESTINATION)) / RHO)) ^ (-ALPHA);
        data(row, 1) = log10((population(batch(row, DESTINATION), 2 ) ^ TAU) * kernel);

        % Count of movements
        data(row, 2) = log10(batch(row, COUNT));
    end
    
    % Plot the data
    plot( data(:,1), data(:,2), 'ko', 'MarkerFaceColor', 'b', 'MarkerSize', 5 ); hold on;
    xlabel('log10(POP^{TAU} * KERNEL)');
    ylabel('log10(COUNT OF TRIPS)');
    title(sprintf('Circulation: %d%%', round(percentage * 100)));
    spi = spi + 1;
end
