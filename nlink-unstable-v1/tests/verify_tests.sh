#!/bin/bash
#
# NexusLink Test Verification Script
# Validates that all test files are properly referenced in CMakeLists.txt
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

section "Verifying Unit Tests"

# Get all .c files in the unit directory
unit_files=$(find unit -name "*.c" | sort)
unit_cmake=$(cat unit/CMakeLists.txt)

echo "Checking unit test files against CMake entries..."
missing_files=0

for file in $unit_files; do
    base_name=$(basename "$file" .c)
    if [[ "$unit_cmake" != *"$base_name"* ]]; then
        error "File $file not referenced in unit/CMakeLists.txt"
        missing_files=$((missing_files + 1))
    else
        success "File $file properly referenced in CMake"
    fi
done

if [ $missing_files -eq 0 ]; then
    success "All unit test files are properly referenced in CMake"
else
    warning "$missing_files unit test files are missing from CMake"
fi

section "Verifying Integration Tests"

# Get all .c files in the integration directory
integration_files=$(find integration -name "*.c" | sort)
integration_cmake=$(cat integration/CMakeLists.txt)

echo "Checking integration test files against CMake entries..."
missing_files=0

for file in $integration_files; do
    base_name=$(basename "$file" .c)
    if [[ "$integration_cmake" != *"$base_name"* ]]; then
        error "File $file not referenced in integration/CMakeLists.txt"
        missing_files=$((missing_files + 1))
    else
        success "File $file properly referenced in CMake"
    fi
done

if [ $missing_files -eq 0 ]; then
    success "All integration test files are properly referenced in CMake"
else
    warning "$missing_files integration test files are missing from CMake"
fi

section "Checking For Misspelled Filenames"

misspelled=$(find . -name "*intergration*")
if [ -n "$misspelled" ]; then
    error "Found files with misspelled 'integration': $misspelled"
else
    success "No misspelled filenames found"
fi

section "Test Directory Structure Verification"

# Verify that test_symbol_resolution.c exists in the correct location
if [ -f "unit/core/test_symbol_resolution.c" ]; then
    success "test_symbol_resolution.c is in the correct location"
else
    error "test_symbol_resolution.c is missing from unit/core/"
fi

# Verify that test_version_constraints.c exists in the correct location
if [ -f "unit/core/test_version_constraints.c" ]; then
    success "test_version_constraints.c is in the correct location"
else
    error "test_version_constraints.c is missing from unit/core/"
fi

# Verify that test_versioned_integration.c (correctly spelled) exists
if [ -f "unit/components/test_versioned_integration.c" ]; then
    success "test_versioned_integration.c is properly named"
else
    error "test_versioned_integration.c is missing or misspelled"
fi

section "Verification Complete"