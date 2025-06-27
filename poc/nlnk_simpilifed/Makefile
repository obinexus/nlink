# Makefile for NexusLink
# Author: Nnamdi Michael Okpala

CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = 

SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = test
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Test files
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_BINS = $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%,$(TEST_SRCS))

# Targets
.PHONY: all clean test

all: directories test

directories:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Build test executables
$(BUILD_DIR)/%: $(TEST_DIR)/%.c $(OBJ_FILES)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $^ -o $@ $(LDFLAGS)

# Run tests
test: $(TEST_BINS)
	@echo "Running tests..."
	@for test in $(TEST_BINS); do \
		echo "\n>>> Running $$test"; \
		./$$test; \
	done

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)