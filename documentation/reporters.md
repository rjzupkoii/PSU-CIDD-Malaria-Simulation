# MaSim Reporters

The malaria simulation (MaSim) exposes a number of different reporters that can be specified via the command line switch
`-r` and listed via the `--lr` switch. Reporters currently write data either to a text file (CSV or TSV formatted) or to
the database backend. 

*Primary* reporters write extensive amounts of information from the simulation, in contrast, 
*secondary* reporters tend to be limited in either what is reported, or have limitations on their use.  

# Reporter Types

### DbReporter
Primary Reporter \
Output: Database
 
The `DbReporter`, short for Database Reporter, uses the PostgreSQL database as the backend and respects the `record_genome_db` 
setting in the configuration file when determining if data genotype data should be stored. If genotype data is stored, 
it is aggregated at the cellular level and can result in large databases.

### DbReporterDistrict
Primary Reporter \
Output: Database

The `DbReporterDistrict` is a derivative of the `DbReporter`, except genotype data will be aggregated to the district level. 