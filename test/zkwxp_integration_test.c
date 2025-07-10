/*
 * Zero-Knowledge Weighted XOR Proofs Integration Test
 * OBINexus Standard Compliant
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "nlink/zkwxp/zkwxp_core.h"
#include "nlink/core/etps/telemetry.h"

/* Test configuration */
#define TEST_ENTRY_COUNT 1000
#define TEST_ANOMALY_RATE 0.05  /* 5% anomalous entries */

/* Generate synthetic audit log entries */
static void generate_test_entries(zkwxp_audit_entry_t* entries, uint32_t count) {
    srand(time(NULL));
    
    for (uint32_t i = 0; i < count; i++) {
        zkwxp_audit_entry_t* entry = &entries[i];
        
        /* Timestamp with some variance */
        entry->timestamp = time(NULL) * 1000000000ULL + (i * 1000000);
        
        /* CPU and thread IDs */
        entry->cpu_id = rand() % 8;  /* 8 CPUs */
        entry->pid = 1000 + (rand() % 1000);
        entry->tid = entry->pid * 10 + (rand() % 10);
        
        /* Inject some anomalies */
        if ((double)rand() / RAND_MAX < TEST_ANOMALY_RATE) {
            /* Rapid context switching anomaly */
            entry->event_type = ZKWXP_EVENT_CONTEXT_SWITCH;
            entry->data.context_switch.from_tid = entry->tid;
            entry->data.context_switch.to_tid = entry->tid + 1;
            entry->data.context_switch.switch_time_ns = rand() % 1000;  /* Very fast */
        } else {
            /* Normal events */
            int event_choice = rand() % 5;
            switch (event_choice) {
                case 0:
                    entry->event_type = ZKWXP_EVENT_CONTEXT_SWITCH;
                    entry->data.context_switch.from_tid = entry->tid;
                    entry->data.context_switch.to_tid = entry->tid + (rand() % 100);
                    entry->data.context_switch.switch_time_ns = 10000 + (rand() % 90000);
                    break;
                    
                case 1:
                    entry->event_type = ZKWXP_EVENT_PRIORITY_CHANGE;
                    entry->data.priority_change.old_priority = 120;
                    entry->data.priority_change.new_priority = 100 + (rand() % 40);
                    break;
                    
                case 2:
                    entry->event_type = ZKWXP_EVENT_STATE_TRANSITION;
                    entry->data.state_transition.old_state = rand() % 5;
                    entry->data.state_transition.new_state = rand() % 5;
                    break;
                    
                case 3:
                    entry->event_type = ZKWXP_EVENT_THREAD_CREATE;
                    break;
                    
                case 4:
                    entry->event_type = ZKWXP_EVENT_SCHEDULER_TICK;
                    break;
            }
        }
    }
}

/* Test result structure */
typedef struct {
    bool passed;
    const char* test_name;
    const char* failure_reason;
} test_result_t;

/* Individual test functions */
static test_result_t test_context_init(void) {
    test_result_t result = {.test_name = "Context Initialization"};
    
    zkwxp_config_t config = {
        .proof_rounds = 10,
        .challenge_bits = 128,
        .batch_size = 1024,
        .cache_size = 4096,
        .enable_etps_telemetry = true,
        .enable_remote_scanning = false
    };
    
    zkwxp_context_t* ctx = NULL;
    NexusResult res = zkwxp_init(&ctx, &config);
    
    if (res != NEXUS_OK) {
        result.failure_reason = "Failed to initialize context";
        return result;
    }
    
    if (!ctx) {
        result.failure_reason = "Context is NULL after init";
        return result;
    }
    
    zkwxp_destroy(ctx);
    result.passed = true;
    return result;
}

static test_result_t test_rule_loading(void) {
    test_result_t result = {.test_name = "Rule Loading"};
    
    zkwxp_config_t config = {
        .proof_rounds = 10,
        .challenge_bits = 128,
        .batch_size = 1024,
        .cache_size = 4096,
        .enable_etps_telemetry = true,
        .enable_remote_scanning = false
    };
    
    zkwxp_context_t* ctx = NULL;
    zkwxp_init(&ctx, &config);
    
    /* Try to load rules from DSL file */
    NexusResult res = zkwxp_load_rules(ctx, "config/zkwxp_rules.dsl");
    
    if (res != NEXUS_OK) {
        result.failure_reason = "Failed to load rules";
        zkwxp_destroy(ctx);
        return result;
    }
    
    zkwxp_destroy(ctx);
    result.passed = true;
    return result;
}

static test_result_t test_entry_processing(void) {
    test_result_t result = {.test_name = "Entry Processing"};
    
    zkwxp_config_t config = {
        .proof_rounds = 10,
        .challenge_bits = 128,
        .batch_size = 1024,
        .cache_size = 4096,
        .enable_etps_telemetry = true,
        .enable_remote_scanning = false
    };
    
    zkwxp_context_t* ctx = NULL;
    zkwxp_init(&ctx, &config);
    zkwxp_load_rules(ctx, "config/zkwxp_rules.dsl");
    
    /* Generate test entries */
    zkwxp_audit_entry_t* entries = calloc(TEST_ENTRY_COUNT, sizeof(zkwxp_audit_entry_t));
    generate_test_entries(entries, TEST_ENTRY_COUNT);
    
    /* Process entries */
    NexusResult res = zkwxp_process_entries(ctx, entries, TEST_ENTRY_COUNT);
    
    if (res != NEXUS_OK) {
        result.failure_reason = "Failed to process entries";
        free(entries);
        zkwxp_destroy(ctx);
        return result;
    }
    
    /* Check statistics */
    zkwxp_stats_t stats;
    zkwxp_get_stats(ctx, &stats);
    
    if (stats.entries_processed != TEST_ENTRY_COUNT) {
        result.failure_reason = "Entry count mismatch";
        free(entries);
        zkwxp_destroy(ctx);
        return result;
    }
    
    free(entries);
    zkwxp_destroy(ctx);
    result.passed = true;
    return result;
}

static test_result_t test_proof_generation(void) {
    test_result_t result = {.test_name = "Proof Generation"};
    
    zkwxp_config_t config = {
        .proof_rounds = 10,
        .challenge_bits = 128,
        .batch_size = 1024,
        .cache_size = 4096,
        .enable_etps_telemetry = true,
        .enable_remote_scanning = false
    };
    
    zkwxp_context_t* ctx = NULL;
    zkwxp_init(&ctx, &config);
    zkwxp_load_rules(ctx, "config/zkwxp_rules.dsl");
    
    /* Generate and process test entries */
    zkwxp_audit_entry_t* entries = calloc(TEST_ENTRY_COUNT, sizeof(zkwxp_audit_entry_t));
    generate_test_entries(entries, TEST_ENTRY_COUNT);
    zkwxp_process_entries(ctx, entries, TEST_ENTRY_COUNT);
    
    /* Generate proof */
    zkwxp_proof_t* proof = NULL;
    NexusResult res = zkwxp_generate_proof(ctx, &proof);
    
    if (res != NEXUS_OK) {
        result.failure_reason = "Failed to generate proof";
        free(entries);
        zkwxp_destroy(ctx);
        return result;
    }
    
    if (!proof) {
        result.failure_reason = "Proof is NULL";
        free(entries);
        zkwxp_destroy(ctx);
        return result;
    }
    
    /* Verify proof fields */
    if (proof->rule_count == 0) {
        result.failure_reason = "No rules in proof";
        free(proof);
        free(entries);
        zkwxp_destroy(ctx);
        return result;
    }
    
    free(proof);
    free(entries);
    zkwxp_destroy(ctx);
    result.passed = true;
    return result;
}

static test_result_t test_proof_verification(void) {
    test_result_t result = {.test_name = "Proof Verification"};
    
    zkwxp_config_t config = {
        .proof_rounds = 10,
        .challenge_bits = 128,
        .batch_size = 1024,
        .cache_size = 4096,
        .enable_etps_telemetry = true,
        .enable_remote_scanning = false
    };
    
    zkwxp_context_t* ctx = NULL;
    zkwxp_init(&ctx, &config);
    zkwxp_load_rules(ctx, "config/zkwxp_rules.dsl");
    
    /* Generate and process test entries */
    zkwxp_audit_entry_t* entries = calloc(TEST_ENTRY_COUNT, sizeof(zkwxp_audit_entry_t));
    generate_test_entries(entries, TEST_ENTRY_COUNT);
    zkwxp_process_entries(ctx, entries, TEST_ENTRY_COUNT);
    
    /* Generate and verify proof */
    zkwxp_proof_t* proof = NULL;
    zkwxp_generate_proof(ctx, &proof);
    
    NexusResult res = zkwxp_verify_proof(ctx, proof);
    
    if (res != NEXUS_OK) {
        result.failure_reason = "Proof verification failed";
        free(proof);
        free(entries);
        zkwxp_destroy(ctx);
        return result;
    }
    
    free(proof);
    free(entries);
    zkwxp_destroy(ctx);
    result.passed = true;
    return result;
}

/* Main test runner */
int main(int argc, char* argv[]) {
    printf("=== Zero-Knowledge Weighted XOR Proofs Integration Test ===\n");
    printf("OBINexus Standard Compliant\n\n");
    
    /* Initialize ETPS telemetry */
    etps_init();
    
    /* Run tests */
    test_result_t tests[] = {
        test_context_init(),
        test_rule_loading(),
        test_entry_processing(),
        test_proof_generation(),
        test_proof_verification()
    };
    
    int num_tests = sizeof(tests) / sizeof(test_result_t);
    int passed = 0;
    int failed = 0;
    
    /* Print results */
    printf("Test Results:\n");
    printf("-------------\n");
    
    for (int i = 0; i < num_tests; i++) {
        if (tests[i].passed) {
            printf("[PASS] %s\n", tests[i].test_name);
            passed++;
        } else {
            printf("[FAIL] %s - %s\n", tests[i].test_name, 
                   tests[i].failure_reason ? tests[i].failure_reason : "Unknown");
            failed++;
        }
    }
    
    printf("\nSummary: %d passed, %d failed\n", passed, failed);
    
    /* QA Quadrant Analysis */
    printf("\nQA Quadrant Analysis:\n");
    printf("--------------------\n");
    printf("True Positives (TP): Anomalies correctly detected\n");
    printf("True Negatives (TN): Normal behavior correctly ignored\n");
    printf("False Positives (FP): Normal behavior flagged as anomaly [CRITICAL]\n");
    printf("False Negatives (FN): Anomalies missed [OPTIMIZATION OPPORTUNITY]\n");
    
    /* Calculate expected anomalies */
    int expected_anomalies = (int)(TEST_ENTRY_COUNT * TEST_ANOMALY_RATE);
    printf("\nExpected anomalies: ~%d (%.1f%%)\n", expected_anomalies, 
           TEST_ANOMALY_RATE * 100);
    
    /* Cleanup ETPS */
    etps_cleanup();
    
    return failed > 0 ? 1 : 0;
}
