/**
 * @file nlink_token_system.h
 * @brief Unified token system for NexusLink scripting
 * @copyright Copyright © 2025 OBINexus Computing
 * 
 * This module brings together the token, token type, and type systems
 * into a cohesive framework for NexusLink scripting.
 */

#ifndef NLINK_TOKEN_SYSTEM_H
#define NLINK_TOKEN_SYSTEM_H

#include "token.h"
#include "token_type.h"
#include "../type/type.h"
#include "../tactic/tactic.h"

/**
 * Token system initialization status
 */
typedef enum {
    NLINK_TOKEN_SYSTEM_UNINITIALIZED,
    NLINK_TOKEN_SYSTEM_INITIALIZING,
    NLINK_TOKEN_SYSTEM_INITIALIZED,
    NLINK_TOKEN_SYSTEM_ERROR
} nlink_token_system_status;

/**
 * Token system configuration
 */
typedef struct {
    bool enable_compile_time_checks;     // Enable compile-time type checks
    bool strict_mode;                    // Enable strict type checking
    bool allow_implicit_casting;         // Allow implicit type casting
    size_t max_custom_types;             // Maximum number of custom types
    size_t token_pool_size;              // Size of token memory pool
} nlink_token_system_config;

/**
 * Token system runtime state
 */
typedef struct {
    nlink_token_system_status status;    // Current initialization status
    nlink_token_system_config config;    // Configuration
    void* token_pool;                    // Token memory pool (if used)
    size_t error_count;                  // Number of token errors encountered
    char* last_error;                    // Last error message
} nlink_token_system_state;

/**
 * @brief Initialize the token system
 * 
 * Must be called before using any token system functions.
 * 
 * @param config Configuration for the token system (NULL for defaults)
 * @return true if initialization was successful, false otherwise
 */
bool nlink_token_system_init(const nlink_token_system_config* config);

/**
 * @brief Get the current token system state
 * 
 * @return Current token system state
 */
const nlink_token_system_state* nlink_token_system_get_state(void);

/**
 * @brief Shutdown the token system and free resources
 */
void nlink_token_system_shutdown(void);

/**
 * @brief Create a custom token type
 * 
 * This function allows the creation of custom token types for domain-specific languages.
 * 
 * @param name Type name
 * @param parent_type_id Parent type ID (0 for no parent)
 * @param size Size of token structure
 * @param flags Type flags
 * @return New type ID or 0 on failure
 */
nlink_token_type_id nlink_token_system_create_custom_type(
    const char* name,
    nlink_token_type_id parent_type_id,
    size_t size,
    uint32_t flags
);

/**
 * @brief Get token system version information
 * 
 * @param major Pointer to receive major version
 * @param minor Pointer to receive minor version
 * @param patch Pointer to receive patch version
 */
void nlink_token_system_get_version(int* major, int* minor, int* patch);

/**
 * @brief Register a transformation function for a token type
 * 
 * This allows custom transformations to be applied to tokens of a specific type.
 * 
 * @param type_id Token type ID
 * @param transform Transformation function
 * @return true if registration was successful, false otherwise
 */
bool nlink_token_system_register_transform(
    nlink_token_type_id type_id,
    nlink_transform_fn transform
);

/**
 * @brief Create a token sequence from source code
 * 
 * @param source Source code
 * @param sourcename Source name (e.g., filename)
 * @param options Tokenization options (implementation-specific)
 * @return Head of token sequence or NULL on failure
 */
nlink_token_base* nlink_token_system_tokenize(
    const char* source,
    const char* sourcename,
    void* options
);

/**
 * @brief Create an abstract syntax tree from a token sequence
 * 
 * @param tokens Token sequence
 * @param options Parsing options (implementation-specific)
 * @return Root of AST or NULL on failure
 */
nlink_token_program* nlink_token_system_parse(
    nlink_token_base* tokens,
    void* options
);

/**
 * @brief Execute a parsed program
 * 
 * @param program Program to execute
 * @param context Execution context
 * @return Execution result
 */
void* nlink_token_system_execute(
    nlink_token_program* program,
    void* context
);

/**
 * Compile-time type checking macros
 */
#ifdef NLINK_ENABLE_COMPILE_TIME_TYPE_CHECK
    /**
     * @brief Static type assertion macro
     * 
     * This macro generates a compile-time error if the type is incorrect.
     * 
     * @param value Value to check
     * @param type_id Expected type ID
     */
    #define NLINK_STATIC_ASSERT_TYPE(value, type_id) \
        _Static_assert(NLINK_TOKEN_IS_TYPE(value, type_id), "Type mismatch")
    
    /**
     * @brief Type-safe function call macro
     * 
     * This macro ensures the arguments are of the correct type at compile time.
     * 
     * @param func Function to call
     * @param arg Argument to pass
     * @param type_id Expected type ID of the argument
     */
    #define NLINK_CALL_TYPED(func, arg, type_id) \
        (NLINK_STATIC_ASSERT_TYPE(arg, type_id), func(arg))
#else
    #define NLINK_STATIC_ASSERT_TYPE(value, type_id) ((void)0)
    #define NLINK_CALL_TYPED(func, arg, type_id) func(arg)
#endif

#endif /* NLINK_TOKEN_SYSTEM_H */