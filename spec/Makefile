# OBINexus QA Specification Makefile

CC = gcc
CFLAGS = -g -O0 -Wall -Wextra -I../include -DETPS_ENABLED=1
LDFLAGS = -L../lib -lnlink -lm -lpthread

# Find all spec files
UNIT_SPECS = $(wildcard unit/*_spec.c)
INTEGRATION_SPECS = $(wildcard integration/*_spec.c)
SYSTEM_SPECS = $(wildcard system/*_spec.c)
PERFORMANCE_SPECS = $(wildcard performance/*_spec.c)

ALL_SPECS = $(UNIT_SPECS) $(INTEGRATION_SPECS) $(SYSTEM_SPECS) $(PERFORMANCE_SPECS)
SPEC_BINARIES = $(ALL_SPECS:.c=)

.PHONY: all clean run unit integration system performance

all: $(SPEC_BINARIES)

%: %.c
	@echo "Building spec: $<"
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

unit: $(UNIT_SPECS:.c=)
	@echo "Running unit specs..."
	@for spec in $(UNIT_SPECS:.c=); do \
		echo "  → Running $$spec"; \
		LD_LIBRARY_PATH=../lib ./$$spec || exit 1; \
	done

integration: $(INTEGRATION_SPECS:.c=)
	@echo "Running integration specs..."
	@for spec in $(INTEGRATION_SPECS:.c=); do \
		echo "  → Running $$spec"; \
		LD_LIBRARY_PATH=../lib ./$$spec || exit 1; \
	done

system: $(SYSTEM_SPECS:.c=)
	@echo "Running system specs..."
	@for spec in $(SYSTEM_SPECS:.c=); do \
		echo "  → Running $$spec"; \
		LD_LIBRARY_PATH=../lib ./$$spec || exit 1; \
	done

performance: $(PERFORMANCE_SPECS:.c=)
	@echo "Running performance specs..."
	@for spec in $(PERFORMANCE_SPECS:.c=); do \
		echo "  → Running $$spec"; \
		LD_LIBRARY_PATH=../lib ./$$spec || exit 1; \
	done

run: all
	@echo "=== Running All QA Specifications ==="
	@$(MAKE) unit
	@$(MAKE) integration
	@$(MAKE) system
	@$(MAKE) performance
	@echo "=== All Specs Complete ==="

clean:
	@rm -f $(SPEC_BINARIES)
	@rm -f unit/*.o integration/*.o system/*.o performance/*.o
