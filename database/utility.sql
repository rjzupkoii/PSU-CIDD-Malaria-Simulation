-- Check the disk usage
SELECT pg_size_pretty(pg_database_size('masim'));

-- Free unusued disk space
VACUUM FULL;

-- Check the current size of the database
SELECT pg_size_pretty(pg_database_size('masim')) AS "DB Size",
  months / 12 AS years, *
FROM 
(SELECT count(*) months FROM sim.monthlydata) md,
(SELECT count(*) site FROM sim.monthlysitedata) msd,
(SELECT count(*) genome FROM sim.monthlygenomedata) mgd