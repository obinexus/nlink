# Makefile for NexusLink Advanced Integration
# Author: Implementation Team

CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -ldl -lpthread

SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = test
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Source files - explicitly define to ensure order and completeness
SRC_FILES = $(SRC_DIR)/nexus_json.c $(SRC_DIR)/nexus_semver.c \
            $(SRC_DIR)/nexus_versioned_symbols.c $(SRC_DIR)/nexus_enhanced_metadata.c \
            $(SRC_DIR)/nexus_lazy_versioned.c

OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Test files
TEST_DIAMOND = test_diamond_dependency
TEST_VERSIONED = test_versioned_integration

# Target for demo build
.PHONY: all clean test demo diamond versioned

all: directories $(BUILD_DIR)/$(TEST_VERSIONED)

directories:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Build diamond dependency test
$(BUILD_DIR)/$(TEST_DIAMOND): $(TEST_DIR)/$(TEST_DIAMOND).c $(OBJ_FILES)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $^ -o $@ $(LDFLAGS)

# Build versioned integration test
$(BUILD_DIR)/$(TEST_VERSIONED): $(TEST_DIR)/$(TEST_VERSIONED).c $(OBJ_FILES)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $^ -o $@ $(LDFLAGS)

# Run diamond dependency test
diamond: $(BUILD_DIR)/$(TEST_DIAMOND)
	./$(BUILD_DIR)/$(TEST_DIAMOND)

# Run versioned integration test
versioned: $(BUILD_DIR)/$(TEST_VERSIONED)
	./$(BUILD_DIR)/$(TEST_VERSIONED)

# Run all tests
test: diamond versioned

# Demo target
demo: versioned
	@echo "Running NexusLink Versioned Integration Demo..."
	@echo "-----------------------------------------------"
	./$(BUILD_DIR)/$(TEST_VERSIONED)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)