% maps.m
%
% Matlab function for generating ASC Files from the MaSim database,
% developed targeting version 2019a.

% TODO Get these settings from the function parameters
configurationId = 8;
monthlyDataId = 1358;   % 4015 days elapsed
getMaSimData(configurationId, monthlyDataId);

function [] = getMaSimData(configurationId, monthlyDataId)
    % Default value for no data
    global NODATA_value;
    NODATA_value = -9999;

    % Test to make sure the JVM is running, install the driver to the path
    if ~usejava('jvm'); error([filename ' requires Java to run']); end
    javaaddpath(strcat(pwd, filesep, 'bin', filesep, 'postgresql-42.2.9.jar'));

    % Open the connection to the database
    connection = database('', 'sim', 'sim', 'org.postgresql.Driver', 'jdbc:postgresql://masimdb.vmhost.psu.edu:5432/masim');
    if connection.MaxDatabaseConnections == -1
        error(['Error connecting to the database: ' connection.Message]);
    end

    % Get the size of the matrix
    query = sprintf('SELECT nrows, ncols, xllcorner, yllcorner, cellsize FROM sim.configuration WHERE id = %d', configurationId);
    configuration = select(connection, query);

    % Select the data
    query = strcat('SELECT sim.location.index, sim.location.x, sim.location.y,', ...
                   'sim.monthlysitedata.population, sim.monthlysitedata.clinicalepisodes', ...
                   ' FROM sim.location', ...
                   ' INNER JOIN sim.monthlysitedata on sim.monthlysitedata.locationid = sim.location.id', ...
                   ' WHERE sim.location.configurationid = %d', ...
                   ' AND sim.monthlysitedata.monthlydataid = %d', ... 
                   ' ORDER BY sim.location.index');
    query = sprintf(query, configurationId, monthlyDataId);
    data = select(connection, query);

    % Done, close the connection
    close(connection);

    % Convert from zero to one indexed x, y coordinates
    data.('x') = data.('x') + 1;
    data.('y') = data.('y') + 1;

    % Extract the relevent data
    population = ones(configuration.nrows, configuration.ncols) * NODATA_value;
    clinical = ones(configuration.nrows, configuration.ncols) * NODATA_value;
    for ndx = 1 : height(data)
        row = data(ndx, :);
        population(row.x, row.y) = row.population;
        clinical(row.x, row.y) = row.clinicalepisodes;
    end
    write('clinical.asc', clinical, configuration);
    write('population.asc', population, configuration);
end

% Save the data provided into a ASC file at the location indicated
function [] = write(filename, data, configuration) 
    global NODATA_value;

    file = fopen(filename, 'w');
    fprintf(file, 'ncols %d%s', size(data, 2), newline);
    fprintf(file, 'nrows %d%s', size(data, 1), newline);
    fprintf(file, 'xllcorner %f%s', configuration.xllcorner, newline);
    fprintf(file, 'yllcorner %f%s', configuration.yllcorner, newline);
    fprintf(file, 'cellsize %d%s', configuration.cellsize, newline);
    fprintf(file, 'NODATA_value %d%s', NODATA_value, newline);
    for ndx = 1 : size(data, 1)
        for ndy = 1 : size(data, 2)
            fprintf(file, '%d ', data(ndx, ndy));
        end
        fprintf(file, newline);
    end
    fclose(file);
end