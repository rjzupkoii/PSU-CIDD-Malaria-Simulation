% movement.m
%
% Matlab function extracting movement information from the MaSim database,
% developed targeting version 2019a.

replicateId = 78;       % Specific replicate to use
configurationId = 29;   % Baseline configuration to use
data = getData(replicateId, configurationId);

function [data] = getData(replicateId, configurationId)

    % Prepare and run the query
    query = strcat( ...
        'SELECT m.timestep, m.individualid, ', ...
        ' m.source, ls.x AS sx, ls.y AS sy, ', ...
        ' m.destination, ld.x AS dx, ld.y AS dy, ', ...
        ' round(sqrt((ld.x - ls.x) ^ 2 + (ld.y - ls.y) ^ 2)::numeric, 2) AS distance ', ...
        ' FROM sim.movement m INNER JOIN sim.replicate r ON r.id = %d ', ...
        ' INNER JOIN sim.location ls ON ls.index = m.source AND ls.configurationid = %d ', ...
        ' INNER JOIN sim.location ld ON ld.index = m.destination AND ld.configurationid = %d');
    query = sprintf(query, replicateId, configurationId, configurationId);
    data = masim(query);

end
