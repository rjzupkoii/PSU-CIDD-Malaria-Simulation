# Simulation Database Infrastructure

The following document is intended provide a walk through of how to get the Malaria Simulation (MaSim) database up and running. While the document is kept as general as opposed, there may be some differences that you encounter due to your local IT requirements. The following guide assumes that the server is running a clean installation of Ubuntu 18.04 LTS 64-bit with ports 80, 443, and 5432 are open.

# Hardware Requirements

The specific requirements for the server are dependent in part upon the number of instances that will be connecting to it while a simuation is running. However, as a baseline the following is a resonable starting point for a virtual machine:

- 4 CPUs
- 8 GB RAM
- 800 GB primary disk

# Installation

## Installation of PostGreSQL

1. Connect to the server

```bash
ssh [User]@[IP address] -p [port]
```

2. Install the GPG key and repository for PostgreSQL packages

```bash
sudo apt-get install wget ca-certificates
wget --quiet -O - https://www.postgresql.org/media/keys/ACCC4CF8.asc | sudo apt-key add -
sudo sh -c 'echo "deb http://apt.postgresql.org/pub/repos/apt/ `lsb_release -cs`-pgdg main" >> /etc/apt/sources.list.d/pgdg.list'
```

3. Install PostgreSQL

```bash
sudo apt-get update
sudo apt-get install postgresql postgresql-contrib
```

4. Verify connection to PostgreSQL

```bash
sudo -u postgres psql postgres
postgres-# \conninfo
postgres-# \q
```

5. Enable service

```bash
sudo update-rc.d postgresql enable
```

## Installation of pgAdmin

1. Install Apache

```bash
sudo apt install apache2
```

2.  Upgrade Python 3

```bash
sudo apt -y upgrade
sudo apt install -y python3-venv
sudo apt install -y build-essential libssl-dev libffi-dev python3-dev
```

3. Preapre build environments

```bash
mkdir environments
cd environments
python3 -m venv py_env
source py_env/bin/activiate
python -m pip install wheel
deactivate
cd ~
```

4. Prepare for pgAdmin 4 

Note that `[user]` should be your user name, directory ownership will be updated later in the process.

```bash
sudo apt install -y libgmp3-dev libpq-dev libapache2-mod-wsgi-py3
sudo mkdir -p /var/lib/pgadmin4/sessions
sudo mkdir /var/lib/pgadmin4/storage
sudo mkdir /var/log/pgadmin4
sudo chown -R [user]:[user] /var/lib/pgadmin4/
sudo chown -R [user]:[user] /var/log/pgadmin4/
```

5. Install pgAdmin 4 

```bash
cd environments
source py_env/bin/activiate
wget https://ftp.postgresql.org/pub/pgadmin/pgadmin4/v3.4/pip/pgadmin4-3.4-py2.py3-none-any.whl
python -m pip install pgadmin4-3.4-py2.py3-none-any.whl 
```

6. Configure pgAdmin 4

Start by creating the file `py_env/lib/python3.6/site-packages/pgadmin4/config_local.py` and adding the following lines:

```
LOG_FILE = '/var/log/pgadmin4/pgadmin4.log'
SQLITE_PATH = '/var/lib/pgadmin4/pgadmin4.db'
SESSION_DB_PATH = '/var/lib/pgadmin4/sessions'
STORAGE_DIR = '/var/lib/pgadmin4/storage'
SERVER_MODE = True
```

Run the pgAdmin setup script to set the login credentials:

```bash
python py_env/lib/python3.6/site-packages/pgadmin4/setup.py
```

Wrap-up by deactiviating the enviornemtn and setting directory ownerhsip:

```bash
deactivate
sudo chown -R www-data:www-data /var/lib/pgadmin4/
sudo chown -R www-data:www-data /var/log/pgadmin4/
```

## Configuration of Apache for pgAdmin
