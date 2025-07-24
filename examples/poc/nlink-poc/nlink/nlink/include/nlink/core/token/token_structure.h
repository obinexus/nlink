/**
 * @file token_structure.h
 * @brief Token structure definitions for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 *
 * This module defines the core token structures that implement the type system,
 * providing concrete representations for statements, expressions, and programs.
 */

#ifndef NLINK_TOKEN_STRUCTURE_H
#define NLINK_TOKEN_STRUCTURE_H

#include "token_type.h"
#include <stddef.h>
#include <stdbool.h>

/**
 * Core token structure with type information
 */
typedef struct nlink_token {
    nlink_token_type_id type_id;  // Type identifier (must be first field for type casting)
    char* value;                  // Token value/text
    size_t line;                  // Source line number
    size_t column;                // Source column number
    struct nlink_token* next;     // Next token in sequence (for token streams)
    void* metadata;               // Additional metadata
} nlink_token;

/**
 * Statement token structure
 */
typedef struct nlink_statement_token {
    nlink_token_type_id type_id;  // Must be NLINK_TYPE_STATEMENT
    nlink_statement_type stmt_type; // Statement subtype
    char* value;                  // Statement text/representation
    size_t line;
    size_t column;
    struct nlink_token* next;
    void* metadata;
    
    // Statement-specific fields
    bool is_executable;           // Whether the statement can be executed
    struct nlink_token** children; // Child tokens (for compound statements)
    size_t child_count;           // Number of children
} nlink_statement_token;

/**
 * Expression token structure
 */
typedef struct nlink_expression_token {
    nlink_token_type_id type_id;  // Must be NLINK_TYPE_EXPRESSION
    nlink_expression_type expr_type; // Expression subtype
    char* value;                  // Expression text/representation
    size_t line;
    size_t column;
    struct nlink_token* next;
    void* metadata;
    
    // Expression-specific fields
    struct nlink_token* left;     // Left operand (for binary expressions)
    struct nlink_token* right;    // Right operand (for binary expressions)
    struct nlink_token* condition; // Condition (for ternary expressions)
    struct nlink_token* operator; // Operator token
} nlink_expression_token;

/**
 * Program token structure
 */
typedef struct nlink_program_token {
    nlink_token_type_id type_id;  // Must be NLINK_TYPE_PROGRAM
    char* value;                  // Program name/identifier
    size_t line;
    size_t column;
    struct nlink_token* next;
    void* metadata;
    
    // Program-specific fields
    struct nlink_statement_token** statements; // Program statements
    size_t statement_count;       // Number of statements
    
    // Execution context
    void* context;                // Execution context
    bool (*execute)(struct nlink_program_token* program); // Execution function
} nlink_program_token;

/**
 * Function token structure
 */
typedef struct nlink_function_token {
    nlink_token_type_id type_id;  // Must be NLINK_TYPE_FUNCTION
    char* value;                  // Function name
    size_t line;
    size_t column;
    struct nlink_token* next;
    void* metadata;
    
    // Function-specific fields
    struct nlink_token** parameters; // Function parameters
    size_t parameter_count;       // Number of parameters
    struct nlink_statement_token* body; // Function body
    
    // Function signature
    char* return_type;            // Return type name
    void* (*invoke)(void** args, size_t arg_count); // Function invoker
} nlink_function_token;

/**
 * Token creation functions
 */

/**
 * Create a basic token
 * 
 * @param type_id Token type identifier
 * @param value Token value/text
 * @param line Source line number
 * @param column Source column number
 * @return New token or NULL on failure
 */
nlink_token* nlink_create_token(nlink_token_type_id type_id, const char* value, 
                              size_t line, size_t column);

/**
 * Create a statement token
 * 
 * @param stmt_type Statement type
 * @param value Statement text/value
 * @param line Source line number
 * @param column Source column number
 * @return New statement token or NULL on failure
 */
nlink_statement_token* nlink_create_statement(nlink_statement_type stmt_type, const char* value,
                                           size_t line, size_t column);

/**
 * Create an expression token
 * 
 * @param expr_type Expression type
 * @param value Expression text/value
 * @param line Source line number
 * @param column Source column number
 * @return New expression token or NULL on failure
 */
nlink_expression_token* nlink_create_expression(nlink_expression_type expr_type, const char* value,
                                             size_t line, size_t column);

/**
 * Create a program token
 * 
 * @param name Program name
 * @param line Source line number
 * @param column Source column number
 * @return New program token or NULL on failure
 */
nlink_program_token* nlink_create_program(const char* name, size_t line, size_t column);

/**
 * Create a function token
 * 
 * @param name Function name
 * @param return_type Return type name
 * @param line Source line number
 * @param column Source column number
 * @return New function token or NULL on failure
 */
nlink_function_token* nlink_create_function(const char* name, const char* return_type,
                                        size_t line, size_t column);

/**
 * Token manipulation functions
 */

/**
 * Free a token and its resources
 * 
 * @param token Token to free
 */
void nlink_free_token(nlink_token* token);

/**
 * Add a statement to a program
 * 
 * @param program Program token
 * @param statement Statement to add
 * @return true if successful
 */
bool nlink_program_add_statement(nlink_program_token* program, nlink_statement_token* statement);

/**
 * Add a child token to a statement
 * 
 * @param statement Statement token
 * @param child Child token to add
 * @return true if successful
 */
bool nlink_statement_add_child(nlink_statement_token* statement, nlink_token* child);

/**
 * Set expression operands
 * 
 * @param expression Expression token
 * @param left Left operand (or NULL)
 * @param operator Operator token (or NULL)
 * @param right Right operand (or NULL)
 * @return true if successful
 */
bool nlink_expression_set_operands(nlink_expression_token* expression, 
                                 nlink_token* left, 
                                 nlink_token* operator,
                                 nlink_token* right);

/**
 * Add a parameter to a function
 * 
 * @param function Function token
 * @param param Parameter token
 * @return true if successful
 */
bool nlink_function_add_parameter(nlink_function_token* function, nlink_token* param);

/**
 * Set function body
 * 
 * @param function Function token
 * @param body Statement token for the function body
 * @return true if successful
 */
bool nlink_function_set_body(nlink_function_token* function, nlink_statement_token* body);

/**
 * Type checking and casting operations
 */

/**
 * Check if a token is a statement
 * 
 * @param token Token to check
 * @return true if token is a statement
 */
bool nlink_is_statement(const nlink_token* token);

/**
 * Check if a token is an expression
 * 
 * @param token Token to check
 * @return true if token is an expression
 */
bool nlink_is_expression(const nlink_token* token);

/**
 * Check if a token is a program
 * 
 * @param token Token to check
 * @return true if token is a program
 */
bool nlink_is_program(const nlink_token* token);

/**
 * Check if a token is a function
 * 
 * @param token Token to check
 * @return true if token is a function
 */
bool nlink_is_function(const nlink_token* token);

/**
 * Cast a token to a statement
 * 
 * @param token Token to cast
 * @return Statement token or NULL if cast fails
 */
nlink_statement_token* nlink_token_as_statement(nlink_token* token);

/**
 * Cast a token to an expression
 * 
 * @param token Token to cast
 * @return Expression token or NULL if cast fails
 */
nlink_expression_token* nlink_token_as_expression(nlink_token* token);

/**
 * Cast a token to a program
 * 
 * @param token Token to cast
 * @return Program token or NULL if cast fails
 */
nlink_program_token* nlink_token_as_program(nlink_token* token);

/**
 * Cast a token to a function
 * 
 * @param token Token to cast
 * @return Function token or NULL if cast fails
 */
nlink_function_token* nlink_token_as_function(nlink_token* token);

#endif /* NLINK_TOKEN_STRUCTURE_H */d