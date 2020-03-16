
-- No treatment
select replicateid,
  cast(substr(filename, 5, position('-' in substr(filename, 5)) - 1) as int) as detection,
  cast(replace(substr(filename, position('-' in substr(filename, 5)) + 5), '.yml', '') as float) as beta,
  avg(eir) as eir, 
  avg(pfprunder5) as pfprunder5, 
  avg(pfpr2to10) as pfpr2to10, 
  avg(pfprall) as pfprall
from sim.monthlydata md 
  inner join sim.monthlysitedata msd on msd.monthlydataid = md.id
  inner join sim.replicate r on r.id = replicateid
  inner join sim.configuration c on c.id = r.configurationid
where dayselapsed > 7305 and dayselapsed < 7701
  and replicateid in (
	  select id from v_replicates where filename like 'mmc-%' and id not in (
	  	select id from v_replicates where filename like 'mmc-%-treat-%'))
group by replicateid, filename
order by detection, beta

-- With treatment
select replicateid,
  cast(substr(filename, 5, position('-' in substr(filename, 5)) - 1) as int) as detection,
  cast(replace(substr(filename, position('treat' in filename) + 6), '.yml', '') as float) as beta,
  avg(eir) as eir, 
  avg(pfprunder5) as pfprunder5, 
  avg(pfpr2to10) as pfpr2to10, 
  avg(pfprall) as pfprall
from sim.monthlydata md 
  inner join sim.monthlysitedata msd on msd.monthlydataid = md.id
  inner join sim.replicate r on r.id = replicateid
  inner join sim.configuration c on c.id = r.configurationid
where dayselapsed > 7305 and dayselapsed < 7701
  and replicateid in (select id from v_replicates where filename like 'mmc-%-treat-%')
group by replicateid, filename
order by detection, beta