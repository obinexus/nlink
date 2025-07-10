#!/bin/bash
# ZK-WXP Zero-Knowledge Proof Validation Script
# Comprehensive testing and validation for the ZKP implementation

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
NC='\033[0m'

# Test configuration
ITERATIONS=1000
BATCH_SIZE=100
TIMEOUT=300

echo -e "${BLUE}ZK-WXP Zero-Knowledge Proof Validation Suite${NC}"
echo "============================================="
echo "Date: $(date)"
echo "Host: $(hostname)"
echo "CPU: $(grep "model name" /proc/cpuinfo | head -1 | cut -d: -f2)"
echo

# Create test output directory
TEST_DIR="zkp_validation_$(date +%Y%m%d_%H%M%S)"
mkdir -p "$TEST_DIR"

# Function to run test with timeout
run_test() {
    local test_name=$1
    local test_cmd=$2
    local expected_result=${3:-0}
    
    echo -ne "${YELLOW}Running $test_name...${NC} "
    
    if timeout $TIMEOUT bash -c "$test_cmd" > "$TEST_DIR/${test_name}.log" 2>&1; then
        if [ $? -eq $expected_result ]; then
            echo -e "${GREEN}PASS${NC}"
            return 0
        else
            echo -e "${RED}FAIL (unexpected exit code)${NC}"
            return 1
        fi
    else
        echo -e "${RED}FAIL (timeout or error)${NC}"
        return 1
    fi
}

# 1. Unit Tests
echo -e "\n${MAGENTA}1. Unit Tests${NC}"
echo "-------------"

run_test "basic_proof_generation" "./bin/test_zkwxp_zkp"

# 2. Cryptographic Correctness Tests
echo -e "\n${MAGENTA}2. Cryptographic Correctness${NC}"
echo "----------------------------"

# Create test program for crypto validation
cat > "$TEST_DIR/test_crypto.c" << 'EOF'
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "zkwxp_proof.h"

int main() {
    zkwxp_proof_context_t ctx;
    zkwxp_proof_init_context(&ctx, NULL);
    
    // Test 1: Commitment binding property
    uint8_t data1[] = "test_data_1";
    uint8_t data2[] = "test_data_2";
    zkwxp_commitment_t comm1, comm2;
    
    zkwxp_proof_generate_commitment(&ctx, data1, sizeof(data1), 1, &comm1);
    zkwxp_proof_generate_commitment(&ctx, data2, sizeof(data2), 1, &comm2);
    
    // Different data should produce different commitments
    assert(memcmp(comm1.value, comm2.value, ZKWXP_COMMITMENT_SIZE) != 0);
    printf("✓ Commitment binding test passed\n");
    
    // Test 2: Field arithmetic correctness
    uint64_t a = 12345, b = 67890, c, d;
    
    // Test multiplicative inverse
    zkwxp_field_inverse(a, &c);
    zkwxp_field_multiply(a, c, &d);
    assert(d == 1);
    printf("✓ Field inverse test passed\n");
    
    // Test 3: Accumulator consistency
    uint8_t elem1[] = "element1";
    uint8_t elem2[] = "element2";
    
    uint64_t state1 = zkwxp_accumulator_get_state(&ctx);
    zkwxp_accumulator_add(&ctx, elem1, sizeof(elem1), 1);
    uint64_t state2 = zkwxp_accumulator_get_state(&ctx);
    zkwxp_accumulator_add(&ctx, elem2, sizeof(elem2), 1);
    uint64_t state3 = zkwxp_accumulator_get_state(&ctx);
    
    assert(state1 != state2);
    assert(state2 != state3);
    assert(state1 != state3);
    printf("✓ Accumulator state test passed\n");
    
    zkwxp_proof_free_context(&ctx);
    return 0;
}
EOF

gcc -o "$TEST_DIR/test_crypto" "$TEST_DIR/test_crypto.c" \
    -Iinclude/nlink -Iinclude/nlink/zkwxp \
    -Llib -lzkwxp_zkp -lcrypto -lssl -lm -pthread

run_test "cryptographic_properties" "$TEST_DIR/test_crypto"

# 3. Performance Benchmarks
echo -e "\n${MAGENTA}3. Performance Benchmarks${NC}"
echo "------------------------"

cat > "$TEST_DIR/benchmark.c" << 'EOF'
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "zkwxp_proof.h"
#include "zkwxp_codex.h"

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int main(int argc, char *argv[]) {
    int iterations = (argc > 1) ? atoi(argv[1]) : 1000;
    
    zkwxp_proof_context_t ctx;
    zkwxp_proof_init_context(&ctx, NULL);
    
    zkwxp_codex_t codex;
    zkwxp_codex_init(&codex, NULL);
    
    // Benchmark proof generation
    double start = get_time();
    for (int i = 0; i < iterations; i++) {
        uint8_t data[64];
        sprintf((char*)data, "test_data_%d", i);
        
        zkwxp_accumulator_add(&ctx, data, sizeof(data), 1);
        
        zkwxp_proof_t proof;
        zkwxp_proof_generate(&ctx, ZKWXP_PROOF_MEMBERSHIP,
                            data, sizeof(data), &proof);
    }
    double gen_time = get_time() - start;
    
    // Benchmark verification
    zkwxp_proof_t proof;
    uint8_t test_data[] = "benchmark_data";
    zkwxp_proof_generate(&ctx, ZKWXP_PROOF_MEMBERSHIP,
                        test_data, sizeof(test_data), &proof);
    
    uint64_t acc_state = zkwxp_accumulator_get_state(&ctx);
    
    start = get_time();
    for (int i = 0; i < iterations; i++) {
        zkwxp_proof_verify(&proof, &acc_state, sizeof(acc_state));
    }
    double verify_time = get_time() - start;
    
    printf("Proof generation: %.2f proofs/sec (%.3f ms/proof)\n",
           iterations / gen_time, gen_time * 1000 / iterations);
    printf("Proof verification: %.2f proofs/sec (%.3f ms/proof)\n",
           iterations / verify_time, verify_time * 1000 / iterations);
    
    // Benchmark codex operations
    start = get_time();
    for (int i = 0; i < iterations / 10; i++) {
        uint64_t entry_id;
        zkwxp_codex_add_proof(&codex, &proof, NULL, 0, &entry_id);
    }
    double codex_time = get_time() - start;
    
    printf("Codex storage: %.2f entries/sec\n", 
           (iterations / 10) / codex_time);
    
    zkwxp_proof_free_context(&ctx);
    zkwxp_codex_destroy(&codex);
    return 0;
}
EOF

gcc -O3 -o "$TEST_DIR/benchmark" "$TEST_DIR/benchmark.c" \
    -Iinclude/nlink -Iinclude/nlink/zkwxp \
    -Llib -lzkwxp_zkp -lcrypto -lssl -lm -pthread

run_test "performance_benchmark" "$TEST_DIR/benchmark $ITERATIONS"

# 4. Memory Safety Tests
echo -e "\n${MAGENTA}4. Memory Safety Tests${NC}"
echo "---------------------"

if command -v valgrind &> /dev/null; then
    run_test "memory_leak_check" \
        "valgrind --leak-check=full --error-exitcode=1 ./bin/test_zkwxp_zkp"
    
    run_test "memory_errors_check" \
        "valgrind --tool=memcheck --error-exitcode=1 $TEST_DIR/test_crypto"
else
    echo -e "${YELLOW}Valgrind not installed, skipping memory tests${NC}"
fi

# 5. Stress Tests
echo -e "\n${MAGENTA}5. Stress Tests${NC}"
echo "--------------"

cat > "$TEST_DIR/stress_test.c" << 'EOF'
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include "zkwxp_proof.h"
#include "zkwxp_codex.h"

#define NUM_THREADS 4
#define OPS_PER_THREAD 100

zkwxp_codex_t g_codex;

void* worker_thread(void* arg) {
    int thread_id = *(int*)arg;
    zkwxp_proof_context_t ctx;
    zkwxp_proof_init_context(&ctx, NULL);
    
    for (int i = 0; i < OPS_PER_THREAD; i++) {
        uint8_t data[64];
        sprintf((char*)data, "thread_%d_data_%d", thread_id, i);
        
        uint64_t entry_id;
        int ret = zkwxp_codex_generate_and_store(&g_codex, &ctx,
                                                ZKWXP_PROOF_MEMBERSHIP,
                                                data, sizeof(data),
                                                &entry_id);
        assert(ret == 0);
    }
    
    zkwxp_proof_free_context(&ctx);
    return NULL;
}

int main() {
    zkwxp_codex_init(&g_codex, NULL);
    
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    
    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, worker_thread, &thread_ids[i]);
    }
    
    // Wait for completion
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Verify results
    codex_stats_t stats;
    zkwxp_codex_get_stats(&g_codex, &stats);
    
    printf("Total entries created: %lu\n", stats.total_entries);
    assert(stats.total_entries == NUM_THREADS * OPS_PER_THREAD);
    
    zkwxp_codex_destroy(&g_codex);
    printf("✓ Concurrent stress test passed\n");
    return 0;
}
EOF

gcc -o "$TEST_DIR/stress_test" "$TEST_DIR/stress_test.c" \
    -Iinclude/nlink -Iinclude/nlink/zkwxp \
    -Llib -lzkwxp_zkp -lcrypto -lssl -lm -pthread

run_test "concurrent_stress_test" "$TEST_DIR/stress_test"

# 6. Integration Tests
echo -e "\n${MAGENTA}6. Integration Tests${NC}"
echo "-------------------"

run_test "full_integration" "./bin/zkwxp_integration"

# 7. Security Validation
echo -e "\n${MAGENTA}7. Security Validation${NC}"
echo "---------------------"

cat > "$TEST_DIR/security_test.c" << 'EOF'
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "zkwxp_proof.h"

int main() {
    zkwxp_proof_context_t ctx1, ctx2;
    zkwxp_proof_init_context(&ctx1, NULL);
    zkwxp_proof_init_context(&ctx2, NULL);
    
    // Test 1: Proof non-transferability
    uint8_t data[] = "secret_data";
    zkwxp_accumulator_add(&ctx1, data, sizeof(data), 1);
    
    zkwxp_proof_t proof;
    zkwxp_proof_generate(&ctx1, ZKWXP_PROOF_MEMBERSHIP,
                        data, sizeof(data), &proof);
    
    // Proof should not verify with different accumulator
    uint64_t acc1 = zkwxp_accumulator_get_state(&ctx1);
    uint64_t acc2 = zkwxp_accumulator_get_state(&ctx2);
    
    assert(zkwxp_proof_verify(&proof, &acc1, sizeof(acc1)) == 0);
    assert(zkwxp_proof_verify(&proof, &acc2, sizeof(acc2)) != 0);
    printf("✓ Proof non-transferability test passed\n");
    
    // Test 2: Proof tampering detection
    zkwxp_proof_t tampered_proof = proof;
    tampered_proof.response.proof_data[0] ^= 0xFF;
    
    assert(zkwxp_proof_verify(&tampered_proof, &acc1, sizeof(acc1)) != 0);
    printf("✓ Proof tampering detection test passed\n");
    
    zkwxp_proof_free_context(&ctx1);
    zkwxp_proof_free_context(&ctx2);
    return 0;
}
EOF

gcc -o "$TEST_DIR/security_test" "$TEST_DIR/security_test.c" \
    -Iinclude/nlink -Iinclude/nlink/zkwxp \
    -Llib -lzkwxp_zkp -lcrypto -lssl -lm -pthread

run_test "security_properties" "$TEST_DIR/security_test"

# Generate validation report
echo -e "\n${MAGENTA}Generating Validation Report${NC}"
echo "============================"

REPORT_FILE="$TEST_DIR/validation_report.txt"
{
    echo "ZK-WXP Zero-Knowledge Proof Validation Report"
    echo "==========================================="
    echo
    echo "Date: $(date)"
    echo "System: $(uname -a)"
    echo
    echo "Test Results:"
    echo "------------"
    
    # Parse test results
    passed=0
    failed=0
    
    for log in "$TEST_DIR"/*.log; do
        if [ -f "$log" ]; then
            test_name=$(basename "$log" .log)
            if grep -q "PASS\|passed\|✓" "$log" 2>/dev/null; then
                echo "✓ $test_name: PASSED"
                ((passed++))
            else
                echo "✗ $test_name: FAILED"
                ((failed++))
            fi
        fi
    done
    
    echo
    echo "Summary:"
    echo "-------"
    echo "Total tests: $((passed + failed))"
    echo "Passed: $passed"
    echo "Failed: $failed"
    
    if [ $failed -eq 0 ]; then
        echo
        echo "VALIDATION RESULT: SUCCESS"
        echo "All tests passed successfully!"
    else
        echo
        echo "VALIDATION RESULT: FAILURE"
        echo "Some tests failed. Check individual logs for details."
    fi
    
} > "$REPORT_FILE"

cat "$REPORT_FILE"

# Archive test results
tar -czf "zkp_validation_results_$(date +%Y%m%d_%H%M%S).tar.gz" "$TEST_DIR"

echo
echo -e "${GREEN}Validation complete. Results archived in: $TEST_DIR${NC}"

# Exit with appropriate code
if [ $failed -eq 0 ]; then
    exit 0
else
    exit 1
fi
