/**
 * @file token_type.c
 * @brief Implementation of the token type system for NexusLink with error handling
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include "token_type.h"
#include "nlink/core/common/nexus_error.h"
#include "nlink/core/common/nexus_result.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * Maximum number of registered token types
 */
#define NLINK_MAX_TOKEN_TYPES 64

/**
 * Type registry for token types
 */
static struct {
    nlink_token_type_info types[NLINK_MAX_TOKEN_TYPES];
    size_t count;
    bool initialized;
} token_type_registry = {0};

/**
 * Token type flags
 */
#define NLINK_TYPE_FLAG_CASTABLE     0x00000001  // Type can be cast to other types
#define NLINK_TYPE_FLAG_ATOMIC       0x00000002  // Type is atomic (cannot be broken down)
#define NLINK_TYPE_FLAG_COMPOSITE    0x00000004  // Type is composite (contains other types)
#define NLINK_TYPE_FLAG_EXECUTABLE   0x00000008  // Type can be executed
#define NLINK_TYPE_FLAG_STATEMENT    0x00000010  // Type is a statement
#define NLINK_TYPE_FLAG_EXPRESSION   0x00000020  // Type is an expression

#include "nlink/core/token/token_type.h"
#include "nlink/core/common/nexus_error.h"
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

/**
 * Initialize the token type system
 * This function registers the core token types and initializes the registry.
 */
void nlink_token_type_system_init(void) {
    if (token_type_registry.initialized) {
        return;  // Already initialized
    }

    // Register statement types
    nlink_register_token_type(
        NLINK_TYPE_STATEMENT,
        "statement",
        sizeof(void*),  // Size will be implementation-dependent
        NLINK_TYPE_FLAG_CASTABLE | NLINK_TYPE_FLAG_STATEMENT,
        NLINK_STMT_UNKNOWN
    );

    // Register expression types
    nlink_register_token_type(
        NLINK_TYPE_EXPRESSION,
        "expression",
        sizeof(void*),  // Size will be implementation-dependent
        NLINK_TYPE_FLAG_CASTABLE | NLINK_TYPE_FLAG_EXPRESSION,
        NLINK_EXPR_UNKNOWN
    );

    // Register program type
    nlink_register_token_type(
        NLINK_TYPE_PROGRAM,
        "program",
        sizeof(void*),  // Size will be implementation-dependent
        NLINK_TYPE_FLAG_EXECUTABLE | NLINK_TYPE_FLAG_COMPOSITE,
        0  // No subtype for program
    );

    // Register other core token types
    nlink_register_token_type(
        NLINK_TYPE_IDENTIFIER,
        "identifier",
        sizeof(void*),
        NLINK_TYPE_FLAG_ATOMIC,
        0
    );

    nlink_register_token_type(
        NLINK_TYPE_KEYWORD,
        "keyword",
        sizeof(void*),
        NLINK_TYPE_FLAG_ATOMIC,
        0
    );

    nlink_register_token_type(
        NLINK_TYPE_OPERATOR,
        "operator",
        sizeof(void*),
        NLINK_TYPE_FLAG_ATOMIC,
        0
    );

    nlink_register_token_type(
        NLINK_TYPE_LITERAL,
        "literal",
        sizeof(void*),
        NLINK_TYPE_FLAG_ATOMIC | NLINK_TYPE_FLAG_CASTABLE,
        0
    );

    nlink_register_token_type(
        NLINK_TYPE_FUNCTION,
        "function",
        sizeof(void*),
        NLINK_TYPE_FLAG_EXECUTABLE | NLINK_TYPE_FLAG_COMPOSITE,
        0
    );

    // Register configuration token types
    nlink_register_token_type(
        NLINK_TOKEN_CONFIG_SECTION,
        "config_section",
        sizeof(nlink_token_config_section),
        NLINK_TYPE_FLAG_COMPOSITE,
        0
    );

    nlink_register_token_type(
        NLINK_TOKEN_CONFIG_PROPERTY,
        "config_property",
        sizeof(nlink_token_config_property),
        NLINK_TYPE_FLAG_COMPOSITE,
        0
    );

    nlink_register_token_type(
        NLINK_TOKEN_CONFIG_ARRAY,
        "config_array",
        sizeof(nlink_token_config_array),
        NLINK_TYPE_FLAG_COMPOSITE,
        0
    );

    nlink_register_token_type(
        NLINK_TOKEN_CONFIG_PATTERN,
        "config_pattern",
        sizeof(nlink_token_config_pattern),
        NLINK_TYPE_FLAG_ATOMIC,
        0
    );

    // Mark the registry as initialized
    token_type_registry.initialized = true;
}

/**
 * Internal function to find a type in the registry
 */
static const nlink_token_type_info* find_type_by_id(nlink_token_type_id type_id) {
    if (!token_type_registry.initialized) {
        return NULL;
    }
    
    for (size_t i = 0; i < token_type_registry.count; i++) {
        if (token_type_registry.types[i].type_id == type_id) {
            return &token_type_registry.types[i];
        }
    }
    
    return NULL;
}

bool nlink_is_token_type(const void* value, nlink_token_type_id type_id) {
    if (value == NULL) {
        return false;
    }
    
    // In a real implementation, this would check runtime type information
    // For now, we'll assume the value has a type_id field at the beginning of its structure
    const nlink_token_type_id* value_type_id = (const nlink_token_type_id*)value;
    
    return *value_type_id == type_id;
}

bool nlink_assert_token_type(const void* value, nlink_token_type_id type_id) {
    if (!nlink_is_token_type(value, type_id)) {
        // In a debug build, this would trigger an assertion failure
        fprintf(stderr, "Type assertion failed: expected type %d, got a different type\n", type_id);
        return false;
    }
    
    return true;
}

void* nlink_token_type_cast(void* value, nlink_token_type_id type_id) {
    if (value == NULL) {
        return NULL;
    }
    
    // Get the current type of the value
    nlink_token_type_id* value_type_id = (nlink_token_type_id*)value;
    
    // If it's already the target type, return it unchanged
    if (*value_type_id == type_id) {
        return value;
    }
    
    // Check if the source type can be cast to the target type
    const nlink_token_type_info* source_info = find_type_by_id(*value_type_id);
    const nlink_token_type_info* target_info = find_type_by_id(type_id);
    
    if (source_info == NULL || target_info == NULL) {
        return NULL;  // Unknown type
    }
    
    // Check if source type is castable
    if (!(source_info->flags & NLINK_TYPE_FLAG_CASTABLE)) {
        return NULL;  // Source type cannot be cast
    }
    
    // Check specific casting rules
    if ((source_info->flags & NLINK_TYPE_FLAG_STATEMENT) && 
        (target_info->flags & NLINK_TYPE_FLAG_STATEMENT)) {
        // Statement to statement cast
        if (nlink_can_cast_statement(source_info->stmt_type, target_info->stmt_type)) {
            // Perform the cast (in a real implementation, this might involve a transformation)
            *value_type_id = type_id;
            return value;
        }
    }
    else if ((source_info->flags & NLINK_TYPE_FLAG_EXPRESSION) && 
             (target_info->flags & NLINK_TYPE_FLAG_EXPRESSION)) {
        // Expression to expression cast
        if (nlink_can_cast_expression(source_info->expr_type, target_info->expr_type)) {
            // Perform the cast
            *value_type_id = type_id;
            return value;
        }
    }
    else if ((source_info->flags & NLINK_TYPE_FLAG_EXPRESSION) && 
             (target_info->flags & NLINK_TYPE_FLAG_STATEMENT)) {
        // Expression to statement cast (all expressions can be statements)
        *value_type_id = type_id;
        return value;
    }
    
    // Cast not allowed
    return NULL;
}

nexus_result nlink_token_type_cast_with_result(void* value, 
                                             nlink_token_type_id type_id,
                                             void** out_token) {
    // Validate input parameters
    if (value == NULL) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_INVALID_ARGUMENT,
            "Value to cast cannot be NULL",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    if (out_token == NULL) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_INVALID_ARGUMENT,
            "Output token pointer cannot be NULL",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    // Get the current type of the value
    nlink_token_type_id* value_type_id = (nlink_token_type_id*)value;
    
    // If it's already the target type, return it unchanged
    if (*value_type_id == type_id) {
        *out_token = value;
        return nexus_success(value, NULL);
    }
    
    // Check if the source type can be cast to the target type
    const nlink_token_type_info* source_info = find_type_by_id(*value_type_id);
    const nlink_token_type_info* target_info = find_type_by_id(type_id);
    
    if (source_info == NULL) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_INVALID_ARGUMENT,
            "Source type is unknown",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    if (target_info == NULL) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_INVALID_ARGUMENT,
            "Target type is unknown",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    // Check if source type is castable
    if (!(source_info->flags & NLINK_TYPE_FLAG_CASTABLE)) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_INVALID_OPERATION,
            "Source type is not castable",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    // Check specific casting rules
    bool cast_allowed = false;
    
    if ((source_info->flags & NLINK_TYPE_FLAG_STATEMENT) && 
        (target_info->flags & NLINK_TYPE_FLAG_STATEMENT)) {
        // Statement to statement cast
        cast_allowed = nlink_can_cast_statement(source_info->stmt_type, target_info->stmt_type);
    }
    else if ((source_info->flags & NLINK_TYPE_FLAG_EXPRESSION) && 
             (target_info->flags & NLINK_TYPE_FLAG_EXPRESSION)) {
        // Expression to expression cast
        cast_allowed = nlink_can_cast_expression(source_info->expr_type, target_info->expr_type);
    }
    else if ((source_info->flags & NLINK_TYPE_FLAG_EXPRESSION) && 
             (target_info->flags & NLINK_TYPE_FLAG_STATEMENT)) {
        // Expression to statement cast (all expressions can be statements)
        cast_allowed = true;
    }
    
    if (!cast_allowed) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_INVALID_OPERATION,
            "Cast not allowed between these types",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    // Perform the cast
    *value_type_id = type_id;
    *out_token = value;
    
    return nexus_success(value, NULL);
}

const nlink_token_type_info* nlink_get_token_type_info(nlink_token_type_id type_id) {
    return find_type_by_id(type_id);
}

bool nlink_register_token_type(nlink_token_type_id type_id, const char* name, 
                              size_t size, uint32_t flags, uint32_t subtype) {
    if (!token_type_registry.initialized) {
        memset(&token_type_registry, 0, sizeof(token_type_registry));
        token_type_registry.initialized = true;
    }
    
    // Check if we've reached the maximum number of types
    if (token_type_registry.count >= NLINK_MAX_TOKEN_TYPES) {
        return false;
    }
    
    // Check if the type already exists
    if (find_type_by_id(type_id) != NULL) {
        return false;  // Type already registered
    }
    
    // Register the new type
    nlink_token_type_info* info = &token_type_registry.types[token_type_registry.count++];
    info->type_id = type_id;
    info->name = name ? strdup(name) : NULL;
    info->size = size;
    info->flags = flags;
    info->subtype = subtype;
    
    return true;
}

nexus_result nlink_register_token_type_with_result(nlink_token_type_id type_id, 
                                                  const char* name, 
                                                  size_t size, uint32_t flags, 
                                                  uint32_t subtype) {
    if (!token_type_registry.initialized) {
        memset(&token_type_registry, 0, sizeof(token_type_registry));
        token_type_registry.initialized = true;
    }
    
    // Check if we've reached the maximum number of types
    if (token_type_registry.count >= NLINK_MAX_TOKEN_TYPES) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_OUT_OF_MEMORY,
            "Maximum number of token types reached",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    // Check if the type already exists
    if (find_type_by_id(type_id) != NULL) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_ALREADY_EXISTS,
            "Token type already registered",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    // Register the new type
    nlink_token_type_info* info = &token_type_registry.types[token_type_registry.count++];
    info->type_id = type_id;
    info->name = name ? strdup(name) : NULL;
    
    if (name != NULL && info->name == NULL) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_OUT_OF_MEMORY,
            "Failed to allocate memory for type name",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }
    
    info->size = size;
    info->flags = flags;
    info->subtype = subtype;
    
    return nexus_success((void*)(uintptr_t)type_id, NULL);
}

bool nlink_can_cast_statement(nlink_statement_type from, nlink_statement_type to) {
    // Define valid statement casts
    if (from == to) {
        return true;  // Same type, always castable
    }
    
    // Special casting rules
    switch (from) {
        case NLINK_STMT_DECLARATION:
            // Declarations can be cast to assignments
            return to == NLINK_STMT_ASSIGNMENT;
            
        case NLINK_STMT_CONDITIONAL:
            // Conditionals can only be cast to themselves
            return false;
            
        case NLINK_STMT_BLOCK:
            // Blocks can be cast to any statement type (containing a single statement)
            return to != NLINK_STMT_UNKNOWN;
            
        default:
            // Default is no casting allowed
            return false;
    }
}

bool nlink_can_cast_expression(nlink_expression_type from, nlink_expression_type to) {
    // Define valid expression casts
    if (from == to) {
        return true;  // Same type, always castable
    }
    
    // Special casting rules
    switch (from) {
        case NLINK_EXPR_GROUP:
            // Grouped expressions can be cast to their inner expression type
            return to != NLINK_EXPR_UNKNOWN;
            
        case NLINK_EXPR_LITERAL:
            // Literals can be cast to identifiers (as named constants)
            return to == NLINK_EXPR_IDENTIFIER;
            
        case NLINK_EXPR_BINARY:
            // Binary expressions cannot be cast to other types
            return false;
            
        default:
            // Default is no casting allowed
            return false;
    }
}

nexus_result nlink_token_type_system_init_with_result(void) {
    if (token_type_registry.initialized) {
        return nexus_success(NULL, NULL);  // Already initialized
    }
    
    // Initialize the registry
    memset(&token_type_registry, 0, sizeof(token_type_registry));
    
    // Register statement types
    nexus_result result = nlink_register_token_type_with_result(
        NLINK_TYPE_STATEMENT, 
        "statement", 
        sizeof(void*),  // Size will be implementation-dependent
        NLINK_TYPE_FLAG_CASTABLE | NLINK_TYPE_FLAG_STATEMENT,
        NLINK_STMT_UNKNOWN
    );
    
    if (result.status != NEXUS_STATUS_SUCCESS) {
        return result;
    }
    
    // Register expression types
    result = nlink_register_token_type_with_result(
        NLINK_TYPE_EXPRESSION, 
        "expression", 
        sizeof(void*),  // Size will be implementation-dependent
        NLINK_TYPE_FLAG_CASTABLE | NLINK_TYPE_FLAG_EXPRESSION,
        NLINK_EXPR_UNKNOWN
    );
    
    if (result.status != NEXUS_STATUS_SUCCESS) {
        return result;
    }
    
    // Register program type
    result = nlink_register_token_type_with_result(
        NLINK_TYPE_PROGRAM, 
        "program", 
        sizeof(void*),  // Size will be implementation-dependent
        NLINK_TYPE_FLAG_EXECUTABLE | NLINK_TYPE_FLAG_COMPOSITE,
        0  // No subtype for program
    );
    
    if (result.status != NEXUS_STATUS_SUCCESS) {
        return result;
    }
    
    // Register other core token types
    result = nlink_register_token_type_with_result(
        NLINK_TYPE_IDENTIFIER, 
        "identifier", 
        sizeof(void*),
        NLINK_TYPE_FLAG_ATOMIC,
        0
    );
    
    if (result.status != NEXUS_STATUS_SUCCESS) {
        return result;
    }
    
    result = nlink_register_token_type_with_result(
        NLINK_TYPE_KEYWORD, 
        "keyword", 
        sizeof(void*),
        NLINK_TYPE_FLAG_ATOMIC,
        0
    );
    
    if (result.status != NEXUS_STATUS_SUCCESS) {
        return result;
    }
    
    result = nlink_register_token_type_with_result(
        NLINK_TYPE_OPERATOR, 
        "operator", 
        sizeof(void*),
        NLINK_TYPE_FLAG_ATOMIC,
        0
    );
    
    if (result.status != NEXUS_STATUS_SUCCESS) {
        return result;
    }
    
    result = nlink_register_token_type_with_result(
        NLINK_TYPE_LITERAL, 
        "literal", 
        sizeof(void*),
        NLINK_TYPE_FLAG_ATOMIC | NLINK_TYPE_FLAG_CASTABLE,
        0
    );
    
    if (result.status != NEXUS_STATUS_SUCCESS) {
        return result;
    }
    
    result = nlink_register_token_type_with_result(
        NLINK_TYPE_FUNCTION, 
        "function", 
        sizeof(void*),
        NLINK_TYPE_FLAG_EXECUTABLE | NLINK_TYPE_FLAG_COMPOSITE,
        0
    );
    
    if (result.status != NEXUS_STATUS_SUCCESS) {
        return result;
    }
    
    // Mark the registry as initialized
    token_type_registry.initialized = true;
    
    return nexus_success(NULL, NULL);
}