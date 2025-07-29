#!/bin/bash
#
# NexusLink Test Restructuring Script
# This script reorganizes the test files to match the expected structure
# Copyright © 2025 OBINexus Computing

set -e  # Exit immediately if a command exits with a non-zero status

# ANSI color codes for better output readability
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Functions
section() {
    echo ""
    echo -e "${BLUE}==== $1 ====${NC}"
}

success() {
    echo -e "${GREEN}✓ $1${NC}"
}

warning() {
    echo -e "${YELLOW}! $1${NC}"
}

error() {
    echo -e "${RED}✗ $1${NC}"
}

# Set script directory as reference point
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

section "Correcting File Organization"

# 1. Create core directory if it doesn't exist
if [ ! -d "unit/core" ]; then
    mkdir -p "unit/core"
    success "Created unit/core directory"
fi

# 2. Move files from integration to unit/core where needed
if [ -f "integration/test_symbol_resolution.c" ]; then
    cp "integration/test_symbol_resolution.c" "unit/core/"
    success "Copied test_symbol_resolution.c to unit/core"
else
    warning "test_symbol_resolution.c not found in integration directory"
fi

if [ -f "integration/test_version_constraints.c" ]; then
    cp "integration/test_version_constraints.c" "unit/core/"
    success "Copied test_version_constraints.c to unit/core"
else
    warning "test_version_constraints.c not found in integration directory"
fi

# 3. Fix the misspelled filename
if [ -f "unit/components/test_versioned_intergration.c" ]; then
    mv "unit/components/test_versioned_intergration.c" "unit/components/test_versioned_integration.c"
    success "Renamed misspelled test_versioned_intergration.c to test_versioned_integration.c"
else
    warning "test_versioned_intergration.c not found"
fi

section "Directory Structure Normalization Complete"
echo "Please review the changes and update the CMakeLists.txt files accordingly."
