-- Create script for MaSim PostgreSQL database

-- Schema: sim
CREATE SCHEMA sim
    AUTHORIZATION sim;

GRANT ALL ON SCHEMA sim TO sim;

-- Table: sim.study
CREATE TABLE sim.study
(
    id integer NOT NULL DEFAULT nextval('sim.study_id_seq'::regclass),
    name character varying COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT study_pkey PRIMARY KEY (id)
)
WITH (
    OIDS = FALSE
)
TABLESPACE pg_default;

ALTER TABLE sim.study
    OWNER to sim;

-- Table: sim.replicate
CREATE TABLE sim.replicate
(
    id integer NOT NULL DEFAULT nextval('sim.replicate_id_seq'::regclass),
    studyid integer NOT NULL,
    seed bigint NOT NULL,
    status integer NOT NULL,
    CONSTRAINT replicate_pkey PRIMARY KEY (id),
    CONSTRAINT replicate_studyid_fk FOREIGN KEY (studyid)
        REFERENCES sim.study (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
WITH (
    OIDS = FALSE
)
TABLESPACE pg_default;

ALTER TABLE sim.replicate
    OWNER to sim;

CREATE INDEX fki_replicate_studyid_fk
    ON sim.replicate USING btree
    (studyid)
    TABLESPACE pg_default;

-- Table: sim.strategy
CREATE TABLE sim.strategy
(
    id integer NOT NULL DEFAULT nextval('sim.strategy_id_seq'::regclass),
    name character varying COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT strategy_pkey PRIMARY KEY (id)
)
WITH (
    OIDS = FALSE
)
TABLESPACE pg_default;

ALTER TABLE sim.strategy
    OWNER to postgres;

-- Table: sim.configuration
CREATE TABLE sim.configuration
(
    id integer NOT NULL DEFAULT nextval('sim.configuration_id_seq'::regclass),
    studyid integer NOT NULL,
    strategyid integer NOT NULL,
    CONSTRAINT configuration_pkey PRIMARY KEY (id),
    CONSTRAINT configuration_strategyid_fk FOREIGN KEY (strategyid)
        REFERENCES sim.strategy (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION,
    CONSTRAINT configuration_studyid_fkey FOREIGN KEY (studyid)
        REFERENCES sim.study (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
WITH (
    OIDS = FALSE
)
TABLESPACE pg_default;

ALTER TABLE sim.configuration
    OWNER to sim;

CREATE INDEX fki_configuration_strategyid_fk
    ON sim.configuration USING btree
    (strategyid)
    TABLESPACE pg_default;

CREATE INDEX fki_configurationstudyid_fk
    ON sim.configuration USING btree
    (studyid)
    TABLESPACE pg_default;

-- Table: sim.location
CREATE TABLE sim.location
(
    id integer NOT NULL DEFAULT nextval('sim.location_id_seq'::regclass),
    configurationid integer NOT NULL,
    index integer NOT NULL,
    beta double precision NOT NULL,
    CONSTRAINT location_pkey PRIMARY KEY (id),
    CONSTRAINT location_index_unique UNIQUE (id, configurationid, index)
,
    CONSTRAINT location_configurationid_fk FOREIGN KEY (configurationid)
        REFERENCES sim.configuration (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
WITH (
    OIDS = FALSE
)
TABLESPACE pg_default;

ALTER TABLE sim.location
    OWNER to sim;

CREATE INDEX fki_location_configurationid_fk
    ON sim.location USING btree
    (configurationid)
    TABLESPACE pg_default;

-- Table: sim.monthlydata
CREATE TABLE sim.monthlydata
(
    id integer NOT NULL DEFAULT nextval('sim.monthlydata_id_seq'::regclass),
    replicateid integer NOT NULL,
    dayselapsed integer NOT NULL,
    modeltime bigint NOT NULL,
    seasonalfactor integer NOT NULL,
    treatmentfailures double precision NOT NULL,
    beta double precision NOT NULL,
    CONSTRAINT monthlydata_pkey PRIMARY KEY (id),
    CONSTRAINT monthlydata_replicateid_fk FOREIGN KEY (replicateid)
        REFERENCES sim.replicate (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
WITH (
    OIDS = FALSE
)
TABLESPACE pg_default;

ALTER TABLE sim.monthlydata
    OWNER to sim;

CREATE INDEX fki_monthlydata_replicateid_fk
    ON sim.monthlydata USING btree
    (replicateid)
    TABLESPACE pg_default;
	
-- Table: sim.monthlygenomedata
CREATE TABLE sim.monthlygenomedata
(
    monthlydataid integer NOT NULL,
    locationid integer NOT NULL,
    genomeid integer NOT NULL,
    occurrences integer NOT NULL,
    weightedfrequency double precision NOT NULL,
    CONSTRAINT monthlygenomedata_pkey PRIMARY KEY (monthlydataid, genomeid, locationid),
    CONSTRAINT monthlygenomedata_monthlydataid_fk FOREIGN KEY (monthlydataid)
        REFERENCES sim.monthlydata (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
WITH (
    OIDS = FALSE
)
TABLESPACE pg_default;

ALTER TABLE sim.monthlygenomedata
    OWNER to sim;
	
CREATE INDEX fki_monthlygenomedata_monthlydataid_fk
    ON sim.monthlygenomedata USING btree
    (monthlydataid)
    TABLESPACE pg_default;
	
-- Table: sim.monthlysitedata
CREATE TABLE sim.monthlysitedata
(
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