% Generate plots that can be used visually verify the movement parameters
% in the model.

% The data file to be processed
FILENAME = 'bf-05.csv';

% Import the raw data from the database, assuming:
% 1: "id"
% 2: "replicateid"
% 3: "timestep"
% 4: "count" or the number of trips
% 5: "source" district or province
% 6: "destination" district or province
movements = csvread(FILENAME, 1, 0);

% Import the distances from the database, assuming:
% 5: "source" district or province
% 6: "destination" district or province
% 7: "distance_km"
raw_distances = csvread('bf_district_dist_centeroid.csv', 1, 0);

% Convert the distances to square matrix
count = max(raw_distances(:,5));
distances = zeros(count, count);
for row = 1:count
    for col = 1:count
        % Get the relevent data
        if row == col, continue; end
        rows = raw_distances( raw_distances(:,5)== row, : );
        data = rows( rows(:,6)== col, : );
        
        % Verify the dimentions are correct
        if size(data, 1) ~= 1
            fprintf('\n\nERROR r=%d c=%d numrows=%d\n\n', row, col, size(EEE,1));
        end

        % Update the matrix
        distances(row, col) = data(1, 7);
    end
end
        
% Get all of the unique timepoints
timepoints = unique(movements(:, 3));

% Start with the first subplot
spi = 1;

% Generate plots of the total trips by distance for each unique day
for day = transpose(timepoints)
    
    % Set the sub plot
    subplot(3, 4, spi);
    
    % Get the movements for the day
    day_movements = movements(movements(:, 3) == day, :);
    rows = size(day_movements, 1);
    data = zeros(rows,2);

    % Convert to log10
    for row = 1:rows
        dist = distances(day_movements(row, 5), day_movements(row, 6));
        data(row, 1) = log10(dist);
        data(row, 2) = log10(day_movements(row, 4));
    end
    
    % Generate the plots
    plot( data(:,1), data(:,2), 'ko', 'MarkerFaceColor', 'b', 'MarkerSize', 5 ); hold on;
      set(gca, 'XTick', [0.699 1 1.699 2 2.699 3]);
      set(gca, 'XTickLabel', {'5','10','50','100','500', '1000'});
      set(gca, 'YTick', [0.699 1 1.699 2 2.699 3 3.699 4]);
      set(gca, 'YTickLabel', {'5', '10', '50', '100', '500', '1000', '5000', '10000'});
    xlabel('DISTANCE IN KILOMETERS');
    ylabel('TOTAL COUNT OF TRIPS');
    axis([1 3.1 0 4.5]);    
    title(sprintf('day=%d', day));
    
    % Move to the next plot
    spi = spi + 1;
end

% Get the popuation of each district
population = csvread('district-population.csv', 1, 0);

KAPPA = 0.1;
ALPHA = 0.5;
BETA = 0.01;
GAMMA = 2.0;

% Generate plots of the total trips by attraction for each unique day
spi = 1;
for day=transpose(timepoints)
    subplot(3, 4, spi);
    
    day_movements = movements(movements(:, 3) == day, :);
    rows = size(day_movements, 1);
    data = zeros(rows,2);

    for row = 1:rows
        dist = distances(day_movements(row, 5), day_movements(row, 6));
        
        % Gravity model measure of attraction
        data(row, 1) = KAPPA * (population(day_movements(row, 5), 2) ^ ALPHA) * ...
                       (population(day_movements(row, 6), 2 ) ^ BETA) / (dist ^ GAMMA) ;
                   
        data(row, 2) = log10(day_movements(row, 4));
    end
    
    plot( data(:,1), data(:,2), 'ko', 'MarkerFaceColor', 'b', 'MarkerSize', 5 ); hold on;
      set(gca, 'YTick', [0.699 1 1.699 2 2.699 3 3.699 4]);
      set(gca, 'YTickLabel', {'5', '10', '50', '100', '500', '1000', '5000', '10000'});
    xlabel('log10(k * (POP^a * POP^b) / DIST^g)');
    ylabel('TOTAL COUNT OF TRIPS');
    title(sprintf('day=%d', day));
    
    spi = spi + 1;
end
