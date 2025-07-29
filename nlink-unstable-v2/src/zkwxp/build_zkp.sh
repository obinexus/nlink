#!/bin/bash
# ZK-WXP Zero-Knowledge Proof Build Script
# Part of the OBINexus NexusLink project

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Build configuration
BUILD_TYPE=${1:-"debug"}
JOBS=${2:-$(nproc)}

echo -e "${BLUE}ZK-WXP Zero-Knowledge Proof System Build${NC}"
echo "=========================================="
echo "Build type: $BUILD_TYPE"
echo "Parallel jobs: $JOBS"
echo

# Check dependencies
echo -e "${YELLOW}Checking dependencies...${NC}"

check_dependency() {
    if ! command -v $1 &> /dev/null; then
        echo -e "${RED}Error: $1 is not installed${NC}"
        exit 1
    fi
    echo -e "${GREEN}✓${NC} $1"
}

check_dependency gcc
check_dependency make
check_dependency pkg-config
check_dependency openssl

# Check OpenSSL development headers
if ! pkg-config --exists openssl; then
    echo -e "${RED}Error: OpenSSL development headers not found${NC}"
    echo "Install with: sudo apt-get install libssl-dev"
    exit 1
fi
echo -e "${GREEN}✓${NC} OpenSSL development headers"

# Create build directories
echo -e "\n${YELLOW}Creating build directories...${NC}"
mkdir -p build/obj/zkwxp
mkdir -p bin
mkdir -p lib
mkdir -p docs/zkp

# Copy header files if not already in place
echo -e "\n${YELLOW}Setting up header files...${NC}"
mkdir -p include/nlink/zkwxp

if [ ! -f "include/nlink/zkwxp/zkwxp_proof.h" ]; then
    echo "Creating zkwxp_proof.h..."
    # Copy from artifacts or create placeholder
    touch include/nlink/zkwxp/zkwxp_proof.h
fi

if [ ! -f "include/nlink/zkwxp/zkwxp_codex.h" ]; then
    echo "Creating zkwxp_codex.h..."
    # Copy from artifacts or create placeholder
    touch include/nlink/zkwxp/zkwxp_codex.h
fi

# Build ZKP libraries
echo -e "\n${YELLOW}Building ZKP libraries...${NC}"

case $BUILD_TYPE in
    "debug")
        make -f src/zkwxp/Makefile.zkp dev -j$JOBS
        ;;
    "release")
        make -f src/zkwxp/Makefile.zkp prod -j$JOBS
        ;;
    "coverage")
        make -f src/zkwxp/Makefile.zkp coverage -j$JOBS
        ;;
    *)
        echo -e "${RED}Unknown build type: $BUILD_TYPE${NC}"
        exit 1
        ;;
esac

# Run tests
echo -e "\n${YELLOW}Running ZKP tests...${NC}"
if make -f src/zkwxp/Makefile.zkp test; then
    echo -e "${GREEN}✓ All tests passed${NC}"
else
    echo -e "${RED}✗ Tests failed${NC}"
    exit 1
fi

# Security analysis (optional)
if [ "$BUILD_TYPE" = "debug" ] || [ "$BUILD_TYPE" = "coverage" ]; then
    echo -e "\n${YELLOW}Running security analysis...${NC}"
    make -f src/zkwxp/Makefile.zkp security-check || true
fi

# Build integration example
echo -e "\n${YELLOW}Building integration example...${NC}"
gcc -Wall -Wextra -O2 -g \
    -Iinclude/nlink -Iinclude/nlink/zkwxp \
    -o bin/zkwxp_integration \
    src/zkwxp/zkwxp_integration.c \
    -Llib -lzkwxp_zkp -lzkwxp \
    -pthread -lcrypto -lssl -lm

# Generate documentation
if command -v doxygen &> /dev/null; then
    echo -e "\n${YELLOW}Generating documentation...${NC}"
    make -f src/zkwxp/Makefile.zkp docs || true
fi

# Create deployment package
echo -e "\n${YELLOW}Creating deployment package...${NC}"
PACKAGE_DIR="zkwxp_zkp_${BUILD_TYPE}_$(date +%Y%m%d)"
mkdir -p "$PACKAGE_DIR"/{lib,include,bin,docs}

cp lib/libzkwxp_zkp.* "$PACKAGE_DIR/lib/"
cp include/nlink/zkwxp/*.h "$PACKAGE_DIR/include/"
cp bin/test_zkwxp_zkp "$PACKAGE_DIR/bin/" 2>/dev/null || true
cp bin/zkwxp_integration "$PACKAGE_DIR/bin/" 2>/dev/null || true
cp src/zkwxp/README_ZKP.md "$PACKAGE_DIR/docs/" 2>/dev/null || true

tar -czf "${PACKAGE_DIR}.tar.gz" "$PACKAGE_DIR"
rm -rf "$PACKAGE_DIR"

echo -e "${GREEN}✓ Package created: ${PACKAGE_DIR}.tar.gz${NC}"

# Summary
echo -e "\n${BLUE}Build Summary${NC}"
echo "============="
echo -e "Libraries built:"
ls -lh lib/libzkwxp_zkp.* 2>/dev/null || echo "No libraries found"
echo
echo -e "Executables built:"
ls -lh bin/test_zkwxp_zkp bin/zkwxp_integration 2>/dev/null || echo "No executables found"
echo

# Git status reminder
if git rev-parse --git-dir > /dev/null 2>&1; then
    echo -e "\n${YELLOW}Git status:${NC}"
    git status --short
    echo
    echo "To commit the ZKP implementation:"
    echo "  git checkout -b feature/zkwxp-zkp"
    echo "  git add src/zkwxp/zkwxp_proof.* src/zkwxp/zkwxp_codex.*"
    echo "  git add include/nlink/zkwxp/zkwxp_proof.h include/nlink/zkwxp/zkwxp_codex.h"
    echo "  git add src/zkwxp/Makefile.zkp src/zkwxp/README_ZKP.md"
    echo "  git commit -m \"feat: Add Zero-Knowledge Proof system for ZK-WXP\""
fi

echo -e "\n${GREEN}✓ ZKP build completed successfully!${NC}"
