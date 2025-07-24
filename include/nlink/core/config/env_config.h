// nlink/core/config/env_config.h
// OBINexus Constitutional Header Guard: OBINEXUS_NLINK_CORE_CONFIG_ENV_CONFIG_H
#ifndef OBINEXUS_NLINK_CORE_CONFIG_ENV_CONFIG_H
#define OBINEXUS_NLINK_CORE_CONFIG_ENV_CONFIG_H

// include/nlink/core/config/env_config.h

#include "nlink/core/config/types.h"
#include "nlink/core/crypto/shannon_entropy.h"

// Environment-specific crypto configuration
typedef struct {
    struct {
        bool enable_shannon_validation;
        double min_entropy_threshold;
        uint32_t validation_sample_size;
        bool enforce_quality_checks;
    } dev;
    
    struct {
        bool enable_telemetry;
        bool enable_validation;
        uint32_t max_telemetry_rate;
        bool fail_on_low_entropy;
    } prod;
} env_crypto_config_t;

// Disjoint environment configuration
typedef struct {
    char environment[32];  // "dev" or "prod"
    env_crypto_config_t crypto;
    
    // Explicit separation
    union {
        struct {
            bool verbose_logging;
            bool debug_assertions;
            bool timing_analysis;
        } dev_only;
        
        struct {
            bool audit_logging;
            bool performance_mode;
            uint32_t rate_limits;
        } prod_only;
    } env_specific;
} nlink_env_config_t;

// Load environment-specific configuration
nlink_config_result_t nlink_load_env_config(const char* env_name,
                                            nlink_env_config_t* config);

#endif

#endif // OBINEXUS_NLINK_CORE_CONFIG_ENV_CONFIG_H
