% diagnostic_population.m
%
% Generate heatmaps for each of the months involved in the data set.
clear;

% Load the data
FILENAME = 'marshall-pop.csv';
TIMESTEP = 1; X = 3; Y = 4; POPULATION = 6;
data = csvread(FILENAME, 1, 0);

% Note the number of rows and cols in the data set
global rows; global cols;
rows = max(data(:, X) + 1);
cols = max(data(:, Y) + 1);

% Start by ploting the population at intilization
map = zeros(rows, cols);
working = data(data(:, 1) == 0, :);
for index = 1:size(working, 1)
    x = working(index, X) + 1;
    y = working(index, Y) + 1;
    map(x, y) = working(index, POPULATION);
end
plot(map, "Inital Population", 1);

% Plot the log10 population for each time step
spi = 2;
for day = transpose(unique(data(:, 1)))
    map = zeros(rows, cols);
    working = data(data(:, 1) == day, :);
    for index = 1:size(working, 1)
        x = working(index, X) + 1;
        y = working(index, Y) + 1;
        map(x, y) = log10(working(index, POPULATION));
    end
    plot(map, sprintf("Timestep = %d", day), spi);
    spi = spi + 1;        
end

% The map still has the log10 data, so use it to do a final plot with the
% popuation information
plot(10 .^ map, "Final Population", spi);

function [] = plot(map, title, spi)
    global rows; global cols;

    % Plot the heatmap
    subplot(3, 5, spi);
    hm = heatmap(map);
    hm.Title = title;
    hm.XDisplayLabels = repmat(' ', cols, 1); 
    hm.YDisplayLabels = repmat(' ', rows, 1);
    grid off;
end

