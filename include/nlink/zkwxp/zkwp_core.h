// nlink/zkwxp/zkwp_core.h
// OBINexus Constitutional Header Guard: OBINEXUS_NLINK_ZKWXP_ZKWP_CORE_H
#ifndef OBINEXUS_NLINK_ZKWXP_ZKWP_CORE_H
#define OBINEXUS_NLINK_ZKWXP_ZKWP_CORE_H

/*
 * NexusLink Zero-Knowledge Weighted XOR Proofs Core
 * OBINexus Standard Compliant
 * 
 * This header defines the core structures and interfaces for the ZK-WXP system
 * integrated with kernel scheduler audit log analysis.
 */


#include <stdint.h>
#include <stdbool.h>
#include "nlink/core/etps/telemetry.h"
#include "nlink/core/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
typedef struct zkwxp_context zkwxp_context_t;
typedef struct zkwxp_rule zkwxp_rule_t;
typedef struct zkwxp_proof zkwxp_proof_t;

/* Weight precision for XOR calculations */
#define ZKWXP_WEIGHT_PRECISION 1000000

/* Maximum rule complexity */
#define ZKWXP_MAX_RULE_DEPTH 16

/* Audit log event types from kernel scheduler */
typedef enum {
    ZKWXP_EVENT_CONTEXT_SWITCH = 0x01,
    ZKWXP_EVENT_PRIORITY_CHANGE = 0x02,
    ZKWXP_EVENT_STATE_TRANSITION = 0x04,
    ZKWXP_EVENT_THREAD_CREATE = 0x08,
    ZKWXP_EVENT_THREAD_DESTROY = 0x10,
    ZKWXP_EVENT_SCHEDULER_TICK = 0x20,
    ZKWXP_EVENT_LOAD_BALANCE = 0x40,
    ZKWXP_EVENT_MIGRATION = 0x80
} zkwxp_event_type_t;

/* Rule priority levels */
typedef enum {
    ZKWXP_PRIORITY_CRITICAL = 1000,
    ZKWXP_PRIORITY_HIGH = 750,
    ZKWXP_PRIORITY_MEDIUM = 500,
    ZKWXP_PRIORITY_LOW = 250,
    ZKWXP_PRIORITY_INFO = 100
} zkwxp_priority_t;

/* Audit log entry structure */
typedef struct {
    uint64_t timestamp;
    uint32_t cpu_id;
    uint32_t pid;
    uint32_t tid;
    zkwxp_event_type_t event_type;
    union {
        struct {
            uint32_t from_tid;
            uint32_t to_tid;
            uint64_t switch_time_ns;
        } context_switch;
        
        struct {
            int32_t old_priority;
            int32_t new_priority;
        } priority_change;
        
        struct {
            uint32_t old_state;
            uint32_t new_state;
        } state_transition;
    } data;
} zkwxp_audit_entry_t;

/* Weighted XOR accumulator */
typedef struct {
    uint64_t xor_value;
    uint64_t weight_sum;
    uint32_t entry_count;
    uint8_t hash_state[32];  /* SHA-256 state */
} zkwxp_accumulator_t;

/* Rule definition structure */
struct zkwxp_rule {
    uint32_t rule_id;
    char name[64];
    zkwxp_priority_t priority;
    uint32_t weight;
    
    /* Rule pattern matching */
    zkwxp_event_type_t event_mask;
    uint32_t pattern_flags;
    
    /* Thresholds for anomaly detection */
    struct {
        uint32_t min_occurrence;
        uint32_t max_occurrence;
        uint64_t time_window_ns;
    } thresholds;
    
    /* DSL expression (compiled) */
    void* dsl_expr;
    uint32_t dsl_expr_size;
};

/* Zero-knowledge proof structure */
struct zkwxp_proof {
    /* Proof metadata */
    uint64_t proof_id;
    uint64_t timestamp;
    uint32_t rule_count;
    
    /* Commitment phase */
    uint8_t commitment[32];
    
    /* Challenge phase */
    uint8_t challenge[32];
    
    /* Response phase */
    zkwxp_accumulator_t accumulator;
    uint8_t response[64];
    
    /* Verification data */
    bool verified;
    uint32_t verification_rounds;
};

/* Context configuration */
typedef struct {
    /* Security parameters */
    uint32_t proof_rounds;
    uint32_t challenge_bits;
    
    /* Performance tuning */
    uint32_t batch_size;
    uint32_t cache_size;
    
    /* Integration settings */
    bool enable_etps_telemetry;
    bool enable_remote_scanning;
    
    /* Manifest gating */
    const char* whitelist_path;
    const char* blacklist_path;
} zkwxp_config_t;

/* Statistics tracking */
typedef struct {
    uint64_t entries_processed;
    uint64_t rules_evaluated;
    uint64_t proofs_generated;
    uint64_t proofs_verified;
    uint32_t anomalies_detected;
    double avg_proof_time_ms;
} zkwxp_stats_t;

/* Core API functions */

/**
 * Initialize ZK-WXP context with configuration
 */
NexusResult zkwxp_init(zkwxp_context_t** ctx, const zkwxp_config_t* config);

/**
 * Load rules from DSL file
 */
NexusResult zkwxp_load_rules(zkwxp_context_t* ctx, const char* dsl_file);

/**
 * Process audit log entries
 */
NexusResult zkwxp_process_entries(zkwxp_context_t* ctx, 
                                  const zkwxp_audit_entry_t* entries,
                                  uint32_t entry_count);

/**
 * Generate zero-knowledge proof
 */
NexusResult zkwxp_generate_proof(zkwxp_context_t* ctx,
                                 zkwxp_proof_t** proof);

/**
 * Verify zero-knowledge proof
 */
NexusResult zkwxp_verify_proof(zkwxp_context_t* ctx,
                               const zkwxp_proof_t* proof);

/**
 * Get statistics
 */
NexusResult zkwxp_get_stats(zkwxp_context_t* ctx, zkwxp_stats_t* stats);

/**
 * Cleanup and destroy context
 */
void zkwxp_destroy(zkwxp_context_t* ctx);

/* Remote scanning API */

/**
 * Initialize remote scanning capability
 */
NexusResult zkwxp_remote_init(zkwxp_context_t* ctx,
                              const char* remote_endpoint,
                              const uint8_t* auth_key,
                              uint32_t auth_key_len);

/**
 * Scan remote kernel data structures
 */
NexusResult zkwxp_remote_scan(zkwxp_context_t* ctx,
                              uint64_t kernel_addr,
                              uint32_t scan_size,
                              zkwxp_proof_t** proof);

/* DSL compiler API */

/**
 * Compile DSL expression to bytecode
 */
NexusResult zkwxp_dsl_compile(const char* expression,
                              void** bytecode,
                              uint32_t* bytecode_size);

/**
 * Free compiled DSL bytecode
 */
void zkwxp_dsl_free(void* bytecode);

#ifdef __cplusplus
}
#endif


#endif // OBINEXUS_NLINK_ZKWXP_ZKWP_CORE_H
