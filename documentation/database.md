# Simulation Database Infrastructure

The following document is intended provide a walk through of how to get the Malaria Simulation (MaSim) database up and running. While the document is kept as general as opposed, there may be some differences that you encounter due to your local IT requirements.

# Hardware Requirements

# Installation


## Installation of PostGreSQL

1. Connect to the server

ssh [User]@[IP address] -p [port]

2. Install the GPG key and repository for PostgreSQL packages

sudo apt-get install wget ca-certificates
wget --quiet -O - https://www.postgresql.org/media/keys/ACCC4CF8.asc | sudo apt-key add -
sudo sh -c 'echo "deb http://apt.postgresql.org/pub/repos/apt/ `lsb_release -cs`-pgdg main" >> /etc/apt/sources.list.d/pgdg.list'

3. Install PostgreSQL

sudo apt-get update
sudo apt-get install postgresql postgresql-contrib

4. Verify connection to PostgreSQL

sudo -u postgres psql postgres

postgres-# \conninfo
postgres-# \q

5. Enable service

sudo update-rc.d postgresql enable