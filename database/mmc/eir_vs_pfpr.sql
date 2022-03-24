-- No treatment
select replicateid,
  cast(substr(filename, 8, 4) as float) as transmission,
  cast(replace(substr(filename, 13), '.yml', '') as float) as beta,
  avg(eir) as eir, 
  avg(pfprunder5) as pfprunder5, 
  avg(pfpr2to10) as pfpr2to10, 
  avg(pfprall) as pfprall
from sim.monthlydata md 
  inner join sim.monthlysitedata msd on msd.monthlydataid = md.id
  inner join sim.replicate r on r.id = replicateid
  inner join sim.configuration c on c.id = r.configurationid
where dayselapsed > 7305 and dayselapsed < 7701
  and replicateid in (select id from v_replicates where filename like 'mmc-pr-%')
group by replicateid, filename
order by transmission, beta

-- With treatment
select replicateid,
  cast(substr(filename, 14, 4) as float) as transmission,
  cast(replace(substr(filename, 19), '.yml', '') as float) as beta,
  avg(eir) as eir, 
  avg(pfprunder5) as pfprunder5, 
  avg(pfpr2to10) as pfpr2to10, 
  avg(pfprall) as pfprall
from sim.monthlydata md 
  inner join sim.monthlysitedata msd on msd.monthlydataid = md.id
  inner join sim.replicate r on r.id = replicateid
  inner join sim.configuration c on c.id = r.configurationid
where dayselapsed > 7305 and dayselapsed < 7701
  and replicateid in (select id from v_replicates where filename like 'mmc-pr-treat%')
group by replicateid, filename
order by transmission, beta