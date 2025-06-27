#!/bin/bash

# NexusLink CLI Build Script
# Aegis Project - Phase 1 Implementation
# Author: Nnamdi Michael Okpala & Development Team

set -e  # Exit on error

# =============================================================================
# BUILD CONFIGURATION AND CONSTANTS
# =============================================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
TARGET="nlink"

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Build configuration
VERBOSE=false
BUILD_TYPE="release"
CLEAN_FIRST=false
RUN_TESTS=false

# =============================================================================
# UTILITY FUNCTIONS
# =============================================================================

log_info() {
    echo -e "${BLUE}[NLINK BUILD]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[NLINK SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[NLINK WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[NLINK ERROR]${NC} $1"
}

show_usage() {
    cat << EOF
NexusLink CLI Build Script
Aegis Project Phase 1 Implementation

USAGE:
    $0 [OPTIONS]

OPTIONS:
    -h, --help          Show this help message
    -v, --verbose       Enable verbose build output
    -c, --clean         Clean before building
    -t, --test          Run tests after successful build
    -d, --debug         Build debug version
    -r, --release       Build release version (default)
    --config-check      Validate build configuration
    --show-config       Display current build settings

EXAMPLES:
    $0                  # Standard release build
    $0 -v -c -t         # Verbose clean build with tests
    $0 -d --test        # Debug build with testing
    $0 --config-check   # Validate build environment

For detailed build targets, use: make help
EOF
}

check_dependencies() {
    log_info "Checking build dependencies"
    
    # Check for required tools
    local missing_deps=()
    
    if ! command -v gcc >/dev/null 2>&1; then
        missing_deps+=("gcc")
    fi
    
    if ! command -v make >/dev/null 2>&1; then
        missing_deps+=("make")
    fi
    
    if [ ${#missing_deps[@]} -gt 0 ]; then
        log_error "Missing required dependencies: ${missing_deps[*]}"
        log_info "Please install the missing tools and try again"
        exit 1
    fi
    
    log_success "All required dependencies found"
}

validate_project_structure() {
    log_info "Validating project structure"
    
    local required_dirs=("core" "cli" "include/core" "include/cli")
    local required_files=("core/config.c" "cli/parser_interface.c" "include/core/config.h" "include/cli/parser_interface.h")
    
    cd "$PROJECT_ROOT"
    
    # Check directories
    for dir in "${required_dirs[@]}"; do
        if [ ! -d "$dir" ]; then
            log_error "Missing required directory: $dir"
            exit 1
        fi
    done
    
    # Check files
    for file in "${required_files[@]}"; do
        if [ ! -f "$file" ]; then
            log_error "Missing required file: $file"
            exit 1
        fi
    done
    
    log_success "Project structure validation complete"
}

show_build_config() {
    cat << EOF
=== NexusLink Build Configuration ===
Project Root: $PROJECT_ROOT
Build Directory: $BUILD_DIR
Target: $TARGET
Build Type: $BUILD_TYPE
Verbose: $VERBOSE
Clean First: $CLEAN_FIRST
Run Tests: $RUN_TESTS
====================================
EOF
}

create_build_dirs() {
    log_info "Creating build directories"
    mkdir -p "$BUILD_DIR"/{core,cli}
    log_success "Build directories created"
}

# =============================================================================
# BUILD FUNCTIONS
# =============================================================================

clean_build() {
    if [ "$CLEAN_FIRST" = true ]; then
        log_info "Cleaning previous build artifacts"
        cd "$PROJECT_ROOT"
        make clean
        log_success "Clean completed"
    fi
}

perform_build() {
    log_info "Starting $BUILD_TYPE build"
    cd "$PROJECT_ROOT"
    
    local make_target="$BUILD_TYPE"
    local make_args=""
    
    if [ "$VERBOSE" = true ]; then
        make_args="V=1"
    fi
    
    if ! make $make_args $make_target; then
        log_error "Build failed"
        exit 1
    fi
    
    log_success "Build completed successfully"
}

run_tests() {
    if [ "$RUN_TESTS" = true ]; then
        log_info "Running post-build tests"
        cd "$PROJECT_ROOT"
        
        if [ -f "./$TARGET" ]; then
            # Run basic functionality tests
            log_info "Testing configuration check"
            ./"$TARGET" --help >/dev/null 2>&1 || log_warning "Help command failed"
            
            log_info "Testing version display"
            ./"$TARGET" --version >/dev/null 2>&1 || log_warning "Version command failed"
            
            log_success "Basic tests completed"
        else
            log_error "Target executable not found: $TARGET"
            exit 1
        fi
    fi
}

# =============================================================================
# MAIN EXECUTION LOGIC
# =============================================================================

main() {
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_usage
                exit 0
                ;;
            -v|--verbose)
                VERBOSE=true
                shift
                ;;
            -c|--clean)
                CLEAN_FIRST=true
                shift
                ;;
            -t|--test)
                RUN_TESTS=true
                shift
                ;;
            -d|--debug)
                BUILD_TYPE="debug"
                shift
                ;;
            -r|--release)
                BUILD_TYPE="release"
                shift
                ;;
            --config-check)
                check_dependencies
                validate_project_structure
                show_build_config
                exit 0
                ;;
            --show-config)
                show_build_config
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
    
    # Execute build process
    log_info "NexusLink CLI Build Process Starting"
    
    if [ "$VERBOSE" = true ]; then
        show_build_config
    fi
    
    check_dependencies
    validate_project_structure
    create_build_dirs
    clean_build
    perform_build
    run_tests
    
    log_success "Build process completed successfully"
    
    # Display next steps
    cat << EOF

=== Build Complete ===
Executable: ./$TARGET
Next steps:
  ./$TARGET --help                    # View usage information
  ./$TARGET --config-check --verbose  # Test configuration parsing
  make test                           # Run comprehensive tests
  make install                        # Install system-wide
=====================
EOF
}

# Execute main function with all arguments
main "$@"
