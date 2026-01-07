#ifndef RANDOMFS_ENGINE_H
#define RANDOMFS_ENGINE_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

void   rfs_engine_init(const char *img_path, int chaos);
int    rfs_engine_file_count(void);
size_t rfs_engine_file_size(int index);
void   rfs_engine_filename(int index, char *out);
int    rfs_engine_read(
          int index,
          char *buf,
          size_t size,
          off_t offset
       );

#endif

