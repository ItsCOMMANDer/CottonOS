#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <string.h>
#include <linux/magic.h>

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

    // idk if we need these 2:
    //mount("proc", "/proc", "proc", 0, NULL);
    //mount("sysfs", "/sys", "sysfs", 0, NULL);
    
    mount("devtmpfs", "/dev", "devtmpfs", 0, NULL);

    {
        char* filename = calloc(strlen("/dev/disk/by-uuid/") + strlen(bootfs_uuid) + 1, sizeof(char));
        strncpy(filename, "/dev/disk/by-uuid/", strlen("/dev/disk/by-uuid/") + 1);
        strncat(filename, bootfs_uuid, strlen(bootfs_uuid));
        printf("full path: \"%s\"\n", filename);
        if(access(filename, F_OK) == 0) {
            log_info("Bootfs Mount", "Valid bootfs uuid provided");
            //! ERROR?
            reboot(RB_HALT_SYSTEM);
        }
        struct statfs bootfs_stat;
        statfs(filename, &bootfs_stat);
        if(bootfs_stat.f_type == EXT4_SUPER_MAGIC) {
            log_warn("Bootfs Mount", "Bootfs is ext4");
        }
        if(mount(filename, "/bootfs", "ext4", 0, NULL) != 0) {
            log_error("Bootfs Mount", "Erro mounting bootfs"); //! ERRORS HERE!!!
            //! ERROR?
            reboot(RB_HALT_SYSTEM);
        }
    }

        {
        char* filename = calloc(strlen("/dev/disk/by-uuid/") + strlen(rootfs_uuid) + 1, sizeof(char));
        strncpy(filename, "/dev/disk/by-uuid/", strlen("/dev/disk/by-uuid/") + 1);
        strncat(filename, bootfs_uuid, strlen(bootfs_uuid));
        if(access(filename, F_OK) != 0) {
            log_error("rootfs Mount", "Invalid rootfs uuid provided");
            //! ERROR?
            reboot(RB_HALT_SYSTEM);
        } else {
            struct statfs rootfs_stat;
            statfs(filename, &rootfs_stat);
            if(rootfs_stat.f_type != EXT4_SUPER_MAGIC) {
                log_warn("rootfs Mount", "expected rootfs to be ext4");
            }
            if(mount(filename, "/rootfs", "ext4", 0, NULL) != 0) {
                log_error("rootfs Mount", "Erro mounting rootfs");
                //! ERROR?
                reboot(RB_HALT_SYSTEM);
            }
        }
    }

    reboot(RB_HALT_SYSTEM);
}