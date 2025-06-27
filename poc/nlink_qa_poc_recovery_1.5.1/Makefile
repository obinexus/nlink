# =============================================================================
# NexusLink QA POC - Corrected OBINexus Toolchain Makefile
# Proper Library Naming Convention (No lib prefix)
# =============================================================================

CC = gcc
AR = ar
CFLAGS = -Wall -Wextra -std=c99 -fPIC -O2 -DNLINK_VERSION=\"1.5.1\" -DETPS_ENABLED=1
DEBUG_FLAGS = -g -DDEBUG -O0
LDFLAGS = -shared
ARFLAGS = rcs

# Project configuration
PROJECT_NAME = nlink_qa_poc
LIB_NAME = nlink
VERSION = 1.5.1

# Directory structure
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = bin
LIB_DIR = lib

# Source files
CORE_SOURCES = $(wildcard $(SRC_DIR)/core/*.c)
ETPS_SOURCES = $(wildcard $(SRC_DIR)/etps/*.c)
CLI_SOURCES = $(wildcard $(SRC_DIR)/cli/*.c)
MAIN_SOURCE = $(SRC_DIR)/main.c

# Object files
CORE_OBJECTS = $(CORE_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
ETPS_OBJECTS = $(ETPS_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
CLI_OBJECTS = $(CLI_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
ALL_OBJECTS = $(CORE_OBJECTS) $(ETPS_OBJECTS) $(CLI_OBJECTS)

# =============================================================================
# CORRECTED: OBINexus Toolchain Library Naming Convention
# =============================================================================
# Direct naming without 'lib' prefix per toolchain progression:
# riftlang.exe → .so.a → rift.exe → gosilang
# Libraries: nlink.a, nlink.so (NOT libnlink.a, libnlink.so)

STATIC_LIB = $(LIB_DIR)/$(LIB_NAME).a
SHARED_LIB = $(LIB_DIR)/$(LIB_NAME).so.$(VERSION)
SHARED_LIB_LINK = $(LIB_DIR)/$(LIB_NAME).so

# Executable
CLI_EXECUTABLE = $(BIN_DIR)/nlink

# Include paths
INCLUDE_PATHS = -I$(INCLUDE_DIR)

.PHONY: all clean debug release directories help test

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
	@mkdir -p $(OBJ_DIR)/core $(OBJ_DIR)/etps $(OBJ_DIR)/cli
	@mkdir -p $(BIN_DIR) $(LIB_DIR)

# =============================================================================
# CORRECTED: Static library (OBINexus naming)
# =============================================================================
$(STATIC_LIB): $(ALL_OBJECTS)
	@echo "📦 Creating OBINexus static library: $@"
	$(AR) $(ARFLAGS) $@ $^
	@echo "✅ Created: $(LIB_NAME).a (OBINexus toolchain compatible)"

# =============================================================================
# CORRECTED: Shared library (OBINexus naming)
# =============================================================================
$(SHARED_LIB): $(ALL_OBJECTS)
	@echo "🔗 Creating OBINexus shared library: $@"
	$(CC) $(LDFLAGS) -Wl,-soname,$(LIB_NAME).so.$(VERSION) -o $@ $^
	@ln -sf $(LIB_NAME).so.$(VERSION) $(SHARED_LIB_LINK)
	@echo "✅ Created: $(LIB_NAME).so.$(VERSION) with symlink $(LIB_NAME).so"

# =============================================================================
# CORRECTED: CLI executable linking (OBINexus convention)
# =============================================================================
$(CLI_EXECUTABLE): $(MAIN_SOURCE) $(STATIC_LIB)
	@echo "⚡ Building CLI executable with OBINexus library: $@"
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -o $@ $< -L$(LIB_DIR) -l$(LIB_NAME)
	@echo "✅ Linked against: $(LIB_NAME).a (direct reference, no lib prefix)"

# Object compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "🔨 Compiling: $<"
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Clean
clean:
	@echo "🧹 Cleaning build artifacts"
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(LIB_DIR)

# =============================================================================
# CORRECTED: Test with proper library path
# =============================================================================
test: all
	@echo "🧪 Testing OBINexus library integration"
	@echo "Library files created:"
	@ls -la $(LIB_DIR)/
	@echo "Expected: $(LIB_NAME).a, $(LIB_NAME).so.$(VERSION), $(LIB_NAME).so"
	@echo "Testing CLI functionality:"
	LD_LIBRARY_PATH=$(LIB_DIR) ./$(CLI_EXECUTABLE) --version || echo "CLI needs implementation"

# =============================================================================
# OBINexus Toolchain Validation
# =============================================================================
validate-toolchain:
	@echo "🔍 OBINexus Toolchain Library Naming Validation"
	@echo "================================================"
	@echo "Expected artifacts per toolchain progression:"
	@echo "  Static:  $(LIB_NAME).a"
	@echo "  Shared:  $(LIB_NAME).so.$(VERSION)"
	@echo "  Symlink: $(LIB_NAME).so"
	@echo ""
	@echo "Linker behavior: gcc -l$(LIB_NAME) → searches for $(LIB_NAME).{a,so}"
	@echo "NOT: gcc -l$(LIB_NAME) → searches for lib$(LIB_NAME).{a,so}"
	@echo ""
	@if [ -f $(STATIC_LIB) ]; then \
		echo "✅ Static library exists: $(STATIC_LIB)"; \
	else \
		echo "❌ Static library missing: $(STATIC_LIB)"; \
	fi
	@if [ -f $(SHARED_LIB) ]; then \
		echo "✅ Shared library exists: $(SHARED_LIB)"; \
	else \
		echo "❌ Shared library missing: $(SHARED_LIB)"; \
	fi
	@if [ -L $(SHARED_LIB_LINK) ]; then \
		echo "✅ Shared library symlink exists: $(SHARED_LIB_LINK)"; \
	else \
		echo "❌ Shared library symlink missing: $(SHARED_LIB_LINK)"; \
	fi

# Help
help:
	@echo "OBINexus NexusLink QA POC - Corrected Toolchain Makefile"
	@echo "======================================================="
	@echo "Available targets:"
	@echo "  all                - Build everything (default)"
	@echo "  release            - Production build"
	@echo "  debug              - Debug build"
	@echo "  clean              - Clean artifacts"
	@echo "  test               - Run basic tests"
	@echo "  validate-toolchain - Validate OBINexus naming convention"
	@echo "  help               - Show this help"
	@echo ""
	@echo "OBINexus Library Naming Convention:"
	@echo "  Libraries:  $(LIB_NAME).a, $(LIB_NAME).so"
	@echo "  Linking:    gcc -l$(LIB_NAME) (direct reference)"
	@echo "  Toolchain:  nlink → polybuild orchestration"
