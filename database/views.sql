-- Clear everything query
delete from sim.monthlysitedata;
delete from sim.monthlygenomedata;
delete from sim.monthlydata;
delete from sim.replicate;

-- Start of aggregation query
select md.id,
  md.dayselapsed,
  sum(msd.population) as population,
  sum(msd.clinicalepisodes) as clinicalepisodes,
  sum(msd.treatments) as treatments
from sim.monthlydata md
  inner join sim.monthlysitedata msd on md.id = msd.monthlydataid
group by
  md.id,
  md.dayselapsed
order by md.id