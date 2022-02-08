# MaSim Reporters

The malaria simulation (MaSim) exposes a number of different reporters that can be specified via the command line switch
`-r` and listed via the `--lr` switch. Reporters currently write data either to a text file (CSV or TSV formatted) or to
the database backend. 

*Primary* reporters write extensive amounts of information from the simulation, in contrast, 
*secondary* reporters tend to be limited in either what is reported, or have limitations on their use.  

# Reporter Types

### Database Reporter (DbReporter)
Primary Reporter \
Output: Database
 
The `DbReporter`, short for Database Reporter, uses the PostgreSQL database as the backend and respects the `record_genome_db` 
setting in the configuration file when determining if data genotype data should be stored. If genotype data is stored, 
it is aggregated at the cellular level and can result in large databases.

### Database Reporter by District (DbReporterDistrict)
Primary Reporter \
Output: Database

The `DbReporterDistrict` is a derivative of the `DbReporter`, except genotype data will be aggregated to the district level.

### Genotype Carriers Reporter (GenotypeCarriers)
Specialist Reporter \
Output: Database

The Genotype Carriers Reporter (`GenotypeCarriers`) is a specialist reporter that works in tandem with either of the 
database reporters and expects to be run after all other reporting has been completed. Which can be done by invoking the
reporter as the last option in the switch (e.g., `-r DbReporterDistrict,GenotypeCarriers`). When the reporter 
initializes it will check for the presence of the `genotypecarriers` column in `sim.monthlysitedata` which is where the 
absolute count of genotype carriers will be stored during model executing. Since the reporter is hard coded for the 
first allele mutation on the second locus it will report the count of either 580Y or 561H, depending on model configuration.

### Seasonal Immunity Reporter (SeasonalImmunity)
Specialist Reporter \
Output: CSV File

The Seasonal Immunity Reporter (`SeasonalImmunity`) is a specialist reporter that scans all locations and individuals 
and aggregates summary data at the climatic zone level. This is useful for determining the role that seasonal 
transmission patterns has upon the genotype evolution and immune response to the parasite. The reporter is hard coded 
for the first allele mutation on the second locus, so while it reports 580Y in the output, 561H could be captured, 
depending upon model configuration.

### PRMC Genotype Reporter

Daily or Monthly Reporter \
Output: CSV File

The PRMC genotype reporter is a report that counts all genotypes in the PRMC as well as in population. To use this reporter, use the code snippet as below:

```c++
void MonthlyReporter::initialize(int job_number, std::string path) {
    ...
    auto prmc = Model::CONFIG->prmc();
    std::stringstream ss;
    prmc->printRecombinedGenotypeHeaders(ss);
    ...
}

void MonthlyReporter::monthly_report(){
    ...
    auto prmc = Model::CONFIG->prmc();
    std::stringstream ss;
    prmc->printRecombinedGenotypeStats(ss); /* For counting genotypes in PRMC */
    prmc->printPopulationRecombinedGenotypeStats(ss); /* For counting genotypes in population */
    ...
}
```
For example, if two genotypes `KYY--C1x` and `TYY-Y1x` are introduced 1 location, `printRecombinedGenotypeStats` will output:

```text
    /* For counting genotypes in PRMC */
    0:-1,0:KYY--C1x,0:KYY--Y1x,0:TYY--C1x,0:TYY--Y1x /* This is headers */
    0,49,2,1,48 /* The number of each genotype in PRMC table */
    0,61,1,2,36
    0,56,1,1,42
    0,49,2,4,45
    ...
```
where `0` is `location 1` follows by possible recombined genotypes. Note that there will be genotype `-1` at the beginning of the output to count the number of no recombination in that location. This `-1` is used to mark the lack of recombined genotypes in low prevalence areas.

On the other hand, `printPopulationRecombinedGenotypeStats` will output the number of genotypes in population in each location. There will be no genotype `-1` in this report.

```text
    /* For counting genotypes in population */
    0:KYY--C1x,0:KYY--Y1x,0:TYY--C1x,0:TYY--Y1x /* This is headers */
    527,0,0,511 /* The number of each genotype in population */
    527,0,0,511
    527,0,0,511
    527,0,0,511
    ...
```

This reports also supports daily reports which can be enabled by setting `daily_report` to `true` in the configuration (see `configuration` section).

