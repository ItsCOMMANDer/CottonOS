//INCLUDES

// External Library includes
#include <blkid/blkid.h>

// Standard library includes
#include <stdio.h>
#include <stdlib.h>

// other startard library includes
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <fcntl.h>

// local headers include
#include "config.h"
#include "util.h"
#include "log.h"
#include "fs.h"

// linux include
#include <linux/magic.h>

// system includes
#include <sys/syscall.h>
#include <sys/reboot.h>
#include <sys/mount.h>
#include <sys/statfs.h>
#include <sys/vfs.h>
#include <sys/utsname.h>

#include <sys/wait.h>


// idk why math.h doesnt have this
#define max(x,y) (x > y ? x : y)


// DEBUG STUFF

#include <dirent.h>
void list_directory(const char *path) {
    struct dirent *entry;
    DIR *dp = opendir(path);

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        // Skip the "." and ".." entries
        if (entry->d_name[0] == '.') {
            continue;
        }
        printf("%s\n", entry->d_name);
    }

    if (closedir(dp) == -1) {
        perror("closedir");
    }
}



int main(int argc, char* argv[]) {
	// check if the file is ran as init process
	if(getpid() != 1) {
		printf("%s must be run as PID 1\n", argv[0]);
		return -1;
	}

	// log the version, i used this to see if qemu actually used the updates version, i have since found a reliable way of ensuring that qemu uses the updated initrd
	log_info("INIT VER", "VERSION 0.0.0-10dev");

	// test for visibility of different log levels
	log_error("Log Test", "ERROR Visible");
	log_warn("Log Test", "WARN Visible");
	log_info("Log Test", "INFO Visible");
	log_debug("Log Test", "DEBUG Visible");

	// print some infos about provided commandline parameters
	log_info("Parameter Info", "Got %i parameters", argc);
	for(int i = 0; i < argc; i++) {
		log_info("Parameter Info", "Parameter %i: \"%s\"", i, argv[i]);
	}

/*
for me: kernel args 101:

"*kernel args* -- *init args*"

-verbosity : sets verbosity level of the logging api

-bootfs-uuid : uuid for bootfs
-bootfs-fstype : filesystem for bootfs 

-root-uuid : uuid for rootfs
-rootfs-fstype : filesystem for rootfs 

*/

	char* rootfs_uuid = NULL;
	char* rootfs_blkdev = NULL;
	char* rootfs_fstype = NULL;

	char* bootfs_blkdev = NULL;
	char* bootfs_uuid = NULL;
	char* bootfs_fstype = NULL;

	// > if that is followed by the words loop, for, while or do , then you can condense the comment to "parse command line options"
	// - @therealdwd (discord)
	// parse command line options
	for(int i = 1; i < argc; i++) {
		if(strncmp(argv[i], "--verbosity", max(strlen(argv[i]), 11)) == 0 ) {
			if(argc - 1 < i + 1) {
				log_warn("Parameter Parsing", "Skipping parsing for last parameter.");
				break;
			}

			if(!isNumber(argv[++i])) {
				log_error("Parameter Parsing", "Parameter for \"verbosity\" has to be a number");
			} else log_setLevel(atoi(argv[i]));

			log_info("Parameter Parsing", "Verbosity has been ajusted to %i", log_getLevel());

			continue;
		}

		if((strncmp(argv[i], "--bootfs-uuid", max(strlen(argv[1]), 13)) == 0) && bootfs_uuid == NULL) {
			if(argc - 1 < i + 1) {
				log_error("Parameter Parsing", "Cannont parse bootfs-uuid parameter.");
				//! ERROR?
				break;
			}

			bootfs_uuid = calloc(strlen(argv[++i]) + 1, sizeof(char));
			strncpy(bootfs_uuid, argv[i], strlen(argv[i]));

			continue;
		}

		if((strncmp(argv[i], "--bootfs-fstype", max(strlen(argv[1]), 15)) == 0) && bootfs_fstype == NULL) {
			if(argc - 1 < i + 1) {
				log_error("Parameter Parsing", "Cannont parse bootfs-fstype parameter.");
				//! ERROR?
				break;
			}

			bootfs_fstype = calloc(strlen(argv[++i]) + 1, sizeof(char));
			strncpy(bootfs_fstype, argv[i], strlen(argv[i]));

			continue;
		}

		if((strncmp(argv[i], "--rootfs-uuid", strlen(argv[1])) == 0) && rootfs_uuid == NULL) {
			if(argc - 1 < i + 1) {
				log_error("Parameter Parsing", "Cannont parse rootfs-uuid parameter.");
				//! ERROR?
				break;
			}

			rootfs_uuid = calloc(strlen(argv[++i]) + 1, sizeof(char));
			strncpy(rootfs_uuid, argv[i], strlen(argv[i]));
			
			continue;
		}

		if((strncmp(argv[i], "--rootfs-fstype", strlen(argv[1])) == 0) && rootfs_fstype == NULL) {
			if(argc - 1 < i + 1) {
				log_error("Parameter Parsing", "Cannont parse rootfs-fstype parameter.");
				//! ERROR?
				break;
			}

			rootfs_fstype = calloc(strlen(argv[++i]) + 1, sizeof(char));
			strncpy(rootfs_fstype, argv[i], strlen(argv[i]));
			
			continue;
		}

	}

	// check if require arguments are provided
	if(rootfs_uuid == NULL) {
		log_error("Parameter Parsing", "No rootfs uuid provided");
		reboot((int)RB_HALT_SYSTEM);
	}

	if(rootfs_fstype == NULL) {
		log_warn("Parameter Parsing", "No fstype for rootfs provided, defaulting to %s", DEFAULT_ROOTFS_FSTYPE);
		rootfs_fstype = calloc(strlen(DEFAULT_ROOTFS_FSTYPE) + 1, sizeof(char));
		strncpy(rootfs_fstype, DEFAULT_ROOTFS_FSTYPE, strlen(DEFAULT_ROOTFS_FSTYPE) + 1);
	}

	if(bootfs_uuid == NULL) {
		log_error("Parameter Parsing", "No bootfs uuid provided");
		reboot((int)RB_HALT_SYSTEM);
	}

	if(bootfs_fstype == NULL) {
		log_warn("Parameter Parsing", "No fstype for bootfs provided, defaulting to %s", DEFAULT_BOOTFS_FSTYPE);
		bootfs_fstype = calloc(strlen(DEFAULT_BOOTFS_FSTYPE) + 1, sizeof(char));
		strncpy(bootfs_fstype, DEFAULT_BOOTFS_FSTYPE, strlen(DEFAULT_BOOTFS_FSTYPE) + 1);
	}

	// print provided arguments
	log_info("Parameter Parsing", "Done parsing parameters");
	log_info("Parameter Parsing", "ROOTFS UUID: \"%s\"", rootfs_uuid);
	log_info("Parameter Parsing", "ROOTFS FSTYPE: \"%s\"", rootfs_fstype);
	log_info("Parameter Parsing", "BOOTFS UUID: \"%s\"", bootfs_uuid);
	log_info("Parameter Parsing", "BOOTFS FSTYPE: \"%s\"", bootfs_fstype);

	// mounting /dev
	{
		int error;
		if((error = mount("devtmpfs", "/dev", "devtmpfs", 0, NULL)) != 0) {
			log_error("DEVFS Mounting", "Failed to mount devfs, code : %i", error);
			//! ERROR?
			reboot((int)RB_HALT_SYSTEM);
		}
	}

	log_info("DEVFS Mounting", "Mounted DEVFS");

	// mouting /sys

	{
		int error;
		if((error = mount("sysfs", "/sys", "sysfs", 0, NULL)) != 0) {
			log_error("SYSFS Mounting", "Failed to mount sysfs, code : %i", error);
			//! ERROR?
			reboot((int)RB_HALT_SYSTEM);
		}
	}

	log_info("SYSFS Mounting", "Mounted SYSFS");

	// mounting /proc

	{
		int error;
		if((error = mount("proc", "/proc", "proc", 0, NULL)) != 0) {
			log_error("PROCFS Mounting", "Failed to mount procfs, code : %i", error);
			//! ERROR?
			reboot((int)RB_HALT_SYSTEM);
		}
	}

	log_info("PROCFS Mounting", "Mounted PROCFS");


	// tldr: check what blockdevice matched with given uuid and fstype

	{
		blkid_cache cache;
		blkid_dev dev;
		blkid_probe probe;
		blkid_dev_iterate iterate;
		const char* blkdev_name;
		const char* blkdev_uuid;
		const char* blkdev_fstype;

		int error;

		if((error = blkid_get_cache(&cache, NULL)) < 0) {
			log_error("BLKID", "BLKID failed to initilize blkid cache with code %i and errno %i", error, errno);
			log_error("BLKID", "errno string: \"%s\"", strerror(errno));
			reboot((int)RB_HALT_SYSTEM);
			//! ERROR?
		}

		log_info("BLKID", "initilized BLKID cache");

		error = 0;

		if((error = blkid_probe_all(cache)) < 0) {
			log_error("BLKID", "BLKID failed to probe all devices with code %i and errno %i", error, errno);
			log_error("BLKID", "errno string: \"%s\"", strerror(errno));
			reboot((int)RB_HALT_SYSTEM);
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
				reboot((int)RB_HALT_SYSTEM);
			}

			error = 0;
			if((error = blkid_do_probe(probe)) != 0) {
				log_error("BLKID", "Failed to do the probeing for device \"%s\" with errno %i", blkdev_name, errno);
				log_error("BLKID", "errno string: \"%s\"", strerror(errno));
				blkid_free_probe(probe);
				continue;
				//! ERROR?
				reboot((int)RB_HALT_SYSTEM);
			}

			if(blkid_probe_lookup_value(probe, "UUID", &blkdev_uuid, NULL) == 0) {
				//DO COMPARISON

				blkid_probe_lookup_value(probe, "TYPE", &blkdev_fstype, NULL);

				log_info("UUID DISCOVERY", "DEV BLOCK: \"%s\" with UUID \"%s\" and file system %s", blkdev_name, blkdev_uuid, blkdev_fstype);
				
				if(strcmp(rootfs_uuid, blkdev_uuid) == 0) {
					log_info("UUID DISCOVERY", "ROOTFS UUID (%s) matches discoverd block device UUID (%s)", rootfs_uuid, blkdev_uuid);
					rootfs_blkdev = calloc(strnlen(blkdev_name, 32) + 1, sizeof(char));
					strncpy(rootfs_blkdev, blkdev_name, strnlen(blkdev_name, 32) + 1);

					if(strcmp(rootfs_fstype, blkdev_fstype) != 0) {
						log_warn("UUID DISCOVERY", "Given file system type doesnt match file system type on partition with given uuid for rootfs, defaulting to %s", blkdev_fstype);

						free(rootfs_fstype);
						rootfs_fstype = calloc(strnlen(blkdev_fstype, 32) + 1, sizeof(char));
						strncpy(rootfs_fstype, blkdev_fstype, strnlen(blkdev_fstype, 32) + 1);
					}
				}

				if(strcmp(bootfs_uuid, blkdev_uuid) == 0) {
					log_info("UUID DISCOVERY", "BOOTFS UUID (%s) matches discoverd block device UUID (%s)", bootfs_uuid, blkdev_uuid);
					bootfs_blkdev = calloc(strnlen(blkdev_name, 32) + 1, sizeof(char));
					strncpy(bootfs_blkdev, blkdev_name, strnlen(blkdev_name, 32) + 1);

					if(strcmp(bootfs_fstype, blkdev_fstype) != 0) {
						log_warn("UUID DISCOVERY", "Given file system type doesnt match file system type on partition with given uuid for bootfs, defaulting to %s", blkdev_fstype);

						free(bootfs_fstype);
						bootfs_fstype = calloc(strnlen(blkdev_fstype, 32) + 1, sizeof(char));
						strncpy(bootfs_fstype, blkdev_fstype, strnlen(blkdev_fstype, 32) + 1);
					}
				}

			} else {
				log_info("BLKID", "Device \"%s\" does not have a uuid (on disk with mbr?)", blkdev_name);
			}

			blkid_free_probe(probe);
		}


		blkid_dev_iterate_end(iterate);
		blkid_put_cache(cache);
	}

	// check if there is a mathcing block device for root/boot fs

	if(rootfs_blkdev == NULL) {
		log_error("Partition Scanning", "No Block device with uuid %s for rootfs found", rootfs_uuid);
		//! ERROR?
		reboot((int)RB_HALT_SYSTEM);
	}

	if(bootfs_blkdev == NULL) {
		log_error("Partition Scanning", "No Block device with uuid %s for bootfs found", bootfs_uuid);
		//! ERROR?
		reboot((int)RB_HALT_SYSTEM);
	}

	log_info("Partition Scanning", "Block device for rootfs has been detected as %s", rootfs_blkdev);
	log_info("Partition Scanning", "rootfs is formatted to %s", rootfs_fstype);

	log_info("Partition Scanning", "Block device for bootfs has been detected as %s", bootfs_blkdev);
	log_info("Partition Scanning", "bootfs is formatted to %s", bootfs_fstype);

	// check if kernel supports mounting root/boot fs

	{
		bool fsUnsupported = false;
		if(fs_fsIsSupported(rootfs_fstype) == false) {
			struct utsname kernelInfo;
			uname(&kernelInfo);
			log_error("Partition Scanning", "Kernel Revision %s does not support mounting %s filesystems", kernelInfo.release, rootfs_fstype);
			fsUnsupported = true;
		}

		if(fs_fsIsSupported(bootfs_fstype) == false) {
			struct utsname kernelInfo;
			uname(&kernelInfo);
			log_error("Partition Scanning", "Kernel Revision %s does not support mounting %s filesystems", kernelInfo.release, bootfs_fstype);
			fsUnsupported = true;
		}

		if(fsUnsupported == true) {
			//! ERROR?
			reboot((int)RB_HALT_SYSTEM);
		}
	}

	log_info("Partition Scanning", "Kernel supports mounting both bootfs and rootfs");

	/*
	
	Self note

	How to switch rootfs:

	mount /proc,/dev and /sys to /rootfs/{proc, dev, sys} using MS_MOVE

	chdir /rootfs

	get old_root (ramfs root) fd

	mount /rootfs to / using MS_MOVE
	
	chroot .
	chdir /

	fork:
		child (fork return val = 0):
			rm -rf / rootfs using old_root fd (if tmpfs/ramfs)
			close old_root fd
			sys_exit
		parent:
			close old_root fd

	*/

	// preparing stuff to change the root filesystem to rootfs

	mount(rootfs_blkdev, "/rootfs", rootfs_fstype, 0, NULL);

	mount("/dev", "/rootfs/dev", "devtmpfs", MS_MOVE, NULL);
	mount("/sys", "/rootfs/sys", "sysfs", MS_MOVE, NULL);
	mount("/proc", "/rootfs/proc", "proc", MS_MOVE, NULL);

	chdir("/rootfs");

	int initrdRoot = open("/", O_RDONLY);

	mount("/rootfs", "/", rootfs_fstype, MS_MOVE, NULL);

	chroot(".");

	chdir("/");

	switch(fork()) {
		case 0: {
			struct statfs initrdRootStat;
			if(fstatfs(initrdRoot, &initrdRootStat) == 0 &&
				(initrdRootStat.f_type == RAMFS_MAGIC ||
				initrdRootStat.f_type == TMPFS_MAGIC)) {
					fs_removeRecusrivly(initrdRoot);
			}
			close(initrdRoot);
			exit(0);
			break; // idk why, we are never gonna be here anyway
		}

		case -1: break;
		
		default: {
			close(initrdRoot);
			break;
		}
	}

	


	// this is so i dont get called out for having skill issues

	free(rootfs_uuid);
	free(rootfs_blkdev);
	free(rootfs_fstype);

	free(bootfs_blkdev);
	free(bootfs_uuid);
	free(bootfs_fstype);

	// we should NEVER have been here

	log_error("INIT END", "AT THE END OF INIT, IDK, I DONT THINK THIS SHOULD HAVE HAPPEND");
	log_error("INIT END", "rebooting in 5 seconds");

	sleep(5);

	reboot((int)RB_POWER_OFF);
}