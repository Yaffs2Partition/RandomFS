#define FUSE_USE_VERSION 31
#define RANDOMFS_VERSION "0.4"

#include <fuse3/fuse.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "randomfs_engine.h"

/* =========================
 * FUSE callbacks
 * ========================= */

static int rfs_getattr(
    const char *path,
    struct stat *st,
    struct fuse_file_info *fi
) {
    (void) fi;
    memset(st, 0, sizeof(*st));

    if (strcmp(path, "/") == 0) {
        st->st_mode = S_IFDIR | 0555;
        st->st_nlink = 2;
        return 0;
    }

    if (path[0] == '/') {
        int count = rfs_engine_file_count();
        for (int i = 0; i < count; i++) {
            char name[16];
            rfs_engine_filename(i, name);
            if (strcmp(path + 1, name) == 0) {
                st->st_mode = S_IFREG | 0444;
                st->st_nlink = 1;
                st->st_size = rfs_engine_file_size(i);
                return 0;
            }
        }
    }

    return -ENOENT;
}

static int rfs_readdir(
    const char *path,
    void *buf,
    fuse_fill_dir_t filler,
    off_t off,
    struct fuse_file_info *fi,
    enum fuse_readdir_flags flags
) {
    (void) off;
    (void) fi;
    (void) flags;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);

    int count = rfs_engine_file_count();
    for (int i = 0; i < count; i++) {
        char name[16];
        rfs_engine_filename(i, name);
        filler(buf, name, NULL, 0, 0);
    }

    return 0;
}

static int rfs_open(const char *path, struct fuse_file_info *fi) {
    (void) fi;
    struct stat st;
    return rfs_getattr(path, &st, NULL);
}

static int rfs_read(
    const char *path,
    char *buf,
    size_t size,
    off_t offset,
    struct fuse_file_info *fi
) {
    (void) fi;

    int count = rfs_engine_file_count();
    for (int i = 0; i < count; i++) {
        char name[16];
        rfs_engine_filename(i, name);
        if (strcmp(path + 1, name) == 0) {
            return rfs_engine_read(i, buf, size, offset);
        }
    }

    return -ENOENT;
}

static const struct fuse_operations rfs_ops = {
    .getattr = rfs_getattr,
    .readdir = rfs_readdir,
    .open    = rfs_open,
    .read    = rfs_read,
};

/* =========================
 * CLI helpers
 * ========================= */

static void print_help(void) {
    puts(
        "RandomFS — entropy-based filesystem\n\n"
        "Usage:\n"
        "  randomfs <image> <mountpoint>\n"
        "  randomfs --chaos <image> <mountpoint>\n\n"
        "Options:\n"
        "  --chaos     Non-deterministic mode\n"
        "  --help      Show this help\n"
        "  --about     About RandomFS\n"
        "  --version   Show version\n"
    );
}

static void print_about(void) {
    puts(
        "RandomFS\n"
        "Read-only entropy-derived filesystem\n"
        "By Yaffs2Partition\n"
    );
}

static void print_version(void) {
    printf("RandomFS %s\n", RANDOMFS_VERSION);
}

/* =========================
 * main (CORRECT)
 * ========================= */

int main(int argc, char *argv[]) {
    /* Simple info commands */
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        print_help();
        return 0;
    }

    if (argc == 2 && strcmp(argv[1], "--about") == 0) {
        print_about();
        return 0;
    }

    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        print_version();
        return 0;
    }

    /* Chaos mode */
    if (argc == 4 && strcmp(argv[1], "--chaos") == 0) {
        rfs_engine_init(argv[2], 1);

        char *fuse_argv[] = {
            argv[0],   /* program name */
            argv[3],   /* mountpoint */
            NULL
        };

        return fuse_main(2, fuse_argv, &rfs_ops, NULL);
    }

    /* Deterministic mode */
    if (argc == 3) {
        rfs_engine_init(argv[1], 0);

        char *fuse_argv[] = {
            argv[0],   /* program name */
            argv[2],   /* mountpoint */
            NULL
        };

        return fuse_main(2, fuse_argv, &rfs_ops, NULL);
    }

    fprintf(stderr, "Invalid arguments. Use --help\n");
    return 1;
}

