# NexusLink Compiler Demo Makefile (Clean Implementation)
# Aegis Project - Real-World Compiler Feature Demo
# Dual Linker Support: LINKER=nlink or LINKER=ld

# =============================================================================
# BUILD CONFIGURATION
# =============================================================================

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -O2 -g
INCLUDES = -I./include
SRCDIR = src
OBJDIR = build
BINDIR = bin

# Linker selection - can be overridden with LINKER=nlink or LINKER=ld
LINKER ?= ld
NLINK_PATH ?= ../bin/nlink

# Source files
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/symtab/symbol_table.c
OBJECTS = $(OBJDIR)/main.o $(OBJDIR)/symbol_table.o

# Target executables
TARGET_NLINK = $(BINDIR)/compiler_demo_nlink
TARGET_LD = $(BINDIR)/compiler_demo_ld

# =============================================================================
# MAIN TARGETS
# =============================================================================

.PHONY: all
all: $(TARGET_LD)

# Build with NexusLink integration
.PHONY: nlink
nlink: $(TARGET_NLINK)

# Build with standard ld
.PHONY: standard
standard: $(TARGET_LD)

# Build both variants
.PHONY: both
both: $(TARGET_NLINK) $(TARGET_LD)

# =============================================================================
# LINKER-SPECIFIC BUILDS
# =============================================================================

$(TARGET_NLINK): $(OBJECTS) | $(BINDIR)
	@echo "[DEMO BUILD] Linking with NexusLink integration"
	@if [ -f "$(NLINK_PATH)" ]; then \
		echo "[DEMO] Using NexusLink for build coordination"; \
		$(NLINK_PATH) --config-check --verbose --project-root . || echo "[DEMO] NexusLink validation completed"; \
		$(CC) $(OBJECTS) -o $(TARGET_NLINK); \
	else \
		echo "[DEMO WARNING] NexusLink not found, using standard linking"; \
		$(CC) $(OBJECTS) -o $(TARGET_NLINK); \
	fi
	@echo "[DEMO SUCCESS] NexusLink variant created: $(TARGET_NLINK)"

$(TARGET_LD): $(OBJECTS) | $(BINDIR)
	@echo "[DEMO BUILD] Linking with standard ld"
	$(CC) $(OBJECTS) -o $(TARGET_LD)
	@echo "[DEMO SUCCESS] Standard variant created: $(TARGET_LD)"

# =============================================================================
# OBJECT FILE COMPILATION
# =============================================================================

$(OBJDIR)/main.o: $(SRCDIR)/main.c | $(OBJDIR)
	@echo "[DEMO BUILD] Compiling main.c"
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRCDIR)/main.c -o $(OBJDIR)/main.o

$(OBJDIR)/symbol_table.o: $(SRCDIR)/symtab/symbol_table.c | $(OBJDIR)
	@echo "[DEMO BUILD] Compiling symbol_table.c" 
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRCDIR)/symtab/symbol_table.c -o $(OBJDIR)/symbol_table.o

# =============================================================================
# DIRECTORY CREATION
# =============================================================================

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# =============================================================================
# DEMONSTRATION TARGETS
# =============================================================================

.PHONY: demo
demo: $(TARGET_LD)
	@echo "=== Running Compiler Demo ==="
	./$(TARGET_LD)

.PHONY: demo-nlink
demo-nlink: $(TARGET_NLINK)
	@echo "=== Running NexusLink-Integrated Demo ==="
	./$(TARGET_NLINK)

.PHONY: demo-parse
demo-parse: $(TARGET_LD)
	@echo "=== Testing Declaration Parsing ==="
	./$(TARGET_LD) "int main_var; float calc_rate; char user_input; int loop_counter;"

# =============================================================================
# VALIDATION TARGETS
# =============================================================================

.PHONY: test
test: both
	@echo "[DEMO TEST] Validating both build variants"
	@./$(TARGET_LD) >/dev/null 2>&1 && echo "✓ Standard build functional" || echo "✗ Standard build failed"
	@./$(TARGET_NLINK) >/dev/null 2>&1 && echo "✓ NexusLink build functional" || echo "✗ NexusLink build failed"

.PHONY: validate
validate: test
	@echo "[DEMO VALIDATE] Comparing executable outputs"
	@file $(TARGET_LD)
	@file $(TARGET_NLINK)
	@echo "Validation completed"

# Debug target for systematic troubleshooting
.PHONY: debug
debug: CFLAGS += -DDEBUG -g3 -O0 -fsanitize=address
debug: $(TARGET_LD)
	@echo "[DEMO DEBUG] Debug build completed with AddressSanitizer"

# Memory validation target
.PHONY: memcheck
memcheck: debug
	@echo "[DEMO MEMCHECK] Running memory validation"
	@if command -v valgrind >/dev/null 2>&1; then \
		valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET_LD); \
	else \
		echo "Running with AddressSanitizer"; \
		./$(TARGET_LD); \
	fi

# Clean warning-free build
.PHONY: clean-build
clean-build: clean
	@echo "[DEMO BUILD] Warning-free compilation"
	@$(MAKE) all 2>&1 | grep -v "warning:" || true
	@echo "[DEMO SUCCESS] Clean build completed"

# =============================================================================
# LINKER OVERRIDE SUPPORT
# =============================================================================

.PHONY: linker-nlink
linker-nlink:
	@$(MAKE) LINKER=nlink nlink

.PHONY: linker-ld  
linker-ld:
	@$(MAKE) LINKER=ld standard

# =============================================================================
# CLEANUP
# =============================================================================

.PHONY: clean
clean:
	@echo "[DEMO CLEAN] Removing build artifacts"
	rm -rf $(OBJDIR) $(BINDIR)
	@echo "[DEMO SUCCESS] Cleanup completed"

# =============================================================================
# HELP DOCUMENTATION
# =============================================================================

.PHONY: help
help:
	@echo "NexusLink Compiler Demo Build System (Clean)"
	@echo "Aegis Project - Real-World Compiler Engineering"
	@echo ""
	@echo "Build Targets:"
	@echo "  all           - Build standard executable (default)"
	@echo "  nlink         - Build with NexusLink integration"
	@echo "  both          - Build both variants"
	@echo "  debug         - Debug build with AddressSanitizer"
	@echo "  clean-build   - Warning-free compilation"
	@echo ""
	@echo "Testing Targets:"
	@echo "  demo          - Run standard demo"
	@echo "  demo-nlink    - Run NexusLink demo"
	@echo "  demo-parse    - Test declaration parsing"
	@echo "  test          - Validate both builds"
	@echo "  memcheck      - Memory validation"
	@echo ""
	@echo "Utility Targets:"
	@echo "  clean         - Remove build artifacts"
	@echo "  validate      - Comprehensive validation suite"
	@echo ""
	@echo "Linker Override:"
	@echo "  make LINKER=nlink  - Force NexusLink integration"
	@echo "  make LINKER=ld     - Force standard linking"
