// include/nlink/core/crypto/shannon_entropy.h
// OBINexus Shannon Entropy: Cryptographic Boundary Management Interface
#ifndef OBINEXUS_NLINK_CORE_CRYPTO_SHANNON_ENTROPY_H
#define OBINEXUS_NLINK_CORE_CRYPTO_SHANNON_ENTROPY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constitutional Entropy Management Interface
int shannon_entropy_initialize(void);
void shannon_entropy_shutdown(void);

// Shannon Entropy Calculation Protocols
double shannon_calculate_entropy(const uint8_t *data, size_t length);
double shannon_calculate_conditional_entropy(const uint8_t *data_a, const uint8_t *data_b, size_t length);

// Constitutional Random Generation
int shannon_get_random_bytes(uint8_t *buffer, size_t length);
int shannon_get_secure_random(uint8_t *buffer, size_t length);

// Entropy Pool Management
int shannon_feed_entropy(const uint8_t *entropy_data, size_t length);
size_t shannon_get_available_entropy(void);
bool shannon_entropy_sufficient(size_t required_bytes);

// Advanced Cryptographic Coordination
int shannon_derive_key(const uint8_t *source, size_t source_len, uint8_t *key, size_t key_len);
int shannon_entropy_stretch(const uint8_t *input, size_t input_len, uint8_t *output, size_t output_len);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_CRYPTO_SHANNON_ENTROPY_H
