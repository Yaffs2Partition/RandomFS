## RandomFS v0.2 released!

## Fixes

- Terminal got stuck while a "urandomed" image was mounted, and so its fixed now, it should run in the background

---

## Dependencies to compile

- Linux
- libfuse3
- GCC or Clang

## How to compile

- Run "make" in the terminal ( Assuming you already are in the RandomFS root directory )

- After its done go to build/executable, there you will find "randomfs"

## How to use

- Create a urandomed image, example: dd if=/dev/urandom of=entropy.img bs=1M count=1

- mounting: ./randomfs entropy.img mnt

- Enjoy :)
