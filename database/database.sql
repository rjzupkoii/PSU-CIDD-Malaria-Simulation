--
-- MaSim database creation script
--

--
-- Settings for this script
--
SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- Create the Schema, set ownership
--
CREATE SCHEMA sim;
ALTER SCHEMA sim OWNER TO sim;
SET default_tablespace = '';
SET default_with_oids = false;

-- 
-- Create Sequences
--
CREATE SEQUENCE sim.configuration_id_seq
    INCREMENT 1
    START 1
    MINVALUE 1
    MAXVALUE 2147483647
    CACHE 1;

ALTER SEQUENCE sim.configuration_id_seq OWNER TO sim;

CREATE SEQUENCE sim.districtmovement_id_seq
    INCREMENT 1
    START 1
    MINVALUE 1
    MAXVALUE 2147483647
    CACHE 1;

ALTER SEQUENCE sim.districtmovement_id_seq OWNER TO sim;

CREATE SEQUENCE sim.location_id_seq
    INCREMENT 1
    START 1
    MINVALUE 1
    MAXVALUE 2147483647
    CACHE 1;

ALTER SEQUENCE sim.location_id_seq OWNER TO sim;

CREATE SEQUENCE sim.monthlydata_id_seq
    INCREMENT 1
    START 1
    MINVALUE 1
    MAXVALUE 2147483647
    CACHE 1;

ALTER SEQUENCE sim.monthlydata_id_seq OWNER TO sim;

CREATE SEQUENCE sim.movement_id_seq
    INCREMENT 1
    START 1
    MINVALUE 1
    MAXVALUE 2147483647
    CACHE 1;

ALTER SEQUENCE sim.movement_id_seq OWNER TO sim;

CREATE SEQUENCE sim.notes_id_seq
    INCREMENT 1
    START 1
    MINVALUE 1
    MAXVALUE 2147483647
    CACHE 1;

ALTER SEQUENCE sim.notes_id_seq OWNER TO sim;

CREATE SEQUENCE sim.replicate_id_seq
    INCREMENT 1
    START 1
    MINVALUE 1
    MAXVALUE 2147483647
    CACHE 1;

ALTER SEQUENCE sim.replicate_id_seq OWNER TO sim;

CREATE SEQUENCE sim.study_id_seq
    INCREMENT 1
    START 1
    MINVALUE 1
    MAXVALUE 2147483647
    CACHE 1;

ALTER SEQUENCE sim.study_id_seq OWNER TO sim;

--
-- Create Tables
--
CREATE TABLE sim.study
(
    id integer NOT NULL DEFAULT nextval('sim.study_id_seq'::regclass),
    name character varying COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT study_pkey PRIMARY KEY (id),
    CONSTRAINT study_name_key UNIQUE (name)
)
WITH ( OIDS = FALSE )
TABLESPACE pg_default;

ALTER TABLE sim.study OWNER to sim;

CREATE TABLE sim.notes
(
    id integer NOT NULL DEFAULT nextval('sim.notes_id_seq'::regclass),
    data character varying COLLATE pg_catalog."default" NOT NULL,
    "user" character varying COLLATE pg_catalog."default" NOT NULL,
    date timestamp with time zone NOT NULL,
    studyid integer NOT NULL,
    CONSTRAINT notes_pkey PRIMARY KEY (id),
    CONSTRAINT notes_studyid_fk FOREIGN KEY (studyid)
        REFERENCES sim.study (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
        NOT VALID
)
WITH ( OIDS = FALSE )
TABLESPACE pg_default;

ALTER TABLE sim.notes  OWNER to sim;

CREATE TABLE sim.configuration
(
    id integer NOT NULL DEFAULT nextval('sim.configuration_id_seq'::regclass),
    yaml character varying COLLATE pg_catalog."default" NOT NULL,
    md5 character varying COLLATE pg_catalog."default" NOT NULL,
    name character varying COLLATE pg_catalog."default",
    notes character varying COLLATE pg_catalog."default",
    filename character varying COLLATE pg_catalog."default",
    ncols integer NOT NULL DEFAULT '-1'::integer,
    nrows integer NOT NULL DEFAULT '-1'::integer,
    xllcorner double precision NOT NULL DEFAULT 0,
    yllcorner double precision NOT NULL DEFAULT 0,
    cellsize integer NOT NULL DEFAULT '-1'::integer,
    studyid integer,
    CONSTRAINT configuration_pkey PRIMARY KEY (id),
    CONSTRAINT configuration_studyid_fk FOREIGN KEY (studyid)
        REFERENCES sim.study (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
        NOT VALID
)
WITH ( OIDS = FALSE )
TABLESPACE pg_default;

ALTER TABLE sim.configuration OWNER to sim;

CREATE TABLE sim.replicate
(
    id integer NOT NULL DEFAULT nextval('sim.replicate_id_seq'::regclass),
    configurationid integer NOT NULL,
    seed bigint NOT NULL,
    starttime timestamp with time zone NOT NULL,
    endtime timestamp with time zone,
    movement character(1) COLLATE pg_catalog."default",
    CONSTRAINT replicate_pkey PRIMARY KEY (id),
    CONSTRAINT replicate_configurationid_fk FOREIGN KEY (configurationid)
        REFERENCES sim.configuration (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
WITH ( OIDS = FALSE )
TABLESPACE pg_default;

ALTER TABLE sim.replicate OWNER to sim;

CREATE TABLE sim.districtmovement
(
    id integer NOT NULL DEFAULT nextval('sim.districtmovement_id_seq'::regclass),
    replicateid integer NOT NULL,
    timestep integer NOT NULL,
    count integer NOT NULL,
    source integer NOT NULL,
    destination integer NOT NULL,
    CONSTRAINT districtmovement_pkey PRIMARY KEY (id),
    CONSTRAINT districtmovement_replicateid_fk FOREIGN KEY (replicateid)
        REFERENCES sim.replicate (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
WITH ( OIDS = FALSE )
TABLESPACE pg_default;

ALTER TABLE sim.districtmovement OWNER to sim;

CREATE TABLE sim.location
(
    id integer NOT NULL DEFAULT nextval('sim.location_id_seq'::regclass),
    configurationid integer NOT NULL,
    index integer NOT NULL,
    x integer NOT NULL,
    y integer NOT NULL,
    beta double precision NOT NULL,
    district integer,
    CONSTRAINT location_pkey PRIMARY KEY (id),
    CONSTRAINT location_index_unique UNIQUE (id, configurationid, index),
    CONSTRAINT location_configurationid_fk FOREIGN KEY (configurationid)
        REFERENCES sim.configuration (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
WITH ( OIDS = FALSE )
TABLESPACE pg_default;

ALTER TABLE sim.location OWNER to sim;

CREATE TABLE sim.monthlydata
(
    id integer NOT NULL DEFAULT nextval('sim.monthlydata_id_seq'::regclass),
    replicateid integer NOT NULL,
    dayselapsed integer NOT NULL,
    modeltime bigint NOT NULL,
    seasonalfactor integer NOT NULL,
    treatmentfailures double precision NOT NULL,
    beta double precision NOT NULL,
    entrytime timestamp with time zone,
    CONSTRAINT monthlydata_pkey PRIMARY KEY (id),
    CONSTRAINT monthlydata_replicateid_fk FOREIGN KEY (replicateid)
        REFERENCES sim.replicate (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
WITH ( OIDS = FALSE )
TABLESPACE pg_default;

ALTER TABLE sim.monthlydata OWNER to sim;

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
    infectedindividuals integer NOT NULL,
    CONSTRAINT monthlysitedata_pkey PRIMARY KEY (monthlydataid, locationid),
    CONSTRAINT "monthlydataid_fk" FOREIGN KEY (monthlydataid)
        REFERENCES sim.monthlydata (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
WITH ( OIDS = FALSE )
TABLESPACE pg_default;

ALTER TABLE sim.monthlysitedata OWNER to sim;

CREATE TABLE sim.movement
(
    id bigint NOT NULL DEFAULT nextval('sim.movement_id_seq'::regclass),
    replicateid integer NOT NULL,
    timestep integer NOT NULL,
    individualid integer NOT NULL,
    source integer NOT NULL,
    destination integer NOT NULL,
    CONSTRAINT movement_pkey PRIMARY KEY (id),
    CONSTRAINT movement_replicateid_fk FOREIGN KEY (replicateid)
        REFERENCES sim.replicate (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
WITH ( OIDS = FALSE )
TABLESPACE pg_default;

ALTER TABLE sim.movement OWNER to sim;

CREATE TABLE sim.genotype
(
    id integer NOT NULL,
    name character varying COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT genotype_pkey PRIMARY KEY (id)
)
WITH ( OIDS = FALSE )
TABLESPACE pg_default;

ALTER TABLE sim.genotype OWNER to sim;

CREATE TABLE sim.monthlygenomedata
(
    monthlydataid integer NOT NULL,
    locationid integer NOT NULL,
    genomeid integer NOT NULL,
    occurrences integer NOT NULL,
    clinicaloccurrences integer NOT NULL,
    occurrences0to5 integer NOT NULL,
    occurrences2to10 integer NOT NULL,
    weightedoccurrences double precision NOT NULL,
    weightedfrequency double precision NOT NULL,
    CONSTRAINT monthlygenomedata_pkey PRIMARY KEY (monthlydataid, genomeid, locationid),
    CONSTRAINT monthlygenomedata_genotypeid_fk FOREIGN KEY (genomeid)
        REFERENCES sim.genotype (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION,
    CONSTRAINT monthlygenomedata_monthlydataid_fk FOREIGN KEY (monthlydataid)
        REFERENCES sim.monthlydata (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE NO ACTION
)
WITH ( OIDS = FALSE )
TABLESPACE pg_default;

ALTER TABLE sim.monthlygenomedata OWNER to sim;

--
-- Create Releationships
--
CREATE INDEX fki_fk_configuration_studyid
    ON sim.configuration USING btree
    (studyid)
    TABLESPACE pg_default;

CREATE INDEX fki_districtmovement_replicateid
    ON sim.districtmovement USING btree
    (replicateid)
    TABLESPACE pg_default;

CREATE INDEX fki_location_configurationid
    ON sim.location USING btree
    (configurationid)
    TABLESPACE pg_default;

CREATE INDEX fki_monthlydata_replicateid
    ON sim.monthlydata USING hash
    (replicateid)
    TABLESPACE pg_default;

CREATE INDEX fki_movement_replicateid
    ON sim.movement USING hash
    (replicateid)
    TABLESPACE pg_default;

CREATE INDEX fki_notes_studyid
    ON sim.notes USING btree
    (studyid)
    TABLESPACE pg_default;

CREATE INDEX fki_replicate_configurationid
    ON sim.replicate USING btree
    (configurationid)
    TABLESPACE pg_default;    