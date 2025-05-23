#!/bin/bash
# Build script for NexusLink (nlink)
# Author: Nnamdi Michael Okpala
# Date: March 28, 2025

# Exit on error
set -e

# Configuration variables
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"
SRC_DIR="${PROJECT_ROOT}/src"
INCLUDE_DIR="${PROJECT_ROOT}/include"
COMPONENTS_DIR="${BUILD_DIR}/components"
OUTPUT_NAME="nlink"

# Colors for output formatting
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print usage information
usage() {
    echo "NexusLink Build System"
    echo "======================"
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -h, --help         Show this help message"
    echo "  -c, --clean        Clean build directory before building"
    echo "  -d, --debug        Build with debug symbols"
    echo "  -r, --release      Build in release mode (default)"
    echo "  -t, --test         Run tests after building"
    echo "  -v, --verbose      Verbose output during build"
    echo "  -p, --poc          Build proof-of-concept code only"
    echo "  --with-components  Build all component libraries"
}

# Clean build directory
clean_build() {
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf "${BUILD_DIR}"
    mkdir -p "${BUILD_DIR}"
    mkdir -p "${COMPONENTS_DIR}"
}

# Create necessary directories
setup_directories() {
    echo -e "${BLUE}Setting up directory structure...${NC}"
    mkdir -p "${BUILD_DIR}"
    mkdir -p "${COMPONENTS_DIR}"
    mkdir -p "${INCLUDE_DIR}"
    
    # Create src directory if it doesn't exist
    if [ ! -d "${SRC_DIR}" ]; then
        mkdir -p "${SRC_DIR}"
        echo -e "${YELLOW}Warning: Source directory created. No source files present.${NC}"
    fi
}

# Copy header files to include directory
setup_headers() {
    echo -e "${BLUE}Setting up header files...${NC}"
    
    # Create nexus_json.h if it doesn't exist in include directory
    if [ ! -f "${INCLUDE_DIR}/nexus_json.h" ]; then
        echo -e "${YELLOW}Warning: Creating nexus_json.h as it wasn't found${NC}"
        cp "${PROJECT_ROOT}/nexus_json.h" "${INCLUDE_DIR}/" 2>/dev/null || true
    fi
    
    # Ensure all header files are in include directory
    mkdir -p "${INCLUDE_DIR}"
    find "${SRC_DIR}" -name "*.h" -exec cp {} "${INCLUDE_DIR}/" \;
    
    # Verify essential headers exist
    MISSING_HEADERS=0
    for header in "nexus_json.h" "nexus_metadata.h" "nexus_symbols.h" "nexus_lazy.h"; do
        if [ ! -f "${INCLUDE_DIR}/${header}" ]; then
            echo -e "${RED}Error: Required header ${header} is missing${NC}"
            MISSING_HEADERS=1
        fi
    done
    
    if [ $MISSING_HEADERS -eq 1 ]; then
        echo -e "${RED}Critical headers are missing. Aborting build.${NC}"
        exit 1
    fi
}

# Build cold function library
build_cold_library() {
    local cflags=$1
    echo -e "${BLUE}Building cold function library...${NC}"
    
    # Check if cold.c exists
    if [ ! -f "${SRC_DIR}/cold.c" ]; then
        echo -e "${YELLOW}Warning: cold.c not found, creating minimal implementation${NC}"
        cat > "${SRC_DIR}/cold.c" << EOF
// Automatically generated cold.c for NexusLink demo
#include <stdio.h>

void cold_function(int x) {
    printf("Lazy-loaded function called with %d\n", x);
}
EOF
    fi
    
    # Build the shared library
    ${CC} ${cflags} -shared -fPIC "${SRC_DIR}/cold.c" -o "${COMPONENTS_DIR}/libcold.so"
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}Successfully built cold function library${NC}"
    else
        echo -e "${RED}Failed to build cold function library${NC}"
        exit 1
    fi
}

# Build all source files
build_source_files() {
    local cflags=$1
    local verbose=$2
    echo -e "${BLUE}Compiling source files...${NC}" >&2
    
    # Add include directory to compiler flags
    cflags="${cflags} -I${INCLUDE_DIR}"
    
    # Find all .c files except cold.c (which is built separately)
    C_FILES=$(find "${SRC_DIR}" -name "*.c" ! -name "cold.c")
    
    # Check if main.c exists
    if [ ! -f "${SRC_DIR}/main.c" ]; then
        echo -e "${RED}Error: main.c not found in source directory${NC}" >&2
        exit 1
    fi
    
    # Compile each source file
    OBJECT_FILES=""
    for src_file in $C_FILES; do
        base_name=$(basename "${src_file}")
        obj_file="${BUILD_DIR}/${base_name%.c}.o"
        OBJECT_FILES="${OBJECT_FILES} ${obj_file}"
        
        if [ "$verbose" = true ]; then
            echo -e "${BLUE}Compiling ${src_file}...${NC}" >&2
            ${CC} ${cflags} -c "${src_file}" -o "${obj_file}"
        else
            ${CC} ${cflags} -c "${src_file}" -o "${obj_file}" 2>/dev/null
        fi
        
        if [ $? -ne 0 ]; then
            echo -e "${RED}Failed to compile ${src_file}${NC}" >&2
            exit 1
        fi
    done
    
    echo -e "${GREEN}Successfully compiled all source files${NC}" >&2
    
    # Return the list of object files without color codes
    echo "${OBJECT_FILES}" | cat
}

# Link the executable
link_executable() {
    local cflags=$1
    local object_files=$2
    echo -e "${BLUE}Linking executable...${NC}"
    
    ${CC} ${cflags} ${object_files} -ldl -lpthread -o "${BUILD_DIR}/${OUTPUT_NAME}"
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}Successfully built ${OUTPUT_NAME} executable${NC}"
        echo -e "${GREEN}Binary located at: ${BUILD_DIR}/${OUTPUT_NAME}${NC}"
        # Make executable
        chmod +x "${BUILD_DIR}/${OUTPUT_NAME}"
    else
        echo -e "${RED}Failed to link ${OUTPUT_NAME} executable${NC}"
        exit 1
    fi
}

# Run the tests
run_tests() {
    echo -e "${BLUE}Running tests...${NC}"
    
    if [ -d "${PROJECT_ROOT}/test" ]; then
        # Execute the test runner if it exists
        if [ -x "${PROJECT_ROOT}/test/run_tests.sh" ]; then
            "${PROJECT_ROOT}/test/run_tests.sh"
        else
            echo -e "${YELLOW}No test runner found. Skipping tests.${NC}"
        fi
    else
        echo -e "${YELLOW}No test directory found. Skipping tests.${NC}"
    fi
}

# Run the demo
run_demo() {
    echo -e "${BLUE}Running NexusLink demonstration...${NC}"
    
    # Set library path to include the components directory
    export LD_LIBRARY_PATH="${COMPONENTS_DIR}:${LD_LIBRARY_PATH}"
    
    # Change to build directory
    cd "${BUILD_DIR}"
    
    # Run the demo
    "./${OUTPUT_NAME}"
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}Demonstration completed successfully${NC}"
    else
        echo -e "${RED}Demonstration failed with error code $?${NC}"
        exit 1
    fi
}

# Build proof-of-concept only
build_poc() {
    echo -e "${BLUE}Building proof-of-concept code only...${NC}"
    
    # Find all necessary source files
    local SOURCE_FILES="${SRC_DIR}/cold.c ${SRC_DIR}/main.c"
    
    # Add nexus_symbols.c and nexus_lazy.c if they exist (for load_cold_function)
    for extra_src in "nexus_lazy.c" "nexus_symbols.c"; do
        if [ -f "${SRC_DIR}/${extra_src}" ]; then
            SOURCE_FILES="${SOURCE_FILES} ${SRC_DIR}/${extra_src}"
        fi
    done
    
    # Compile all source files
    local OBJ_FILES=""
    for src in ${SOURCE_FILES}; do
        base_name=$(basename "${src}")
        obj_file="${BUILD_DIR}/${base_name%.c}.o"
        ${CC} -c "${src}" -o "${obj_file}" -I"${INCLUDE_DIR}"
        OBJ_FILES="${OBJ_FILES} ${obj_file}"
    done
    
    # Link all object files
    ${CC} -o "${BUILD_DIR}/nexus_poc" ${OBJ_FILES} -ldl    
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}Successfully built proof-of-concept executable${NC}"
        echo -e "${GREEN}Binary located at: ${BUILD_DIR}/nexus_poc${NC}"
        chmod +x "${BUILD_DIR}/nexus_poc"
    else
        echo -e "${RED}Failed to build proof-of-concept${NC}"
        exit 1
    fi
}

# Print build summary
print_summary() {
    echo -e "\n${GREEN}====== NexusLink Build Summary ======${NC}"
    echo -e "Build directory: ${BUILD_DIR}"
    echo -e "Binary: ${BUILD_DIR}/${OUTPUT_NAME}"
    echo -e "Components directory: ${COMPONENTS_DIR}"
    echo -e "Build mode: $1"
    echo -e "Compiler: ${CC}"
    echo -e "${GREEN}====================================${NC}\n"
}

# Main execution function
main() {
    local clean=false
    local build_type="release"
    local run_tests_after=false
    local verbose=false
    local poc_only=false
    local build_components=false
    local run_demo_after=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                usage
                exit 0
                ;;
            -c|--clean)
                clean=true
                shift
                ;;
            -d|--debug)
                build_type="debug"
                shift
                ;;
            -r|--release)
                build_type="release"
                shift
                ;;
            -t|--test)
                run_tests_after=true
                shift
                ;;
            -v|--verbose)
                verbose=true
                shift
                ;;
            -p|--poc)
                poc_only=true
                shift
                ;;
            --with-components)
                build_components=true
                shift
                ;;
            --run-demo)
                run_demo_after=true
                shift
                ;;
            *)
                echo -e "${RED}Unknown option: $1${NC}"
                usage
                exit 1
                ;;
        esac
    done
    
    # Banner
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}     NexusLink Build System v1.0        ${NC}"
    echo -e "${GREEN}     Author: Nnamdi Michael Okpala      ${NC}"
    echo -e "${GREEN}========================================${NC}"
    
    # Detect compiler - prefer clang over gcc
    if command -v clang &> /dev/null; then
        CC="clang"
        CXX="clang++"
    else
        CC="gcc"
        CXX="g++"
    fi
    
    # Adjust flags based on build type
    if [ "${build_type}" = "debug" ]; then
        CFLAGS="-g -O0 -Wall -Wextra"
        echo -e "${BLUE}Build mode: Debug${NC}"
    else
        CFLAGS="-O3 -Wall -Wextra"
        echo -e "${BLUE}Build mode: Release${NC}"
    fi
    
    # Add -v flag for verbose output
    if [ "${verbose}" = true ]; then
        CFLAGS="${CFLAGS} -v"
    fi
    
    # Clean if requested
    if [ "${clean}" = true ]; then
        clean_build
    fi
    
    # Setup directories and headers
    setup_directories
    setup_headers
    
    # Build proof-of-concept only
    if [ "${poc_only}" = true ]; then
        build_poc
        print_summary "${build_type} (PoC only)"
        exit 0
    fi
    
    # Build cold function library (component)
    if [ "${build_components}" = true ]; then
        build_cold_library "${CFLAGS}"
    fi
    
    # Build source files
    OBJECT_FILES=$(build_source_files "${CFLAGS}" "${verbose}")
    
    # Link executable
    link_executable "${CFLAGS}" "${OBJECT_FILES}"
    
    # Run tests if requested
    if [ "${run_tests_after}" = true ]; then
        run_tests
    fi
    
    # Run demo if requested
    if [ "${run_demo_after}" = true ]; then
        run_demo
    fi
    
    # Print build summary
    print_summary "${build_type}"
    
    echo -e "${GREEN}NexusLink build completed successfully!${NC}"
}

# Execute main function
main "$@"