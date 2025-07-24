// src/core/crypto/shannon_entropy.c
#include <nlink/core/crypto/shannon_entropy.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

// Linear Congruential Generator for O(n) PRNG
typedef struct {
    uint64_t state;
    uint64_t multiplier;
    uint64_t increment;
} lcg_state_t;

static lcg_state_t g_prng = {
    .state = 0x123456789ABCDEF0ULL,
    .multiplier = 6364136223846793005ULL,
    .increment = 1442695040888963407ULL
};

crypto_context_t* shannon_crypto_init(crypto_primitive_t primitive_type, 
                                      const char* env_mode) {
    crypto_context_t* ctx = calloc(1, sizeof(crypto_context_t));
    if (!ctx) return NULL;
    
    // Create ETPS context with environment tagging
    char context_name[128];
    snprintf(context_name, sizeof(context_name), 
             "crypto_%s_%s", 
             env_mode ? env_mode : "unknown",
             primitive_type == CRYPTO_PRIMITIVE_SHA256 ? "sha256" : "other");
    
    ctx->etps_ctx = etps_context_create(context_name);
    if (!ctx->etps_ctx) {
        free(ctx);
        return NULL;
    }
    
    ctx->primitive_type = primitive_type;
    ctx->production_mode = (env_mode && strcmp(env_mode, "prod") == 0);
    
    // Initialize PRNG with entropy seed
    if (primitive_type == CRYPTO_PRIMITIVE_PRNG_LINEAR) {
        FILE* urandom = fopen("/dev/urandom", "rb");
        if (urandom) {
            fread(&g_prng.state, sizeof(uint64_t), 1, urandom);
            fclose(urandom);
        }
    }
    
    // Log initialization
    etps_log_info(ctx->etps_ctx, ETPS_COMPONENT_CORE, 
                  "shannon_crypto_init", 
                  "Cryptographic context initialized");
    
    return ctx;
}

double shannon_calculate_entropy(crypto_context_t* ctx, 
                                const uint8_t* data, 
                                size_t size) {
    if (!ctx || !data || size == 0) return 0.0;
    
    // Frequency table for byte values
    uint64_t freq[256] = {0};
    
    // O(n) frequency calculation
    for (size_t i = 0; i < size; i++) {
        freq[data[i]]++;
    }
    
    // Calculate Shannon entropy H = -Σ(p_i * log2(p_i))
    double entropy = 0.0;
    double size_d = (double)size;
    
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            double p = freq[i] / size_d;
            entropy -= p * log2(p);
        }
    }
    
    // Update metrics
    ctx->metrics.entropy_bits = entropy;
    ctx->metrics.sample_count = size;
    ctx->metrics.meets_threshold = (entropy >= SHANNON_MIN_ENTROPY_BITS);
    
    // Update telemetry
    shannon_update_telemetry(ctx, "entropy_calculation", true);
    
    return entropy;
}

bool shannon_validate_crypto_quality(crypto_context_t* ctx,
                                    const uint8_t* data,
                                    size_t size) {
    if (!ctx || !data || size < 256) {
        shannon_update_telemetry(ctx, "crypto_validation", false);
        return false;
    }
    
    // Calculate entropy
    double entropy = shannon_calculate_entropy(ctx, data, size);
    
    // Chi-square test for uniform distribution
    uint64_t freq[256] = {0};
    for (size_t i = 0; i < size; i++) {
        freq[data[i]]++;
    }
    
    double expected = (double)size / 256.0;
    double chi_square = 0.0;
    
    for (int i = 0; i < 256; i++) {
        double diff = freq[i] - expected;
        chi_square += (diff * diff) / expected;
    }
    
    ctx->metrics.chi_square = chi_square;
    
    // Critical value for 255 degrees of freedom at 0.01 significance
    const double critical_value = 310.457;
    bool chi_passed = (chi_square < critical_value);
    
    bool quality_passed = ctx->metrics.meets_threshold && chi_passed;
    
    // Log validation result
    if (!quality_passed) {
        etps_log_error(ctx->etps_ctx, ETPS_COMPONENT_CORE,
                      ETPS_ERROR_INVALID_INPUT, 
                      "shannon_validate_crypto_quality",
                      "Cryptographic quality validation failed");
    }
    
    shannon_update_telemetry(ctx, "crypto_validation", quality_passed);
    return quality_passed;
}

int shannon_prng_generate(crypto_context_t* ctx, 
                         uint8_t* output, 
                         size_t size) {
    if (!ctx || !output || size == 0) return -1;
    
    // O(n) Linear Congruential Generator
    for (size_t i = 0; i < size; i++) {
        g_prng.state = g_prng.state * g_prng.multiplier + g_prng.increment;
        output[i] = (uint8_t)(g_prng.state >> 56);  // Use high bits
    }
    
    // Validate output quality in dev mode
    if (!ctx->production_mode) {
        bool quality_ok = shannon_validate_crypto_quality(ctx, output, size);
        if (!quality_ok) {
            etps_log_error(ctx->etps_ctx, ETPS_COMPONENT_CORE,
                          ETPS_ERROR_INVALID_INPUT,
                          "shannon_prng_generate",
                          "PRNG output failed quality validation");
            return -1;
        }
    }
    
    ctx->operation_count++;
    shannon_update_telemetry(ctx, "prng_generate", true);
    return 0;
}

void shannon_update_telemetry(crypto_context_t* ctx,
                             const char* operation,
                             bool success) {
    if (!ctx || !ctx->etps_ctx) return;
    
    // Create telemetry event
    char message[256];
    snprintf(message, sizeof(message),
             "Crypto op=%s prim=%d entropy=%.3f samples=%lu success=%d mode=%s",
             operation,
             ctx->primitive_type,
             ctx->metrics.entropy_bits,
             ctx->metrics.sample_count,
             success,
             ctx->production_mode ? "prod" : "dev");
    
    if (success) {
        etps_log_info(ctx->etps_ctx, ETPS_COMPONENT_CORE,
                     "shannon_crypto", message);
    } else {
        etps_log_error(ctx->etps_ctx, ETPS_COMPONENT_CORE,
                      ETPS_ERROR_INVALID_INPUT,
                      "shannon_crypto", message);
    }
}

void shannon_crypto_destroy(crypto_context_t* ctx) {
    if (!ctx) return;
    
    shannon_update_telemetry(ctx, "crypto_destroy", true);
    
    if (ctx->etps_ctx) {
        etps_context_destroy(ctx->etps_ctx);
    }
    
    // Clear sensitive data
    memset(ctx, 0, sizeof(crypto_context_t));
    free(ctx);
}
