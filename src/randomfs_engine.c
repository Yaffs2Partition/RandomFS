// randomfs_engine.c — RandomFS v0.2 engine
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "randomfs_engine.h"

// Alphanumeric chars for filenames
static const char chars[] = "abcdefghijklmnopqrstuvwxyz0123456789";

// Generate a random string (filename) of length n
void rfs_random_name(char *buf, size_t n) {
    for (size_t i = 0; i < n - 1; i++) {
        buf[i] = chars[rand() % (sizeof(chars) - 1)];
    }
    buf[n - 1] = '\0';
}

// Return a random file size in bytes
size_t rfs_random_file_size() {
    return 512 + rand() % 3585; // 512–4096 bytes
}

// Return a random number of entries in a directory
int rfs_random_num_entries() {
    return 3 + rand() % 5; // 3–7 entries
}

