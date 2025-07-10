#!/bin/bash

# NexusLink CLI Project Setup Script
# Aegis Project - Phase 1 Implementation
# Comprehensive project initialization and build environment configuration

set -e

# =============================================================================
# PROJECT CONFIGURATION AND CONSTANTS
# =============================================================================

PROJECT_NAME="NexusLink CLI Configuration Parser"
PROJECT_VERSION="1.0.0"
PROJECT_ROOT="$(pwd)"

# Color codes for setup output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

# Setup configuration
FORCE_SETUP=false
VERBOSE=false
CREATE_SAMPLE_CONFIG=false
INSTALL_HOOKS=false

# =============================================================================
# UTILITY FUNCTIONS
# =============================================================================

log_header() {
    echo -e "\n${BOLD}${BLUE}=== $1 ===${NC}\n"
}

log_info() {
    echo -e "${BLUE}[SETUP]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_verbose() {
    if [ "$VERBOSE" = true ]; then
        echo -e "${CYAN}[VERBOSE]${NC} $1"
    fi
}

show_usage() {
    cat << EOF
${PROJECT_NAME} Setup Script
Aegis Project Phase 1 Implementation

USAGE:
    $0 [OPTIONS]

OPTIONS:
    -h, --help              Show this help message
    -v, --verbose           Enable verbose setup output
    -f, --force             Force setup even if files exist
    -c, --create-samples    Create sample configuration files
    -g, --install-hooks     Install git hooks for development
    --check-only            Check setup without making changes
    --clean                 Clean and reset project structure

DESCRIPTION:
    This script initializes the NexusLink CLI project structure,
    creates necessary directories, sets up build scripts, and
    validates the development environment.

EXAMPLES:
    $0                      # Standard setup
    $0 -v -c               # Verbose setup with sample configs
    $0 --check-only        # Validate current setup
    $0 --clean             # Reset project structure
EOF
}

# =============================================================================
# SETUP VALIDATION FUNCTIONS
# =============================================================================

check_system_requirements() {
    log_header "System Requirements Validation"
    
    local missing_tools=()
    
    # Check essential build tools
    if ! command -v gcc >/dev/null 2>&1; then
        missing_tools+=("gcc")
    fi
    
    if ! command -v make >/dev/null 2>&1; then
        missing_tools+=("make")
    fi
    
    if ! command -v bash >/dev/null 2>&1; then
        missing_tools+=("bash")
    fi
    
    # Check optional tools
    local optional_tools=("git" "clang-format" "cppcheck")
    local missing_optional=()
    
    for tool in "${optional_tools[@]}"; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            missing_optional+=("$tool")
        fi
    done
    
    # Report results
    if [ ${#missing_tools[@]} -gt 0 ]; then
        log_error "Missing required tools: ${missing_tools[*]}"
        log_info "Please install the missing tools and run setup again"
        exit 1
    fi
    
    log_success "All required build tools found"
    
    if [ ${#missing_optional[@]} -gt 0 ]; then
        log_warning "Missing optional tools: ${missing_optional[*]}"
        log_info "Install these tools for enhanced development experience"
    fi
    
    log_verbose "GCC Version: $(gcc --version | head -n1)"
    log_verbose "Make Version: $(make --version | head -n1)"
}

validate_project_structure() {
    log_header "Project Structure Validation"
    
    local expected_dirs=("core" "cli" "include" "include/core" "include/cli")
    local expected_files=("core/config.c" "cli/parser_interface.c" "include/core/config.h" "include/cli/parser_interface.h")
    
    # Check directories
    for dir in "${expected_dirs[@]}"; do
        if [ -d "$dir" ]; then
            log_success "Directory exists: $dir"
        else
            log_error "Missing directory: $dir"
            return 1
        fi
    done
    
    # Check files
    for file in "${expected_files[@]}"; do
        if [ -f "$file" ]; then
            log_success "Source file exists: $file"
            log_verbose "  Size: $(wc -l < "$file") lines"
        else
            log_error "Missing source file: $file"
            return 1
        fi
    done
    
    log_success "Project structure validation complete"
}

# =============================================================================
# SETUP FUNCTIONS
# =============================================================================

create_project_directories() {
    log_header "Creating Project Directories"
    
    local directories=(
        "scripts"
        "build"
        "docs"
        "examples"
        "test_workspace"
        ".git/hooks"
    )
    
    for dir in "${directories[@]}"; do
        if [ ! -d "$dir" ] || [ "$FORCE_SETUP" = true ]; then
            mkdir -p "$dir"
            log_success "Created directory: $dir"
        else
            log_verbose "Directory already exists: $dir"
        fi
    done
}

setup_build_scripts() {
    log_header "Setting Up Build Scripts"
    
    # Make scripts executable
    local scripts=("scripts/build.sh" "scripts/run_tests.sh")
    
    for script in "${scripts[@]}"; do
        if [ -f "$script" ]; then
            chmod +x "$script"
            log_success "Made executable: $script"
        else
            log_warning "Script not found: $script"
        fi
    done
    
    # Validate Makefile
    if [ -f "Makefile" ]; then
        log_success "Makefile found and validated"
        log_verbose "Available targets: $(make -qp 2>/dev/null | grep '^[^#[:space:]].*:' | cut -d: -f1 | sort -u | head -10 | tr '\n' ' ')"
    else
        log_error "Makefile not found"
        return 1
    fi
}

create_sample_configurations() {
    if [ "$CREATE_SAMPLE_CONFIG" = true ]; then
        log_header "Creating Sample Configuration Files"
        
        # Create examples directory structure
        mkdir -p examples/{single_pass,multi_pass}/{src,comp1,comp2}
        
        # Single-pass example
        cat > examples/single_pass/pkg.nlink << 'EOF'
# NexusLink Single-Pass Configuration Example
[project]
name = single_pass_example
version = 1.0.0
entry_point = src/main.c

[build]
pass_mode = single
experimental_mode = false
strict_mode = true

[threading]
worker_count = 4
queue_depth = 64
stack_size_kb = 512
enable_work_stealing = true

[features]
unicode_normalization = true
isomorphic_reduction = true
debug_symbols = false
EOF
        
        # Multi-pass example
        cat > examples/multi_pass/pkg.nlink << 'EOF'
# NexusLink Multi-Pass Configuration Example
[project]
name = multi_pass_example
version = 1.0.0
entry_point = src/main.c

[build]
pass_mode = multi
experimental_mode = true
strict_mode = true

[threading]
worker_count = 8
queue_depth = 128
stack_size_kb = 1024
enable_work_stealing = true

[features]
unicode_normalization = true
isomorphic_reduction = true
debug_symbols = true
parallel_ast_optimization = true
EOF
        
        # Component configurations
        cat > examples/multi_pass/comp1/nlink.txt << 'EOF'
[component]
name = utility_component
version = 1.0.0

[compilation]
optimization_level = 2
max_compile_time = 60
parallel_allowed = true
EOF
        
        cat > examples/multi_pass/comp2/nlink.txt << 'EOF'
[component]
name = core_component
version = 1.0.0

[compilation]
optimization_level = 3
max_compile_time = 120
parallel_allowed = true
EOF
        
        log_success "Sample configuration files created in examples/"
    fi
}

install_development_hooks() {
    if [ "$INSTALL_HOOKS" = true ] && [ -d ".git" ]; then
        log_header "Installing Development Hooks"
        
        # Pre-commit hook for code formatting
        cat > .git/hooks/pre-commit << 'EOF'
#!/bin/bash
# NexusLink Pre-commit Hook - Code Quality Validation

echo "Running pre-commit validation..."

# Check for clang-format
if command -v clang-format >/dev/null 2>&1; then
    echo "Formatting source code..."
    make format
fi

# Run static analysis if available
if command -v cppcheck >/dev/null 2>&1; then
    echo "Running static analysis..."
    make analyze
fi

# Ensure build succeeds
echo "Testing build..."
if ! make clean && make all; then
    echo "Build failed - commit rejected"
    exit 1
fi

echo "Pre-commit validation passed"
EOF
        
        chmod +x .git/hooks/pre-commit
        log_success "Pre-commit hook installed"
        
        # Pre-push hook for comprehensive testing
        cat > .git/hooks/pre-push << 'EOF'
#!/bin/bash
# NexusLink Pre-push Hook - Comprehensive Testing

echo "Running pre-push validation..."

# Run full test suite
if ! make test; then
    echo "Tests failed - push rejected"
    exit 1
fi

echo "Pre-push validation passed"
EOF
        
        chmod +x .git/hooks/pre-push
        log_success "Pre-push hook installed"
    fi
}

create_documentation() {
    log_header "Creating Documentation Structure"
    
    # Create README if it doesn't exist
    if [ ! -f "README.md" ] || [ "$FORCE_SETUP" = true ]; then
        cat > README.md << EOF
# ${PROJECT_NAME}

Aegis Project Phase 1 Implementation - Systematic Configuration Parsing and Build Mode Resolution

## Overview

The NexusLink CLI provides comprehensive configuration parsing and validation for modular build systems following waterfall methodology principles. This implementation establishes the foundational architecture for deterministic pass-mode resolution and component discovery.

## Quick Start

\`\`\`bash
# Build the project
make all

# Run configuration validation
./nlink --config-check --verbose

# Test component discovery
./nlink --discover-components

# Run comprehensive tests
make test
\`\`\`

## Architecture

- **pkg.nlink**: Root manifest with global constraints and pass-mode declaration
- **nlink.txt**: Optional subcomponent coordination for multi-pass builds
- **Single-pass mode**: Linear execution chain without subcomponent discovery
- **Multi-pass mode**: Dependency-orchestrated coordination across components

## Build System

\`\`\`bash
make all          # Build executable
make clean        # Clean build artifacts
make test         # Run test suite
make install      # System installation
make help         # Show all targets
\`\`\`

## Configuration Format

See \`examples/\` directory for sample pkg.nlink and nlink.txt configurations demonstrating both single-pass and multi-pass project structures.

## Development

\`\`\`bash
./scripts/build.sh --verbose --test    # Development build with testing
./scripts/run_tests.sh --verbose       # Comprehensive test execution
\`\`\`

## Technical Specifications

- **Threading Pool**: Configurable worker threads with work-stealing scheduler
- **Unicode Normalization**: USCN-based isomorphic reduction for encoding consistency
- **Dependency Injection**: IoC pattern for systematic testing and validation
- **Error Propagation**: Waterfall methodology with comprehensive error handling

---

**Project**: Aegis Development Framework  
**Author**: Nnamdi Michael Okpala & Development Team  
**Architecture**: Waterfall Methodology with Systematic Validation
EOF
        
        log_success "README.md created"
    fi
    
    # Create basic development documentation
    mkdir -p docs
    cat > docs/BUILD.md << 'EOF'
# Build System Documentation

## Requirements
- GCC 4.9+ or Clang 3.5+
- GNU Make 3.81+
- POSIX-compliant shell
- pthread support

## Build Configuration
See Makefile for detailed build targets and configuration options.

## Testing Framework
Comprehensive test suite with functional, integration, and performance validation.
EOF
    
    log_success "Build documentation created"
}

# =============================================================================
# CLEANUP FUNCTIONS
# =============================================================================

clean_project() {
    log_header "Cleaning Project Structure"
    
    local clean_dirs=("build" "test_workspace")
    local clean_files=("nlink" "*.o" "core/*.o" "cli/*.o")
    
    for dir in "${clean_dirs[@]}"; do
        if [ -d "$dir" ]; then
            rm -rf "$dir"
            log_success "Removed directory: $dir"
        fi
    done
    
    for pattern in "${clean_files[@]}"; do
        if ls $pattern 2>/dev/null | grep -q .; then
            rm -f $pattern
            log_success "Removed files: $pattern"
        fi
    done
    
    log_success "Project cleanup completed"
}

# =============================================================================
# MAIN EXECUTION LOGIC
# =============================================================================

show_setup_summary() {
    log_header "Setup Summary"
    
    echo -e "${BOLD}Project:${NC} ${PROJECT_NAME}"
    echo -e "${BOLD}Version:${NC} ${PROJECT_VERSION}"
    echo -e "${BOLD}Root:${NC} ${PROJECT_ROOT}"
    echo
    
    if [ -f "nlink" ]; then
        echo -e "${GREEN}V${NC} Executable built successfully"
    else
        echo -e "${YELLOW}	${NC} Executable not built (run 'make all')"
    fi
    
    if [ -f "Makefile" ]; then
        echo -e "${GREEN}V${NC} Build system configured"
    else
        echo -e "${RED}?${NC} Build system missing"
    fi
    
    if [ -d "scripts" ] && [ -x "scripts/build.sh" ]; then
        echo -e "${GREEN}V${NC} Development scripts ready"
    else
        echo -e "${YELLOW}	${NC} Development scripts not configured"
    fi
    
    echo
    echo -e "${BOLD}Next Steps:${NC}"
    echo "  make all                    # Build the executable"
    echo "  ./nlink --help             # View usage information"
    echo "  make test                  # Run test suite"
    echo "  ./scripts/build.sh -v -t   # Development build with tests"
}

main() {
    local check_only=false
    local clean_only=false
    
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
            -f|--force)
                FORCE_SETUP=true
                shift
                ;;
            -c|--create-samples)
                CREATE_SAMPLE_CONFIG=true
                shift
                ;;
            -g|--install-hooks)
                INSTALL_HOOKS=true
                shift
                ;;
            --check-only)
                check_only=true
                shift
                ;;
            --clean)
                clean_only=true
                shift
                ;;
            *)
                log_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
    
    # Show project header
    echo -e "\n${BOLD}${CYAN}${PROJECT_NAME}${NC}"
    echo -e "${BOLD}Aegis Project Phase 1 Implementation Setup${NC}\n"
    
    # Execute based on mode
    if [ "$clean_only" = true ]; then
        clean_project
        exit 0
    fi
    
    # Standard setup process
    check_system_requirements
    
    if [ "$check_only" = true ]; then
        validate_project_structure
        show_setup_summary
        exit 0
    fi
    
    # Full setup execution
    validate_project_structure
    create_project_directories
    setup_build_scripts
    create_sample_configurations
    install_development_hooks
    create_documentation
    
    # Final validation and summary
    log_success "Setup process completed successfully"
    show_setup_summary
    
    echo -e "\n${GREEN}${BOLD}NexusLink CLI setup complete!${NC}"
    echo -e "Run '${BOLD}make all${NC}' to build the project.\n"
}

# Execute main function with all arguments
main "$@"
