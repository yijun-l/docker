# Compiler and Flags
CC = gcc
CFLAGS = 
LFLAGS = 

# Directories
BUILD_DIR = build
BIN_DIR = bin
BIN_DST = /usr/local/bin

# Source files and object files
SRC =  main.c
OBJ = $(SRC:%.c=$(BUILD_DIR)/%.o)

# Executable name
EXEC = adocker

# Default target
all: $(BIN_DIR)/$(EXEC)
	sudo cp $(BIN_DIR)/$(EXEC) $(BIN_DST)

# Rule to build executable
$(BIN_DIR)/$(EXEC): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LFLAGS) $(OBJ) -o $@ $(LDFLAGS) 

# Rule to build object files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: all

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) 
	sudo rm -rf $(BIN_DST)/$(EXEC)

# Targets
.PHONY: all clean