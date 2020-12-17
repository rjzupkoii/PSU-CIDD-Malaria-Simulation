-- Check the disk usage
SELECT pg_size_pretty(pg_database_size('masim'));

-- Check number of rows in key tables
SELECT relname, reltuples::BIGINT as approximate_row_count 
FROM pg_class WHERE relname in ('monthlydata', 'monthlygenomedata', 'monthlysitedata')

-- Free unusued disk space
VACUUM FULL;

-- Monitor the progress of a vacuum
SELECT heap_blks_scanned / CAST(heap_blks_total AS NUMERIC) * 100 AS heap_blks_percent, progress.*, activity.query
FROM pg_stat_progress_vacuum AS progress
INNER JOIN pg_stat_activity AS activity ON activity.pid = progress.pid;

-- Check the current size of the database
SELECT pg_size_pretty(pg_database_size('masim')) AS "DB Size",
  months / 12 AS years, *
FROM 
(SELECT count(*) months FROM sim.monthlydata) md,
(SELECT count(*) site FROM sim.monthlysitedata) msd,
(SELECT count(*) genome FROM sim.monthlygenomedata) mgd

-- Running time vs PfPR 
select * from v_runningstats
inner join (
select monthlydataid, avg(pfprall) 
from sim.monthlysitedata 
group by monthlydataid) stats on stats.monthlydataid = id
order by id desc

-- Running time per replicate
SELECT starttime, runningtime, EXTRACT(EPOCH FROM runningtime) / days AS seconds, days
FROM (
SELECT starttime, now() - starttime runningtime, max(dayselapsed) AS days
FROM sim.replicate r
  INNER JOIN sim.monthlydata md ON md.replicateid = r.id
WHERE r.id = 35877
GROUP BY starttime) sq