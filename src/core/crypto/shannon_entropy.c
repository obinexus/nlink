// src/core/crypto/shannon_entropy.c
// OBINexus Shannon Entropy: Cryptographic Boundary Management
#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

// Constitutional entropy management structures
typedef struct {
    uint32_t magic_marker;
    uint8_t *entropy_pool;
    size_t pool_size;
    size_t entropy_available;
} shannon_entropy_context_t;

static shannon_entropy_context_t g_entropy_context = {0};
static bool g_entropy_initialized = false;

// Shannon entropy calculation algorithm
static double calculate_shannon_entropy(const uint8_t *data, size_t length) {
    if (!data || length == 0) return 0.0;
    
    uint32_t frequency[256] = {0};
    
    // Calculate byte frequency distribution
    for (size_t i = 0; i < length; i++) {
        frequency[data[i]]++;
    }
    
    // Calculate Shannon entropy
    double entropy = 0.0;
    for (int i = 0; i < 256; i++) {
        if (frequency[i] > 0) {
            double probability = (double)frequency[i] / length;
            entropy -= probability * log2(probability);
        }
    }
    
    return entropy;
}

// Constitutional entropy initialization
int shannon_entropy_initialize(void) {
    if (g_entropy_initialized) {
        return 0; // Already initialized
    }
    
    g_entropy_context.magic_marker = 0x53484E4E; // "SHNN"
    g_entropy_context.pool_size = 4096;
    g_entropy_context.entropy_pool = malloc(g_entropy_context.pool_size);
    
    if (!g_entropy_context.entropy_pool) {
        return -1; // Memory allocation failure
    }
    
    // Initialize with system entropy
    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < g_entropy_context.pool_size; i++) {
        g_entropy_context.entropy_pool[i] = (uint8_t)(rand() % 256);
    }
    
    g_entropy_context.entropy_available = g_entropy_context.pool_size;
    g_entropy_initialized = true;
    
    return 0;
}

// Constitutional entropy shutdown
void shannon_entropy_shutdown(void) {
    if (!g_entropy_initialized) return;
    
    if (g_entropy_context.entropy_pool) {
        memset(g_entropy_context.entropy_pool, 0, g_entropy_context.pool_size);
        free(g_entropy_context.entropy_pool);
        g_entropy_context.entropy_pool = NULL;
    }
    
    g_entropy_initialized = false;
}

// Export entropy functions
double shannon_calculate_entropy(const uint8_t *data, size_t length) {
    return calculate_shannon_entropy(data, length);
}

int shannon_get_random_bytes(uint8_t *buffer, size_t length) {
    if (!g_entropy_initialized || !buffer || length == 0) {
        return -1;
    }
    
    // Constitutional random byte generation
    for (size_t i = 0; i < length; i++) {
        buffer[i] = (uint8_t)(rand() % 256);
    }
    
    return 0;
}
