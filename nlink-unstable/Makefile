# NexusLink (NLink) Makefile
# Author: Nnamdi Michael Okpala

CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-parameter -g -fPIC
LDFLAGS = -shared -ldl -lpthread

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
LIB_DIR = lib
TEST_DIR = test

# Source files
CORE_SRCS = $(wildcard $(SRC_DIR)/core/*.c)
COMPONENT_SRCS = $(wildcard $(SRC_DIR)/component/*.c)
MINIMIZER_SRCS = $(wildcard $(SRC_DIR)/minimizer/*.c)

# Object files
CORE_OBJS = $(patsubst $(SRC_DIR)/core/%.c,$(BUILD_DIR)/core/%.o,$(CORE_SRCS))
COMPONENT_OBJS = $(patsubst $(SRC_DIR)/component/%.c,$(BUILD_DIR)/component/%.o,$(COMPONENT_SRCS))
MINIMIZER_OBJS = $(patsubst $(SRC_DIR)/minimizer/%.c,$(BUILD_DIR)/minimizer/%.o,$(MINIMIZER_SRCS))

# Library names
CORE_LIB = $(LIB_DIR)/libnexus_core.so
COMPONENT_LIB = $(LIB_DIR)/libnexus_component.so
MINIMIZER_LIB = $(LIB_DIR)/libokpala_minimizer.so

# Default target
all: directories $(CORE_LIB) $(COMPONENT_LIB) $(MINIMIZER_LIB)

# Create directories
directories:
	mkdir -p $(BUILD_DIR)/core
	mkdir -p $(BUILD_DIR)/component
	mkdir -p $(BUILD_DIR)/minimizer
	mkdir -p $(LIB_DIR)

# Compile core sources
$(BUILD_DIR)/core/%.o: $(SRC_DIR)/core/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Compile component sources
$(BUILD_DIR)/component/%.o: $(SRC_DIR)/component/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Compile minimizer sources
$(BUILD_DIR)/minimizer/%.o: $(SRC_DIR)/minimizer/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Link core library
$(CORE_LIB): $(CORE_OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

# Link component library
$(COMPONENT_LIB): $(COMPONENT_OBJS)
	$(CC) $(LDFLAGS) $^ $(CORE_LIB) -o $@

# Link minimizer library
$(MINIMIZER_LIB): $(MINIMIZER_OBJS)
	$(CC) $(LDFLAGS) $^ $(CORE_LIB) -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(LIB_DIR)

# Run tests
test: all
	@echo "Running tests..."
	@cd $(TEST_DIR) && ./run_tests.sh

# Install libraries and headers
install: all
	mkdir -p /usr/local/lib/nexuslink
	mkdir -p /usr/local/include/nexuslink
	cp $(LIB_DIR)/*.so /usr/local/lib/nexuslink/
	cp $(INCLUDE_DIR)/*.h /usr/local/include/nexuslink/
	ldconfig

.PHONY: all directories clean test install
