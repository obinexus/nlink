// nlink/core/crypto/nlink_crypto_entropy.h
// OBINexus Constitutional Header Guard: OBINEXUS_NLINK_CORE_CRYPTO_NLINK_CRYPTO_ENTROPY_H
#ifndef OBINEXUS_NLINK_CORE_CRYPTO_NLINK_CRYPTO_ENTROPY_H
#define OBINEXUS_NLINK_CORE_CRYPTO_NLINK_CRYPTO_ENTROPY_H

// include/nlink/core/crypto/shannon_entropy.h

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "nlink/core/etps/telemetry.h"

#ifdef __cplusplus
extern "C" {
#endif

// Shannon Entropy Validation Thresholds
#define SHANNON_MIN_ENTROPY_BITS 7.0  // Minimum entropy for cryptographic quality
#define SHANNON_MAX_BIAS 0.01         // Maximum allowable bias

// Cryptographic Primitive Types
typedef enum {
    CRYPTO_PRIMITIVE_SHA256,
    CRYPTO_PRIMITIVE_SHA512,
    CRYPTO_PRIMITIVE_PRNG_LINEAR,  // O(n) PRNG
    CRYPTO_PRIMITIVE_PRNG_MERSENNE,
    CRYPTO_PRIMITIVE_AES_CTR
} crypto_primitive_t;

// Shannon Entropy Metrics
typedef struct {
    double entropy_bits;           // Calculated Shannon entropy
    double chi_square;            // Chi-square test result
    uint64_t sample_count;        // Number of samples analyzed
    bool meets_threshold;         // Passes cryptographic quality threshold
    etps_guid_t correlation_guid; // ETPS correlation ID
} shannon_metrics_t;

// Cryptographic Context with ETPS Integration
typedef struct {
    etps_context_t* etps_ctx;
    crypto_primitive_t primitive_type;
    shannon_metrics_t metrics;
    uint64_t operation_count;
    bool production_mode;  // Explicit env separation
} crypto_context_t;

// =============================================================================
// Core Shannon Entropy Functions
// =============================================================================

/**
 * Initialize cryptographic context with ETPS telemetry
 * @param primitive_type Type of cryptographic primitive
 * @param env_mode "dev" or "prod" for explicit environment separation
 * @return Initialized context or NULL on failure
 */
crypto_context_t* shannon_crypto_init(crypto_primitive_t primitive_type, 
                                      const char* env_mode);

/**
 * Calculate Shannon entropy for data buffer
 * @param ctx Crypto context
 * @param data Input data buffer
 * @param size Data size in bytes
 * @return Shannon entropy in bits
 */
double shannon_calculate_entropy(crypto_context_t* ctx, 
                                const uint8_t* data, 
                                size_t size);

/**
 * Validate cryptographic quality using Shannon entropy
 * @param ctx Crypto context
 * @param data Data to validate
 * @param size Data size
 * @return true if meets cryptographic standards
 */
bool shannon_validate_crypto_quality(crypto_context_t* ctx,
                                    const uint8_t* data,
                                    size_t size);

/**
 * O(n) PRNG with Shannon entropy validation
 * @param ctx Crypto context
 * @param output Output buffer
 * @param size Bytes to generate
 * @return 0 on success, error code otherwise
 */
int shannon_prng_generate(crypto_context_t* ctx, 
                         uint8_t* output, 
                         size_t size);

/**
 * Update ETPS telemetry with crypto metrics
 * @param ctx Crypto context
 * @param operation Operation name
 * @param success Operation result
 */
void shannon_update_telemetry(crypto_context_t* ctx,
                             const char* operation,
                             bool success);

/**
 * Clean up crypto context
 * @param ctx Context to destroy
 */
void shannon_crypto_destroy(crypto_context_t* ctx);

#ifdef __cplusplus
}
#endif


#endif // OBINEXUS_NLINK_CORE_CRYPTO_NLINK_CRYPTO_ENTROPY_H
