# MaSim Reporters

The malaria simulation (MaSim) exposes a number of different reporters that can be specified via the command line switch `-r` and listed via the `--lr` switch. Reporters currently write data either to a text file (CSV or TSV formatted) or to the database backend. 

*Primary* reporters write extensive amounts of information from the simulation, in contrast, *secondary* reporters tend to be limited in either what is reported, or have limitations on their use.  

# Reporter Types

### Database Reporter (DbReporter)
Primary Reporter \
Output: Database
 
The `DbReporter`, short for Database Reporter, uses the PostgreSQL database as the backend and respects the `record_genome_db` setting in the configuration file when determining if data genotype data should be stored. If genotype data is stored, it is aggregated at the cellular level and can result in large databases.

### Database Reporter by District (DbReporterDistrict)
Primary Reporter \
Output: Database

The `DbReporterDistrict` is a derivative of the `DbReporter`, except genotype data will be aggregated to the district level.

### Genotype Carriers Reporter (GenotypeCarriers)
Specialist Reporter \
Output: Database

The Genotype Carriers Reporter (`GenotypeCarriers`) is a specialist reporter that works in tandem with either of the database reporters and expects to be run after all other reporting has been completed. Which can be done by invoking the reporter as the last option in the switch (e.g., `-r DbReporterDistrict,GenotypeCarriers`). When the reporter 
initializes it will check for the presence of the `genotypecarriers` column in `sim.monthlysitedata` which is where the absolute count of genotype carriers will be stored during model executing. Since the reporter is hard coded for the first allele mutation on the second locus it will report the count of either 580Y or 561H, depending on model configuration.

### Monthly Reporter (MonthlyReporter)
Primary Reporter \
Output: TSV File

The Monthly Reporter `MonthlyReporter` is the original reporter used by the simulation to report the simulation status and produces two tab separated values (TSV) file as described below.

### Seasonal Immunity Reporter (SeasonalImmunity)
Specialist Reporter \
Output: CSV File

The Seasonal Immunity Reporter (`SeasonalImmunity`) is a specialist reporter that scans all locations and individuals and aggregates summary data at the climatic zone level. This is useful for determining the role that seasonal transmission patterns has upon the genotype evolution and immune response to the parasite. The reporter is hard coded for the first allele mutation on the second locus, so while it reports 580Y in the output, 561H could be captured, depending upon model configuration.

# Reporter Data Files

## Monthly Reporter
The `MonthlyReporter` generates two data files in tab separated values (TSV) format with group separators indicated by the sentinel value `-1111` where *n* is a zero-indexed location id.

**monthly_reporter_*n*.txt** - Summary data for the model generated at the end of each simulated month.

| Block | Column Number(s) | Description |
| ---|--- | --- |
| Summary | 1 |  Model time, number of days elapsed |
| | 2 | Model time, calendar date as system time |
| | 3 - 5 | Model time, calendar date (Year, Month, Day) |
| | 6 | Seasonal factor |
| | 7 | Treatment coverage, probability to be treated (0 - 1) |
| | 8 | Treatment coverage, probability to be treated (0 - 10) |
| | 9 | Population size |
| | 10 | Group separator |
| EIR and PfRP | 1 + (*n* * 5) | EIR by location per year | 
| | 2 + (*n* * 5) | Group separator |
| | 3 + (*n* * 5) | Blood slide prevalence, PfPR < 5 |
| | 4 + (*n* * 5) | Blood slide prevalence, PfPR 2 - 2 | 
| | 5 + (*n* * 5) | Blood slide prevalence, PfPr all |
| Infections by Location | 1 | Group separator |
| | 2 + *n* | Number of new infections by location |
| Treatments by Location | 1 | Group separator |
| | 2 + *n* | Number of treatments by location |
| Clinical Episodes by Location | 1 | Group separator |
| | 2 + *n* | Number of clinical episodes by location |
| Genotype Frequency | 1 | Group separator |
| | *...* | See genotype frequency discussion

**summary_reporter_*n*.txt** Summary that is generated after the model has completed execution.

| Block | Column Number(s) | Description |
| --- | --- | --- |
| Summary I | 1 | Random seed value |
|  | 2 | Number of locations | 
|  | 3 | *Beta* value |
|  | 4 | Population size |
| EIR and PfRP | 1 + (*n* * 5) | EIR by location per year | 
| | 2 + (*n* * 5) | Group separator |
| | 3 + (*n* * 5) | Blood slide prevalence, PfPR < 5 |
| | 4 + (*n* * 5) | Blood slide prevalence, PfPR 2 - 2 | 
| | 5 + (*n* * 5) | Blood slide prevalence, PfPr all |
| Summary II | 1 | Treatment strategy id |
| | 2 | Percent treatment failures per year | 


## Genotype Frequency
Genotype frequency is a complex entry that follows a similar output structure, but is generated differently depending upon the approach selected.

**Genotype frequency, by weighted number of parasite-positive individuals**
```
FOR EACH location
  FOR EACH state per person
    FOR EACH age class per person
      IF the person has no parasites THEN CONTINUE
      Update the total count
      Count the number of genotypes per person
      Adjust the count for the region
    END
  END
  // Formats the genotype per location as TSV
  FOR EACH genotype
    OUTPUT << weighted value << '\t'
  END
END
OUTPUT << "-1111\t"
// Format the summary genotype results as TSV
FOR EACH genotype
  OUTPUT << weighted value << 't'
END
OUTPUT << "-1111\t"
OUTPUT << Total count
```
