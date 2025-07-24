/**
 * @file marshal.c
 * @brief NexusLink Data Marshaling System
 * @methodology Waterfall - Phase 2 Implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "nlink/core/etps/telemetry.h"

// Marshal format types
typedef enum {
    MARSHAL_FORMAT_BINARY = 0,
    MARSHAL_FORMAT_JSON = 1,
    MARSHAL_FORMAT_XML = 2,
    MARSHAL_FORMAT_MSGPACK = 3
} marshal_format_t;

// Marshal buffer
typedef struct {
    uint8_t* data;
    size_t size;
    size_t capacity;
    size_t position;
} marshal_buffer_t;

// Marshal context
typedef struct {
    etps_context_t* etps_ctx;
    marshal_format_t format;
    marshal_buffer_t* buffer;
    bool error_state;
} marshal_context_t;

// Create marshal buffer
static marshal_buffer_t* marshal_buffer_create(size_t initial_capacity) {
    marshal_buffer_t* buf = calloc(1, sizeof(marshal_buffer_t));
    if (!buf) return NULL;
    
    buf->capacity = initial_capacity > 0 ? initial_capacity : 1024;
    buf->data = malloc(buf->capacity);
    if (!buf->data) {
        free(buf);
        return NULL;
    }
    
    buf->size = 0;
    buf->position = 0;
    return buf;
}

// Grow buffer if needed
static int marshal_buffer_ensure_capacity(marshal_buffer_t* buf, size_t required) {
    if (buf->size + required <= buf->capacity) return 0;
    
    size_t new_capacity = buf->capacity * 2;
    while (new_capacity < buf->size + required) {
        new_capacity *= 2;
    }
    
    uint8_t* new_data = realloc(buf->data, new_capacity);
    if (!new_data) return -1;
    
    buf->data = new_data;
    buf->capacity = new_capacity;
    return 0;
}

// Write data to buffer
static int marshal_buffer_write(marshal_buffer_t* buf, const void* data, size_t size) {
    if (marshal_buffer_ensure_capacity(buf, size) != 0) return -1;
    
    memcpy(buf->data + buf->size, data, size);
    buf->size += size;
    return 0;
}

// Create marshal context
marshal_context_t* marshal_create(marshal_format_t format) {
    marshal_context_t* ctx = calloc(1, sizeof(marshal_context_t));
    if (!ctx) return NULL;
    
    ctx->etps_ctx = etps_context_create("marshal");
    ctx->format = format;
    ctx->buffer = marshal_buffer_create(4096);
    
    if (!ctx->buffer) {
        etps_context_destroy(ctx->etps_ctx);
        free(ctx);
        return NULL;
    }
    
    etps_log_info(ctx->etps_ctx, ETPS_COMPONENT_MARSHAL,
                  "marshal_create", "Marshal context created");
    
    return ctx;
}

// Marshal integer
int marshal_int32(marshal_context_t* ctx, const char* name, int32_t value) {
    if (!ctx || ctx->error_state) return -1;
    
    switch (ctx->format) {
        case MARSHAL_FORMAT_BINARY: {
            // Write type tag + name length + name + value
            uint8_t type_tag = 0x01; // INT32
            uint8_t name_len = strlen(name);
            
            marshal_buffer_write(ctx->buffer, &type_tag, 1);
            marshal_buffer_write(ctx->buffer, &name_len, 1);
            marshal_buffer_write(ctx->buffer, name, name_len);
            marshal_buffer_write(ctx->buffer, &value, sizeof(int32_t));
            break;
        }
        
        case MARSHAL_FORMAT_JSON: {
            char json_str[256];
            snprintf(json_str, sizeof(json_str), "\"%s\":%d,", name, value);
            marshal_buffer_write(ctx->buffer, json_str, strlen(json_str));
            break;
        }
        
        default:
            ctx->error_state = true;
            return -1;
    }
    
    return 0;
}

// Marshal string
int marshal_string(marshal_context_t* ctx, const char* name, const char* value) {
    if (!ctx || ctx->error_state || !name || !value) return -1;
    
    switch (ctx->format) {
        case MARSHAL_FORMAT_BINARY: {
            uint8_t type_tag = 0x02; // STRING
            uint8_t name_len = strlen(name);
            uint16_t value_len = strlen(value);
            
            marshal_buffer_write(ctx->buffer, &type_tag, 1);
            marshal_buffer_write(ctx->buffer, &name_len, 1);
            marshal_buffer_write(ctx->buffer, name, name_len);
            marshal_buffer_write(ctx->buffer, &value_len, sizeof(uint16_t));
            marshal_buffer_write(ctx->buffer, value, value_len);
            break;
        }
        
        case MARSHAL_FORMAT_JSON: {
            char json_str[1024];
            snprintf(json_str, sizeof(json_str), "\"%s\":\"%s\",", name, value);
            marshal_buffer_write(ctx->buffer, json_str, strlen(json_str));
            break;
        }
        
        default:
            ctx->error_state = true;
            return -1;
    }
    
    return 0;
}

// Marshal binary data
int marshal_binary(marshal_context_t* ctx, const char* name, const uint8_t* data, size_t size) {
    if (!ctx || ctx->error_state || !name || !data) return -1;
    
    switch (ctx->format) {
        case MARSHAL_FORMAT_BINARY: {
            uint8_t type_tag = 0x03; // BINARY
            uint8_t name_len = strlen(name);
            uint32_t data_len = size;
            
            marshal_buffer_write(ctx->buffer, &type_tag, 1);
            marshal_buffer_write(ctx->buffer, &name_len, 1);
            marshal_buffer_write(ctx->buffer, name, name_len);
            marshal_buffer_write(ctx->buffer, &data_len, sizeof(uint32_t));
            marshal_buffer_write(ctx->buffer, data, size);
            break;
        }
        
        case MARSHAL_FORMAT_JSON: {
            // Base64 encode for JSON
            char* encoded = malloc(size * 2); // Simplified
            if (!encoded) return -1;
            
            // Simple hex encoding for demo
            char hex[3];
            encoded[0] = '\0';
            for (size_t i = 0; i < size; i++) {
                snprintf(hex, sizeof(hex), "%02x", data[i]);
                strcat(encoded, hex);
            }
            
            char json_str[2048];
            snprintf(json_str, sizeof(json_str), "\"%s\":\"%s\",", name, encoded);
            marshal_buffer_write(ctx->buffer, json_str, strlen(json_str));
            
            free(encoded);
            break;
        }
        
        default:
            ctx->error_state = true;
            return -1;
    }
    
    return 0;
}

// Finalize marshaling
int marshal_finalize(marshal_context_t* ctx, uint8_t** output, size_t* output_size) {
    if (!ctx || ctx->error_state || !output || !output_size) return -1;
    
    // Format-specific finalization
    if (ctx->format == MARSHAL_FORMAT_JSON) {
        // Remove trailing comma and add closing brace
        if (ctx->buffer->size > 0 && ctx->buffer->data[ctx->buffer->size - 1] == ',') {
            ctx->buffer->size--;
        }
        marshal_buffer_write(ctx->buffer, "}", 1);
        
        // Add opening brace at the beginning
        uint8_t* final_data = malloc(ctx->buffer->size + 1);
        if (!final_data) return -1;
        
        final_data[0] = '{';
        memcpy(final_data + 1, ctx->buffer->data, ctx->buffer->size);
        *output = final_data;
        *output_size = ctx->buffer->size + 1;
    } else {
        *output = malloc(ctx->buffer->size);
        if (!*output) return -1;
        
        memcpy(*output, ctx->buffer->data, ctx->buffer->size);
        *output_size = ctx->buffer->size;
    }
    
    etps_log_info(ctx->etps_ctx, ETPS_COMPONENT_MARSHAL,
                  "marshal_finalize", "Marshaling completed");
    
    return 0;
}

// Destroy marshal context
void marshal_destroy(marshal_context_t* ctx) {
    if (!ctx) return;
    
    if (ctx->buffer) {
        free(ctx->buffer->data);
        free(ctx->buffer);
    }
    
    etps_context_destroy(ctx->etps_ctx);
    free(ctx);
}
