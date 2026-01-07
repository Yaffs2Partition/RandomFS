#define FUSE_USE_VERSION 31

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <fuse3/fuse.h>
#include <time.h>
#include "randomfs_engine.h"
#include "hash.h"

static const char *g_img_path = NULL;
static uint32_t g_seed = 0;

// ---------------------- Random helpers ----------------------
void random_name(char *buf, size_t n) {
    static const char chars[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    for (size_t i = 0; i < n - 1; i++) {
        buf[i] = chars[rand() % (sizeof(chars) - 1)];
    }
    buf[n - 1] = '\0';
}

// Generate a random number in [min, max]
int rand_range(int min, int max) {
    return min + rand() % (max - min + 1);
}

// ---------------------- FUSE callbacks ----------------------
static int rfs_getattr(const char *path, struct stat *stbuf,
                       struct fuse_file_info *fi) {
    (void) fi;
    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    } else {
        stbuf->st_mode = S_IFREG | 0644;
        stbuf->st_nlink = 1;
        stbuf->st_size = rand_range(512, 4096); // random file size
    }
    return 0;
}

static int rfs_readdir(const char *path, void *buf,
                       fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi,
                       enum fuse_readdir_flags flags) {
    (void) offset;
    (void) fi;
    (void) flags;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);

    int num_files = rand_range(3, 7); // random number of files
    char name[9]; // 8 chars + null
    for (int i = 0; i < num_files; i++) {
        random_name(name, sizeof(name));
        filler(buf, name, NULL, 0, 0);
    }

    return 0;
}

// ---------------------- FUSE operations ----------------------
static struct fuse_operations rfs_oper = {
    .getattr = rfs_getattr,
    .readdir = rfs_readdir,
};

// ---------------------- Main ----------------------
int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <image-file> <mount-point> [seed]\n", argv[0]);
        return 1;
    }

    g_img_path = argv[1];
    const char *mount_point = argv[2];

    if (argc >= 4) {
        g_seed = (uint32_t)atoi(argv[3]);
    }

    srand(g_seed); // seed randomness

    printf("RandomFS v0.2 starting with seed %u\n", g_seed);

    // Pass only mount point to FUSE
    char *fuse_argv[2];
    fuse_argv[0] = argv[0];          // program name
    fuse_argv[1] = (char *)mount_point;

    return fuse_main(2, fuse_argv, &rfs_oper, NULL);
}

