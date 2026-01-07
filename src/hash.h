#ifndef RANDOMFS_HASH_H
#define RANDOMFS_HASH_H

#include <stdint.h>
#include <stddef.h>

static inline uint64_t rfs_hash64(const void *data, size_t len) {
    const uint8_t *p = data;
    uint64_t h = 1469598103934665603ULL;
    for (size_t i = 0; i < len; i++) {
        h ^= p[i];
        h *= 1099511628211ULL;
    }
    return h;
}

#endif

