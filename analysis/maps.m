% maps.m
%
% Matlab function for generating ASC Files from the MaSim database,
% developed targeting version 2019a.

% Relevant variables to connect
DatabaseName = 'MaSim';
UserName = 'sim';
Password = 'sim';

% Test to make sure the JVM is running, install the driver to the path
if ~usejava('jvm'); error([filename ' requires Java to run']); end
javaaddpath(strcat(pwd, filesep, 'postgresql-42.2.9.jar'));

% Open the connection to the database
connection = database('', 'sim', 'sim', 'org.postgresql.Driver', 'jdbc:postgresql://masimdb.vmhost.psu.edu:5432/masim');
if connection.MaxDatabaseConnections == -1
    error(['Error connecting to the database: ' connection.Message]);
end
    
% Done, close the connection
close(connection);