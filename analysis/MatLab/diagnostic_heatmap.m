% diagnostic_heatmap.m
%
% Generate heatmaps for each of the months involved in the data set and
% plot them using the coordinates given for the raster.
FILENAME = 'marshall-all.csv';

% Load the data
data = csvread(FILENAME, 1, 0);
mapping = csvread('Reference/bf_cell_to_district_map.csv', 1, 0);

% In theory all cells should be present as sources
sources = unique(data(:, 5));
destinations = unique(data(:, 6));

% Print some generic data
fprintf('Unique cells in map: %d\n', max(mapping(:, 3)));
fprintf('Unique sources in movements: %d\n', size(sources, 1));
fprintf('Unique destinations in movements: %d\n', size(destinations, 1));
fprintf('Maximum shared trips: %d\n',  max(data(:, 4)));

% Prepare the "map" for the trips
rows = max(mapping(:, 4) + 1);
cols = max(mapping(:, 5) + 1);

% Set the first index
spi = 1;

for day = transpose(unique(data(:, 3)))

    % Zero out the matrix
    trips = zeros(rows, cols);
    
    % Note the interdistrict movement to the given location
    for index = transpose(destinations)
        row = mapping(mapping(:, 3) == index, 4) + 1;
        col = mapping(mapping(:, 3) == index, 5) + 1;
        temp = data(data(:, 3) == day, :);
        trips(row, col) = sum(temp(temp(:, 6) == index, 4));
    end

    % Plot the heatmap
    subplot(3, 4, spi);
    hm = heatmap(trips);
    hm.Title = sprintf("Timestep = %d", day);
    hm.XDisplayLabels = repmat(' ', cols, 1); 
    hm.YDisplayLabels = repmat(' ', rows, 1);
    grid off;
    
    % Move to the next heatmap
    fprintf('Rendered timestep: %d\n', day);
    spi = spi + 1;
end
