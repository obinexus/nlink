# =============================================================================
# NexusLink QA POC - Enhanced Makefile with SemVerX ETPS Integration
# =============================================================================

CC = gcc
AR = ar
CFLAGS = -Wall -Wextra -std=c99 -fPIC -O2 -DNLINK_VERSION=\"1.0.0\" -DETPS_ENABLED=1 -DSEMVERX_ENABLED=1
DEBUG_FLAGS = -g -DDEBUG -O0 -DETPS_DEBUG_MODE=1
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

# Source files (enhanced for SemVerX)
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

# Library targets
STATIC_LIB = $(LIB_DIR)/$(LIB_NAME).a
SHARED_LIB = $(LIB_DIR)/$(LIB_NAME).so.$(VERSION)
SHARED_LIB_LINK = $(LIB_DIR)/$(LIB_NAME).so

# Executable
CLI_EXECUTABLE = $(BIN_DIR)/nlink

# Include paths
INCLUDE_PATHS = -I$(INCLUDE_DIR)

.PHONY: all clean test debug release directories help semverx-test

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
	$(CC) $(LDFLAGS) -Wl,-soname,$(LIB_NAME).so.$(VERSION) -o $@ $^
	@ln -sf $(LIB_NAME).so.$(VERSION) $(SHARED_LIB_LINK)

# CLI executable
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

# SemVerX testing
semverx-test: all
	@echo "🧪 Running SemVerX ETPS tests"
	LD_LIBRARY_PATH=$(LIB_DIR) ./$(CLI_EXECUTABLE) --etps-test
	LD_LIBRARY_PATH=$(LIB_DIR) ./$(CLI_EXECUTABLE) --validate-compatibility
	LD_LIBRARY_PATH=$(LIB_DIR) ./$(CLI_EXECUTABLE) --semverx-status

# Version info
version:
	@./$(CLI_EXECUTABLE) --version

# Help
help:
<<<<<<< HEAD
<<<<<<< HEAD
	@echo "NexusLink C Library Build System with ETPS Integration"
	@echo "====================================================="
	@echo ""
<<<<<<< HEAD
	@echo "Build Targets:"
	@echo "  all              - Production build (default)"
	@echo "  release          - Optimized production build"
	@echo "  debug            - Debug build with symbols"
	@echo "  clean            - Remove build artifacts"
	@echo "  distclean        - Deep clean all generated files"
<<<<<<< HEAD

# =============================================================================
# Enhanced Integration Test Targets
# =============================================================================

integration-tests-verbose: $(INTEGRATION_TESTS)
	@echo "[INTEGRATION TESTS] Running verbose integration test suite..."
	@$(INTEGRATION_TESTS) || echo "⚠️  Some integration tests failed - check output above"

test-python-only:
	@echo "[PYTHON TEST] Testing Python package isolation..."
	@cd examples/python-package && python3 -c "import sys; sys.path.insert(0, 'src'); from nlink_marshal import create_marshaller; m = create_marshaller(); print('✅ Python package working')"

test-dependencies:
	@echo "[DEPENDENCY CHECK] Checking system dependencies..."
	@python3 --version
	@command -v javac >/dev/null 2>&1 && echo "✅ Java compiler available" || echo "⚠️  Java compiler not found"
	@python3 -c "import Cython; print('✅ Cython available')" 2>/dev/null || echo "⚠️  Cython not available"

integration-diagnostic: test-dependencies test-python-only integration-tests-verbose
	@echo "[DIAGNOSTIC] Integration test diagnostic completed"
=======
	@echo ""
	@echo "Library Targets:"
	@echo "  $(STATIC_LIB)   - Static library"
	@echo "  $(SHARED_LIB)   - Shared library"
	@echo "  $(CLI_EXECUTABLE)        - CLI executable"
	@echo ""
	@echo "Installation:"
	@echo "  install          - Install system-wide"
	@echo "  uninstall        - Remove system installation"
	@echo ""
	@echo "Development:"
	@echo "  example          - Demonstrate library usage"
=======
	@echo "Main Targets:"
	@echo "  all              - Build everything (default)"
	@echo "  release          - Production build"
	@echo "  debug            - Debug build with ETPS telemetry"
	@echo "  test             - Build unit tests"
	@echo "  test-run         - Build and run unit tests"
	@echo "  example          - Build example program with ETPS"
	@echo "  examples         - Build all marshalling artifact CLIs"
	@echo "  integration-test - Run cross-language integration tests"
	@echo "  etps-validation  - Run Error Telemetry Point System validation"
>>>>>>> dev
	@echo "  info             - Show build information"
	@echo "  clean            - Remove build artifacts"
	@echo "  help             - Show this help"
	@echo ""
	@echo "FIXED Library Usage:"
	@echo "  gcc -I$(INCLUDE_DIR) -L$(LIB_DIR) program.c -l$(LIB_NAME)"
<<<<<<< HEAD
	@echo "  #include <$(PROJECT_NAME)/cli/parser.h>"
	@echo "  #include <$(PROJECT_NAME)/core/config.h>"

# =============================================================================
# Dependency tracking (automatic header dependencies)
# =============================================================================

-include $(ALL_OBJECTS:.o=.d)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDE_PATHS) -MM -MT $(@:.d=.o) $< > $@
>>>>>>> dev
=======
	@echo ""
	@echo "ETPS Features:"
	@echo "  - GUID-based correlation for all operations"
	@echo "  - High-resolution timestamp tracking"
	@echo "  - Structured error telemetry (JSON output)"
	@echo "  - Cross-language binding validation"
	@echo "  - Zero-trust security integration"
>>>>>>> dev
=======
	@echo "Available targets:"
	@echo "  all      - Build everything (default)"
	@echo "  release  - Production build"
	@echo "  debug    - Debug build"
	@echo "  clean    - Clean build artifacts"
	@echo "  version  - Show version"
	@echo "  help     - Show this help"
>>>>>>> dev
=======
	@echo "📋 OBINexus NexusLink Build System"
	@echo "Available targets:"
	@echo "  all           - Build everything (default)"
	@echo "  release       - Production build"
	@echo "  debug         - Debug build"
	@echo "  clean         - Clean artifacts"
	@echo "  semverx-test  - Run SemVerX tests"
	@echo "  version       - Show version"
	@echo "  help          - Show this help"
>>>>>>> dev
