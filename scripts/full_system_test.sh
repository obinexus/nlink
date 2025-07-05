#!/bin/bash
# NexusLink Full System Integration Test
# Aegis Project - Comprehensive Validation

set -e

# Test counters
TESTS_RUN=0
TESTS_PASSED=0
TESTS_FAILED=0

# Test function
run_test() {
    local test_name="$1"
    local test_command="$2"
    
    echo -n "Testing $test_name... "
    TESTS_RUN=$((TESTS_RUN + 1))
    
    if eval "$test_command" > /dev/null 2>&1; then
        echo "✓ PASS"
        TESTS_PASSED=$((TESTS_PASSED + 1))
    else
        echo "✗ FAIL"
        TESTS_FAILED=$((TESTS_FAILED + 1))
    fi
}

echo "=== NexusLink Full System Test ==="
echo "Starting comprehensive validation..."

# Test 1: Binary existence
run_test "nlink binary" "[ -f bin/nlink ]"

# Test 2: ETPS functionality
run_test "ETPS telemetry" "./bin/nlink --etps-test"

# Test 3: Configuration loading
run_test "Config loading" "[ -f config/nlink.conf ]"

# Test 4: Demo build
run_test "Demo build" "cd demo && make clean && make all"

# Test 5: Demo execution
run_test "Demo execution" "cd demo && LD_LIBRARY_PATH=../lib ./nlink_demo"

# Test 6: Python POC
run_test "Python POC" "python3 poc/python_bridge.py"

# Test 7: Spec framework
run_test "Spec framework" "[ -d spec ] && [ -f spec/Makefile ]"

# Summary
echo ""
echo "=== Test Summary ==="
echo "Total Tests: $TESTS_RUN"
echo "Passed: $TESTS_PASSED"
echo "Failed: $TESTS_FAILED"
echo ""

if [ $TESTS_FAILED -eq 0 ]; then
    echo "✅ All tests passed!"
    exit 0
else
    echo "❌ Some tests failed"
    exit 1
fi
