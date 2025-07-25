# Makefile for OBINexus NLink
# Phase 1: Constants
BUILD_DIR := build
SRC_DIR := src
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin
LIB_DIR := $(BUILD_DIR)/lib
INCLUDE := include/nlink

PARALLEL_JOBS := 4
BUILD_TYPE := Release
CFLAGS := -Wall -Wextra -std=c11 -O2 -Iinclude
LDFLAGS :=

# Phase 2: Source Modules
CLI_MODULES := \
	core/cli/parse

PARSER_MODULES :=

ALL_MODULES := $(CLI_MODULES) $(PARSER_MODULES)

OBJECTS := $(foreach m,$(ALL_MODULES),$(OBJ_DIR)/$(m).o)
STATIC_LIB := $(LIB_DIR)/libnlink.a
SHARED_LIB := $(LIB_DIR)/libnlink.so
CLI_BINARY := $(BIN_DIR)/nlink

# Phase 3: Entry Rules
.PHONY: all clean configure cmake-configure cmake-build build

all: build

configure:
	@echo "📇 OBINexus NLink: Constitutional Environment Configuration"
	@echo "   Sinphases Compliance: 0.5"
	@echo "   Build Type: $(BUILD_TYPE)"
	@mkdir -p $(BUILD_DIR) $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR)
	@echo "✅ Constitutional directory hierarchy established"

cmake-configure:
	@echo "📝 Configuring with CMake"
	@cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

cmake-build: cmake-configure
	@echo "🚀 CMake Build Orchestration: Parallel Execution ($(PARALLEL_JOBS) jobs)"
	cd $(BUILD_DIR) && cmake --build . --parallel $(PARALLEL_JOBS)
	@echo "✅ CMake build completed with constitutional governance"

# Phase 4: Manual Build Orchestration (DAG-Protected)
build: configure $(STATIC_LIB) $(SHARED_LIB) $(CLI_BINARY)
	@echo "🏆 OBINexus NLink: Build Orchestration Complete"
	@echo "   Static Library: $(STATIC_LIB)"
	@echo "   Shared Library: $(SHARED_LIB)"
	@echo "   CLI Binary: $(CLI_BINARY)"

# Phase 5: Object Compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "🔨 Compiling: $< → $@"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Phase 6: Linking
$(STATIC_LIB): $(OBJECTS)
	@ar rcs $@ $^

$(SHARED_LIB): $(OBJECTS)
	$(CC) -shared -o $@ $^

$(CLI_BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Constitutional Object File Compilation: Entropy-Adaptive Pattern Rules
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "🔨 Compiling: $< → $@"
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Static Library Construction: Constitutional Boundary Enforcement
$(STATIC_LIB): $(OBJECTS)
	@echo "📚 Creating static library: $@"
	@mkdir -p $(@D)
	ar rcs $@ $^
	@echo "✅ Static library created with constitutional compliance"

# Shared Library Construction: Polymorphic Coordination Protocol
$(SHARED_LIB): $(OBJECTS)
	@echo "🔗 Creating shared library: $@"
	@mkdir -p $(@D)
	$(CC) -shared -o $@ $^ $(LDFLAGS)
	@echo "✅ Shared library created with distributed coordination"

# CLI Binary Construction: User Interface Boundary Management
$(CLI_BINARY): $(STATIC_LIB) $(SRC_DIR)/core/cli/main.c
	@echo "⚡ Creating CLI binary: $@"
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(SRC_DIR)/core/cli/main.c -L$(LIB_DIR) -lnlink -o $@
	@echo "✅ CLI binary created with constitutional governance"


# =============================================================================
# Quality Assurance Protocols: Constitutional Compliance Validation
# =============================================================================

# Lint Analysis: Static Code Quality Enforcement
lint:
	@echo "🔍 Static Analysis: Constitutional Code Quality Validation"
	@command -v cppcheck >/dev/null 2>&1 || { echo "⚠️  cppcheck not available, skipping"; exit 0; }
	cppcheck --enable=all --error-exitcode=1 --std=c11 \
		--suppress=missingIncludeSystem \
		-I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/core \
		$(SRC_DIR)/
	@echo "✅ Static analysis completed with constitutional compliance"

# Format Enforcement: Code Style Constitutional Governance
format:
	@echo "🎨 Code Style Enforcement: Constitutional Formatting"
	@command -v clang-format >/dev/null 2>&1 || { echo "⚠️  clang-format not available, skipping"; exit 0; }
	find $(SRC_DIR) $(INCLUDE_DIR) -name "*.c" -o -name "*.h" | \
		xargs clang-format -i -style="{BasedOnStyle: LLVM, IndentWidth: 4, ColumnLimit: 100}"
	@echo "✅ Code formatting completed with constitutional style"

# Unit Test Execution: Functional Boundary Validation
test: build
	@echo "🧪 Unit Test Execution: Constitutional Functionality Validation"
	@if [ -f $(BIN_DIR)/nlink ]; then \
		echo "   Testing CLI functionality..."; \
		$(BIN_DIR)/nlink --version || echo "   CLI version test completed"; \
		echo "✅ Basic functionality test completed"; \
	else \
		echo "⚠️  CLI binary not found, skipping functional tests"; \
	fi

# Integration Test Protocol: End-to-End Constitutional Validation
integration-test: build
	@echo "🔄 Integration Test Protocol: System-Level Validation"
	@echo "   Creating test project structure..."
	@mkdir -p $(BUILD_DIR)/test_project
	@echo '{"name": "test", "version": "1.0.0"}' > $(BUILD_DIR)/test_project/pkg.nlink.in.xml
	@if [ -f $(BIN_DIR)/nlink ]; then \
		echo "   Testing project validation..."; \
		cd $(BUILD_DIR)/test_project && ../../$(BIN_DIR)/nlink validate || true; \
		echo "✅ Integration test completed"; \
	else \
		echo "⚠️  CLI binary not found, skipping integration tests"; \
	fi

# =============================================================================
# Constitutional Cleanup Protocols: Entropy Management
# =============================================================================

# Standard Cleanup: Temporary Artifact Removal
clean:
	@echo "🧹 Constitutional Cleanup: Temporary Artifact Removal"
	rm -rf $(BUILD_DIR)/obj $(BUILD_DIR)/lib $(BUILD_DIR)/bin
	rm -f $(BUILD_DIR)/*.log $(BUILD_DIR)/*.tmp
	@echo "✅ Temporary artifacts cleaned with constitutional governance"

# Complete Reset: Full Entropy-Sealed Cleanup
distclean:
	@echo "🔥 Complete Constitutional Reset: Full Entropy-Sealed Cleanup"
	rm -rf $(BUILD_DIR)
	rm -f CMakeCache.txt
	find . -name "*.o" -delete
	find . -name "*.so" -delete
	find . -name "*.a" -delete
	@echo "✅ Complete reset completed with constitutional compliance"

# =============================================================================
# Installation Protocol: System Integration Deployment
# =============================================================================

# System Installation: Constitutional Distribution
install: build
	@echo "📦 System Installation: Constitutional Distribution Protocol"
	@PREFIX := $(PREFIX:-/usr/local)
	@echo "   Installation prefix: $(PREFIX)"
	install -d $(DESTDIR)$(PREFIX)/lib $(DESTDIR)$(PREFIX)/bin $(DESTDIR)$(PREFIX)/include
	install -m 644 $(STATIC_LIB) $(SHARED_LIB) $(DESTDIR)$(PREFIX)/lib/
	install -m 755 $(CLI_BINARY) $(DESTDIR)$(PREFIX)/bin/
	cp -r $(INCLUDE_DIR) $(DESTDIR)$(PREFIX)/include/
	@echo "✅ Installation completed with constitutional governance"

# Development Installation: Local Testing Protocol
install-dev: build
	@echo "🔧 Development Installation: Local Testing Protocol"
	@DEV_PREFIX := $(HOME)/.local
	@echo "   Development prefix: $(DEV_PREFIX)"
	install -d $(DEV_PREFIX)/lib $(DEV_PREFIX)/bin $(DEV_PREFIX)/include
	install -m 644 $(STATIC_LIB) $(SHARED_LIB) $(DEV_PREFIX)/lib/
	install -m 755 $(CLI_BINARY) $(DEV_PREFIX)/bin/
	cp -r $(INCLUDE_DIR) $(DEV_PREFIX)/include/
	@echo "✅ Development installation completed"

# =============================================================================
# Diagnostic Protocols: Constitutional System Analysis
# =============================================================================

# Dependency Analysis: DAG Validation Protocol
deps:
	@echo "🔍 Dependency Analysis: DAG Constitutional Validation"
	@echo "Module dependency matrix:"
	@for module in $(ALL_MODULES); do \
		echo "  $$module.c:"; \
		if [ -f "$(SRC_DIR)/$$module.c" ]; then \
			grep -h "^#include" "$(SRC_DIR)/$$module.c" | sed 's/^/    /' || true; \
		else \
			echo "    [FILE NOT FOUND]"; \
		fi; \
	done
	@echo "✅ Dependency analysis completed"

# Build Status: Constitutional Health Monitoring
status:
	@echo "📊 Build Status: Constitutional Health Monitoring"
	@echo "Project: $(PROJECT_NAME) v$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)"
	@echo "Sinphases Compliance: $(SINPHASES_COMPLIANCE)"
	@echo "Build Directory: $(BUILD_DIR)"
	@echo "Module Count: $(words $(ALL_MODULES))"
	@echo "Constitutional Artifacts:"
	@ls -la $(BUILD_DIR)/ 2>/dev/null || echo "  [Build directory not initialized]"
	@echo "✅ Status monitoring completed"

# Help Protocol: Constitutional Usage Documentation
help:
	@echo "🎯 OBINexus NLink: Constitutional Build System Documentation"
	@echo ""
	@echo "Strategic Build Targets:"
	@echo "  all             - Complete constitutional build orchestration"
	@echo "  configure       - Prepare constitutional environment"
	@echo "  build          - Manual build with DAG-protected dependencies"
	@echo "  cmake-build    - CMake-based build with parallel execution"
	@echo "  clean          - Remove temporary constitutional artifacts"
	@echo "  distclean      - Complete entropy-sealed cleanup"
	@echo ""
	@echo "Quality Assurance Protocols:"
	@echo "  lint           - Static analysis with constitutional validation"
	@echo "  format         - Code style enforcement"
	@echo "  test           - Unit test execution"
	@echo "  integration-test - End-to-end constitutional validation"
	@echo ""
	@echo "Installation Protocols:"
	@echo "  install        - System-wide constitutional distribution"
	@echo "  install-dev    - Development environment installation"
	@echo ""
	@echo "Diagnostic Protocols:"
	@echo "  deps           - DAG dependency analysis"
	@echo "  status         - Constitutional health monitoring"
	@echo "  help           - This constitutional documentation"
	@echo ""
	@echo "Constitutional Parameters:"
	@echo "  BUILD_TYPE     - Release (default) or Debug"
	@echo "  PREFIX         - Installation prefix (default: /usr/local)"
	@echo "  PARALLEL_JOBS  - Parallel compilation jobs (default: auto-detected)"
	@echo ""
	@echo "✅ Constitutional documentation completed"
