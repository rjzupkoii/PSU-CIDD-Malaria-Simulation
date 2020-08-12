
CREATE OR REPLACE PROCEDURE public.delete_replicate(
	replicate_id integer)
LANGUAGE 'plpgsql'

AS $BODY$
DECLARE
  ENDTIME TIMESTAMP WITH TIME ZONE;
BEGIN
  -- Check to make sure an endtime is not set, implying that there is data
  SELECT sim.replicate.endtime INTO ENDTIME FROM sim.replicate where id = REPLICATE_ID;
  IF ENDTIME IS NOT NULL THEN
  	RAISE NOTICE 'Replicate appears to have a complete run, exiting.';
	RETURN;
  ELSE
    RAISE NOTICE 'Culling replicate %', REPLICATE_ID;
  END IF;

  -- Delete the genome data
  DELETE FROM sim.monthlygenomedata WHERE monthlydataid IN (
	  SELECT id FROM sim.monthlydata WHERE replicateid = REPLICATE_ID);

  -- Delete the site data
  DELETE FROM sim.monthlysitedata WHERE monthlydataid IN (
	  SELECT id FROM sim.monthlydata WHERE replicateid = REPLICATE_ID);

  -- Delete the monthly data
  DELETE FROM sim.monthlydata WHERE replicateid = REPLICATE_ID;
  
  -- Delete any movement enteries
  DELETE FROM sim.districtmovement WHERE replicateid = REPLICATE_ID;
  DELETE FROM sim.movement WHERE replicateid = REPLICATE_ID;
  
  -- Delete the replicate entry
  DELETE FROM sim.replicate WHERE id = REPLICATE_ID;
  
  -- Report complete
  RAISE NOTICE 'Complete';
END $BODY$;

CREATE OR REPLACE PROCEDURE public.delete_study(
	study_id integer)
LANGUAGE 'plpgsql'

AS $BODY$
DECLARE
record RECORD;
BEGIN
	-- Delete the replicate
    FOR record IN select distinct r.id from sim.configuration c inner join sim.replicate r on r.configurationid = c.id where studyid = study_id LOOP
        CALL delete_replicate(record.id);
    END LOOP;
	
	-- Delete the configuration
	DELETE FROM sim.location WHERE configurationid IN (
		SELECT id FROM sim.configuration WHERE studyid = study_id);
	DELETE FROM sim.configuration WHERE studyid = study_id;
	
	-- Delete the study
	DELETE FROM sim.notes WHERE studyid = study_id;
	DELETE FROM sim.study WHERE id = study_id;
	
	-- Report complete
  	RAISE NOTICE 'Complete';
	
END $BODY$;