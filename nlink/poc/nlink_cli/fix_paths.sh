#!/bin/bash

# NexusLink CLI Path Resolution Fix Script
# Aegis Project - Phase 1 Implementation
# Systematic correction of include path architecture following nlink_enhanced pattern

set -e

# =============================================================================
# PATH RESOLUTION CONFIGURATION
# =============================================================================

PROJECT_ROOT="$(pwd)"
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
NC='\033[0m'

log_info() {
    echo -e "${BLUE}[PATH FIX]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# =============================================================================
# SYSTEMATIC PATH RESOLUTION
# =============================================================================

fix_include_paths() {
    log_info "Correcting include path architecture"
    
    # Fix core/config.c include paths
    if [ -f "core/config.c" ]; then
        sed -i 's|#include "nlink/core/config.h"|#include "core/config.h"|g' core/config.c
        log_success "Fixed core/config.c include paths"
    fi
    
    # Fix cli/parser_interface.c include paths  
    if [ -f "cli/parser_interface.c" ]; then
        sed -i 's|#include "nlink/cli/parser_interface.h"|#include "cli/parser_interface.h"|g' cli/parser_interface.c
        sed -i 's|#include "nlink/core/config.h"|#include "core/config.h"|g' cli/parser_interface.c
        log_success "Fixed cli/parser_interface.c include paths"
    fi
    
    # Fix include/cli/parser_interface.h include paths
    if [ -f "include/cli/parser_interface.h" ]; then
        sed -i 's|#include "nlink/core/config.h"|#include "core/config.h"|g' include/cli/parser_interface.h
        log_success "Fixed include/cli/parser_interface.h include paths"
    fi
}

create_build_directories() {
    log_info "Creating systematic build directory structure"
    
    mkdir -p build/{core,cli}
    mkdir -p scripts
    
    log_success "Build directories created"
}

validate_project_structure() {
    log_info "Validating corrected project structure"
    
    local required_files=(
        "core/config.c"
        "cli/parser_interface.c" 
        "include/core/config.h"
        "include/cli/parser_interface.h"
        "Makefile"
    )
    
    for file in "${required_files[@]}"; do
        if [ -f "$file" ]; then
            log_success "Found: $file"
        else
            log_warning "Missing: $file"
            return 1
        fi
    done
    
    return 0
}

test_compilation() {
    log_info "Testing systematic compilation with corrected paths"
    
    # Clean previous artifacts
    make clean 2>/dev/null || true
    
    # Test compilation
    if make all; then
        log_success "Compilation successful with corrected paths"
        return 0
    else
        log_warning "Compilation still failing - additional debugging required"
        return 1
    fi
}

create_alternative_structure() {
    log_info "Creating alternative include structure following nlink_enhanced pattern"
    
    # Create nlink subdirectory structure for compatibility
    mkdir -p include/nlink/{core,cli}
    
    # Create symbolic links to maintain both path structures
    if [ ! -L "include/nlink/core/config.h" ]; then
        ln -sf "../../core/config.h" "include/nlink/core/config.h"
        log_success "Created symbolic link: include/nlink/core/config.h"
    fi
    
    if [ ! -L "include/nlink/cli/parser_interface.h" ]; then
        ln -sf "../../cli/parser_interface.h" "include/nlink/cli/parser_interface.h" 
        log_success "Created symbolic link: include/nlink/cli/parser_interface.h"
    fi
}

show_resolution_summary() {
    echo -e "\n${BLUE}=== Path Resolution Summary ===${NC}"
    echo "✓ Include paths corrected to match directory structure"
    echo "✓ Build directories created systematically"
    echo "✓ Alternative nlink/ structure available for compatibility"
    echo ""
    echo "Next steps:"
    echo "  make clean && make all    # Test corrected compilation"
    echo "  ./nlink --help           # Verify executable functionality"
    echo "  make test                # Run comprehensive validation"
    echo "==============================="
}

# =============================================================================
# MAIN EXECUTION
# =============================================================================

main() {
    log_info "NexusLink CLI Path Resolution Fix - Aegis Project"
    
    # Execute systematic path resolution
    fix_include_paths
    create_build_directories
    
    # Create alternative structure for maximum compatibility
    create_alternative_structure
    
    # Validate project structure
    if validate_project_structure; then
        log_success "Project structure validation passed"
    else
        log_warning "Project structure validation failed"
        exit 1
    fi
    
    # Test compilation
    if test_compilation; then
        log_success "Path resolution fix successful"
    else
        log_warning "Additional debugging may be required"
    fi
    
    show_resolution_summary
}

# Execute main function
main "$@"
