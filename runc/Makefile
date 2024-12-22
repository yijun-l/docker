# Compiler and Flags
CC = gcc
CFLAGS = -Wall
LFLAGS = -lrt -pthread

# Directories
BUILD_DIR = build
BIN_DIR = bin
ROOTFS_DIR = rootfs

# Source files and object files
SRC = common.c container.c main.c overlayfs.c ipc.c
OBJ = $(SRC:%.c=$(BUILD_DIR)/%.o)

# Executable name
EXEC = arunc

# Copy Exe Script
COPY_EXE_SCRIPT = ./copy_exe.sh
COMMANDS = bash ps ls id hostname ip env mount cat ipcs

# Targets
.PHONY: all clean run

# Default target
all: $(BIN_DIR)/$(EXEC)
	@for cmd in $(COMMANDS); do \
		$(COPY_EXE_SCRIPT) $$cmd; \
	done

# Rule to build executable
$(BIN_DIR)/$(EXEC): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LFLAGS) $(OBJ) -o $@ $(LDFLAGS) 

# Rule to build object files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the build, bin and rootfs directories
clean:
	sudo umount -f $(ROOTFS_DIR)/merged_dir || true
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(ROOTFS_DIR)
	
# Run the executable
run: all
	sudo $(BIN_DIR)/$(EXEC)