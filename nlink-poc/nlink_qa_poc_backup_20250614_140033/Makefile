# =============================================================================
# NexusLink QA POC - Fixed Makefile with Proper Library Linking
# =============================================================================

CC = gcc
AR = ar
CFLAGS = -Wall -Wextra -std=c99 -fPIC -O2 -DNLINK_VERSION=\"1.0.0\" -DETPS_ENABLED=1 -DSEMVERX_ENABLED=1
DEBUG_FLAGS = -g -DDEBUG -O0
LDFLAGS = -shared
ARFLAGS = rcs

# Project configuration
PROJECT_NAME = nlink_qa_poc
LIB_NAME = nlink
VERSION = 1.0.0

# Directory structure
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = bin
LIB_DIR = lib

# Source files
CLI_SOURCES = $(wildcard $(SRC_DIR)/cli/*.c)
CORE_SOURCES = $(wildcard $(SRC_DIR)/core/*.c)
ETPS_SOURCES = $(wildcard $(SRC_DIR)/etps/*.c)
MAIN_SOURCE = $(SRC_DIR)/main.c
NLINK_CORE_SOURCE = $(SRC_DIR)/nlink.c

# Object files
CLI_OBJECTS = $(CLI_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
CORE_OBJECTS = $(CORE_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
ETPS_OBJECTS = $(ETPS_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
NLINK_CORE_OBJECT = $(NLINK_CORE_SOURCE:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
ALL_OBJECTS = $(CLI_OBJECTS) $(CORE_OBJECTS) $(ETPS_OBJECTS) $(NLINK_CORE_OBJECT)

# Library targets - FIXED: Use standard naming convention
STATIC_LIB = $(LIB_DIR)/lib$(LIB_NAME).a
SHARED_LIB = $(LIB_DIR)/lib$(LIB_NAME).so.$(VERSION)
SHARED_LIB_LINK = $(LIB_DIR)/lib$(LIB_NAME).so

# Executable
CLI_EXECUTABLE = $(BIN_DIR)/nlink

# Include paths
INCLUDE_PATHS = -I$(INCLUDE_DIR)

.PHONY: all clean debug release directories help

# Default target
all: release

# Production build
release: CFLAGS += -DNDEBUG -O2
release: directories $(STATIC_LIB) $(SHARED_LIB) $(CLI_EXECUTABLE)

# Debug build
debug: CFLAGS += $(DEBUG_FLAGS)
debug: directories $(STATIC_LIB) $(SHARED_LIB) $(CLI_EXECUTABLE)

# Create directories
directories:
	@mkdir -p $(OBJ_DIR)/cli $(OBJ_DIR)/core $(OBJ_DIR)/etps
	@mkdir -p $(BIN_DIR) $(LIB_DIR)

# Static library
$(STATIC_LIB): $(ALL_OBJECTS)
	@echo "📦 Creating static library: $@"
	$(AR) $(ARFLAGS) $@ $^

# Shared library
$(SHARED_LIB): $(ALL_OBJECTS)
	@echo "🔗 Creating shared library: $@"
	$(CC) $(LDFLAGS) -Wl,-soname,lib$(LIB_NAME).so.$(VERSION) -o $@ $^
	@ln -sf lib$(LIB_NAME).so.$(VERSION) $(SHARED_LIB_LINK)

# CLI executable - FIXED: Use correct library path
$(CLI_EXECUTABLE): $(MAIN_SOURCE) $(STATIC_LIB)
	@echo "⚡ Building CLI executable: $@"
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -o $@ $< -L$(LIB_DIR) -l$(LIB_NAME)

# Object compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "🔨 Compiling: $<"
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Clean
clean:
	@echo "🧹 Cleaning build artifacts"
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(LIB_DIR)
	rm -f *.log *.json

# Test
test: all
	@echo "🧪 Testing ETPS functionality"
	LD_LIBRARY_PATH=$(LIB_DIR) ./$(CLI_EXECUTABLE) --etps-test

# Help
help:
	@echo "Available targets:"
	@echo "  all      - Build everything (default)"
	@echo "  release  - Production build"
	@echo "  debug    - Debug build"
	@echo "  clean    - Clean artifacts"
	@echo "  test     - Run tests"
	@echo "  help     - Show this help"
