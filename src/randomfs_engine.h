#ifndef RANDOMFS_ENGINE_H
#define RANDOMFS_ENGINE_H

#include <stddef.h>
#include <stdint.h>
#include <unistd.h> // for off_t

#define RFS_MAX_FILES 8
#define RFS_FILE_SIZE 4096

void rfs_engine_init(const char *img_path);
int  rfs_engine_list(char names[RFS_MAX_FILES][9]);
int  rfs_engine_read(const char *name, char *buf, size_t size, off_t offset);

#endif

