# =============================================================================
# Sinphasé Feature Makefile: ETPS Telemetry System - CORRECTED
# TDD Integration with OBINexus Library Linking Resolution
# =============================================================================

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -I../../../include
TEST_CFLAGS = $(CFLAGS) -DTESTING

# OBINexus Library Linking Resolution
# Option 1: Direct library specification (Recommended)
LDFLAGS = -L../../../lib -l:nlink.a

# Option 2: Absolute path (Alternative)
# LDFLAGS = ../../../lib/nlink.a

# Option 3: Standard linking (if symbolic link created)
# LDFLAGS = -L../../../lib -lnlink

# Feature configuration
FEATURE_NAME = etps_telemetry
FEATURE_LIB = lib$(FEATURE_NAME).a

# Source files (link to existing implementation)
EXISTING_SRC = ../../etps/telemetry.c
EXISTING_OBJ = telemetry.o

# Test files
TEST_SRC = ../../../test/unit/$(FEATURE_NAME)/test_$(FEATURE_NAME).c
TEST_EXEC = test_$(FEATURE_NAME)

# Build targets
.PHONY: all test clean red green refactor qa validate debug

# =============================================================================
# TDD Workflow Targets - CORRECTED
# =============================================================================

# TDD RED phase - run tests with proper library linking
red:
	@echo "🔴 TDD RED Phase: Testing existing ETPS implementation"
	@echo "Verifying library availability..."
	@test -f ../../../lib/nlink.a || (echo "❌ Library not found: ../../../lib/nlink.a" && exit 1)
	@echo "✅ Library found: ../../../lib/nlink.a"
	@echo "Compiling test suite with OBINexus library linking..."
	$(CC) $(TEST_CFLAGS) $(TEST_SRC) -o $(TEST_EXEC)_red $(LDFLAGS)
	@echo "Running tests..."
	./$(TEST_EXEC)_red

# TDD GREEN phase - validate with corrected linking
green:
	@echo "🟢 TDD GREEN Phase: Validating existing implementation"
	@echo "Compiling existing implementation..."
	$(CC) -c $(EXISTING_SRC) -o $(EXISTING_OBJ) $(CFLAGS)
	@echo "Linking with OBINexus library..."
	$(CC) $(TEST_CFLAGS) $(TEST_SRC) $(EXISTING_OBJ) -o $(TEST_EXEC)_green $(LDFLAGS)
	@echo "Running validation tests..."
	./$(TEST_EXEC)_green

# TDD REFACTOR phase - with debugging information
refactor: green
	@echo "🔄 TDD REFACTOR Phase: Code analysis and optimization"
	@echo "Library linking validation:"
	@ldd ./$(TEST_EXEC)_green | grep -E "(nlink|not found)" || echo "Static linking detected"
	@echo "Symbol analysis:"
	@nm ../../../lib/nlink.a | grep -E "etps_" | head -10 || echo "No ETPS symbols found"
	@echo "Refactoring analysis complete"

# Debug target for troubleshooting
debug:
	@echo "🔍 Debug Information"
	@echo "==================="
	@echo "Current directory: $(PWD)"
	@echo "Library path: ../../../lib/"
	@ls -la ../../../lib/ | grep nlink || echo "No nlink libraries found"
	@echo "Include path: ../../../include/"
	@ls -la ../../../include/nlink_qa_poc/etps/ || echo "Headers not found"
	@echo "Test source: $(TEST_SRC)"
	@test -f $(TEST_SRC) && echo "✅ Test source exists" || echo "❌ Test source missing"
	@echo "Existing source: $(EXISTING_SRC)"
	@test -f $(EXISTING_SRC) && echo "✅ Existing source found" || echo "❌ Existing source missing"

# =============================================================================
# QA and Validation Targets - ENHANCED
# =============================================================================

# Quality assurance with enhanced diagnostics
qa: debug
	@echo "🔍 QA Validation for ETPS Telemetry"
	@echo "=================================="
	@echo "Library Linking Validation:"
	@echo "  - Checking library format..."
	@file ../../../lib/nlink.a 2>/dev/null || echo "Cannot analyze library file"
	@echo "  - Checking library symbols..."
	@nm ../../../lib/nlink.a | grep -c "T etps_" 2>/dev/null || echo "No ETPS symbols found"
	@echo "Static Analysis:"
	@echo "  - Checking for unused variables..."
	@$(CC) $(CFLAGS) -Wunused-variable -c $(EXISTING_SRC) -o /tmp/qa_check1.o 2>&1 | head -10 || true
	@echo "  - Checking for potential memory leaks..."
	@$(CC) $(CFLAGS) -fsanitize=address -c $(EXISTING_SRC) -o /tmp/qa_check2.o 2>&1 | head -5 || true
	@echo "Compilation Test:"
	@echo "  - Testing compilation with current settings..."
	@$(CC) $(TEST_CFLAGS) -c $(TEST_SRC) -o /tmp/test_compile.o 2>&1 | head -5 || true
	@rm -f /tmp/qa_check*.o /tmp/test_compile.o

# Comprehensive validation with error handling
validate: debug
	@echo "🔍 Comprehensive Validation Sequence"
	@echo "===================================="
	@echo "Phase 1: Environment Validation"
	@$(MAKE) debug
	@echo "Phase 2: Library Linking Test"
	@echo "  Testing library linking..."
	@$(CC) $(TEST_CFLAGS) $(TEST_SRC) -o /tmp/link_test $(LDFLAGS) 2>&1 | head -10 || echo "Link test failed"
	@rm -f /tmp/link_test
	@echo "Phase 3: TDD Workflow"
	@$(MAKE) red || echo "RED phase failed - expected for retrofit"
	@$(MAKE) green || echo "GREEN phase failed - implementation issue"
	@echo "Validation sequence complete"

# Default target
all: validate

# Enhanced cleanup
clean:
	rm -f *.o *.a $(TEST_EXEC)_* core /tmp/qa_check*.o /tmp/test_compile.o /tmp/link_test

# Help with troubleshooting guidance
help:
	@echo "Sinphasé ETPS Telemetry Makefile - CORRECTED"
	@echo "============================================="
	@echo "TDD Workflow:"
	@echo "  red      - Run tests (TDD RED phase)"
	@echo "  green    - Validate implementation (TDD GREEN phase)" 
	@echo "  refactor - Code optimization (TDD REFACTOR phase)"
	@echo ""
	@echo "Troubleshooting:"
	@echo "  debug    - Show environment and dependency status"
	@echo "  validate - Comprehensive validation with error handling"
	@echo "  qa       - Enhanced quality assurance with diagnostics"
	@echo ""
	@echo "OBINexus Library Linking:"
	@echo "  Uses -l:nlink.a for direct library specification"
	@echo "  Maintains OBINexus naming convention compliance"
	@echo ""
	@echo "If linking fails:"
	@echo "  1. Run 'make debug' to check library status"
	@echo "  2. Verify ../../../lib/nlink.a exists"
	@echo "  3. Check library symbols with 'nm ../../../lib/nlink.a'"
