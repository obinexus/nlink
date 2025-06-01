# Makefile for NexusLink CLI Configuration Parser
# Aegis Project - Phase 1 Implementation
# Author: Nnamdi Michael Okpala & Development Team

# =============================================================================
# BUILD CONFIGURATION
# =============================================================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -O2 -g
LDFLAGS = -lpthread
INCLUDES = -I./include

# Project structure
TARGET = nlink
BUILD_DIR = build
SRC_DIRS = core cli
INCLUDE_DIRS = include/core include/cli

# Source files
CORE_SOURCES = core/config.c
CLI_SOURCES = cli/parser_interface.c
ALL_SOURCES = $(CORE_SOURCES) $(CLI_SOURCES)

# Object files
CORE_OBJECTS = $(BUILD_DIR)/core/config.o
CLI_OBJECTS = $(BUILD_DIR)/cli/parser_interface.o
ALL_OBJECTS = $(CORE_OBJECTS) $(CLI_OBJECTS)

# Header dependencies
CORE_HEADERS = include/core/config.h
CLI_HEADERS = include/cli/parser_interface.h
ALL_HEADERS = $(CORE_HEADERS) $(CLI_HEADERS)

# =============================================================================
# BUILD TARGETS
# =============================================================================

# Default target
.PHONY: all
all: $(TARGET)

# Main executable target
$(TARGET): $(ALL_OBJECTS)
	@echo "[NLINK BUILD] Linking executable: $(TARGET)"
	$(CC) $(ALL_OBJECTS) $(LDFLAGS) -o $(TARGET)
	@echo "[NLINK SUCCESS] Build completed: $(TARGET)"

# Core module compilation
$(BUILD_DIR)/core/config.o: core/config.c $(CORE_HEADERS) | $(BUILD_DIR)/core
	@echo "[NLINK BUILD] Compiling core configuration module"
	$(CC) $(CFLAGS) $(INCLUDES) -c core/config.c -o $(BUILD_DIR)/core/config.o

# CLI module compilation
$(BUILD_DIR)/cli/parser_interface.o: cli/parser_interface.c $(CLI_HEADERS) $(CORE_HEADERS) | $(BUILD_DIR)/cli
	@echo "[NLINK BUILD] Compiling CLI parser interface module"
	$(CC) $(CFLAGS) $(INCLUDES) -c cli/parser_interface.c -o $(BUILD_DIR)/cli/parser_interface.o

# Build directory creation
$(BUILD_DIR)/core:
	@echo "[NLINK BUILD] Creating build directories"
	mkdir -p $(BUILD_DIR)/core

$(BUILD_DIR)/cli:
	mkdir -p $(BUILD_DIR)/cli

# =============================================================================
# DEVELOPMENT AND TESTING TARGETS
# =============================================================================

# Debug build with additional symbols
.PHONY: debug
debug: CFLAGS += -DDEBUG -g3 -O0
debug: $(TARGET)

# Release build with optimizations
.PHONY: release
release: CFLAGS += -DNDEBUG -O3 -march=native
release: $(TARGET)

# Static analysis and code quality
.PHONY: analyze
analyze:
	@echo "[NLINK ANALYZE] Running static analysis"
	@command -v cppcheck >/dev/null 2>&1 || { echo "cppcheck not found, skipping analysis"; exit 0; }
	cppcheck --enable=all --std=c99 $(INCLUDES) $(ALL_SOURCES)

# Code formatting with clang-format
.PHONY: format
format:
	@echo "[NLINK FORMAT] Formatting source code"
	@command -v clang-format >/dev/null 2>&1 || { echo "clang-format not found, skipping formatting"; exit 0; }
	clang-format -i $(ALL_SOURCES) $(ALL_HEADERS)

# =============================================================================
# TESTING AND VALIDATION TARGETS
# =============================================================================

# Functional testing with sample configurations
.PHONY: test
test: $(TARGET)
	@echo "[NLINK TEST] Running functional validation tests"
	@./scripts/run_tests.sh

# Integration testing with verbose output
.PHONY: test-verbose
test-verbose: $(TARGET)
	@echo "[NLINK TEST] Running comprehensive integration tests"
	@./scripts/run_tests.sh --verbose

# Configuration validation test
.PHONY: test-config
test-config: $(TARGET)
	@echo "[NLINK TEST] Testing configuration parsing"
	./$(TARGET) --config-check --verbose || true

# Component discovery test  
.PHONY: test-discovery
test-discovery: $(TARGET)
	@echo "[NLINK TEST] Testing component discovery"
	./$(TARGET) --discover-components --verbose || true

# Threading validation test
.PHONY: test-threading
test-threading: $(TARGET)
	@echo "[NLINK TEST] Testing threading configuration"
	./$(TARGET) --validate-threading --verbose || true

# =============================================================================
# INSTALLATION AND PACKAGING TARGETS
# =============================================================================

# Installation directories
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
INCLUDEDIR = $(PREFIX)/include/nlink
DOCDIR = $(PREFIX)/share/doc/nlink

# Install target
.PHONY: install
install: $(TARGET)
	@echo "[NLINK INSTALL] Installing to $(PREFIX)"
	install -d $(BINDIR)
	install -m 755 $(TARGET) $(BINDIR)
	install -d $(INCLUDEDIR)/core
	install -d $(INCLUDEDIR)/cli
	install -m 644 include/core/*.h $(INCLUDEDIR)/core/
	install -m 644 include/cli/*.h $(INCLUDEDIR)/cli/
	@echo "[NLINK SUCCESS] Installation completed"

# Uninstall target
.PHONY: uninstall
uninstall:
	@echo "[NLINK UNINSTALL] Removing installation"
	rm -f $(BINDIR)/$(TARGET)
	rm -rf $(INCLUDEDIR)
	@echo "[NLINK SUCCESS] Uninstallation completed"

# Package creation
.PHONY: package
package: clean release
	@echo "[NLINK PACKAGE] Creating distribution package"
	@./scripts/create_package.sh

# =============================================================================
# MAINTENANCE AND CLEANUP TARGETS
# =============================================================================

# Clean build artifacts
.PHONY: clean
clean:
	@echo "[NLINK CLEAN] Removing build artifacts"
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)
	rm -f *.o core/*.o cli/*.o
	@echo "[NLINK SUCCESS] Cleanup completed"

# Deep clean including configuration files
.PHONY: distclean
distclean: clean
	@echo "[NLINK DISTCLEAN] Performing deep cleanup"
	rm -f config.log config.cache
	rm -rf autom4te.cache
	find . -name "*~" -delete
	find . -name "*.orig" -delete
	find . -name "*.rej" -delete

# Show build configuration
.PHONY: config
config:
	@echo "=== NexusLink Build Configuration ==="
	@echo "Target: $(TARGET)"
	@echo "Compiler: $(CC)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "Includes: $(INCLUDES)"
	@echo "Sources: $(ALL_SOURCES)"
	@echo "Objects: $(ALL_OBJECTS)"
	@echo "Headers: $(ALL_HEADERS)"
	@echo "=================================="

# =============================================================================
# HELP AND DOCUMENTATION
# =============================================================================

# Display available targets
.PHONY: help
help:
	@echo "NexusLink CLI Configuration Parser - Build System"
	@echo "Aegis Project Phase 1 Implementation"
	@echo ""
	@echo "Available targets:"
	@echo "  all          - Build the main executable (default)"
	@echo "  debug        - Build with debug symbols and reduced optimization"
	@echo "  release      - Build with full optimization for production"
	@echo "  clean        - Remove build artifacts and executables"
	@echo "  distclean    - Deep clean including temporary files"
	@echo "  test         - Run functional validation tests"
	@echo "  test-verbose - Run comprehensive integration tests"
	@echo "  test-config  - Test configuration parsing specifically"
	@echo "  test-discovery - Test component discovery functionality"
	@echo "  test-threading - Test threading configuration validation"
	@echo "  analyze      - Run static code analysis (requires cppcheck)"
	@echo "  format       - Format source code (requires clang-format)"
	@echo "  install      - Install to system (default: /usr/local)"
	@echo "  uninstall    - Remove system installation"
	@echo "  package      - Create distribution package"
	@echo "  config       - Display build configuration"
	@echo "  help         - Display this help message"
	@echo ""
	@echo "Environment variables:"
	@echo "  CC           - Compiler to use (default: gcc)"
	@echo "  PREFIX       - Installation prefix (default: /usr/local)"
	@echo "  CFLAGS       - Additional compiler flags"
	@echo "  LDFLAGS      - Additional linker flags"

# =============================================================================
# DEPENDENCY MANAGEMENT
# =============================================================================

# Automatic dependency generation
-include $(ALL_OBJECTS:.o=.d)

$(BUILD_DIR)/%.d: %.c | $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -MM -MT $(@:.d=.o) $< > $@

# Force rebuild on Makefile changes
$(ALL_OBJECTS): Makefile

# =============================================================================
# PLATFORM-SPECIFIC CONFIGURATIONS
# =============================================================================

# Windows-specific adjustments
ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
    LDFLAGS += -lws2_32
    RM = del /Q
    MKDIR = mkdir
else
    # Unix-like systems
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        LDFLAGS += -lrt
    endif
    ifeq ($(UNAME_S),Darwin)
        # macOS-specific flags if needed
    endif
endif
