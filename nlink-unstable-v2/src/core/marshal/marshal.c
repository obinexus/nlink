#include <core/marshal/marshal.h>
#include <core/marshal/marshal_types.h>
// src/core/marshal/marshal.c
// OBINexus Marshal: Data Serialization Coordination Protocol
#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Constitutional marshaling implementation
marshal_result_t marshal_create_context(marshal_context_t **ctx, size_t buffer_size) {
    if (!ctx || buffer_size == 0) {
        return MARSHAL_ERROR_INVALID_PARAMETER;
    }
    
    *ctx = malloc(sizeof(struct marshal_context));
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
    (*ctx)->initialized = true;
    (*ctx)->metadata_registry = NULL;
    (*ctx)->registry_size = 0;
    
    return MARSHAL_SUCCESS;
}

marshal_result_t marshal_destroy_context(marshal_context_t *ctx) {
    if (!ctx) {
        return MARSHAL_ERROR_INVALID_PARAMETER;
    }
    
    if (ctx->buffer) {
        free(ctx->buffer);
    }
    
    if (ctx->metadata_registry) {
        free(ctx->metadata_registry);
    }
    
    free(ctx);
    return MARSHAL_SUCCESS;
}

marshal_result_t marshal_reset_context(marshal_context_t *ctx) {
    if (!ctx || !ctx->initialized) {
        return MARSHAL_ERROR_INVALID_PARAMETER;
    }
    
    ctx->position = 0;
    return MARSHAL_SUCCESS;
}

marshal_result_t marshal_write_uint8(marshal_context_t *ctx, uint8_t value) {
    if (!ctx || !ctx->initialized) {
        return MARSHAL_ERROR_INVALID_PARAMETER;
    }
    
    if (ctx->position + sizeof(uint8_t) > ctx->buffer_size) {
        return MARSHAL_ERROR_BUFFER_OVERFLOW;
    }
    
    memcpy(ctx->buffer + ctx->position, &value, sizeof(uint8_t));
    ctx->position += sizeof(uint8_t);
    
    return MARSHAL_SUCCESS;
}

marshal_result_t marshal_write_uint16(marshal_context_t *ctx, uint16_t value) {
    if (!ctx || !ctx->initialized) {
        return MARSHAL_ERROR_INVALID_PARAMETER;
    }
    
    if (ctx->position + sizeof(uint16_t) > ctx->buffer_size) {
        return MARSHAL_ERROR_BUFFER_OVERFLOW;
    }
    
    memcpy(ctx->buffer + ctx->position, &value, sizeof(uint16_t));
    ctx->position += sizeof(uint16_t);
    
    return MARSHAL_SUCCESS;
}

marshal_result_t marshal_write_uint32(marshal_context_t *ctx, uint32_t value) {
    if (!ctx || !ctx->initialized) {
        return MARSHAL_ERROR_INVALID_PARAMETER;
    }
    
    if (ctx->position + sizeof(uint32_t) > ctx->buffer_size) {
        return MARSHAL_ERROR_BUFFER_OVERFLOW;
    }
    
    memcpy(ctx->buffer + ctx->position, &value, sizeof(uint32_t));
    ctx->position += sizeof(uint32_t);
    
    return MARSHAL_SUCCESS;
}

marshal_result_t marshal_read_uint32(marshal_context_t *ctx, uint32_t *value) {
    if (!ctx || !ctx->initialized || !value) {
        return MARSHAL_ERROR_INVALID_PARAMETER;
    }
    
    if (ctx->position + sizeof(uint32_t) > ctx->buffer_size) {
        return MARSHAL_ERROR_BUFFER_OVERFLOW;
    }
    
    memcpy(value, ctx->buffer + ctx->position, sizeof(uint32_t));
    ctx->position += sizeof(uint32_t);
    
    return MARSHAL_SUCCESS;
}

marshal_result_t marshal_get_buffer(marshal_context_t *ctx, uint8_t **buffer, size_t *length) {
    if (!ctx || !ctx->initialized || !buffer || !length) {
        return MARSHAL_ERROR_INVALID_PARAMETER;
    }
    
    *buffer = ctx->buffer;
    *length = ctx->position;
    
    return MARSHAL_SUCCESS;
}

marshal_result_t marshal_get_position(marshal_context_t *ctx, size_t *position) {
    if (!ctx || !ctx->initialized || !position) {
        return MARSHAL_ERROR_INVALID_PARAMETER;
    }
    
    *position = ctx->position;
    return MARSHAL_SUCCESS;
}

marshal_result_t marshal_set_position(marshal_context_t *ctx, size_t position) {
    if (!ctx || !ctx->initialized || position > ctx->buffer_size) {
        return MARSHAL_ERROR_INVALID_PARAMETER;
    }
    
    ctx->position = position;
    return MARSHAL_SUCCESS;
}
