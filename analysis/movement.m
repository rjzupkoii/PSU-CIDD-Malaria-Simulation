% movement.m
%
% Load a movement file and generate plots overlaying the actual movement.

raw = csvread('movement.csv', 0, 0);
plotYear(raw, 1, 99);

% Plot one year of twelve months of data.
%
% raw   - the raw data to be plotted.
% year  - the one indexed year to plot.
% focal - the array indexed focal pixel.
function [] = plotYear(raw, year, focal)
    % Determine the upper / lower bounds for days
    lower = (year - 1) * 365;
    upper = year * 365;
    
    % Select the data
    raw = raw(raw(:, 2) == focal, :);
    raw = raw(and((raw(:, 1) >= lower), (raw(:, 1) <= upper)), :);
    
    % Covert the day to index
    raw(:, 1) = floor(raw(:, 1) / 30) + 1;
    
    % Convert the movements
    movements = zeros(size(raw, 1), 4);
    movements(:, 1) = mod(raw(:, 2), 10) + 0.5;
    movements(:, 2) = floor(raw(:, 2) / 10) + 0.5;
    movements(:, 3) = mod(raw(:, 3), 10) + 0.5;
    movements(:, 4) = floor(raw(:, 3) / 10) + 0.5;
    
    for ndx  = 1:size(raw, 1)
        % Set the subplot
        if raw(ndx, 1) > 12, continue, end
        month = raw(ndx, 1);
        offset = (month > 4) + (month > 8);
        subplot(3, 5, month + offset);
        
        % Plot the data
        plot([movements(ndx, 1) movements(ndx, 3)], [movements(ndx, 2) movements(ndx, 4)], 'r-');
        hold on;
        
        set(gca, 'YDir', 'reverse');
        set(gca, 'XTick', 1:10, 'YTick', 1:10);   
        grid on;
    end    
    
    % Plot the subregion to the side as a reference
	subplot(3, 5, 10);
    region = imread('subregion.png');
    imshow(region);
end