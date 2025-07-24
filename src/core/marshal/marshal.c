// src/core/marshal/marshal.c
// OBINexus Marshal: Data Serialization Coordination Protocol
#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Constitutional marshaling structures
typedef struct {
    uint32_t magic_marker;
    uint8_t *buffer;
    size_t buffer_size;
    size_t position;
} marshal_context_t;

typedef enum {
    MARSHAL_SUCCESS = 0,
    MARSHAL_ERROR_INVALID_PARAMETER = -1,
    MARSHAL_ERROR_BUFFER_OVERFLOW = -2,
    MARSHAL_ERROR_MEMORY_ALLOCATION = -3
} marshal_result_t;

// Constitutional marshaling implementation
marshal_result_t marshal_create_context(marshal_context_t **ctx, size_t buffer_size) {
    if (!ctx || buffer_size == 0) {
        return MARSHAL_ERROR_INVALID_PARAMETER;
    }
    
    *ctx = malloc(sizeof(marshal_context_t));
    if (!*ctx) {
        return MARSHAL_ERROR_MEMORY_ALLOCATION;
    }
    
    (*ctx)->magic_marker = 0x4D415253; // "MARS"
    (*ctx)->buffer = malloc(buffer_size);
    if (!(*ctx)->buffer) {
        free(*ctx);
        return MARSHAL_ERROR_MEMORY_ALLOCATION;
    }
    
    (*ctx)->buffer_size = buffer_size;
    (*ctx)->position = 0;
    
    return MARSHAL_SUCCESS;
}

marshal_result_t marshal_destroy_context(marshal_context_t *ctx) {
    if (!ctx) {
        return MARSHAL_ERROR_INVALID_PARAMETER;
    }
    
    if (ctx->buffer) {
        free(ctx->buffer);
    }
    free(ctx);
    
    return MARSHAL_SUCCESS;
}

marshal_result_t marshal_write_uint32(marshal_context_t *ctx, uint32_t value) {
    if (!ctx || ctx->position + sizeof(uint32_t) > ctx->buffer_size) {
        return MARSHAL_ERROR_BUFFER_OVERFLOW;
    }
    
    memcpy(ctx->buffer + ctx->position, &value, sizeof(uint32_t));
    ctx->position += sizeof(uint32_t);
    
    return MARSHAL_SUCCESS;
}

marshal_result_t marshal_read_uint32(marshal_context_t *ctx, uint32_t *value) {
    if (!ctx || !value || ctx->position + sizeof(uint32_t) > ctx->buffer_size) {
        return MARSHAL_ERROR_INVALID_PARAMETER;
    }
    
    memcpy(value, ctx->buffer + ctx->position, sizeof(uint32_t));
    ctx->position += sizeof(uint32_t);
    
    return MARSHAL_SUCCESS;
}
