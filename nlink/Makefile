# NexusLink (NLink) Makefile
# Author: Nnamdi Michael Okpala

# Directory structure
PROJECT_ROOT := $(shell pwd)
SRC_DIR := $(PROJECT_ROOT)/src
INCLUDE_DIR := $(PROJECT_ROOT)/include
BUILD_DIR := $(PROJECT_ROOT)/build
CMAKE_BUILD_DIR := $(BUILD_DIR)/cmake

# Default options
CMAKE_OPTIONS := -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON

# Default target
.PHONY: all
all: build

# Create build directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(CMAKE_BUILD_DIR): | $(BUILD_DIR)
	mkdir -p $(CMAKE_BUILD_DIR)

# Configure with CMake
.PHONY: configure
configure: | $(CMAKE_BUILD_DIR)
	cd $(CMAKE_BUILD_DIR) && cmake $(CMAKE_OPTIONS) $(PROJECT_ROOT)

# Build the project
.PHONY: build
build: configure
	cd $(CMAKE_BUILD_DIR) && cmake --build .

# Build in debug mode
.PHONY: debug
debug:
	$(MAKE) build CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON"

# Build in release mode
.PHONY: release
release:
	$(MAKE) build CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON"

# Run tests
.PHONY: test
test: build
	cd $(CMAKE_BUILD_DIR) && ctest --output-on-failure

# Run all tests with custom target
.PHONY: run-tests
run-tests: build
	cd $(CMAKE_BUILD_DIR) && cmake --build . --target run_all_tests

# Clean build artifacts
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# Install the library and headers
.PHONY: install
install: build
	cd $(CMAKE_BUILD_DIR) && cmake --install .

# Generate docs
.PHONY: docs
docs: build
	cd $(CMAKE_BUILD_DIR) && cmake --build . --target docs

# Validate include paths
.PHONY: validate-includes
validate-includes: build
	cd $(CMAKE_BUILD_DIR) && cmake --build . --target validate_includes

# Fix include paths
.PHONY: fix-includes
fix-includes: build
	cd $(CMAKE_BUILD_DIR) && cmake --build . --target fix_includes

# Create packages
.PHONY: package
package: build
	cd $(CMAKE_BUILD_DIR) && cmake --build . --target package

# Create source packages
.PHONY: package-source
package-source: configure
	cd $(CMAKE_BUILD_DIR) && cmake --build . --target package_source

# Complete development cycle
.PHONY: dev-cycle
dev-cycle: build
	cd $(CMAKE_BUILD_DIR) && cmake --build . --target dev_cycle

# Check code style with clang-format
.PHONY: check-style
check-style:
	find $(SRC_DIR) $(INCLUDE_DIR) -name "*.c" -o -name "*.h" | xargs clang-format -i

# Build with coverage instrumentation
.PHONY: coverage
coverage:
	$(MAKE) build CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DENABLE_COVERAGE=ON"
	cd $(CMAKE_BUILD_DIR) && cmake --build . --target coverage

# Show help
.PHONY: help
help:
	@echo "NexusLink Makefile targets:"
	@echo "  all              - Build the project (default)"
	@echo "  configure        - Configure the build system"
	@echo "  build            - Build the project"
	@echo "  debug            - Build in debug mode"
	@echo "  release          - Build in release mode"
	@echo "  test             - Run tests"
	@echo "  run-tests        - Run all tests (run_all_tests target)"
	@echo "  clean            - Remove all build artifacts"
	@echo "  install          - Install the library and headers"
	@echo "  docs             - Generate documentation"
	@echo "  validate-includes - Validate include paths"
	@echo "  fix-includes     - Fix include paths"
	@echo "  package          - Create binary packages"
	@echo "  package-source   - Create source packages"
	@echo "  dev-cycle        - Run the complete development cycle"
	@echo "  check-style      - Check code style with clang-format"
	@echo "  coverage         - Build with coverage instrumentation"

# Components
.PHONY: components
components:
	@for comp in $(shell ls $(SRC_DIR)/core); do \
		echo "$$comp"; \
	done