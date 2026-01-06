#define FUSE_USE_VERSION 35

#include <fuse3/fuse.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdint.h>
#include <errno.h>

#include "hash.h"

#define NODE_SIZE       512
#define MAX_ENTRIES     8
#define MAX_FILE_SIZE   (64 * 1024)

/* entropy source */
static int src_fd = -1;
static off_t src_size = 0;

/* read a deterministic node derived from path */
static void rfs_read_node(const char *path, uint8_t *buf)
{
    uint64_t h = rfs_hash_path(path);
    off_t off = h % (src_size - NODE_SIZE);

    pread(src_fd, buf, NODE_SIZE, off);
}

/* ===== filesystem operations ===== */

static int rfs_getattr(
    const char *path,
    struct stat *st,
    struct fuse_file_info *fi)
{
    (void) fi;
    memset(st, 0, sizeof(*st));

    uint8_t node[NODE_SIZE];
    rfs_read_node(path, node);

    /* root is always a directory */
    if (strcmp(path, "/") == 0 || (node[0] & 1)) {
        st->st_mode  = S_IFDIR | 0555;
        st->st_nlink = 2;
    } else {
        st->st_mode  = S_IFREG | 0444;
        st->st_nlink = 1;
        st->st_size  = (node[1] % MAX_FILE_SIZE) + 1;
    }

    return 0;
}

static int rfs_readdir(
    const char *path,
    void *buf,
    fuse_fill_dir_t filler,
    off_t off,
    struct fuse_file_info *fi,
    enum fuse_readdir_flags flags)
{
    (void) off;
    (void) fi;
    (void) flags;

    filler(buf, ".",  NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);

    uint8_t node[NODE_SIZE];
    rfs_read_node(path, node);

    int count = node[2] % MAX_ENTRIES;

    for (int i = 0; i < count; i++) {
        char name[16];
        snprintf(name, sizeof(name), "r%02x", node[3 + i]);
        filler(buf, name, NULL, 0, 0);
    }

    return 0;
}

static int rfs_open(const char *path, struct fuse_file_info *fi)
{
    (void) path;

    if ((fi->flags & O_ACCMODE) != O_RDONLY)
        return -EACCES;

    return 0;
}

static int rfs_read(
    const char *path,
    char *buf,
    size_t size,
    off_t offset,
    struct fuse_file_info *fi)
{
    (void) fi;

    uint64_t h = rfs_hash_path(path);
    off_t base = h % (src_size - MAX_FILE_SIZE);

    return pread(src_fd, buf, size, base + offset);
}

/* ===== fuse ops table ===== */

static const struct fuse_operations rfs_ops = {
    .getattr = rfs_getattr,
    .readdir = rfs_readdir,
    .open    = rfs_open,
    .read    = rfs_read,
};

/* ===== main ===== */

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr,
            "usage: %s <entropy-source> <mountpoint>\n",
            argv[0]);
        return 1;
    }

    src_fd = open(argv[1], O_RDONLY);
    if (src_fd < 0) {
        perror("open");
        return 1;
    }

    src_size = lseek(src_fd, 0, SEEK_END);
    if (src_size < NODE_SIZE * 2) {
        fprintf(stderr, "source too small\n");
        return 1;
    }

    char *fuse_argv[] = {
        argv[0],
        argv[2],
        "-f",
        "-o", "ro"
    };

    return fuse_main(
        5,
        fuse_argv,
        &rfs_ops,
        NULL
    );
}

