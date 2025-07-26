#!/bin/bash
# OBINexus NexusLink POC Build Script

set -e

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${BLUE}=== NexusLink POC Build System ===${NC}"

# Phase 1: Build C components
echo -e "${GREEN}[1/4] Building nlink core...${NC}"
make clean
make debug ETPS_ENABLED=1

# Phase 2: Setup Python POC environment
echo -e "${GREEN}[2/4] Setting up Python POC environment...${NC}"
python3 -m venv poc/venv
source poc/venv/bin/activate
pip install -r poc/requirements.txt 2>/dev/null || echo "No requirements.txt found"

# Phase 3: Run QA specs
echo -e "${GREEN}[3/4] Running QA specifications...${NC}"
make -C spec all

# Phase 4: Integration test
echo -e "${GREEN}[4/4] Running integration tests...${NC}"
./bin/nlink --etps-test --json
python3 poc/python_bridge.py

echo -e "${BLUE}=== Build Complete ===${NC}"
