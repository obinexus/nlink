# OBINexus NexusLink Build System
# Supports debug/release builds with proper output structure

# Compiler settings
CC := gcc
CXX := g++
AR := ar
CFLAGS_COMMON := -Wall -Wextra -Wpedantic -I./include -std=c11
CXXFLAGS_COMMON := -Wall -Wextra -Wpedantic -I./include -std=c++17

# Build configurations
CFLAGS_DEBUG := $(CFLAGS_COMMON) -g -O0 -DDEBUG
CFLAGS_RELEASE := $(CFLAGS_COMMON) -O3 -DNDEBUG -ffunction-sections -fdata-sections

# Directories
SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
BUILD_DEBUG := $(BUILD_DIR)/debug
BUILD_RELEASE := $(BUILD_DIR)/release

# Feature modules
FEATURES := semverx parser schema minimizer etps symbols pipeline cli tatit mpsystem spsystem

# Source collection
CORE_SOURCES := $(foreach feat,$(FEATURES),$(wildcard $(SRC_DIR)/core/$(feat)/*.c))
CLI_SOURCES := $(wildcard $(SRC_DIR)/cli/*.c)
ALL_SOURCES := $(CORE_SOURCES) $(CLI_SOURCES) $(SRC_DIR)/main.c

# Object files for both configurations
OBJECTS_DEBUG := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DEBUG)/obj/%.o,$(ALL_SOURCES))
OBJECTS_RELEASE := $(patsubst $(SRC_DIR)/%.c,$(BUILD_RELEASE)/obj/%.o,$(ALL_SOURCES))

# Targets
LIB_DEBUG := $(BUILD_DEBUG)/lib/nlink.so
LIB_STATIC_DEBUG := $(BUILD_DEBUG)/lib/nlink.a
BIN_DEBUG := $(BUILD_DEBUG)/bin/nlink

LIB_RELEASE := $(BUILD_RELEASE)/lib/nlink.so
LIB_STATIC_RELEASE := $(BUILD_RELEASE)/lib/nlink.a
BIN_RELEASE := $(BUILD_RELEASE)/bin/nlink

# Default target
.DEFAULT_GOAL := release

# Phony targets
.PHONY: all debug release clean test features qa-test

all: debug release

debug: $(BIN_DEBUG)
release: $(BIN_RELEASE)

# Debug build rules
$(BUILD_DEBUG)/obj/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_DEBUG) -fPIC -c $< -o $@

$(LIB_DEBUG): $(filter-out $(BUILD_DEBUG)/obj/main.o,$(OBJECTS_DEBUG))
	@mkdir -p $(dir $@)
	$(CC) -shared -o $@ $^
	@ln -sf nlink.so $(BUILD_DEBUG)/lib/libnlink.so

$(LIB_STATIC_DEBUG): $(filter-out $(BUILD_DEBUG)/obj/main.o,$(OBJECTS_DEBUG))
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $^
	@ln -sf nlink.a $(BUILD_DEBUG)/lib/libnlink.a

$(BIN_DEBUG): $(BUILD_DEBUG)/obj/main.o $(LIB_DEBUG)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_DEBUG) -o $@ $< -L$(BUILD_DEBUG)/lib -lnlink -Wl,-rpath,'$$ORIGIN/../lib'

# Release build rules (with optimization)
$(BUILD_RELEASE)/obj/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_RELEASE) -fPIC -c $< -o $@

$(LIB_RELEASE): $(filter-out $(BUILD_RELEASE)/obj/main.o,$(OBJECTS_RELEASE))
	@mkdir -p $(dir $@)
	$(CC) -shared -o $@ $^ -Wl,--gc-sections
	@ln -sf nlink.so $(BUILD_RELEASE)/lib/libnlink.so

$(LIB_STATIC_RELEASE): $(filter-out $(BUILD_RELEASE)/obj/main.o,$(OBJECTS_RELEASE))
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $^
	@ln -sf nlink.a $(BUILD_RELEASE)/lib/libnlink.a

$(BIN_RELEASE): $(BUILD_RELEASE)/obj/main.o $(LIB_RELEASE)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_RELEASE) -o $@ $< -L$(BUILD_RELEASE)/lib -lnlink -Wl,-rpath,'$$ORIGIN/../lib' -Wl,--gc-sections
	@strip $@
	@echo "Binary size: $$(du -h $@ | cut -f1)"

# Feature listing
features:
	@echo "NexusLink Features:"
	@for f in $(FEATURES); do \
		count=$$(find $(SRC_DIR)/core/$$f -name "*.c" 2>/dev/null | wc -l); \
		printf "  %-12s: %3d source files\n" "$$f" "$$count"; \
	done

# QA Testing (TP/TN/FP/FN validation)
qa-test: debug
	@echo "Running QA Soundness Tests..."
	@echo "TP: True Positive tests"
	@echo "TN: True Negative tests"
	@echo "FP: False Positive tests (MUST BE ZERO)"
	@echo "FN: False Negative tests"
	@$(BUILD_DEBUG)/bin/nlink --qa-validate

# ETPS test
etps-test: debug
	@echo "Testing ETPS Telemetry..."
	@LD_LIBRARY_PATH=$(BUILD_DEBUG)/lib $(BUILD_DEBUG)/bin/nlink --etps-test --json

# Clean
clean:
	rm -rf $(BUILD_DIR)

# Install targets
install: release
	@echo "Installing NexusLink..."
	@mkdir -p /usr/local/lib /usr/local/bin /usr/local/include
	@cp $(LIB_RELEASE) /usr/local/lib/
	@cp $(LIB_STATIC_RELEASE) /usr/local/lib/
	@cp $(BIN_RELEASE) /usr/local/bin/nlink
	@cp -r $(INC_DIR)/nlink /usr/local/include/
	@ldconfig

# Help
help:
	@echo "NexusLink Build System"
	@echo "====================="
	@echo "Targets:"
	@echo "  debug     - Build debug version"
	@echo "  release   - Build optimized release version"
	@echo "  clean     - Remove all build artifacts"
	@echo "  test      - Run test suite"
	@echo "  qa-test   - Run QA soundness validation"
	@echo "  etps-test - Test ETPS telemetry system"
	@echo "  features  - List all features"
	@echo "  install   - Install release build"
