// include/nlink/core/marshal/marshal.h
// OBINexus Marshal: Data Serialization Coordination Interface
#ifndef OBINEXUS_NLINK_CORE_MARSHAL_MARSHAL_H
#define OBINEXUS_NLINK_CORE_MARSHAL_MARSHAL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <nlink/core/marshal/marshal_types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constitutional Marshal Result System
typedef enum {
    MARSHAL_SUCCESS = 0,
    MARSHAL_ERROR_INVALID_PARAMETER = -1,
    MARSHAL_ERROR_BUFFER_OVERFLOW = -2,
    MARSHAL_ERROR_MEMORY_ALLOCATION = -3,
    MARSHAL_ERROR_SERIALIZATION_FAILED = -4,
    MARSHAL_ERROR_DESERIALIZATION_FAILED = -5,
    MARSHAL_ERROR_TYPE_MISMATCH = -6,
    MARSHAL_ERROR_CORRUPTION_DETECTED = -7
} marshal_result_t;

// Constitutional Marshal Context (Complete Type Definition)
typedef struct marshal_context marshal_context_t;

// Context Management Operations
marshal_result_t marshal_create_context(marshal_context_t **ctx, size_t buffer_size);
marshal_result_t marshal_destroy_context(marshal_context_t *ctx);
marshal_result_t marshal_reset_context(marshal_context_t *ctx);
marshal_result_t marshal_configure_context(marshal_context_t *ctx, const marshal_config_t *config);

// Basic Serialization Operations
marshal_result_t marshal_write_uint8(marshal_context_t *ctx, uint8_t value);
marshal_result_t marshal_write_uint16(marshal_context_t *ctx, uint16_t value);
marshal_result_t marshal_write_uint32(marshal_context_t *ctx, uint32_t value);
marshal_result_t marshal_write_uint64(marshal_context_t *ctx, uint64_t value);
marshal_result_t marshal_write_bytes(marshal_context_t *ctx, const uint8_t *data, size_t length);
marshal_result_t marshal_write_string(marshal_context_t *ctx, const char *str);

// Basic Deserialization Operations
marshal_result_t marshal_read_uint8(marshal_context_t *ctx, uint8_t *value);
marshal_result_t marshal_read_uint16(marshal_context_t *ctx, uint16_t *value);
marshal_result_t marshal_read_uint32(marshal_context_t *ctx, uint32_t *value);
marshal_result_t marshal_read_uint64(marshal_context_t *ctx, uint64_t *value);
marshal_result_t marshal_read_bytes(marshal_context_t *ctx, uint8_t *buffer, size_t length);
marshal_result_t marshal_read_string(marshal_context_t *ctx, char *buffer, size_t buffer_size);

// Advanced Marshal Coordination
marshal_result_t marshal_get_buffer(marshal_context_t *ctx, uint8_t **buffer, size_t *length);
marshal_result_t marshal_get_position(marshal_context_t *ctx, size_t *position);
marshal_result_t marshal_set_position(marshal_context_t *ctx, size_t position);
marshal_result_t marshal_get_capacity(marshal_context_t *ctx, size_t *capacity);

// Constitutional Buffer Management
marshal_result_t marshal_expand_buffer(marshal_context_t *ctx, size_t additional_size);
marshal_result_t marshal_compact_buffer(marshal_context_t *ctx);
marshal_result_t marshal_validate_integrity(marshal_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_MARSHAL_MARSHAL_H
