% masin.m
%
% Matlab function to query the MaSim database, developed targeting version 2019a.

function [data] = masim(query)
%MASIM Connect to the MaSim database and run the query provided

    % Test to make sure the JVM is running, install the driver to the path
    if ~usejava('jvm'); error([filename ' requires Java to run']); end
    javaaddpath(strcat(pwd, filesep, 'bin', filesep, 'postgresql-42.2.9.jar'));

    % Open the connection to the database
    connection = database('', 'sim', 'sim', 'org.postgresql.Driver', 'jdbc:postgresql://masimdb.vmhost.psu.edu:5432/masim');
    if connection.MaxDatabaseConnections == -1
        error(['Error connecting to the database: ' connection.Message]);
    end
    
    % Query for the data
    data = select(connection, query);
    
    % Close the connection
    close(connection);
end