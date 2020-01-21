function [distances] = getDistances()
%GETDISTANCES Load the summary distance information

    % Import the distances from the database, assuming:
    %
    % 5 - source      - id of source district
    % 6 - destination - id of destination district
    % 7 - distance_km - centeriod distance between the two districts
    raw = csvread('bf_district_dist_centeroid.csv', 1, 0);
    
    % Prepare the zeroed matrix
    count = max(raw(:,5));
    distances = zeros(count, count);
    
    for row = 1:count
        for col = 1:count
            % Get the relevent data
            if row == col, continue; end
            rows = raw( raw(:,5)== row, : );
            data = rows( rows(:,6)== col, : );

            % Verify the dimentions are correct
            if size(data, 1) ~= 1
                fprintf('\n\nERROR r=%d c=%d numrows=%d\n\n', row, col, size(EEE,1));
            end

            % Update the matrix
            distances(row, col) = data(1, 7);
        end
    end
end

