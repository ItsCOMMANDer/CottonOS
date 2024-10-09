#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

bool fs_fsIsSupported(char* fstype) {
	// open file containg list of supported filesystems
	int fd = open("/proc/filesystems", O_RDONLY);

	//check if opening file was successful
	if(fd == -1) {
		return false;
	}

	// get length including \0
	size_t len = 0;
	{
		char tmp;
		while(read(fd, &tmp, 1) > 0) {len++;}
	}

	lseek(fd, 0, SEEK_SET);

	char* buffer = calloc(len, 1);

	read(fd, buffer, len);

	close(fd);

	/*
	TLDR How this works
	/proc/filesystems contains a list of filesystems that the kernel can mount int this format:

	"(nodev)\tfstype\n" - the "nodev" is in "()" because theyre optional, "\t" and "\n" are the actual tab/newline characters

	we just loop over the file byte-by-byte until we are at the start of "fstype" and check if each character is the same 
	if characters dont match then we loop until the next fstype, if they do, we continue, if the fstype if at its end (\n) and the given fstype (\0)
		then its supported
	when we're at the end of the file without match, its not supported
	*/

	int idx = 0;
	int cnt = 0; // index in string to compare
	bool fstypeParse = false;
	while(buffer[idx] != '\0') {
		if(fstypeParse == true) {
			if(buffer[idx] == '\n' && fstype[cnt] == '\0') {return true;}
			if(fstype[cnt] != buffer[idx]) {
				cnt = 0;
				fstypeParse = false;
			} else cnt++;
		}
		if(buffer[idx] == '(') {idx+=5; continue;}
		if(buffer[idx] == '\t') fstypeParse = true;
		if(buffer[idx] == '\n') fstypeParse = false;
		idx++;
	}

	free(buffer);

	return false;
}

void fs_removeRecusrivly(int fd) {
    DIR* rootDir = fdopendir(fd);

    int rootDirfd = dirfd(rootDir);

    struct stat rootStat;
    fstat(rootDirfd, &rootStat);

    bool isDir = false;
    
    while(1) {
        struct dirent *subDir;

        if(!(subDir = readdir(rootDir))) {
            break; // No more sub directories
        }

   		if (!strcmp(subDir->d_name, ".") || !strcmp(subDir->d_name, "..")) {
            continue;
        }

#ifdef _DIRENT_HAVE_D_TYPE
		if (subDir->d_type == DT_DIR || subDir->d_type == DT_UNKNOWN)
#endif
        {
            struct stat subDirStat;

            fstatat(rootDirfd, subDir->d_name, &subDirStat, AT_SYMLINK_NOFOLLOW);

            if(subDirStat.st_dev != rootStat.st_dev) continue; // bad if subdir is based on another mount

            if(S_ISDIR(subDirStat.st_mode)) {
                int cfd = openat(rootDirfd, subDir->d_name, O_RDONLY);
                if(cfd >= 0) fs_removeRecusrivly(cfd);
                isDir = true;
            }
        }

        unlinkat(rootDirfd, subDir->d_name, isDir ? AT_REMOVEDIR : 0);
    }

    if(isDir == true) closedir(rootDir);
    else close(rootDirfd);
}