#!/bin/bash
#
# NexusLink Test Runner
# Platform-independent CTest-based runner
# Copyright © 2025 OBINexus Computing

set -e  # Exit immediately if a command exits with a non-zero status

# ANSI color codes for better output readability
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Banner
echo -e "${BLUE}"
echo "  _   _ _     _       _    "
echo " | \ | | |   (_)     | |   "
echo " |  \| | |    _ _ __ | | __"
echo " | . \` | |   | | '_ \| |/ /"
echo " | |\  | |___| | | | |   < "
echo " |_| \_|\_____/_|_| |_|_|\_\\"
echo "                           "
echo -e "${NC}"
echo -e "${BLUE}NexusLink Test Runner (CTest-based)${NC}"
echo -e "${CYAN}Copyright © 2025 OBINexus Computing${NC}"
echo ""

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

# Determine script directory for relative paths
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${PROJECT_ROOT}/build"

# Check if build directory exists
if [ ! -d "${BUILD_DIR}" ]; then
    section "Creating Build Directory"
    mkdir -p "${BUILD_DIR}"
    success "Build directory created"
fi

# Change to build directory
cd "${BUILD_DIR}"

# Parse command line arguments
BUILD_TYPE="Debug"
RUN_PARALLEL=true
VERBOSE=false
TESTS_FILTER=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --release)
            BUILD_TYPE="Release"
            shift
            ;;
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --sequential)
            RUN_PARALLEL=false
            shift
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        --filter=*)
            TESTS_FILTER="${1#*=}"
            shift
            ;;
        *)
            warning "Unknown option: $1"
            shift
            ;;
    esac
done

# Configure the project
section "Configuring Project"
cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} "${PROJECT_ROOT}"
success "Project configured with build type: ${BUILD_TYPE}"

# Build the tests
section "Building Tests"
CMAKE_BUILD_PARALLEL_LEVEL=$(nproc) cmake --build . --config ${BUILD_TYPE}
success "Tests built successfully"

# Run the tests
section "Running Tests"

# Set CTest options
CTEST_OPTIONS=""
if [ "$RUN_PARALLEL" = true ]; then
    CTEST_OPTIONS="${CTEST_OPTIONS} -j$(nproc)"
fi

if [ "$VERBOSE" = true ]; then
    CTEST_OPTIONS="${CTEST_OPTIONS} -V"
else
    CTEST_OPTIONS="${CTEST_OPTIONS} --output-on-failure"
fi

if [ -n "$TESTS_FILTER" ]; then
    CTEST_OPTIONS="${CTEST_OPTIONS} -R ${TESTS_FILTER}"
    echo -e "${YELLOW}Running tests matching: ${TESTS_FILTER}${NC}"
else
    echo -e "${YELLOW}Running all tests${NC}"
fi

# Print test list
ctest --show-only | grep "Test #" | sed 's/^ *//'

# Run tests with CTest
set +e  # Allow for test failures
ctest ${CTEST_OPTIONS} --output-log test_output.log
TEST_RESULT=$?
set -e  # Back to fail-on-error

# Print summary
section "Test Summary"
PASS_COUNT=$(grep -c "Test Passed" test_output.log)
FAIL_COUNT=$(grep -c "Test Failed" test_output.log)
TOTAL_COUNT=$((PASS_COUNT + FAIL_COUNT))

echo "Passed: ${PASS_COUNT}"
echo "Failed: ${FAIL_COUNT}"
echo "Total: ${TOTAL_COUNT}"

# Print detailed failures if tests failed
if [ $FAIL_COUNT -gt 0 ]; then
    section "Failed Tests"
    grep -A 5 "The following tests FAILED" test_output.log
    error "Some tests failed."
    exit 1
else
    success "All tests passed!"
    exit 0
fi