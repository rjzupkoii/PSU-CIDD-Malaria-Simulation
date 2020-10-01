
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

  -- Store the indicies as a temporary table
  CREATE TEMP TABLE monthlydataidex AS
  SELECT id FROM sim.monthlydata WHERE replicateid = REPLICATE_ID;

  -- Delete the genome data
  DELETE FROM sim.monthlygenomedata WHERE monthlydataid IN (SELECT id FROM monthlydataidex);

  -- Delete the site data
  DELETE FROM sim.monthlysitedata WHERE monthlydataid IN (SELECT id FROM monthlydataidex);

  -- Delete the monthly data
  DELETE FROM sim.monthlydata WHERE replicateid = REPLICATE_ID;
  
  -- Delete any movement enteries
  DELETE FROM sim.districtmovement WHERE replicateid = REPLICATE_ID;
  DELETE FROM sim.movement WHERE replicateid = REPLICATE_ID;
  
  -- Delete the replicate entry
  DELETE FROM sim.replicate WHERE id = REPLICATE_ID;
  
  -- Drop the temporary table
  DROP TABLE monthlydataidex;  
  
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
	-- Delete the replicates
    FOR record IN select distinct r.id from sim.configuration c inner join sim.replicate r on r.configurationid = c.id where studyid = STUDY_ID LOOP
        CALL delete_replicate(record.id, true);
    END LOOP;
	
  	-- Store the indicies as a temporary table
	CREATE TEMP TABLE configurationindex AS
	SELECT id FROM sim.configuration WHERE studyid = STUDY_ID;

	-- Delete the configurations
	DELETE FROM sim.location WHERE configurationid IN (SELECT id FROM configurationindex);
	DELETE FROM sim.configuration WHERE studyid = STUDY_ID;
		
	-- Delete the study
	DELETE FROM sim.notes WHERE studyid = STUDY_ID;
	DELETE FROM sim.study WHERE id = STUDY_ID;
	
	-- Drop the temporary table
  	DROP TABLE configurationindex;  
	
	-- Report complete
  	RAISE NOTICE 'Complete';
	
END $BODY$;