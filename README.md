# RandomFS

RandomFS is a **read-only, deterministic FUSE filesystem** that translates
random data (such as `/dev/urandom`-filled drives) into a virtual filesystem
containing directories and files.

There is **no real filesystem on disk** — all structure is *derived* from raw
entropy.

> Same source + same path = same files, always.

> A drive filled with random /dev/urandom garbage is what we call a "urandomed drive".
---

## Features

- Read-only
- Userspace (FUSE, no kernel modules)
- Works on:
  - regular files
  - block devices
  - fully urandomed drives
- Deterministic structure
- Zero metadata storage
- Portable across Linux distributions

---

## How it works

RandomFS treats the source device or file as a large byte array.

For any filesystem path:

1. The path is hashed
2. The hash is mapped to an offset in the source
3. Bytes at that offset are interpreted as:
   - directory metadata, or
   - file metadata and content

Nothing is written back to the source.

---

## Design principles

- **Entropy is the storage**
- **Structure is an illusion**
- **No scanning of the device**
- **No persistent state**
- **Safe by design**

---

## Requirements

- Linux
- libfuse3
- GCC or Clang

## How to compile:

1 - Cd into the folder

2 - Run "make" in the terminal ( Assuming you already have the required dependencies )

3 - By the time you blink, its already done compiling :)

## How to use

1 - Create a .img that is any desired size. example: dd if=/dev/urandom of=filename.img bs=1M count=32 && mkdir mnt

2 - Run this in the terminal: ./randomfs filename.img mnt

3 - Enjoy :)
