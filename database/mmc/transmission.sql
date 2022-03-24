select replicateid,
  cast(replace(substr(filename, 21), '.yml', '') as float) as transmission,
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
	  select id from v_replicates where filename like 'mmc2-%')
group by replicateid, filename
order by transmission