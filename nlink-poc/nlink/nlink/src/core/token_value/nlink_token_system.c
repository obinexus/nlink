/**
 * @file nlink_token_system.c
 * @brief Implementation of the unified token system for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include "nlink_token_system.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * Token system version
 */
#define NLINK_TOKEN_SYSTEM_VERSION_MAJOR 1
#define NLINK_TOKEN_SYSTEM_VERSION_MINOR 0
#define NLINK_TOKEN_SYSTEM_VERSION_PATCH 0

/**
 * Default token system configuration
 */
static const nlink_token_system_config default_config = {
    .enable_compile_time_checks = false,
    .strict_mode = false,
    .allow_implicit_casting = true,
    .max_custom_types = 32,
    .token_pool_size = 4096
};

/**
 * Global token system state
 */
static nlink_token_system_state system_state = {
    .status = NLINK_TOKEN_SYSTEM_UNINITIALIZED,
    .config = {0},
    .token_pool = NULL,
    .error_count = 0,
    .last_error = NULL
};

/**
 * Custom token type registry
 */
typedef struct {
    nlink_token_type_id type_id;
    nlink_token_type_id parent_type_id;
    nlink_transform_fn transform;
} nlink_custom_type_entry;

static struct {
    nlink_custom_type_entry* entries;
    size_t count;
    size_t capacity;
} custom_type_registry = {0};

/**
 * @brief Set the last error message
 * 
 * @param format Format string
 * @param ... Format arguments
 */
static void set_last_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    // Free previous error message if any
    if (system_state.last_error != NULL) {
        free(system_state.last_error);
        system_state.last_error = NULL;
    }
    
    // Determine required buffer size
    va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);
    
    if (size < 0) {
        va_end(args);
        system_state.error_count++;
        return;
    }
    
    // Allocate buffer and format error message
    system_state.last_error = malloc(size + 1);
    if (system_state.last_error != NULL) {
        vsnprintf(system_state.last_error, size + 1, format, args);
    }
    
    va_end(args);
    system_state.error_count++;
}

bool nlink_token_system_init(const nlink_token_system_config* config) {
    // Check if already initialized
    if (system_state.status == NLINK_TOKEN_SYSTEM_INITIALIZED) {
        return true;
    }
    
    // Prevent recursive initialization
    if (system_state.status == NLINK_TOKEN_SYSTEM_INITIALIZING) {
        set_last_error("Recursive token system initialization detected");
        system_state.status = NLINK_TOKEN_SYSTEM_ERROR;
        return false;
    }
    
    system_state.status = NLINK_TOKEN_SYSTEM_INITIALIZING;
    
    // Copy configuration (use defaults if NULL)
    if (config != NULL) {
        memcpy(&system_state.config, config, sizeof(nlink_token_system_config));
    } else {
        memcpy(&system_state.config, &default_config, sizeof(nlink_token_system_config));
    }
    
    // Initialize token type system
    nlink_token_type_system_init();
    
    // Initialize custom type registry
    custom_type_registry.capacity = system_state.config.max_custom_types;
    custom_type_registry.entries = malloc(sizeof(nlink_custom_type_entry) * custom_type_registry.capacity);
    if (custom_type_registry.entries == NULL) {
        set_last_error("Failed to allocate memory for custom type registry");
        system_state.status = NLINK_TOKEN_SYSTEM_ERROR;
        return false;
    }
    custom_type_registry.count = 0;
    
    // Initialize token pool if enabled
    if (system_state.config.token_pool_size > 0) {
        system_state.token_pool = malloc(system_state.config.token_pool_size);
        if (system_state.token_pool == NULL) {
            set_last_error("Failed to allocate memory for token pool");
            free(custom_type_registry.entries);
            custom_type_registry.entries = NULL;
            system_state.status = NLINK_TOKEN_SYSTEM_ERROR;
            return false;
        }
    }
    
    // Set status to initialized
    system_state.status = NLINK_TOKEN_SYSTEM_INITIALIZED;
    return true;
}

const nlink_token_system_state* nlink_token_system_get_state(void) {
    return &system_state;
}

void nlink_token_system_shutdown(void) {
    // Check if initialized
    if (system_state.status != NLINK_TOKEN_SYSTEM_INITIALIZED) {
        return;
    }
    
    // Free token pool
    if (system_state.token_pool != NULL) {
        free(system_state.token_pool);
        system_state.token_pool = NULL;
    }
    
    // Free custom type registry
    if (custom_type_registry.entries != NULL) {
        free(custom_type_registry.entries);
        custom_type_registry.entries = NULL;
        custom_type_registry.count = 0;
        custom_type_registry.capacity = 0;
    }
    
    // Free last error message
    if (system_state.last_error != NULL) {
        free(system_state.last_error);
        system_state.last_error = NULL;
    }
    
    // Reset state
    system_state.status = NLINK_TOKEN_SYSTEM_UNINITIALIZED;
    system_state.error_count = 0;
}

nlink_token_type_id nlink_token_system_create_custom_type(
    const char* name,
    nlink_token_type_id parent_type_id,
    size_t size,
    uint32_t flags
) {
    // Check if initialized
    if (system_state.status != NLINK_TOKEN_SYSTEM_INITIALIZED) {
        set_last_error("Token system not initialized");
        return 0;
    }
    
    // Check if custom type registry is full
    if (custom_type_registry.count >= custom_type_registry.capacity) {
        set_last_error("Custom type registry full");
        return 0;
    }
    
    // Generate a new type ID
    nlink_token_type_id type_id = NLINK_TYPE_CUSTOM_BASE + custom_type_registry.count;
    
    // Register the type
    if (!nlink_register_token_type(type_id, name, size, flags, 0)) {
        set_last_error("Failed to register custom type");
        return 0;
    }
    
    // Add to custom type registry
    nlink_custom_type_entry* entry = &custom_type_registry.entries[custom_type_registry.count++];
    entry->type_id = type_id;
    entry->parent_type_id = parent_type_id;
    entry->transform = NULL;
    
    return type_id;
}

void nlink_token_system_get_version(int* major, int* minor, int* patch) {
    if (major != NULL) {
        *major = NLINK_TOKEN_SYSTEM_VERSION_MAJOR;
    }
    if (minor != NULL) {
        *minor = NLINK_TOKEN_SYSTEM_VERSION_MINOR;
    }
    if (patch != NULL) {
        *patch = NLINK_TOKEN_SYSTEM_VERSION_PATCH;
    }
}

bool nlink_token_system_register_transform(
    nlink_token_type_id type_id,
    nlink_transform_fn transform
) {
    // Check if initialized
    if (system_state.status != NLINK_TOKEN_SYSTEM_INITIALIZED) {
        set_last_error("Token system not initialized");
        return false;
    }
    
    // Check if transform function is NULL
    if (transform == NULL) {
        set_last_error("Transform function cannot be NULL");
        return false;
    }
    
    // Check if type exists
    if (nlink_get_token_type_info(type_id) == NULL) {
        set_last_error("Invalid token type ID");
        return false;
    }
    
    // If it's a built-in type, we don't store the transform function
    if (type_id < NLINK_TYPE_CUSTOM_BASE) {
        // For built-in types, we don't currently store transforms
        // This could be enhanced in a future implementation
        return true;
    }
    
    // Find the custom type entry
    for (size_t i = 0; i < custom_type_registry.count; i++) {
        if (custom_type_registry.entries[i].type_id == type_id) {
            custom_type_registry.entries[i].transform = transform;
            return true;
        }
    }
    
    set_last_error("Custom type not found in registry");
    return false;
}

nlink_token_base* nlink_token_system_tokenize(
    const char* source,
    const char* sourcename,
    void* options
) {
    // Check if initialized
    if (system_state.status != NLINK_TOKEN_SYSTEM_INITIALIZED) {
        set_last_error("Token system not initialized");
        return NULL;
    }
    
    // Check source
    if (source == NULL) {
        set_last_error("Source cannot be NULL");
        return NULL;
    }
    
    // Create a tokenizer context
    nlink_tokenizer_context* context = nlink_tokenizer_create(source, options);
    if (context == NULL) {
        set_last_error("Failed to create tokenizer context");
        return NULL;
    }
    
    // Tokenize the source
    nlink_token** tokens = nlink_tokenize_source(source, options);
    if (tokens == NULL) {
        nlink_tokenizer_free(context);
        set_last_error("Tokenization failed");
        return NULL;
    }
    
    // Convert token array to linked list
    nlink_token_base* head = NULL;
    nlink_token_base* tail = NULL;
    
    for (size_t i = 0; tokens[i] != NULL; i++) {
        // Create a token of the appropriate type based on the token's type
        nlink_token_base* token = NULL;
        
        // This is a simplified implementation - in a real system, we would
        // need to create the appropriate token type based on the token's type
        switch (tokens[i]->type) {
            case NLINK_TOKEN_IDENTIFIER:
                token = (nlink_token_base*)nlink_token_create_identifier(
                    tokens[i]->value,
                    tokens[i]->line,
                    tokens[i]->column
                );
                break;
            default:
                // For simplicity, treat other token types as simple base tokens
                token = nlink_token_create(
                    NLINK_TYPE_UNKNOWN,
                    tokens[i]->value,
                    tokens[i]->line,
                    tokens[i]->column
                );
                break;
        }
        
        if (token == NULL) {
            // Free all tokens created so far
            nlink_token_list_free(head);
            for (size_t j = i; tokens[j] != NULL; j++) {
                nlink_token_free(tokens[j]);
            }
            free(tokens);
            nlink_tokenizer_free(context);
            set_last_error("Failed to create token");
            return NULL;
        }
        
        // Add to linked list
        if (head == NULL) {
            head = token;
            tail = token;
        } else {
            tail->next = token;
            tail = token;
        }
        
        // Free the original token
        nlink_token_free(tokens[i]);
    }
    
    // Free the token array
    free(tokens);
    
    // Free the tokenizer context
    nlink_tokenizer_free(context);
    
    return head;
}

nlink_token_program* nlink_token_system_parse(
    nlink_token_base* tokens,
    void* options
) {
    // This is a placeholder for the actual parsing implementation
    // In a real system, this would construct an AST from the token stream
    
    // Check if initialized
    if (system_state.status != NLINK_TOKEN_SYSTEM_INITIALIZED) {
        set_last_error("Token system not initialized");
        return NULL;
    }
    
    // Check tokens
    if (tokens == NULL) {
        set_last_error("Token list cannot be NULL");
        return NULL;
    }
    
    // Create a program token
    nlink_token_program* program = nlink_token_create_program(
        "main",  // Default module name
        tokens->line,
        tokens->column
    );
    
    if (program == NULL) {
        set_last_error("Failed to create program token");
        return NULL;
    }
    
    // In a real implementation, we would parse the token stream and populate the program
    // For now, we just return an empty program
    
    return program;
}

void* nlink_token_system_execute(
    nlink_token_program* program,
    void* context
) {
    // This is a placeholder for the actual execution implementation
    // In a real system, this would interpret or compile and execute the program
    
    // Check if initialized
    if (system_state.status != NLINK_TOKEN_SYSTEM_INITIALIZED) {
        set_last_error("Token system not initialized");
        return NULL;
    }
    
    // Check program
    if (program == NULL) {
        set_last_error("Program cannot be NULL");
        return NULL;
    }
    
    // In a real implementation, we would execute the program and return its result
    // For now, we just return NULL
    
    return NULL;
}