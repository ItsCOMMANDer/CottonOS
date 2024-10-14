# Services

Service files will be in plaintext.
When starting a service, the service file, usually with the file extension .cisv, the service manager will search trough all folders in COTTON_INIT_SERVICE_DIR (which is defined in the cotton init service manager config file)

File "format":

```

# Comment

Name=Service Name
Description=Service Description

Execute="/bin/service_example" --example-flag "test;test2" -e "example"

Logfile=/var/example/log.txt

Requires="Internet.service" "sql_server.service" "something_else.service" # Only start this service if the required services are running


Start=Manual # Possible values are Manual|On-Boot

# For continuous services
# like web servers, data bases, wifi managers etc

On-Fail-Death=Restart # Possible Options are Restart|Northing

# For non-continuous services
# like backups, they dont need to run in the background. They need to make a backup, and end

Schedule=Manual # Start manually
Schedule=0D 2H 30M 15S # Restart every 2 Hours, 30 Minutes and 15 Seconds

TimeoutStopSec=10 # Defaults to 10, specified how long to wait after sinding SIGTERM to send SIGKILL

```