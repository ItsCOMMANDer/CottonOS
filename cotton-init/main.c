#include <blkid/blkid.h>

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <string.h>
#include <linux/magic.h>
#include <errno.h>
#include <dirent.h> 

#include "util.h"
#include "log.h"

#include <sys/reboot.h>
#include <sys/mount.h>
#include <sys/statfs.h>
#include <sys/vfs.h>

int main(int argc, char* argv[]) {
    if(getpid() != 1) {
        printf("%s must be run as PID 1\n", argv[0]);
        return -1;
    }

    log_info("INIT VER", "VERSION 0.0.0-4");

    log_error("Log Test", "ERROR Visible");
    log_warn("Log Test", "WARN Visible");
    log_info("Log Test", "INFO Visible");
    log_debug("Log Test", "DEBUG Visible");

    log_info("Parameter Info", "Got %i parameters", argc);
    for(int i = 0; i < argc; i++) {
        log_info("Parameter Info", "Parameter %i: \"%s\"", i, argv[i]);
    }

/*
for me: kernel args 101:

"*kernel args* -- *init args*"

-v / --verbosity : sets verbosity level of the logging api
-b / --boot-fs : uuid for bootfs
-r / --root-fs : uuid for rootfs

*/

    char* rootfs_uuid = NULL;
    char* bootfs_uuid = NULL;

    for(int i = 1; i < argc; i++) {
        if((strncmp(argv[i], "-v", strlen(argv[i])) == 0  || strncmp(argv[i], "--verbosity", strlen(argv[i])) == 0)) {
            if(argc - 1 < i + 1) {
                log_warn("Parameter Parsing", "Skipping parsing for last parameter.");
                break;
            }

            if(!isNumber(argv[++i])) {
                log_error("Parameter Parsing", "Parameter for \"verbosity\" has to be a number");
            }

            log_setLevel(atoi(argv[i]));

            continue;
        }

        if(((strncmp(argv[i], "-b", strlen(argv[1])) == 0  || strncmp(argv[i], "--bootfs", strlen(argv[1])) == 0)) && bootfs_uuid == NULL) {
            if(argc - 1 < i + 1) {
                log_error("Parameter Parsing", "Cannon parse bootfs parameter.");
                //! ERROR?
                break;
            }

            bootfs_uuid = calloc(strlen(argv[++i]), sizeof(char));
            strncpy(bootfs_uuid, argv[i], strlen(argv[i]));

            continue;
        }

        if(((strncmp(argv[i], "-r", strlen(argv[1])) == 0  || strncmp(argv[i], "--rootfs", strlen(argv[1])) == 0)) && rootfs_uuid == NULL) {
            if(argc - 1 < i + 1) {
                log_error("Parameter Parsing", "Cannon parse rootfs parameter.");
                //! ERROR?
                break;
            }

            rootfs_uuid = calloc(strlen(argv[++i]), sizeof(char));
            strncpy(rootfs_uuid, argv[i], strlen(argv[i]));

            continue;
        }

    }

    log_info("Parameter Parsing", "Log level set to %i", log_getLevel());

    if(rootfs_uuid == NULL) {
        log_error("Parameter Parsing", "No rootfs uuid provided");
        reboot(RB_HALT_SYSTEM);
    }

    if(bootfs_uuid == NULL) {
        log_error("Parameter Parsing", "No bootfs uuid provided");
        reboot(RB_HALT_SYSTEM);
    }

    log_info("Parameter Parsing", "Done parsing parameters");

    {
        int error;
        if((error = mount("devtmpfs", "/dev", "devtmpfs", 0, NULL)) != 0) {
            log_error("DEVFS Mounting", "Failed to mount devfs, code : %i", error);
            //! ERROR?
        }
    }

    log_info("DEVFS Mounting", "Mounted DEVFS");

       {
        int error;
        if((error = mount("sysfs", "/sys", "sysfs", 0, NULL)) != 0) {
            log_error("SYSFS Mounting", "Failed to mount sysfs, code : %i", error);
            //! ERROR?
        }
    }

    log_info("SYSFS Mounting", "Mounted SYSFS");

    {
        int error;
        if((error = mount("proc", "/proc", "proc", 0, NULL)) != 0) {
            log_error("PROCFS Mounting", "Failed to mount procfs, code : %i", error);
            //! ERROR?
        }
    }

    log_info("PROCFS Mounting", "Mounted PROCFS");

    {
        blkid_cache cache;
        blkid_dev dev;
        blkid_probe probe;
        blkid_dev_iterate iterate;
        const char* blkdev_name;
        const char* blkdev_uuid;

        int error;

        if((error = blkid_get_cache(&cache, NULL)) < 0) {
            log_error("BLKID", "BLKID failed to initilize blkid cache with code %i and errno %i", error, errno);
            log_error("BLKID", "errno string: \"%s\"", strerror(errno));
            reboot(RB_HALT_SYSTEM);
            //! ERROR?
        }

        log_info("BLKID", "initilized BLKID cache");

        error = 0;

        if((error = blkid_probe_all(cache)) < 0) {
            log_error("BLKID", "BLKID failed to probe all devices with code %i and errno %i", error, errno);
            log_error("BLKID", "errno string: \"%s\"", strerror(errno));
            reboot(RB_HALT_SYSTEM);
            //! ERROR?
        }

        log_info("BLKID", "probed all (devices?)");

        iterate = blkid_dev_iterate_begin(cache);
        while(blkid_dev_next(iterate, &dev) == 0) {
            blkdev_name = blkid_dev_devname(dev);
            probe = blkid_new_probe_from_filename(blkdev_name);
            if(probe == NULL) {
                log_error("BLKID", "Failed to craete probe for device \"%s\" with errno %i", blkdev_name, errno);
                log_error("BLKID", "errno string: \"%s\"", strerror(errno));
                continue;
                //! ERROR?
                reboot(RB_HALT_SYSTEM);
            }

            error = 0;
            if((error = blkid_do_probe(probe)) != 0) {
                log_error("BLKID", "Failed to do the probeing for device \"%s\" with errno %i", blkdev_name, errno);
                log_error("BLKID", "errno string: \"%s\"", strerror(errno));
                blkid_free_probe(probe);
                continue;
                //! ERROR?
                reboot(RB_HALT_SYSTEM);
            }

            if(blkid_probe_lookup_value(probe, "UUID", &blkdev_uuid, NULL) == 0) {
                //DO COMPARISON
                log_info("UUID DISCOVERD", "DEV BLOCK: \"%s\" with UUID \"%s\"", blkdev_name, blkdev_uuid);
            } else {
                log_info("BLKID", "Device \"%s\" does not have a uuid (on disk with mbr?)", blkdev_name);
            }

            blkid_free_probe(probe);
        }

        blkid_dev_iterate_end(iterate);
        blkid_put_cache(cache);
    }

    
    log_error("INIT END", "AT THE END OF INIT, IDK, I DONT THINK THIS SHOULD HAVE HAPPEND LOL");


    reboot(RB_HALT_SYSTEM);
}