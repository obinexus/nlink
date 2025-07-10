/**
 * NexusLink ETPS (Error Telemetry Point System) - Fixed Implementation
 * OBINexus Aegis Engineering - Resolves clock_gettime compilation errors
 */

// Define feature test macros before any includes
#define _POSIX_C_SOURCE 199309L
#define _GNU_SOURCE

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "nlink_qa_poc/etps/telemetry.h"

// Fallback GUID generation using multiple entropy sources
static uint64_t generate_guid(void) {
    uint64_t guid = 0;
    
    // Method 1: Try clock_gettime (POSIX)
    #ifdef _POSIX_TIMERS
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == 0) {
        guid = (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
    } else
    #endif
    {
        // Method 2: Fallback to gettimeofday
        struct timeval tv;
        if (gettimeofday(&tv, NULL) == 0) {
            guid = (uint64_t)tv.tv_sec * 1000000ULL + (uint64_t)tv.tv_usec;
        } else {
            // Method 3: Last resort - use time() and add process ID
            guid = (uint64_t)time(NULL) * 1000000ULL + (uint64_t)getpid();
        }
    }
    
    // Add some randomness by XORing with address of local variable
    guid ^= (uint64_t)(uintptr_t)&guid;
    
    return guid;
}

etps_context_t* etps_context_create(const char* context_name) {
    etps_context_t* ctx = malloc(sizeof(etps_context_t));
    if (!ctx) {
        return NULL;
    }
    
    // Initialize context
    ctx->binding_guid = generate_guid();
    ctx->created_time = generate_guid();  // Use same function for consistency
    ctx->last_activity = ctx->created_time;
    ctx->is_active = true;
    
    // Set context name
    if (context_name && strlen(context_name) > 0) {
        strncpy(ctx->context_name, context_name, sizeof(ctx->context_name) - 1);
        ctx->context_name[sizeof(ctx->context_name) - 1] = '\0';
    } else {
        strcpy(ctx->context_name, "unknown");
    }
    
    return ctx;
}

void etps_context_destroy(etps_context_t* ctx) {
    if (ctx) {
        ctx->is_active = false;
        free(ctx);
    }
}

bool etps_validate_input(etps_context_t* ctx, const char* param_name, const void* value, const char* type) {
    // Basic validation
    if (!ctx || !param_name || !type) {
        return false;
    }
    
    // Update last activity
    ctx->last_activity = generate_guid();
    
    // Check if value is NULL
    if (!value) {
        etps_log_error(ctx, ETPS_COMPONENT_CORE, ETPS_ERROR_INVALID_INPUT, 
                      "etps_validate_input", "NULL value provided");
        return false;
    }
    
    // Type-specific validation
    if (strcmp(type, "string") == 0) {
        const char* str = (const char*)value;
        if (strlen(str) == 0) {
            etps_log_error(ctx, ETPS_COMPONENT_CORE, ETPS_ERROR_INVALID_INPUT, 
                          "etps_validate_input", "Empty string provided");
            return false;
        }
    }
    
    return true;
}

bool etps_validate_config(etps_context_t* ctx, const char* buffer, size_t size) {
    if (!ctx || !buffer || size == 0) {
        return false;
    }
    
    // Update last activity
    ctx->last_activity = generate_guid();
    
    // Basic buffer validation
    if (size > 1024 * 1024) { // 1MB limit
        etps_log_error(ctx, ETPS_COMPONENT_CONFIG, ETPS_ERROR_INVALID_INPUT, 
                      "etps_validate_config", "Buffer too large");
        return false;
    }
    
    // Check for null termination if it's supposed to be a string
    bool has_null_term = false;
    for (size_t i = 0; i < size; i++) {
        if (buffer[i] == '\0') {
            has_null_term = true;
            break;
        }
    }
    
    if (!has_null_term) {
        etps_log_error(ctx, ETPS_COMPONENT_CONFIG, ETPS_ERROR_INVALID_INPUT, 
                      "etps_validate_config", "Buffer not null-terminated");
        return false;
    }
    
    return true;
}

void etps_log_error(etps_context_t* ctx, etps_component_t component, 
                   etps_error_code_t error_code, const char* function, const char* message) {
    if (!ctx || !function || !message) {
        return;
    }
    
    // Update last activity
    ctx->last_activity = generate_guid();
    
    // Format and output error message
    fprintf(stderr, "[ETPS_ERROR] GUID:%lu Component:%d Error:%d Function:%s Message:%s Context:%s\n", 
            (unsigned long)ctx->binding_guid, component, error_code, function, message, ctx->context_name);
    
    // Flush stderr to ensure immediate output
    fflush(stderr);
}

void etps_log_info(etps_context_t* ctx, etps_component_t component, 
                  const char* function, const char* message) {
    if (!ctx || !function || !message) {
        return;
    }
    
    // Update last activity
    ctx->last_activity = generate_guid();
    
    // Format and output info message
    printf("[ETPS_INFO] GUID:%lu Component:%d Function:%s Message:%s Context:%s\n", 
           (unsigned long)ctx->binding_guid, component, function, message, ctx->context_name);
    
    // Flush stdout to ensure immediate output
    fflush(stdout);
}

// Additional utility functions for ETPS
uint64_t etps_get_current_timestamp(void) {
    return generate_guid();
}

const char* etps_get_component_name(etps_component_t component) {
    switch (component) {
        case ETPS_COMPONENT_CONFIG: return "CONFIG";
        case ETPS_COMPONENT_CLI: return "CLI";
        case ETPS_COMPONENT_CORE: return "CORE";
        case ETPS_COMPONENT_PARSER: return "PARSER";
        default: return "UNKNOWN";
    }
}

const char* etps_get_error_name(etps_error_code_t error_code) {
    switch (error_code) {
        case ETPS_ERROR_NONE: return "NONE";
        case ETPS_ERROR_INVALID_INPUT: return "INVALID_INPUT";
        case ETPS_ERROR_MEMORY_FAULT: return "MEMORY_FAULT";
        case ETPS_ERROR_CONFIG_PARSE: return "CONFIG_PARSE";
        case ETPS_ERROR_FILE_IO: return "FILE_IO";
        default: return "UNKNOWN_ERROR";
    }
}
