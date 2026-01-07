#include "randomfs_engine.h"
#include "randomfs_config.h"
#include "hash.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static uint64_t file_seeds[RFS_MAX_FILES];

void rfs_engine_init(const char *img_path) {
    int fd = open(img_path, O_RDONLY);
    uint8_t buf[4096];
    ssize_t n = read(fd, buf, sizeof(buf));
    close(fd);

    if (!rfs_chaos_mode)
        rfs_seed = rfs_hash64(buf, n);
    else
        rfs_seed = ((uint64_t)time(NULL) << 32) ^ getpid();

    srand((unsigned)rfs_seed);

    for (int i = 0; i < RFS_MAX_FILES; i++)
        file_seeds[i] = rand();
}

int rfs_engine_list(char names[RFS_MAX_FILES][9]) {
    for (int i = 0; i < RFS_MAX_FILES; i++) {
        uint32_t v = (uint32_t)file_seeds[i];
        for (int j = 0; j < 8; j++) {
            names[i][j] = "abcdefghijklmnopqrstuvwxyz0123456789"[v % 36];
            v /= 36;
        }
        names[i][8] = 0;
    }
    return RFS_MAX_FILES;
}

int rfs_engine_read(const char *name, char *buf, size_t size, off_t offset) {
    uint64_t h = rfs_hash64(name, strlen(name)) ^ rfs_seed;
    if (offset >= RFS_FILE_SIZE)
        return 0;

    if (offset + size > RFS_FILE_SIZE)
        size = RFS_FILE_SIZE - offset;

    for (size_t i = 0; i < size; i++) {
        h = h * 6364136223846793005ULL + 1;
        buf[i] = (char)(h >> 56);
    }
    return (int)size;
}

