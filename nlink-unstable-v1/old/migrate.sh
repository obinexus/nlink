#!/bin/bash
#
# NexusLink Project Migration Script
# This script reorganizes the NexusLink code according to the new structure
#
# Copyright © 2025 OBINexus Computing

set -e  # Exit on any error

# ANSI color codes for better output readability
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Display banner
echo -e "${BLUE}=========================${NC}"
echo -e "${BLUE} NexusLink Project Migration ${NC}"
echo -e "${BLUE}=========================${NC}"
echo ""

# Create backup directory
echo -e "${YELLOW}Creating backup...${NC}"
BACKUP_DIR="backup_$(date +%Y%m%d_%H%M%S)"
mkdir -p ${BACKUP_DIR}/include
mkdir -p ${BACKUP_DIR}/src

# Backup current files
cp -r include/* ${BACKUP_DIR}/include/
cp -r src/* ${BACKUP_DIR}/src/
echo -e "${GREEN}Backup created in ${BACKUP_DIR}${NC}"

# Create new directory structure
echo -e "${YELLOW}Creating new directory structure...${NC}"

# Create include directories
mkdir -p include/nexus/common
mkdir -p include/nexus/core
mkdir -p include/nexus/symbols
mkdir -p include/nexus/versioning
mkdir -p include/nexus/metadata
mkdir -p include/nexus/minimizer

# Create src directories
mkdir -p src/common
mkdir -p src/core
mkdir -p src/symbols
mkdir -p src/versioning
mkdir -p src/metadata
mkdir -p src/minimizer

# Copy new common headers
echo -e "${YELLOW}Adding common header files...${NC}"
if [ -f include/nexus/common/types.h ]; then
    echo "Common type header already exists"
else
    # This would be the new header file creation - already done in previous step
    echo "Please add the new common headers manually"
fi

# Add CMake files
echo -e "${YELLOW}Adding CMake files...${NC}"
cp ${BACKUP_DIR}/src/CMakeLists.txt ${BACKUP_DIR}/src/CMakeLists.txt.bak
echo "Backup of CMakeLists.txt created"

echo "Please manually add the new CMakeLists.txt files"

# Provide instructions for next steps
echo ""
echo -e "${GREEN}Migration preparation complete!${NC}"
echo ""
echo -e "${YELLOW}Next steps:${NC}"
echo "1. Update include paths in source files to use the new structure"
echo "2. Add the new CMakeLists.txt files from the artifacts"
echo "3. Build the project with the new structure:"
echo "   mkdir -p build && cd build && cmake .. && make"
echo ""
echo -e "${BLUE}Remember that a backup of your original files is in:${NC} ${BACKUP_DIR}"
