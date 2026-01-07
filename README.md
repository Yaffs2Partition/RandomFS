# RandomFS Version 0.3
---
## New Features
- Help page
- About page
- Version

## Bug Fixes
- Fixed files always being 4 KiB
- Fixed files always being 6 when mounted

---
## How to use
- Chaos mode: ./randomfs --chaos entropy.img mnt
- Normal mode: ./randomfs entropy.img
- Help page: ./randomfs --help
- About page: ./randomfs --about
- Program version: ./randomfs --version

# Dependencies
- Linux
- libfuse3
- GCC or Clang

# How to compile
- Make sure you have the dependencies
- Run "make" in the root of the source code using the terminal
- Takes around 2 seconds to compile

