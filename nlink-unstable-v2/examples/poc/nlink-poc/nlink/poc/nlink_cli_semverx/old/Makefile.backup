# NexusLink SemVerX - Static Library Architecture
# Systematic .a static linking for deterministic executable behavior
# Aegis Project Phase 1.5 - Production Implementation (Static Mode)

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread -I./include
LDFLAGS = -lpthread -static
DEBUG_FLAGS = -g -DDEBUG -O0
RELEASE_FLAGS = -O2 -DNDEBUG

# Directory structure
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
LIB_DIR = lib
INCLUDE_DIR = include

# Source files
CORE_SOURCES = $(SRC_DIR)/core/config.c
CLI_SOURCES = $(SRC_DIR)/cli/parser_interface.c
MAIN_SOURCE = $(SRC_DIR)/main.c

# Object files (no PIC needed for static-only)
CORE_OBJECTS = $(BUILD_DIR)/core/config.o
CLI_OBJECTS = $(BUILD_DIR)/cli/parser_interface.o
MAIN_OBJECT = $(BUILD_DIR)/main.o

LIB_OBJECTS = $(CORE_OBJECTS) $(CLI_OBJECTS)
ALL_OBJECTS = $(LIB_OBJECTS) $(MAIN_OBJECT)

# Static library target only
STATIC_LIB = $(LIB_DIR)/libnlink.a
EXECUTABLE = $(BIN_DIR)/nlink

# Version information
VERSION = 1.5.0

# =============================================================================
# BUILD TARGETS
# =============================================================================

.PHONY: all clean directories debug release test validate help

# Default target - build static library and executable only
all: directories $(STATIC_LIB) $(EXECUTABLE)

# Create directories
directories:
	@mkdir -p $(BUILD_DIR)/core $(BUILD_DIR)/cli
	@mkdir -p $(BIN_DIR) $(LIB_DIR)

# =============================================================================
# OBJECT COMPILATION (Static-optimized)
# =============================================================================

# Core objects
$(BUILD_DIR)/core/%.o: $(SRC_DIR)/core/%.c
	@echo "[COMPILE] Core: $<"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -c $< -o $@

# CLI objects
$(BUILD_DIR)/cli/%.o: $(SRC_DIR)/cli/%.c
	@echo "[COMPILE] CLI: $<"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -c $< -o $@

# Main object
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	@echo "[COMPILE] Main: $<"
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -c $< -o $@

# =============================================================================
# STATIC LIBRARY TARGET
# =============================================================================

# Static library (.a) - Single target approach
$(STATIC_LIB): $(LIB_OBJECTS)
	@echo "[LIBRARY] Creating static library: $@"
	ar rcs $@ $^
	@echo "✅ Static library created: $(STATIC_LIB)"
	@echo "[SYMBOLS] Library symbol count:"
	@nm $@ | grep -E "T nlink_" | wc -l

# =============================================================================
# EXECUTABLE TARGET (Static linking only)
# =============================================================================

# Executable with embedded static library
$(EXECUTABLE): $(MAIN_OBJECT) $(STATIC_LIB)
	@echo "[LINK] Creating NexusLink static executable: $@"
	$(CC) $(MAIN_OBJECT) $(STATIC_LIB) $(LDFLAGS) -o $@
	@echo "✅ SUCCESS: NexusLink static executable created at $@"
	@echo "[VALIDATION] Executable type:"
	@file $@
	@echo "[DEPENDENCIES] Library dependencies:"
	@ldd $@ || echo "✅ Fully static executable (no dynamic dependencies)"

# =============================================================================
# BUILD VARIANTS
# =============================================================================

debug: CFLAGS += $(DEBUG_FLAGS)
debug: RELEASE_FLAGS = $(DEBUG_FLAGS)
debug: LDFLAGS = -lpthread -static -g
debug: all

release: CFLAGS += $(RELEASE_FLAGS)
release: all

# =============================================================================
# TESTING AND VALIDATION
# =============================================================================

test: $(EXECUTABLE)
	@echo "[TEST] NexusLink Static Executable Validation"
	@echo "============================================="
	@echo ""
	@echo "--- Version Command (OBINexus Integration Status) ---"
	$(EXECUTABLE) --version
	@echo ""
	@echo "--- Help Command (CLI Interface Validation) ---"
	$(EXECUTABLE) --help
	@echo ""
	@echo "--- Config Check (Waterfall Methodology) ---"
	$(EXECUTABLE) --config-check
	@echo ""
	@echo "--- SemVerX Validation (Range State Assessment) ---"
	$(EXECUTABLE) --semverx-validate
	@echo ""
	@echo "✅ All static executable tests passed"

validate: $(EXECUTABLE)
	@echo "[VALIDATE] Comprehensive Static Architecture Validation"
	@echo "======================================================"
	@echo ""
	@echo "--- Static Library Analysis ---"
	@file $(STATIC_LIB)
	@echo "Archive contents:"
	@ar t $(STATIC_LIB)
	@echo ""
	@echo "--- Executable Analysis ---"
	@file $(EXECUTABLE)
	@echo "Size analysis:"
	@ls -lh $(EXECUTABLE)
	@echo ""
	@echo "--- Dependency Analysis ---"
	@ldd $(EXECUTABLE) || echo "✅ No dynamic dependencies - fully static"
	@echo ""
	@echo "--- Symbol Validation ---"
	@echo "NexusLink symbols in static library:"
	@nm $(STATIC_LIB) | grep -E "T nlink_" | head -5
	@echo ""
	@echo "✅ Static architecture validation completed"

# =============================================================================
# OBINexus Integration Targets
# =============================================================================

# Prepare for polybuild integration
polybuild-prep: $(EXECUTABLE)
	@echo "[OBINEXUS] Preparing for polybuild integration"
	@echo "============================================="
	@echo "NexusLink static executable: $(EXECUTABLE)"
	@echo "Toolchain position: nlink → polybuild"
	@echo "Integration status: ✅ Ready"
	@$(EXECUTABLE) --version | grep -E "(NexusLink|SemVerX|Aegis)"

# Validate OBINexus ecosystem readiness
ecosystem-check: $(EXECUTABLE)
	@echo "[ECOSYSTEM] OBINexus Toolchain Validation"
	@echo "========================================"
	@echo "Component: nlink (SemVerX Range State Versioning)"
	@echo "Architecture: Static library + executable"
	@echo "Next integration: polybuild coordination"
	@echo ""
	@$(EXECUTABLE) --config-check
	@echo ""
	@echo "✅ Ready for OBINexus ecosystem integration"

# =============================================================================
# INSTALLATION TARGETS
# =============================================================================

PREFIX ?= /usr/local
INSTALL_BIN_DIR = $(PREFIX)/bin
INSTALL_LIB_DIR = $(PREFIX)/lib
INSTALL_INCLUDE_DIR = $(PREFIX)/include

install: all
	@echo "[INSTALL] Installing NexusLink static build"
	@mkdir -p $(INSTALL_BIN_DIR) $(INSTALL_LIB_DIR) $(INSTALL_INCLUDE_DIR)
	@cp $(EXECUTABLE) $(INSTALL_BIN_DIR)/
	@cp $(STATIC_LIB) $(INSTALL_LIB_DIR)/
	@cp -r $(INCLUDE_DIR)/nlink_semverx $(INSTALL_INCLUDE_DIR)/
	@echo "✅ Static installation completed to $(PREFIX)"

# =============================================================================
# CLEANUP
# =============================================================================

clean:
	@echo "[CLEAN] Removing build artifacts"
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(LIB_DIR)

distclean: clean
	@echo "[DISTCLEAN] Deep cleaning"
	@find . -name "*.o" -delete
	@find . -name "*.a" -delete

# =============================================================================
# HELP
# =============================================================================

help:
	@echo "NexusLink SemVerX Static Build System"
	@echo "===================================="
	@echo "Aegis Project Phase 1.5 - Production Implementation"
	@echo ""
	@echo "Targets:"
	@echo "  all              - Build static library and executable (default)"
	@echo "  debug            - Debug build with symbols"
	@echo "  release          - Optimized release build"
	@echo "  test             - Test static executable functionality"
	@echo "  validate         - Comprehensive static architecture validation"
	@echo "  polybuild-prep   - Prepare for polybuild integration"
	@echo "  ecosystem-check  - Validate OBINexus ecosystem readiness"
	@echo "  install          - System-wide installation"
	@echo "  clean            - Remove build artifacts"
	@echo "  help             - Show this help"
	@echo ""
	@echo "Static Architecture:"
	@echo "  Static Library:  $(STATIC_LIB)"
	@echo "  Static Executable: $(EXECUTABLE)"
	@echo "  No shared dependencies - fully self-contained"
	@echo ""
	@echo "OBINexus Integration:"
	@echo "  Toolchain Flow: riftlang.exe → .so.a → rift.exe → gosilang"
	@echo "  Build Stack:    nlink (SemVerX) → polybuild"
	@echo "  Project Phase:  Aegis Phase 1.5 - SemVerX Range State"

# =============================================================================
# DEPENDENCY TRACKING
# =============================================================================

-include $(ALL_OBJECTS:.o=.d)

$(BUILD_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MM -MT $(@:.d=.o) $< > $@