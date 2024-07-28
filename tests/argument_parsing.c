#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <string.h>

#include "util.h"
#include "log.h"

#include <sys/reboot.h>
#include <sys/mount.h>
#include <sys/statfs.h>
#include <sys/vfs.h>

int main(int argc, char* argv[]) {
/*

-v / --verbosity : sets verbosity level of the logging api
-b / --boot-fs : uuid for bootfs
-r / --root-fs : uuid for rootfs

*/

    char* rootfs_uuid = NULL;
    char* bootfs_uuid = NULL;

    uint64_t error;

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

    if(rootfs_uuid == NULL) {
        printf("ROOTFS UNSET\n");
        return -1;
    }

    if(bootfs_uuid == NULL) {
        printf("BOOTFS UNSET\n");
        return -1;
    }

    printf("BOOTFS UUID: \"%s\"\nROOTFS UUID: \"%s\"\nLog Level: %d\n", bootfs_uuid, rootfs_uuid, log_getLevel());

    return 0;
}