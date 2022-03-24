-- Select the interdistrict movements between cells
SELECT *
FROM (
SELECT b.*, sim.location.district AS dd
FROM (SELECT a.*, sim.location.district AS sd
  FROM (SELECT * FROM sim.districtmovement WHERE replicateid = 87 AND timestep = 31) a
    INNER JOIN sim.location ON sim.location.index = source AND sim.location.configurationid = 61) b
  INNER JOIN sim.location ON sim.location.index = destination AND sim.location.configurationid = 61) c
WHERE dd != sd

-- Select the movement across various scales
SELECT r.id replicate, substring(c.filename, 10, 4)::double precision AS scale, sum(dm.count), dm.source, dm.destination
FROM sim.replicate r
  INNER JOIN sim.configuration c ON c.id = configurationid
  INNER JOIN sim.districtmovement dm ON dm.replicateid = r.id
WHERE filename like --FILENAME
group by replicate, scale, source, destination
order by scale

-- Select population by cell
SELECT md.dayselapsed, l.index, l.x, l.y, l.district, msd.population
FROM sim.monthlydata md
  INNER JOIN sim.monthlysitedata msd ON msd.monthlydataid = md.id
  INNER JOIN sim.replicate r ON r.id = md.replicateid
  INNER JOIN sim.location l ON l.configurationid = r.configurationid AND l.id = msd.locationid
WHERE replicateid = --REPLICATEID