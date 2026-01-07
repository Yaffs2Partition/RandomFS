#include "randomfs_engine.h"
#include "hash.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_FILES 64
#define MIN_FILES 3
#define MIN_SIZE  512
#define MAX_SIZE  (64 * 1024)

static uint64_t g_seed;
static int g_fd;
static int g_file_count;
static size_t g_sizes[MAX_FILES];

void rfs_engine_init(const char *img_path, int chaos) {
    g_fd = open(img_path, O_RDONLY);
    if (g_fd < 0) {
        perror("open");
        exit(1);
    }

    uint8_t buf[4096];
    ssize_t n = read(g_fd, buf, sizeof(buf));
    if (n <= 0) {
        perror("read");
        exit(1);
    }

    g_seed = rfs_hash64(buf, n);

    if (chaos) {
        g_seed ^= (uint64_t)random();
    }

    g_file_count = (g_seed % (MAX_FILES - MIN_FILES)) + MIN_FILES;

    for (int i = 0; i < g_file_count; i++) {
        uint64_t h = rfs_hash64(&i, sizeof(i)) ^ g_seed;
        g_sizes[i] = (h % (MAX_SIZE - MIN_SIZE)) + MIN_SIZE;
    }
}

int rfs_engine_file_count(void) {
    return g_file_count;
}

size_t rfs_engine_file_size(int index) {
    return g_sizes[index];
}

void rfs_engine_filename(int index, char *out) {
    uint64_t h = rfs_hash64(&index, sizeof(index)) ^ g_seed;
    snprintf(out, 16, "%08lx", (unsigned long)(h & 0xffffffff));
}

int rfs_engine_read(int index, char *buf, size_t size, off_t offset) {
    if (offset >= (off_t)g_sizes[index])
        return 0;

    if (offset + size > g_sizes[index])
        size = g_sizes[index] - offset;

    uint64_t local_seed = g_seed ^ index ^ offset;
    for (size_t i = 0; i < size; i++) {
        local_seed = rfs_hash64(&local_seed, sizeof(local_seed));
        buf[i] = (char)(local_seed & 0xff);
    }

    return size;
}

