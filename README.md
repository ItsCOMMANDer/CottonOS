
# Readme


## Introduction

(This repo really only contains the init system and scripts to make the initrd.cpio.gz)

Cotton OS will be an minimalistic linux distro that focuses on minimizing ram usage.

## Motivation

The main reason I'm making this is fun, but also because i thought it'd be a fun 10th Grade CS Project.

## Compilation

To compile the the `init` executable execute `make all` <br>
To make the initrd just run `make initrd`

## Contributions

If you'd like to contribute to CottonOS, think again. It's my homework.

## cotton-init
I dont know what else to write here, so ill explain how cotton-init will work.
I am also sorta making it as i gol. While i have some idea on what i need to do and how i'll do it, the exact implementations (or ideas of them) dont exist yet 

cotton-init will be the init system of CottonOS. Why did i decide to make my own init system? Idk, for fun?

cotton-init will be executed trough the initrd of CottonOS.
The CottonOS partition stucture is simple:

```
+-----+--------+--------------+
| ESP | BOOTFS |    ROOTFS    |
+-----+--------+--------------+
```

ESP is just the Efi System Partition containing the bootloader (GRUB)
BOOTFS contains kernel, initrd, recovery initrd and possibly some boot start drivers, but i havent decided on that part yet.
ROOTFS contains the root filesystem

cotton-init will first parse the commandline arguments it has been given.

Then it mounts `/dev` `/sys` and `/proc` which is needed by `libblkid` which we'll use in the next step

Now we use `libblkid` to see what partitions are detected by linux (the kernel), what filesystem are on them an what uuid they have

If we found partition that match the given uuids for root and boot fs, well store them

After that we'll check if the kernel supports mounting boot and root fs using `/proc/filesystems` 

Now (the part i am sorta stuck at) we need to mount rootfs on root. sadly, sys_pivot_root wont work as it can change to root form a (init)ramfs

Later i need t parse configuration files, daemons (demons? deamons? idk) and communication to other processes that want to manage systems, like starting and stopping daemons ect
