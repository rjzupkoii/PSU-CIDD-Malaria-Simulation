-- Progress towards completion (assuming 11 years)
SELECT *, round(cast(cast(days AS float) / 4018.0 * 100 AS numeric), 2) AS "Percent Complete"
FROM (
SELECT replicateid, max(dayselapsed) AS days, 
  (SELECT filename 
   FROM sim.replicate 
   INNER JOIN sim.configuration ON sim.configuration.id = sim.replicate.configurationid 
   WHERE  sim.replicate.id = replicateid)
FROM sim.monthlydata
GROUP BY replicateid) sq
WHERE days > 0
ORDER BY days DESC

-- Model execution time
SELECT configurationid, sim.replicate.id AS replicateid, filename,
  starttime, endtime, endtime - starttime AS time 
FROM sim.replicate
  INNER JOIN sim.configuration ON sim.configuration.id = sim.replicate.configurationid
WHERE endtime IS NOT NULL
ORDER BY configurationid