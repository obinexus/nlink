# Makefile for NLink SDL Project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = -lSDL2

# Directories
SRC_DIR = src
SOUND_DIR = sound-processing
WINDOW_DIR = game-windows
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin
OBJ_DIR = $(BUILD_DIR)/obj
NLINK_DIR = $(BUILD_DIR)/nlink

# Source files
SOURCES = $(SRC_DIR)/main.c

# Object files
OBJECTS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SOURCES)))

# Include paths
INCLUDES = -I$(SOUND_DIR)/include \
           -I$(WINDOW_DIR)/include \
           -I./include

# Target executable
TARGET = $(BIN_DIR)/nlink_sdl_demo

# Default target
all: directories $(TARGET)

# Create directories
directories:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(NLINK_DIR)

# Link final executable
$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Run the demo
run: all
	$(TARGET)

.PHONY: all directories clean run
