#ifndef COTTON_INIT_FS_H
#define COTTON_INIT_FS_H

bool fs_fsIsSupported(char* fstype);

//void fs_removeRecusrivly(int fd);
int fs_removeRecusrivly(int fd);

#endif