-- Create script for MaSim PostgreSQL database

-- Schema: sim
CREATE SCHEMA sim
    AUTHORIZATION sim;

GRANT ALL ON SCHEMA sim TO sim;

-- Table: sim.monthlydata
CREATE TABLE sim.monthlydata
(
    id integer NOT NULL DEFAULT nextval('sim.monthlydata_id_seq'::regclass),
    runid integer NOT NULL,
    dayselapsed integer NOT NULL,
    modeltime bigint NOT NULL,
    seasonalfactor integer NOT NULL,
    treatmentfailures double precision NOT NULL,
    beta double precision NOT NULL,
    CONSTRAINT monthlydata_pkey PRIMARY KEY (id)
)
WITH (
    OIDS = FALSE
)
TABLESPACE pg_default;

ALTER TABLE sim.monthlydata
    OWNER to sim;
	
-- Table: sim.monthlysitedata
CREATE TABLE sim.monthlysitedata
(
--	id serial NOT NULL,
    monthlydataid integer NOT NULL,
    locationid integer NOT NULL,
    population integer NOT NULL,
    clinicalepisodes integer NOT NULL,
    treatments integer NOT NULL,
    treatmentfailures integer NOT NULL,
    eir double precision NOT NULL,
    pfprunder5 double precision NOT NULL,
    pfpr2to10 double precision NOT NULL,
    pfprall double precision NOT NULL,
    CONSTRAINT monthlysitedata_pkey PRIMARY KEY (monthlydataid, locationid),
    CONSTRAINT "MonhtlyDataId_FK" FOREIGN KEY (monthlydataid)
        REFERENCES sim.monthlydata (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
WITH (
    OIDS = FALSE
)
TABLESPACE pg_default;

ALTER TABLE sim.monthlysitedata
    OWNER to sim;