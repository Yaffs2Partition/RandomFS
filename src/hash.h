#ifndef RANDOMFS_HASH_H
#define RANDOMFS_HASH_H

#include <stdint.h>

/*
 * RandomFS path hash
 * 64-bit FNV-1a
 *
 * Properties:
 * - deterministic
 * - fast
 * - stable across architectures
 * - good enough for filesystem paths
 */

#define RFS_FNV_OFFSET 1469598103934665603ULL
#define RFS_FNV_PRIME  1099511628211ULL

static inline uint64_t rfs_hash_path(const char *path)
{
    uint64_t h = RFS_FNV_OFFSET;

    while (*path) {
        h ^= (uint8_t)*path++;
        h *= RFS_FNV_PRIME;
    }

    return h;
}

#endif /* RANDOMFS_HASH_H */

