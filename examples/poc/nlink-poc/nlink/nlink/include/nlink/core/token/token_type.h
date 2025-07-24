/**
 * @file token_type.h
 * @brief Type system for NexusLink tokens with error handling integration
 * @copyright Copyright © 2025 OBINexus Computing
 */

#ifndef NLINK_TOKEN_TYPE_H
#define NLINK_TOKEN_TYPE_H

#include "nlink/core/common/types.h"
#include "nlink/core/common/nexus_result.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Token type identifiers
 */
typedef uint32_t nlink_token_type_id;

/**
 * Base token type IDs
 */
#define NLINK_TYPE_UNKNOWN     0
#define NLINK_TYPE_IDENTIFIER  1
#define NLINK_TYPE_KEYWORD     2
#define NLINK_TYPE_OPERATOR    3
#define NLINK_TYPE_LITERAL     4
#define NLINK_TYPE_STATEMENT   5
#define NLINK_TYPE_EXPRESSION  6
#define NLINK_TYPE_PROGRAM     7
#define NLINK_TYPE_FUNCTION    8
#define NLINK_TYPE_CUSTOM_BASE 1000

/* Configuration token types */
#define NLINK_TOKEN_CONFIG_SECTION  2001
#define NLINK_TOKEN_CONFIG_PROPERTY 2002
#define NLINK_TOKEN_CONFIG_ARRAY    2003
#define NLINK_TOKEN_CONFIG_PATTERN  2004

/**
 * Statement type enumeration
 */
typedef enum {
    NLINK_STMT_UNKNOWN,
    NLINK_STMT_DECLARATION,
    NLINK_STMT_ASSIGNMENT,
    NLINK_STMT_CONDITIONAL,
    NLINK_STMT_LOOP,
    NLINK_STMT_RETURN,
    NLINK_STMT_BLOCK,
    NLINK_STMT_EXPRESSION,
    NLINK_STMT_IMPORT,
    NLINK_STMT_EXPORT,
    NLINK_STMT_PIPELINE
} nlink_statement_type;

/**
 * Expression type enumeration
 */
typedef enum {
    NLINK_EXPR_UNKNOWN,
    NLINK_EXPR_BINARY,
    NLINK_EXPR_UNARY,
    NLINK_EXPR_CALL,
    NLINK_EXPR_GROUP,
    NLINK_EXPR_MEMBER,
    NLINK_EXPR_INDEX,
    NLINK_EXPR_LAMBDA,
    NLINK_EXPR_LITERAL,
    NLINK_EXPR_IDENTIFIER
} nlink_expression_type;

/**
 * Token type information structure
 */
typedef struct {
    nlink_token_type_id type_id;
    const char* name;
    size_t size;
    uint32_t flags;
    uint32_t subtype;
    union {
        nlink_statement_type stmt_type;
        nlink_expression_type expr_type;
    };
} nlink_token_type_info;

/**
 * Forward declaration for token base structure
 */
typedef struct nlink_token_base nlink_token_base;

/**
 * Transformation function type for token processing
 */
typedef void* (*nlink_transform_fn)(void* data, void* context);

/**
 * Check if a value is of the specified token type
 * @param value Value to check
 * @param type_id Expected type ID
 * @return true if the value is of the specified type
 */
bool nlink_is_token_type(const void* value, nlink_token_type_id type_id);

/**
 * Assert that a value is of the specified token type
 * @param value Value to check
 * @param type_id Expected type ID
 * @return true if the assertion passed, false otherwise
 */
bool nlink_assert_token_type(const void* value, nlink_token_type_id type_id);

/**
 * Get type information for a token type
 * @param type_id Type ID
 * @return Type information or NULL if type is unknown
 */
const nlink_token_type_info* nlink_get_token_type_info(nlink_token_type_id type_id);

/**
 * Register a new token type
 * @param type_id Type ID
 * @param name Type name
 * @param size Size of the token structure
 * @param flags Type flags
 * @param subtype Subtype identifier
 * @return true if registration succeeded
 */
bool nlink_register_token_type(nlink_token_type_id type_id, const char* name, 
                              size_t size, uint32_t flags, uint32_t subtype);

/**
 * Register a new token type with result handling
 * @param type_id Type ID
 * @param name Type name
 * @param size Size of the token structure
 * @param flags Type flags
 * @param subtype Subtype identifier
 * @return Result with success or error information
 */
nexus_result nlink_register_token_type_with_result(nlink_token_type_id type_id, 
                                                  const char* name, 
                                                  size_t size, uint32_t flags, 
                                                  uint32_t subtype);

/**
 * Check if a statement type can be cast to another
 * @param from Source statement type
 * @param to Target statement type
 * @return true if the cast is allowed
 */
bool nlink_can_cast_statement(nlink_statement_type from, nlink_statement_type to);

/**
 * Check if an expression type can be cast to another
 * @param from Source expression type
 * @param to Target expression type
 * @return true if the cast is allowed
 */
bool nlink_can_cast_expression(nlink_expression_type from, nlink_expression_type to);

/**
 * Cast a token to a different type
 * @param value Token to cast
 * @param type_id Target type ID
 * @return Cast token or NULL if cast is not allowed
 */
void* nlink_token_type_cast(void* value, nlink_token_type_id type_id);

/**
 * Cast a token to a different type with result handling
 * @param value Token to cast
 * @param type_id Target type ID
 * @param out_token Pointer to store the cast token
 * @return Result with success or error information
 */
nexus_result nlink_token_type_cast_with_result(void* value, 
                                             nlink_token_type_id type_id,
                                             void** out_token);

/**
 * Initialize the token type system
 */
void nlink_token_type_system_init(void);

/**
 * Initialize the token type system with result handling
 * @return Result with success or error information
 */
nexus_result nlink_token_type_system_init_with_result(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_TOKEN_TYPE_H */