#ifndef RANDOMFS_ENGINE_H
#define RANDOMFS_ENGINE_H

#include <stddef.h>

void rfs_random_name(char *buf, size_t n);
size_t rfs_random_file_size();
int rfs_random_num_entries();

#endif // RANDOMFS_ENGINE_H

