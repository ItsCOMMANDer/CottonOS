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
    char* bootfs_uuid = "ea243254-7e01-4bd7-83f4-0b13bc93d3ca";

    {
        char* filename = calloc(strlen("/dev/disk/by-uuid/") + strlen(bootfs_uuid) + 1, sizeof(char));
        strncpy(filename, "/dev/disk/by-uuid/", strlen("/dev/disk/by-uuid/"));
        strncat(filename, bootfs_uuid, strlen(bootfs_uuid));
        printf("full path: \"%s\"\n", filename);
        if(access(filename, F_OK) == 0) {
            log_info("Bootfs Mount", "Valid bootfs uuid provided");
            //! ERROR?
        }
        struct statfs bootfs_stat;
        statfs(filename, &bootfs_stat);
        if(bootfs_stat.f_type == EXT4_SUPER_MAGIC) {
            log_warn("Bootfs Mount", "Bootfs is ext4");
        }
        //if(mount(filename, "/bootfs", "ext4", 0, NULL) != 0) {
        //    log_error("Bootfs Mount", "Erro mounting bootfs");
        //}
        //! ERROR?
    }
}