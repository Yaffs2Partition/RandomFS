#ifndef RANDOMFS_HASH_H
#define RANDOMFS_HASH_H

#include <stdint.h>
#include <stddef.h>

// Simple XOR hash for demo purposes
static inline uint32_t rfs_hash(const uint8_t *data, size_t len) {
    uint32_t h = 0;
    for (size_t i = 0; i < len; i++) {
        h ^= ((uint32_t)data[i]) << (i % 24);
    }
    return h;
}

#endif // RANDOMFS_HASH_H

