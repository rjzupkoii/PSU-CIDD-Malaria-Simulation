% analysis.m
% 
% This script is used to verify that the movement data from the model shows
% linear scaling in realtion to the rescaling of the population. If no
% emergent effects appear, this allows for the movement verification to
% proceed at a reduced popuation size allowing for quicker turnaround.

% Data to be reviewed, assuming:
%
% 1 - scaling     - pop. scaling value
% 4 - timestep    - days since start of model
% 5 - count       - number of movements between source and destination
% 6 - source      - id of source district
% 7 - destination - id of destination district
DATA = 'scaling_study_data.csv';
movements = csvread(DATA, 1, 0);

% Import the distances from the database
distances = getDistances();

% Get all of the unique timepoints and scaling
scaling = unique(movements(:, 1));
timepoints = unique(movements(:, 4));

% Get the total sum for each scaling factor
count = [];
for scale = sort(transpose(scaling))
    count = [count; scale sum(movements(movements(:, 1) == scale, 5))];
end

% Plot the results
x = count(:, 1);
y = count(:, 2);
fittedX = linspace(min(x), max(x), 200);
fittedY = polyval(coeffs, fittedX);
plot(fittedX, fittedY, 'r-', 'LineWidth', 3); hold on;
plot(x, y, '*', 'LineWidth', 2, 'MarkerSize', 15); hold on;
xlabel('POP. SCALING FACTOR');
ylabel('TOTAL COUNT OF TRIPS');
title('Population Scaling, with Fit Line');

% Calculate the R^2
[coeffs, S] = polyfit(x, y, 1);
r2 = 1 - (S.normr/norm(y - mean(y)))^2;
fprintf('R^2 = %d\n', r2);
