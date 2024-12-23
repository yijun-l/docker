# Compiler and Flags
CC = gcc
CFLAGS = 
LFLAGS = -lutil

# Directories
BUILD_DIR = build
BIN_DIR = bin
LOG_DIR = ../log

# Source files and object files
SRC = common.c main.c unix_socket.c event_loop.c init_runc.c
OBJ = $(SRC:%.c=$(BUILD_DIR)/%.o)

# Executable name
EXEC = ashim

PID_FILE = /var/run/$(EXEC).pid

# Default target
all: $(BIN_DIR)/$(EXEC)
	@mkdir -p $(LOG_DIR)

# Rule to build executable
$(BIN_DIR)/$(EXEC): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LFLAGS) $(OBJ) -o $@ $(LDFLAGS) 

# Rule to build object files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	@if [ -f $(PID_FILE) ]; then \
		echo "PID file $(PID_FILE) exists. $(EXEC) is already running."; \
	else \
		echo "Starting $(EXEC)..."; \
		sudo $(BIN_DIR)/$(EXEC); \
		echo "$(EXEC) started."; \
    fi
	
# Clean up the build, bin and rootfs directories
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@if [ -f $(PID_FILE) ]; then \
		echo "Stopping $(EXEC)..."; \
		sudo kill `cat $(PID_FILE)`; \
		echo "$(EXEC) stopped."; \
	else \
		echo "$(EXEC) is not running."; \
	fi	

# Targets
.PHONY: all clean