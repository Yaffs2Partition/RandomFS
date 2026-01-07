#define FUSE_USE_VERSION 31

#include <fuse3/fuse.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>   // PATH_MAX
#include <unistd.h>   // realpath

#include "randomfs_engine.h"
#include "randomfs_config.h"

int rfs_chaos_mode = 0;
uint64_t rfs_seed = 0;

static int rfs_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    (void)fi;
    memset(stbuf, 0, sizeof(*stbuf));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0555;
        stbuf->st_nlink = 2;
        return 0;
    }

    if (path[0] == '/' && strlen(path) == 9) {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = RFS_FILE_SIZE;
        return 0;
    }

    return -ENOENT;
}

static int rfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t off, struct fuse_file_info *fi,
                       enum fuse_readdir_flags flags) {
    (void)off; (void)fi; (void)flags;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);

    char names[RFS_MAX_FILES][9];
    int n = rfs_engine_list(names);
    for (int i = 0; i < n; i++)
        filler(buf, names[i], NULL, 0, 0);

    return 0;
}

static int rfs_open(const char *path, struct fuse_file_info *fi) {
    if (path[0] != '/' || strlen(path) != 9)
        return -ENOENT;
    return 0;
}

static int rfs_read(const char *path, char *buf, size_t size,
                    off_t offset, struct fuse_file_info *fi) {
    (void)fi;
    return rfs_engine_read(path + 1, buf, size, offset);
}

static struct fuse_operations rfs_ops = {
    .getattr = rfs_getattr,
    .readdir = rfs_readdir,
    .open    = rfs_open,
    .read    = rfs_read,
};

int main(int argc, char **argv) {
    char img_abs[PATH_MAX];
    char mnt_abs[PATH_MAX];

    if (argc == 3 || (argc == 4 && strcmp(argv[1], "--chaos") == 0)) {
        int chaos = 0;
        int img_idx = 1, mnt_idx = 2;

        if (argc == 4) {
            chaos = 1;
            img_idx = 2;
            mnt_idx = 3;
            rfs_chaos_mode = 1;
        }

        // resolve absolute path for entropy.img
        if (!realpath(argv[img_idx], img_abs)) {
            perror("entropy.img");
            return 1;
        }

        // resolve absolute path for mountpoint (or fallback to relative)
        if (!realpath(argv[mnt_idx], mnt_abs)) {
            strncpy(mnt_abs, argv[mnt_idx], PATH_MAX);
            mnt_abs[PATH_MAX-1] = 0;
        }

        rfs_engine_init(img_abs);

        char *fuse_argv[3] = { argv[0], mnt_abs, NULL };
        return fuse_main(2, fuse_argv, &rfs_ops, NULL);
    }

    fprintf(stderr,
        "usage:\n"
        "  %s entropy.img mnt\n"
        "  %s --chaos entropy.img mnt\n", argv[0], argv[0]);
    return 1;
}

