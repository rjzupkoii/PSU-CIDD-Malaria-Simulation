
-- Count and bin all of the individual movements that took place within the given time frame
SELECT movements, count(*) total
FROM (
  SELECT individualid, count(*) AS movements
  FROM sim.movement
  WHERE replicateid = 90
    AND timestep < 366
  GROUP BY individualid) indvidual_count
GROUP BY movements
ORDER BY total DESC

-- Count and bin all of the movements that took place based upon distance traveled
SELECT distance AS distance_km, count(*)
FROM (
  SELECT m.timestep, m.individualid, 
    m.source, ls.x AS sx, ls.y AS sy, 
    m.destination, ld.x AS dx, ld.y AS dy, 
    round(sqrt((ld.x - ls.x) ^ 2 + (ld.y - ls.y) ^ 2)::numeric * 5, 0) AS distance 
  FROM sim.movement m INNER JOIN sim.replicate r ON r.id = 90
    INNER JOIN sim.location ls ON ls.index = m.source AND ls.configurationid = 31
    INNER JOIN sim.location ld ON ld.index = m.destination AND ld.configurationid = 31
  WHERE m.timestep < 366
) movements
GROUP BY distance 
ORDER BY distance ASC