% maps.m
%
% Matlab function for generating ASC Files from the MaSim database.

% TODO List out the relevant variables to connect
DatabaseName = 'MaSim';
UserName = 'sim';
Password = 'sim';
DatabaseURL = 'jdbc:postgresql://host:port/masim'

% Open the connection to the database
% https://www.mathworks.com/help/database/ug/database.html
connection = database(DatabaseName, UserName, Password, 'org.postgresql.Driver', DatabaseURL)

% Prepare the query
query = 'SELECT * FROM sim.MonthlySiteData';
data = select(connection, query);

% TODO Convert the data into a matrix

% TODO Write the data to a an ASC File