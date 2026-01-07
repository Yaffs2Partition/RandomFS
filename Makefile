CC = gcc
CFLAGS = -Wall -O2 `pkg-config fuse3 --cflags`
LDFLAGS = `pkg-config fuse3 --libs`

SRC = src/randomfs.c src/randomfs_engine.c
OBJ = $(patsubst src/%.c,build/obj/%.o,$(SRC))
BIN = build/executable/randomfs

# Default target
all: $(BIN)

# Compile .c -> .o in build/obj
build/obj/%.o: src/%.c | build/obj
	$(CC) $(CFLAGS) -c $< -o $@

# Make sure obj and executable folders exist
build/obj:
	mkdir -p build/obj

build/executable:
	mkdir -p build/executable

# Link objects -> binary
$(BIN): $(OBJ) | build/executable
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

# Clean
clean:
	rm -rf build
	rm -f randomfs
