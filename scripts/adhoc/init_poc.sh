#!/bin/bash
# NexusLink POC Initialization Script
# OBINexus Standard Compliant

set -e

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${BLUE}=== NexusLink POC Initialization ===${NC}"
echo -e "Project Root: $PROJECT_ROOT"

# Function to create directory with logging
create_dir() {
    local dir=$1
    if [ ! -d "$dir" ]; then
        mkdir -p "$dir"
        echo -e "${GREEN}✓${NC} Created: $dir"
    else
        echo -e "${YELLOW}→${NC} Exists: $dir"
    fi
}

# Phase 1: Directory Structure
echo -e "\n${BLUE}[Phase 1] Creating directory structure...${NC}"
create_dir "$PROJECT_ROOT/poc"
create_dir "$PROJECT_ROOT/spec/unit"
create_dir "$PROJECT_ROOT/spec/integration"
create_dir "$PROJECT_ROOT/spec/system"
create_dir "$PROJECT_ROOT/spec/performance"
create_dir "$PROJECT_ROOT/spec/fixtures"
create_dir "$PROJECT_ROOT/spec/helpers"
create_dir "$PROJECT_ROOT/config"
create_dir "$PROJECT_ROOT/logs"

# Phase 2: Configuration Files
echo -e "\n${BLUE}[Phase 2] Setting up configuration...${NC}"

# Create main config if not exists
if [ ! -f "$PROJECT_ROOT/config/nlink.conf" ]; then
    cat > "$PROJECT_ROOT/config/nlink.conf" << 'EOF'
# NexusLink Configuration File
# OBINexus Standard Compliant

[global]
project_name = nlink
version = 1.0.0
environment = development

[etps]
enabled = true
log_level = debug
output_format = json
log_file = logs/nlink_etps.log
correlation_guid = true
timestamp_precision = nanosecond
buffer_size = 8192

[etps.components]
core = true
crypto = true
parser = true
cli = true
marshal = true

[crypto]
shannon_entropy_enabled = true
min_entropy_threshold = 7.0
validation_sample_size = 1024
prng_algorithm = linear_congruential

[build]
orchestrator = polybuild
toolchain = nlink
optimization_level = 2
debug_symbols = true

[cli]
default_command = help
json_output = true
verbose_mode = false
color_output = true

[paths]
include_dir = include/nlink
source_dir = src
build_dir = build
lib_dir = lib
bin_dir = bin
spec_dir = spec

[qa]
test_framework = obin_spec
coverage_threshold = 80
static_analysis = true
memory_check = valgrind
EOF
    echo -e "${GREEN}✓${NC} Created: config/nlink.conf"
else
    echo -e "${YELLOW}→${NC} Exists: config/nlink.conf"
fi

# Phase 3: Python POC Environment
echo -e "\n${BLUE}[Phase 3] Setting up Python POC...${NC}"

# Check Python availability
if command -v python3 &> /dev/null; then
    echo -e "${GREEN}✓${NC} Python3 found: $(python3 --version)"
else
    echo -e "${RED}✗${NC} Python3 not found. Please install Python 3.6+"
    exit 1
fi

# Create Python requirements
cat > "$PROJECT_ROOT/poc/requirements.txt" << 'EOF'
# NexusLink POC Python Requirements
pytest>=6.0.0
pytest-cov>=2.10.0
black>=20.8b1
flake8>=3.8.0
mypy>=0.790
EOF
echo -e "${GREEN}✓${NC} Created: poc/requirements.txt"

# Phase 4: Spec Framework Setup
echo -e "\n${BLUE}[Phase 4] Setting up QA spec framework...${NC}"

# Create spec Makefile
cat > "$PROJECT_ROOT/spec/Makefile" << 'EOF'
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
EOF
echo -e "${GREEN}✓${NC} Created: spec/Makefile"

# Phase 5: Git Integration
echo -e "\n${BLUE}[Phase 5] Setting up Git integration...${NC}"

# Create .gitignore for POC
cat > "$PROJECT_ROOT/.gitignore" << 'EOF'
# Build outputs
build/
lib/
bin/
*.o
*.a
*.so

# POC artifacts
poc/venv/
poc/__pycache__/
*.pyc
*.pyo

# Logs
logs/
*.log
nlink_etps.log
nlink_etps_poc.log

# Spec outputs
spec/unit/*_spec
spec/integration/*_spec
spec/system/*_spec
spec/performance/*_spec
!spec/**/*_spec.c

# Editor files
.vscode/
.idea/
*.swp
*.swo

# OS files
.DS_Store
Thumbs.db
EOF
echo -e "${GREEN}✓${NC} Created: .gitignore"

# Phase 6: Validation
echo -e "\n${BLUE}[Phase 6] Validating setup...${NC}"

# Check for required headers
if [ -f "$PROJECT_ROOT/include/nlink/core/etps/telemetry.h" ]; then
    echo -e "${GREEN}✓${NC} ETPS headers found"
else
    echo -e "${YELLOW}!${NC} ETPS headers not found - ensure nlink is properly installed"
fi

# Summary
echo -e "\n${BLUE}=== Initialization Complete ===${NC}"
echo -e "Next steps:"
echo -e "  1. Build nlink: ${GREEN}make clean && make debug${NC}"
echo -e "  2. Run POC setup: ${GREEN}make poc-setup${NC}"
echo -e "  3. Run specs: ${GREEN}make spec-run${NC}"
echo -e "  4. Test integration: ${GREEN}./bin/nlink --poc-test${NC}"

# Create quick-start script
cat > "$PROJECT_ROOT/quick-start.sh" << 'EOF'
#!/bin/bash
# NexusLink Quick Start

echo "🚀 NexusLink Quick Start"
echo "Building system..."
make clean && make debug

echo "Setting up POC..."
make poc-setup

echo "Running QA specs..."
make spec-run

echo "Testing ETPS..."
./bin/nlink --etps-test --json

echo "✅ Quick start complete!"
EOF
chmod +x "$PROJECT_ROOT/quick-start.sh"
echo -e "${GREEN}✓${NC} Created: quick-start.sh"
