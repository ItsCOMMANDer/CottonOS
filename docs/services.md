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

Requires="Internet.service" "sql_server.service" "something_else.service"


Start=Manual # Possible values are Manual|On-Boot

# For continuous services
# like web servers, data bases, wifi managers etc

On-Fail-Death=Restart # Possible Options are Restart|Northing

# For non-continuous services
# like backups, they dont need to run in the background. They need to make a backup, and end

Schedule=Manual
# Schedule=Manual - Restart manually
# Schedule=Hourly 2 - Restart ever 2 hours after start of the service -> service started 14:30, next start 16:30
# Schedule=Daily 3 - every 3 days after the start of the service -> service started 25.04.2024 20:00, next start 27.04.2024 20:00
# Schedule=Monthly 1 - every month after the start of the service started -> started 06.03.2024 12:00, next start on 06.04.2024 12:00

# Schedule=Time 00:00 08:00 16:00 - start at 00:00, 08:00 and 16:00 (0 am, 8 am and 4 pm for Americans)
# Schedule=Date 01.XX.XX 10.XX.XX 15.XX.XX 25.XX.XX - every day on the 1st, 10th, 15th and 25th, may be combined with "Schedule=Time ..." 

```