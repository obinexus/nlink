# Zero-Knowledge Weighted XOR Proofs Makefile
# OBINexus Standard Compliant

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -g -O2 -fPIC -pthread
CFLAGS += -I../../include
CFLAGS += -DNLINK_BUILDING_ZKWXP_LIB

# Source files
SOURCES = zkwxp_core.c \
          dsl/zkwxp_dsl.c \
          remote/zkwxp_remote.c \
          qa/zkwxp_qa.c

OBJECTS = $(SOURCES:.c=.o)

# Library name
LIBRARY = libzkwxp.so
STATIC_LIB = libzkwxp.a

# Test files
TEST_SOURCES = ../../test/zkwxp_integration_test.c
TEST_BINARY = zkwxp_test

# NLink dependencies
NLINK_LIBS = -L../../lib -lnlink -letps -lcrypto_entropy

# Build targets
.PHONY: all clean test install

all: $(LIBRARY) $(STATIC_LIB)

# Build shared library
$(LIBRARY): $(OBJECTS)
	$(CC) -shared -o $@ $^ $(NLINK_LIBS)

# Build static library
$(STATIC_LIB): $(OBJECTS)
	ar rcs $@ $^

# Compile source files
%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Build test binary
test: $(LIBRARY)
	$(CC) $(CFLAGS) $(TEST_SOURCES) -o $(TEST_BINARY) \
		-L. -lzkwxp $(NLINK_LIBS) -Wl,-rpath,.

# Install library
install: $(LIBRARY) $(STATIC_LIB)
	@echo "Installing ZK-WXP libraries..."
	@mkdir -p ../../lib
	@cp $(LIBRARY) ../../lib/
	@cp $(STATIC_LIB) ../../lib/
	@echo "Installation complete"

# Clean build artifacts
clean:
	@echo "Cleaning ZK-WXP build artifacts..."
	@rm -f $(OBJECTS)
	@rm -f $(LIBRARY) $(STATIC_LIB)
	@rm -f $(TEST_BINARY)
	@rm -rf dsl/*.o remote/*.o qa/*.o

# QA validation target
qa-validate: test
	@echo "Running QA validation..."
	./$(TEST_BINARY) --qa-mode
	@echo "QA validation complete"

# Generate documentation
docs:
	@echo "Generating ZK-WXP documentation..."
	doxygen ../../config/zkwxp.doxyfile

# Performance profiling
profile: test
	valgrind --tool=callgrind ./$(TEST_BINARY)
	@echo "Profile data saved to callgrind.out.*"
