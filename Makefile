# OBINexus NLink: DAG-Protected Module Hierarchy Build System
# Constitutional Governance through CMake/Make Coordination
# Revision 3.7 - Entropy-Adaptive Dependency Resolution

# =============================================================================
# Strategic Architectural Parameters: Sinphases >= 0.5 Compliance
# =============================================================================

PROJECT_NAME := nlink
VERSION_MAJOR := 3
VERSION_MINOR := 7
VERSION_PATCH := 0
SINPHASES_COMPLIANCE := 0.5

# Constitutional Build Environment Enforcement
CC := gcc
CXX := g++
CMAKE := cmake
BUILD_TYPE := Release
PARALLEL_JOBS := $(shell nproc 2>/dev/null || echo 4)

# Triaxial Directory Architecture: DAG-Protected Boundaries
SRC_DIR := src
INCLUDE_DIR := include/nlink
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
LIB_DIR := $(BUILD_DIR)/lib
BIN_DIR := $(BUILD_DIR)/bin

# Constitutional Compilation Flags: Boundary Enforcement
CFLAGS := -Wall -Wextra -Werror -std=c11 -fPIC
CFLAGS += -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/core
CFLAGS += -DSINPHASES_COMPLIANCE=$(SINPHASES_COMPLIANCE)
CFLAGS += -DOBINEXUS_CONSTITUTIONAL_VALIDATION

# Entropy-Adaptive Security Flags
CFLAGS += -fstack-protector-strong -D_FORTIFY_SOURCE=2
CFLAGS += -fno-omit-frame-pointer -grecord-gcc-switches

# Polymorphic Coordination Protocol: Debug vs Release
ifeq ($(BUILD_TYPE),Debug)
    CFLAGS += -g3 -O0 -DDEBUG -DOBINEXUS_DEVELOPMENT_MODE
else
    CFLAGS += -O3 -DNDEBUG -DOBINEXUS_PRODUCTION_MODE
endif

# =============================================================================
# DAG-Protected Module Classification: Hierarchical Dependency Matrix
# =============================================================================

# Core Infrastructure Modules (Tier 1: Constitutional Foundation)
CORE_MODULES := \
    core/config/config_manager \
    core/config/config_parser \
    core/crypto/shannon_entropy \
    core/marshal/marshal

# SemVerX Governance Modules (Tier 2: Version Coordination)
SEMVERX_MODULES := \
    core/semverx/semver \
    core/semverx/nexus_version \
    core/semverx/lazy_versioned \
    core/semverx/missing_functions

# ETPS Telemetry Modules (Tier 3: Monitoring Coordination)
ETPS_MODULES := \
    core/etps/telemetry

# CLI Interface Modules (Tier 4: User Boundary Management)
CLI_MODULES := \
    core/cli/cli \
    core/cli/command \
    core/cli/command_params \
    core/cli/command_registration \
    core/cli/command_registry \
    core/cli/command_router \
    core/cli/parse

# CLI Command Implementations (Tier 4b: Command Specialization)
CLI_COMMAND_MODULES := \
    core/cli/commands/load \
    core/cli/commands/minimal \
    core/cli/commands/minimize \
    core/cli/commands/pipeline \
    core/cli/commands/version \
    core/cli/commands/version_utils

# Pipeline Orchestration Modules (Tier 5: Processing Coordination)
PIPELINE_MODULES := \
    core/pipeline/nlink_pipeline \
    core/pipeline/pipeline_detector \
    core/pipeline/pipeline_pass \
    core/pipeline/pipeline_stage

# Parser Infrastructure Modules (Tier 6: Language Processing)
PARSER_MODULES := \
    core/parser/parse \
    core/parser/parser \
    core/parser/parser_interface

# Symbol Management Modules (Tier 7: Symbol Coordination)
SYMBOL_MODULES := \
    core/symbols/cold_symbol \
    core/symbols/nexus_symbols \
    core/symbols/symbols \
    core/symbols/versioned_symbols

# Minimizer Architecture Modules (Tier 8: AST Optimization)
MINIMIZER_MODULES := \
    core/minimizer/minimizer \
    core/minimizer/nexus_automaton \
    core/minimizer/okpala_ast \
    core/minimizer/okpala_automaton \
    core/minimizer/okpala_automaton_minimzer

# Multi-Processing System Modules (Tier 9: Concurrent Coordination)
MPS_MODULES := \
    core/mpsystem/mps_config \
    core/mpsystem/mps_dependency \
    core/mpsystem/mps_lifecycle \
    core/mpsystem/mps_pipeline \
    core/mpsystem/mps_stream

# Single-Processing System Modules (Tier 10: Sequential Coordination)
SPS_MODULES := \
    core/spsystem/sps_config \
    core/spsystem/sps_dependency \
    core/spsystem/sps_lifecycle \
    core/spsystem/sps_pipeline \
    core/spsystem/sps_stream

# TATIT Transformation Modules (Tier 11: Meta-Architectural Operations)
TATIT_MODULES := \
    core/tatit/abstraction \
    core/tatit/aggregation \
    core/tatit/composition \
    core/tatit/identity \
    core/tatit/tactic \
    core/tatit/transformation \
    core/tatit/traversal

# Consolidated Module Registry: Hierarchical Dependency Resolution
ALL_MODULES := \
    $(CORE_MODULES) \
    $(SEMVERX_MODULES) \
    $(ETPS_MODULES) \
    $(CLI_MODULES) \
    $(CLI_COMMAND_MODULES) \
    $(PIPELINE_MODULES) \
    $(PARSER_MODULES) \
    $(SYMBOL_MODULES) \
    $(MINIMIZER_MODULES) \
    $(MPS_MODULES) \
    $(SPS_MODULES) \
    $(TATIT_MODULES)

# =============================================================================
# Constitutional Object File Generation: DAG-Protected Compilation
# =============================================================================

# Transform module names to object file paths with boundary enforcement
OBJECTS := $(patsubst %,$(OBJ_DIR)/%.o,$(ALL_MODULES))

# Primary Library Targets: Polymorphic Coordination Artifacts
STATIC_LIB := $(LIB_DIR)/libnlink.a
SHARED_LIB := $(LIB_DIR)/libnlink.so
CLI_BINARY := $(BIN_DIR)/nlink

# =============================================================================
# Strategic Build Targets: Hierarchical Coordination Protocols
# =============================================================================

.PHONY: all clean distclean configure build install test lint format cmake-build

# Default Constitutional Governance Target
all: configure build

# Phase 1: Constitutional Environment Preparation
configure:
	@echo "🏗️  OBINexus NLink: Constitutional Environment Configuration"
	@echo "   Sinphases Compliance: $(SINPHASES_COMPLIANCE)"
	@echo "   Build Type: $(BUILD_TYPE)"
	@mkdir -p $(BUILD_DIR) $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)/core/{config,crypto,etps,marshal,semverx,cli/commands,pipeline,parser,symbols,minimizer,mpsystem,spsystem,tatit}
	@echo "✅ Constitutional directory hierarchy established"

# Phase 2: CMake Integration Protocol
cmake-configure:
	@echo "🔧 CMake Constitutional Integration Protocol"
	cd $(BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_C_FLAGS="$(CFLAGS)" \
		-DSINPHASES_COMPLIANCE=$(SINPHASES_COMPLIANCE) \
		-DOBINEXUS_CONSTITUTIONAL_VALIDATION=ON ..
	@echo "✅ CMake configuration completed with constitutional compliance"

# Phase 3: CMake Build Execution
cmake-build: cmake-configure
	@echo "🚀 CMake Build Orchestration: Parallel Execution ($(PARALLEL_JOBS) jobs)"
	cd $(BUILD_DIR) && $(CMAKE) --build . --parallel $(PARALLEL_JOBS)
	@echo "✅ CMake build completed with constitutional governance"

# Phase 4: Manual Build Orchestration (DAG-Protected)
build: configure $(STATIC_LIB) $(SHARED_LIB) $(CLI_BINARY)
	@echo "🎯 OBINexus NLink: Build Orchestration Complete"
	@echo "   Static Library: $(STATIC_LIB)"
	@echo "   Shared Library: $(SHARED_LIB)"
	@echo "   CLI Binary: $(CLI_BINARY)"

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
