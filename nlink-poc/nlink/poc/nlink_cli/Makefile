# NexusLink CLI Enhanced Makefile with Unit Testing Support
# Aegis Project Phase 1 Implementation
# Quality-First Development with Comprehensive Testing

# =============================================================================
# BUILD CONFIGURATION
# =============================================================================

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread -I./include
LDFLAGS = -pthread
DEBUG_FLAGS = -g -DDEBUG -O0
RELEASE_FLAGS = -O2 -DNDEBUG

# Directories
SRC_DIR = .
BUILD_DIR = build
LIB_DIR = lib
BIN_DIR = bin
TEST_DIR = test
UNIT_TEST_DIR = $(TEST_DIR)/unit
INCLUDE_DIR = include

# Source files
CORE_SOURCES = core/config.c
CLI_SOURCES = cli/parser_interface.c
MAIN_SOURCE = main.c

# Object files
CORE_OBJECTS = $(BUILD_DIR)/core/config.o
CLI_OBJECTS = $(BUILD_DIR)/cli/parser_interface.o
MAIN_OBJECT = $(BUILD_DIR)/main.o

# Test sources
TEST_SOURCES = $(UNIT_TEST_DIR)/test_config.c \
               $(UNIT_TEST_DIR)/test_cli_parser.c \
               $(UNIT_TEST_DIR)/test_enhanced_metadata.c

# Test executables
TEST_EXECUTABLES = $(BUILD_DIR)/test/test_config \
                   $(BUILD_DIR)/test/test_cli_parser \
                   $(BUILD_DIR)/test/test_enhanced_metadata

# Library files
STATIC_LIB = $(LIB_DIR)/libnlink.a
SHARED_LIB = $(LIB_DIR)/libnlink.so
EXECUTABLE = $(BIN_DIR)/nlink

# =============================================================================
# DEFAULT TARGET
# =============================================================================

.PHONY: all
all: directories $(STATIC_LIB) $(EXECUTABLE)

# =============================================================================
# DIRECTORY CREATION
# =============================================================================

.PHONY: directories
directories:
	@mkdir -p $(BUILD_DIR)/core $(BUILD_DIR)/cli $(BUILD_DIR)/test
	@mkdir -p $(LIB_DIR) $(BIN_DIR)
	@mkdir -p $(UNIT_TEST_DIR)

# =============================================================================
# COMPILATION TARGETS
# =============================================================================

# Core library objects
$(BUILD_DIR)/core/config.o: core/config.c $(INCLUDE_DIR)/core/config.h
	@echo "[BUILD] Compiling core/config.c"
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -c $< -o $@

# CLI interface objects
$(BUILD_DIR)/cli/parser_interface.o: cli/parser_interface.c $(INCLUDE_DIR)/cli/parser_interface.h
	@echo "[BUILD] Compiling cli/parser_interface.c"
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -c $< -o $@

# Main executable object
$(BUILD_DIR)/main.o: main.c
	@echo "[BUILD] Compiling main.c"
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -c $< -o $@

# =============================================================================
# LIBRARY TARGETS
# =============================================================================

# Static library
$(STATIC_LIB): $(CORE_OBJECTS) $(CLI_OBJECTS)
	@echo "[NLINK SUCCESS] Creating static library: $@"
	ar rcs $@ $^

# Shared library
$(SHARED_LIB): $(CORE_OBJECTS) $(CLI_OBJECTS)
	@echo "[NLINK SUCCESS] Creating shared library: $@"
	$(CC) -shared -o $@ $^ $(LDFLAGS)

# =============================================================================
# EXECUTABLE TARGETS
# =============================================================================

# Main executable (statically linked)
$(EXECUTABLE): $(MAIN_OBJECT) $(STATIC_LIB)
	@echo "[NLINK SUCCESS] Creating static executable: $@"
	$(CC) -o $@ $< -L$(LIB_DIR) -lnlink $(LDFLAGS)

# =============================================================================
# UNIT TEST TARGETS
# =============================================================================

# Configuration parser unit tests
$(BUILD_DIR)/test/test_config: $(UNIT_TEST_DIR)/test_config.c $(STATIC_LIB)
	@echo "[TEST BUILD] Compiling configuration unit tests"
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o $@ $< -L$(LIB_DIR) -lnlink $(LDFLAGS)

# CLI parser unit tests
$(BUILD_DIR)/test/test_cli_parser: $(UNIT_TEST_DIR)/test_cli_parser.c $(STATIC_LIB)
	@echo "[TEST BUILD] Compiling CLI parser unit tests"
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o $@ $< -L$(LIB_DIR) -lnlink $(LDFLAGS)

# Enhanced metadata unit tests
$(BUILD_DIR)/test/test_enhanced_metadata: $(UNIT_TEST_DIR)/test_enhanced_metadata.c $(STATIC_LIB)
	@echo "[TEST BUILD] Compiling enhanced metadata unit tests"
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o $@ $< -L$(LIB_DIR) -lnlink $(LDFLAGS)

# =============================================================================
# QUALITY ASSURANCE TARGETS
# =============================================================================

.PHONY: test-unit
test-unit: $(TEST_EXECUTABLES)
	@echo "=== Running Unit Test Suite ==="
	@for test in $(TEST_EXECUTABLES); do \
		echo "Running $$test..."; \
		./$$test || exit 1; \
	done
	@echo "✅ All unit tests passed"

.PHONY: test-integration
test-integration: $(EXECUTABLE)
	@echo "=== Running Integration Tests ==="
	./scripts/run_tests.sh --verbose
	@echo "✅ Integration tests completed"

.PHONY: test-all
test-all: test-unit test-integration
	@echo "✅ Complete test suite finished successfully"

.PHONY: validate
validate: $(EXECUTABLE) test-unit
	@echo "=== NexusLink Quality Validation ==="
	@echo "[VALIDATE] Checking executable dependencies"
	@ldd $(EXECUTABLE) | grep -E "(not found|error)" && exit 1 || echo "✅ Dependencies verified"
	@echo "[VALIDATE] Checking library symbols"
	@nm $(STATIC_LIB) | grep -q "nlink_config_init" || (echo "❌ Missing required symbols" && exit 1)
	@echo "✅ Symbol exports verified"
	@echo "[VALIDATE] Running basic functionality test"
	@./$(EXECUTABLE) --help > /dev/null || (echo "❌ Basic functionality failed" && exit 1)
	@echo "✅ Basic functionality verified"
	@echo "[VALIDATE] Running unit tests"
	@$(MAKE) test-unit
	@echo "✅ All validation checks passed"

# =============================================================================
# BUILD VARIANTS
# =============================================================================

.PHONY: debug
debug: CFLAGS += $(DEBUG_FLAGS)
debug: all

.PHONY: release
release: CFLAGS += $(RELEASE_FLAGS)
release: all

.PHONY: all-variants
all-variants: $(STATIC_LIB) $(SHARED_LIB) $(EXECUTABLE)

# =============================================================================
# DEVELOPMENT TARGETS
# =============================================================================

.PHONY: symbols
symbols: $(STATIC_LIB)
	@echo "=== Static Library Symbols ==="
	@nm $(STATIC_LIB) | grep -E "^[0-9a-fA-F]+ [T] " | cut -d' ' -f3 | sort
	@echo "=========================="

.PHONY: analyze
analyze:
	@echo "=== Static Analysis ==="
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --std=c99 $(CORE_SOURCES) $(CLI_SOURCES) $(MAIN_SOURCE); \
	else \
		echo "cppcheck not available, skipping static analysis"; \
	fi

.PHONY: format
format:
	@echo "=== Code Formatting ==="
	@if command -v clang-format >/dev/null 2>&1; then \
		find . -name "*.c" -o -name "*.h" | xargs clang-format -i; \
		echo "✅ Code formatted"; \
	else \
		echo "clang-format not available, skipping formatting"; \
	fi

# =============================================================================
# QUALITY GATE ENFORCEMENT
# =============================================================================

.PHONY: quality-gate
quality-gate: clean all validate test-all
	@echo "🎯 QUALITY GATE PASSED - Ready for Integration"
	@echo "=====================================​"
	@echo "✅ Clean build successful"
	@echo "✅ All unit tests passed"
	@echo "✅ All integration tests passed"  
	@echo "✅ Static executable verified"
	@echo "✅ Library symbols validated"
	@echo "✅ Dependencies checked"
	@echo "====================================="

.PHONY: pre-commit
pre-commit: format analyze quality-gate
	@echo "🚀 PRE-COMMIT VALIDATION COMPLETE"
	@echo "Ready for version control commit"

# =============================================================================
# INSTALLATION TARGETS
# =============================================================================

PREFIX ?= /usr/local
INSTALL_BIN_DIR = $(PREFIX)/bin
INSTALL_LIB_DIR = $(PREFIX)/lib
INSTALL_INCLUDE_DIR = $(PREFIX)/include

.PHONY: install
install: all
	@echo "Installing NexusLink CLI..."
	@mkdir -p $(INSTALL_BIN_DIR) $(INSTALL_LIB_DIR) $(INSTALL_INCLUDE_DIR)
	@cp $(EXECUTABLE) $(INSTALL_BIN_DIR)/
	@cp $(STATIC_LIB) $(INSTALL_LIB_DIR)/
	@cp -r $(INCLUDE_DIR)/nlink $(INSTALL_INCLUDE_DIR)/
	@echo "✅ Installation completed to $(PREFIX)"

.PHONY: uninstall
uninstall:
	@echo "Uninstalling NexusLink CLI..."
	@rm -f $(INSTALL_BIN_DIR)/nlink
	@rm -f $(INSTALL_LIB_DIR)/libnlink.a
	@rm -rf $(INSTALL_INCLUDE_DIR)/nlink
	@echo "✅ Uninstallation completed"

# =============================================================================
# CLEANUP TARGETS
# =============================================================================

.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(LIB_DIR)
	@rm -rf $(BIN_DIR)
	@echo "✅ Clean completed"

.PHONY: distclean
distclean: clean
	@echo "Deep cleaning all generated files..."
	@find . -name "*.o" -delete
	@find . -name "*.a" -delete
	@find . -name "*.so" -delete
	@find . -name "*~" -delete
	@echo "✅ Distribution clean completed"

# =============================================================================
# HELP TARGET
# =============================================================================

.PHONY: help
help:
	@echo "NexusLink CLI Build System"
	@echo "=========================="
	@echo ""
	@echo "Build Targets:"
	@echo "  all              - Build static library and executable (default)"
	@echo "  all-variants     - Build both static and shared library variants"
	@echo "  debug            - Build with debug symbols and reduced optimization"
	@echo "  release          - Build with full optimization (default)"
	@echo ""
	@echo "Testing Targets:"
	@echo "  test-unit        - Run unit tests"
	@echo "  test-integration - Run integration tests"
	@echo "  test-all         - Run complete test suite"
	@echo "  validate         - Comprehensive validation of build artifacts"
	@echo ""
	@echo "Quality Assurance:"
	@echo "  quality-gate     - Complete quality validation pipeline"
	@echo "  pre-commit       - Pre-commit validation with formatting"
	@echo "  analyze          - Static code analysis"
	@echo "  format           - Code formatting with clang-format"
	@echo "  symbols          - Display library symbol exports"
	@echo ""
	@echo "Installation:"
	@echo "  install          - Install system-wide (PREFIX=/usr/local)"
	@echo "  uninstall        - Remove system-wide installation"
	@echo ""
	@echo "Cleanup:"
	@echo "  clean            - Remove build artifacts"
	@echo "  distclean        - Deep clean all generated files"
	@echo ""
	@echo "Examples:"
	@echo "  make quality-gate              # Full quality validation"
	@echo "  make test-all                  # Run all tests"
	@echo "  make install PREFIX=/opt/nlink # Custom installation path"
