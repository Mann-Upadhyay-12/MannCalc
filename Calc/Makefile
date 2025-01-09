

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -lm

debug: CFLAGS += -g
debug: all

release: CFLAGS += -O2
release: all
# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INCLUDE_DIR = include
OBJ_DIR = $(BUILD_DIR)/obj

# Executable
TARGET = $(BIN_DIR)/calc.out

# Source files and object files
SRC_FILES = $(wildcard $(SRC_DIR)/**/*.c $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Include paths
INCLUDES = -I$(INCLUDE_DIR)

# Add -DUNDER_CONSTRUCTION__ to skip compiling files under construction and add #ifdef UNDER_CONSTRUCTION__ in tha code 
CFLAGS += -DUNDER_CONSTRUCTION__


# Default target
.PHONY: all
all: $(TARGET)

# Link the final executable
$(TARGET): $(OBJ_FILES) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJ_FILES) -o $@

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Create necessary directories
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Clean up build artifacts
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run the program
.PHONY: run
run: $(TARGET)
	./$(TARGET)

# Run tests (if applicable)
.PHONY: test

test:
		$(MAKE) -C tests

