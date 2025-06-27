#!/bin/bash

# NexusLink CLI Testing Script
# Aegis Project - Phase 1 Implementation
# Comprehensive validation and integration testing framework

set -e

# =============================================================================
# TEST CONFIGURATION AND CONSTANTS
# =============================================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
TARGET="$PROJECT_ROOT/nlink"
TEST_DIR="$PROJECT_ROOT/test_workspace"
VERBOSE=false

# Color codes for test output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

# Test counters
TESTS_RUN=0
TESTS_PASSED=0
TESTS_FAILED=0

# =============================================================================
# UTILITY FUNCTIONS
# =============================================================================

log_test() {
    echo -e "${CYAN}[TEST]${NC} $1"
}

log_pass() {
    echo -e "${GREEN}[PASS]${NC} $1"
    ((TESTS_PASSED++))
}

log_fail() {
    echo -e "${RED}[FAIL]${NC} $1"
    ((TESTS_FAILED++))
}

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

run_test() {
    local test_name="$1"
    local test_command="$2"
    local expected_exit_code="${3:-0}"
    
    ((TESTS_RUN++))
    
    if [ "$VERBOSE" = true ]; then
        log_test "Running: $test_name"
        log_info "Command: $test_command"
    fi
    
    if eval "$test_command" >/dev/null 2>&1; then
        local actual_exit_code=$?
        if [ $actual_exit_code -eq $expected_exit_code ]; then
            log_pass "$test_name"
            return 0
        else
            log_fail "$test_name (exit code: $actual_exit_code, expected: $expected_exit_code)"
            return 1
        fi
    else
        local actual_exit_code=$?
        if [ $actual_exit_code -eq $expected_exit_code ]; then
            log_pass "$test_name"
            return 0
        else
            log_fail "$test_name (exit code: $actual_exit_code, expected: $expected_exit_code)"
            return 1
        fi
    fi
}

# =============================================================================
# TEST ENVIRONMENT SETUP
# =============================================================================

setup_test_environment() {
    log_info "Setting up test environment"
    
    # Create test workspace
    rm -rf "$TEST_DIR"
    mkdir -p "$TEST_DIR"
    
    # Verify target executable exists
    if [ ! -f "$TARGET" ]; then
        log_fail "Target executable not found: $TARGET"
        log_info "Please run 'make all' to build the executable first"
        exit 1
    fi
    
    # Make executable if needed
    chmod +x "$TARGET"
    
    log_pass "Test environment setup complete"
}

cleanup_test_environment() {
    log_info "Cleaning up test environment"
    rm -rf "$TEST_DIR"
    log_pass "Test environment cleanup complete"
}

# =============================================================================
# BASIC FUNCTIONALITY TESTS
# =============================================================================

test_basic_functionality() {
    log_info "Running basic functionality tests"
    
    # Test help display
    run_test "Help command execution" "$TARGET --help"
    
    # Test version display
    run_test "Version command execution" "$TARGET --version"
    
    # Test invalid arguments
    run_test "Invalid argument handling" "$TARGET --invalid-option" 1
    
    # Test executable without arguments (should default to config-check)
    run_test "Default command execution" "$TARGET" 2  # Expects config not found
}

# =============================================================================
# CONFIGURATION PARSING TESTS
# =============================================================================

create_test_pkg_config() {
    local config_path="$1"
    cat > "$config_path" << 'EOF'
[project]
name = test_project
version = 1.0.0
entry_point = main.c

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
}

create_test_nlink_config() {
    local config_path="$1"
    cat > "$config_path" << 'EOF'
[component]
name = test_component
version = 1.0.0

[compilation]
optimization_level = 2
max_compile_time = 60
parallel_allowed = true
EOF
}

test_configuration_parsing() {
    log_info "Running configuration parsing tests"
    
    # Create test project structure
    local test_project="$TEST_DIR/test_project"
    mkdir -p "$test_project"
    mkdir -p "$test_project/component1"
    mkdir -p "$test_project/component2"
    
    # Create test configuration files
    create_test_pkg_config "$test_project/pkg.nlink"
    create_test_nlink_config "$test_project/component1/nlink.txt"
    create_test_nlink_config "$test_project/component2/nlink.txt"
    
    # Test configuration check with valid config
    run_test "Valid configuration parsing" "$TARGET --config-check --project-root $test_project"
    
    # Test parse-only mode
    run_test "Parse-only mode" "$TARGET --parse-only --project-root $test_project"
    
    # Test configuration check with verbose output
    if [ "$VERBOSE" = true ]; then
        run_test "Verbose configuration check" "$TARGET --config-check --verbose --project-root $test_project"
    fi
    
    # Test missing configuration file
    local empty_project="$TEST_DIR/empty_project"
    mkdir -p "$empty_project"
    run_test "Missing configuration handling" "$TARGET --config-check --project-root $empty_project" 2
}

# =============================================================================
# COMPONENT DISCOVERY TESTS
# =============================================================================

test_component_discovery() {
    log_info "Running component discovery tests"
    
    # Test single component project
    local single_project="$TEST_DIR/single_project"
    mkdir -p "$single_project/src"
    create_test_pkg_config "$single_project/pkg.nlink"
    
    run_test "Single component discovery" "$TARGET --discover-components --project-root $single_project"
    
    # Test multi-component project
    local multi_project="$TEST_DIR/multi_project"
    mkdir -p "$multi_project"/{comp1,comp2,comp3}
    create_test_pkg_config "$multi_project/pkg.nlink"
    create_test_nlink_config "$multi_project/comp1/nlink.txt"
    create_test_nlink_config "$multi_project/comp2/nlink.txt"
    create_test_nlink_config "$multi_project/comp3/nlink.txt"
    
    run_test "Multi-component discovery" "$TARGET --discover-components --project-root $multi_project"
    
    # Test verbose component discovery
    if [ "$VERBOSE" = true ]; then
        run_test "Verbose component discovery" "$TARGET --discover-components --verbose --project-root $multi_project"
    fi
}

# =============================================================================
# THREADING VALIDATION TESTS
# =============================================================================

test_threading_validation() {
    log_info "Running threading validation tests"
    
    # Create project with valid threading config
    local thread_project="$TEST_DIR/thread_project"
    mkdir -p "$thread_project"
    create_test_pkg_config "$thread_project/pkg.nlink"
    
    run_test "Threading configuration validation" "$TARGET --validate-threading --project-root $thread_project"
    
    # Create project with invalid threading config
    local invalid_thread_project="$TEST_DIR/invalid_thread_project"
    mkdir -p "$invalid_thread_project"
    cat > "$invalid_thread_project/pkg.nlink" << 'EOF'
[project]
name = invalid_thread_test
version = 1.0.0
entry_point = main.c

[threading]
worker_count = 0
queue_depth = 0
stack_size_kb = 32
EOF
    
    run_test "Invalid threading configuration detection" "$TARGET --validate-threading --project-root $invalid_thread_project" 5
}

# =============================================================================
# ERROR HANDLING TESTS
# =============================================================================

test_error_handling() {
    log_info "Running error handling tests"
    
    # Test nonexistent project root
    run_test "Nonexistent project root handling" "$TARGET --config-check --project-root /nonexistent/path" 2
    
    # Test malformed configuration file
    local malformed_project="$TEST_DIR/malformed_project"
    mkdir -p "$malformed_project"
    cat > "$malformed_project/pkg.nlink" << 'EOF'
This is not a valid configuration file
[invalid section
missing equals sign
EOF
    
    run_test "Malformed configuration handling" "$TARGET --config-check --project-root $malformed_project" 3
    
    # Test permission errors (if applicable)
    if [ "$(uname)" != "Darwin" ]; then  # Skip on macOS due to SIP
        local restricted_project="$TEST_DIR/restricted_project"
        mkdir -p "$restricted_project"
        create_test_pkg_config "$restricted_project/pkg.nlink"
        chmod 000 "$restricted_project/pkg.nlink"
        
        run_test "Permission error handling" "$TARGET --config-check --project-root $restricted_project" 3
        
        # Restore permissions for cleanup
        chmod 644 "$restricted_project/pkg.nlink"
    fi
}

# =============================================================================
# PERFORMANCE AND STRESS TESTS
# =============================================================================

test_performance() {
    log_info "Running performance tests"
    
    # Create large project structure
    local large_project="$TEST_DIR/large_project"
    mkdir -p "$large_project"
    create_test_pkg_config "$large_project/pkg.nlink"
    
    # Create many components
    for i in {1..20}; do
        mkdir -p "$large_project/component$i"
        create_test_nlink_config "$large_project/component$i/nlink.txt"
    done
    
    # Test performance with large project
    local start_time=$(date +%s.%N)
    run_test "Large project discovery performance" "$TARGET --discover-components --project-root $large_project"
    local end_time=$(date +%s.%N)
    local duration=$(echo "$end_time - $start_time" | bc -l)
    
    log_info "Performance test completed in ${duration}s"
    
    # Verify reasonable performance (less than 5 seconds)
    if (( $(echo "$duration < 5.0" | bc -l) )); then
        log_pass "Performance within acceptable limits"
    else
        log_warning "Performance test took longer than expected: ${duration}s"
    fi
}

# =============================================================================
# MAIN TEST EXECUTION
# =============================================================================

show_usage() {
    cat << EOF
NexusLink CLI Testing Script
Aegis Project Phase 1 Implementation

USAGE:
    $0 [OPTIONS]

OPTIONS:
    -h, --help          Show this help message
    -v, --verbose       Enable verbose test output
    --basic-only        Run only basic functionality tests
    --config-only       Run only configuration parsing tests
    --discovery-only    Run only component discovery tests
    --threading-only    Run only threading validation tests
    --performance-only  Run only performance tests
    --error-only        Run only error handling tests

EXAMPLES:
    $0                  # Run all tests
    $0 --verbose        # Run all tests with verbose output
    $0 --basic-only     # Run only basic functionality tests
EOF
}

show_test_summary() {
    echo
    echo "=== Test Summary ==="
    echo "Tests Run:    $TESTS_RUN"
    echo "Tests Passed: $TESTS_PASSED"
    echo "Tests Failed: $TESTS_FAILED"
    
    if [ $TESTS_FAILED -eq 0 ]; then
        log_pass "All tests passed successfully!"
        echo "===================="
        return 0
    else
        log_fail "$TESTS_FAILED test(s) failed"
        echo "===================="
        return 1
    fi
}

main() {
    local run_basic=true
    local run_config=true
    local run_discovery=true
    local run_threading=true
    local run_performance=true
    local run_error=true
    
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
            --basic-only)
                run_config=false
                run_discovery=false
                run_threading=false
                run_performance=false
                run_error=false
                shift
                ;;
            --config-only)
                run_basic=false
                run_discovery=false
                run_threading=false
                run_performance=false
                run_error=false
                shift
                ;;
            --discovery-only)
                run_basic=false
                run_config=false
                run_threading=false
                run_performance=false
                run_error=false
                shift
                ;;
            --threading-only)
                run_basic=false
                run_config=false
                run_discovery=false
                run_performance=false
                run_error=false
                shift
                ;;
            --performance-only)
                run_basic=false
                run_config=false
                run_discovery=false
                run_threading=false
                run_error=false
                shift
                ;;
            --error-only)
                run_basic=false
                run_config=false
                run_discovery=false
                run_threading=false
                run_performance=false
                shift
                ;;
            *)
                log_fail "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
    
    log_info "NexusLink CLI Test Suite Starting"
    
    # Setup test environment
    setup_test_environment
    
    # Run selected test suites
    if [ "$run_basic" = true ]; then
        test_basic_functionality
    fi
    
    if [ "$run_config" = true ]; then
        test_configuration_parsing
    fi
    
    if [ "$run_discovery" = true ]; then
        test_component_discovery
    fi
    
    if [ "$run_threading" = true ]; then
        test_threading_validation
    fi
    
    if [ "$run_error" = true ]; then
        test_error_handling
    fi
    
    if [ "$run_performance" = true ]; then
        test_performance
    fi
    
    # Cleanup and show results
    cleanup_test_environment
    show_test_summary
}

# Execute main function with all arguments
main "$@"
