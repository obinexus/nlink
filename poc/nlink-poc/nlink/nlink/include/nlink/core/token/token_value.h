/**
 * @file token_value.h
 * @brief Token value operations for NexusLink with error handling integration
 * @copyright Copyright © 2025 OBINexus Computing
 */

#ifndef NLINK_TOKEN_VALUE_H
#define NLINK_TOKEN_VALUE_H

#include "nlink/core/common/types.h"
#include "nlink/core/common/nexus_result.h"
#include "token_type.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Base token structure
 */
struct nlink_token_base {
    nlink_token_type_id type_id;
    size_t line;
    size_t column;
    const char* source;
    struct nlink_token_base* next;
};

/**
 * Identifier token structure
 */
typedef struct {
    nlink_token_base base;
    char* name;
    void* symbol;
} nlink_token_identifier;

/**
 * Literal token structure
 */
typedef struct {
    nlink_token_base base;
    uint32_t value_type;
    union {
        int64_t int_value;
        double float_value;
        char* string_value;
        bool bool_value;
        void* complex_value;
    };
} nlink_token_literal;

/**
 * Operator token structure
 */
typedef struct {
    nlink_token_base base;
    uint32_t op_type;
    uint32_t precedence;
    bool is_unary;
} nlink_token_operator;

/**
 * Keyword token structure
 */
typedef struct {
    nlink_token_base base;
    uint32_t keyword_type;
} nlink_token_keyword;

/**
 * Expression token structure
 */
typedef struct {
    nlink_token_base base;
    nlink_expression_type expr_type;
    union {
        struct {
            void* left;
            void* right;
            void* op;
        } binary;
        struct {
            void* operand;
            void* op;
            bool prefix;
        } unary;
        struct {
            void* callee;
            void** args;
            size_t arg_count;
        } call;
        struct {
            void* expr;
        } group;
        struct {
            void* object;
            void* property;
        } member;
        struct {
            void* object;
            void* index;
        } index;
        struct {
            void** params;
            size_t param_count;
            void* body;
        } lambda;
        void* literal;
        void* identifier;
    };
} nlink_token_expression;

/**
 * Statement token structure
 */
typedef struct {
    nlink_token_base base;
    nlink_statement_type stmt_type;
    union {
        struct {
            void* id;
            void* init;
            uint32_t type_hint;
        } declaration;
        struct {
            void* target;
            void* value;
        } assignment;
        struct {
            void* condition;
            void* consequent;
            void* alternate;
        } conditional;
        struct {
            void* init;
            void* condition;
            void* update;
            void* body;
        } loop;
        struct {
            void* value;
        } return_stmt;
        struct {
            void** statements;
            size_t count;
        } block;
        struct {
            void* expr;
        } expression;
        struct {
            char* module_name;
            void** imports;
            size_t import_count;
        } import;
        struct {
            void** exports;
            size_t export_count;
        } export;
        struct {
            char* pipeline_name;
            void** stages;
            size_t stage_count;
        } pipeline;
    };
} nlink_token_statement;

/**
 * Program token structure
 */
typedef struct {
    nlink_token_base base;
    void** statements;
    size_t statement_count;
    char* module_name;
    void* symbol_table;
} nlink_token_program;

/**
 * Configuration section token
 */
typedef struct {
    nlink_token_base base;
    char* section_name;
    void** properties;
    size_t property_count;
    uint32_t depth;
} nlink_token_config_section;

/**
 * Configuration property token
 */
typedef struct {
    nlink_token_base base;
    char* key;
    void* value;
    bool is_required;
} nlink_token_config_property;

/**
 * Configuration array token
 */
typedef struct {
    nlink_token_base base;
    void** items;
    size_t item_count;
} nlink_token_config_array;

/**
 * Configuration pattern token
 */
typedef struct {
    nlink_token_base base;
    char* pattern;
    bool is_recursive;
} nlink_token_config_pattern;

/**
 * Configuration context for hierarchical processing
 */
typedef struct {
    void* current_section;
    void* root_section;
    void** token_stack;
    size_t stack_depth;
    size_t stack_capacity;
    NexusContext* error_context;
} nlink_config_context;

/**
 * Create a new base token
 * @param type_id Type ID
 * @param source Source text
 * @param line Line number
 * @param column Column number
 * @return New token or NULL on failure
 */
nlink_token_base* nlink_token_create(nlink_token_type_id type_id, 
                                    const char* source,
                                    size_t line, size_t column);

/**
 * Create a new base token with result handling
 * @param type_id Type ID
 * @param source Source text
 * @param line Line number
 * @param column Column number
 * @param out_token Pointer to store the created token
 * @return Result with success or error information
 */
nexus_result nlink_token_create_with_result(nlink_token_type_id type_id, 
                                           const char* source,
                                           size_t line, size_t column,
                                           nlink_token_base** out_token);

/**
 * Free a token and its resources
 * @param token Token to free
 */
void nlink_token_free(nlink_token_base* token);

/**
 * Free a token list
 * @param token Head of the token list
 */
void nlink_token_list_free(nlink_token_base* token);

/**
 * Transform a token
 * @param token Token to transform
 * @param transform Transformation function
 * @param context Transformation context
 * @return Transformed token or NULL on failure
 */
nlink_token_base* nlink_token_transform(nlink_token_base* token,
                                       nlink_transform_fn transform,
                                       void* context);

/**
 * Transform a token with result handling
 * @param token Token to transform
 * @param transform Transformation function
 * @param context Transformation context
 * @param out_token Pointer to store the transformed token
 * @return Result with success or error information
 */
nexus_result nlink_token_transform_with_result(nlink_token_base* token,
                                              nexus_result (*transform)(void*, void*),
                                              void* context,
                                              nlink_token_base** out_token);

/**
 * Create an identifier token
 * @param name Identifier name
 * @param line Line number
 * @param column Column number
 * @return New token or NULL on failure
 */
nlink_token_identifier* nlink_token_create_identifier(const char* name, 
                                                    size_t line, size_t column);

/**
 * Create an identifier token with result handling
 * @param name Identifier name
 * @param line Line number
 * @param column Column number
 * @param out_token Pointer to store the created token
 * @return Result with success or error information
 */
nexus_result nlink_token_create_identifier_with_result(const char* name, 
                                                     size_t line, size_t column,
                                                     nlink_token_identifier** out_token);

/**
 * Create a literal token
 * @param value_type Type of the literal value
 * @param value Literal value
 * @param line Line number
 * @param column Column number
 * @return New token or NULL on failure
 */
nlink_token_literal* nlink_token_create_literal(uint32_t value_type, 
                                              const void* value,
                                              size_t line, size_t column);

/**
 * Create a literal token with result handling
 * @param value_type Type of the literal value
 * @param value Literal value
 * @param line Line number
 * @param column Column number
 * @param out_token Pointer to store the created token
 * @return Result with success or error information
 */
nexus_result nlink_token_create_literal_with_result(uint32_t value_type, 
                                                  const void* value,
                                                  size_t line, size_t column,
                                                  nlink_token_literal** out_token);

/**
 * Create an operator token
 * @param op_type Operator type
 * @param text Operator text
 * @param line Line number
 * @param column Column number
 * @return New token or NULL on failure
 */
nlink_token_operator* nlink_token_create_operator(uint32_t op_type, 
                                                const char* text,
                                                size_t line, size_t column);

/**
 * Create an operator token with result handling
 * @param op_type Operator type
 * @param text Operator text
 * @param line Line number
 * @param column Column number
 * @param out_token Pointer to store the created token
 * @return Result with success or error information
 */
nexus_result nlink_token_create_operator_with_result(uint32_t op_type, 
                                                   const char* text,
                                                   size_t line, size_t column,
                                                   nlink_token_operator** out_token);

/**
 * Create a keyword token
 * @param keyword_type Keyword type
 * @param text Keyword text
 * @param line Line number
 * @param column Column number
 * @return New token or NULL on failure
 */
nlink_token_keyword* nlink_token_create_keyword(uint32_t keyword_type, 
                                              const char* text,
                                              size_t line, size_t column);

/**
 * Create a keyword token with result handling
 * @param keyword_type Keyword type
 * @param text Keyword text
 * @param line Line number
 * @param column Column number
 * @param out_token Pointer to store the created token
 * @return Result with success or error information
 */
nexus_result nlink_token_create_keyword_with_result(uint32_t keyword_type, 
                                                  const char* text,
                                                  size_t line, size_t column,
                                                  nlink_token_keyword** out_token);

/**
 * Create an expression token
 * @param expr_type Expression type
 * @param line Line number
 * @param column Column number
 * @return New token or NULL on failure
 */
nlink_token_expression* nlink_token_create_expression(nlink_expression_type expr_type,
                                                    size_t line, size_t column);

/**
 * Create an expression token with result handling
 * @param expr_type Expression type
 * @param line Line number
 * @param column Column number
 * @param out_token Pointer to store the created token
 * @return Result with success or error information
 */
nexus_result nlink_token_create_expression_with_result(nlink_expression_type expr_type,
                                                     size_t line, size_t column,
                                                     nlink_token_expression** out_token);

/**
 * Create a statement token
 * @param stmt_type Statement type
 * @param line Line number
 * @param column Column number
 * @return New token or NULL on failure
 */
nlink_token_statement* nlink_token_create_statement(nlink_statement_type stmt_type,
                                                  size_t line, size_t column);

/**
 * Create a statement token with result handling
 * @param stmt_type Statement type
 * @param line Line number
 * @param column Column number
 * @param out_token Pointer to store the created token
 * @return Result with success or error information
 */
nexus_result nlink_token_create_statement_with_result(nlink_statement_type stmt_type,
                                                    size_t line, size_t column,
                                                    nlink_token_statement** out_token);

/**
 * Create a program token
 * @param module_name Module name
 * @param line Line number
 * @param column Column number
 * @return New token or NULL on failure
 */
nlink_token_program* nlink_token_create_program(const char* module_name,
                                              size_t line, size_t column);

/**
 * Create a program token with result handling
 * @param module_name Module name
 * @param line Line number
 * @param column Column number
 * @param out_token Pointer to store the created token
 * @return Result with success or error information
 */
nexus_result nlink_token_create_program_with_result(const char* module_name,
                                                  size_t line, size_t column,
                                                  nlink_token_program** out_token);

/**
 * Create a configuration section token
 * @param name Section name
 * @param line Line number
 * @param column Column number
 * @return New token or NULL on failure
 */
nlink_token_config_section* nlink_token_create_config_section(const char* name,
                                                            size_t line, size_t column);

/**
 * Create a configuration section token with result handling
 * @param name Section name
 * @param line Line number
 * @param column Column number
 * @param out_token Pointer to store the created token
 * @return Result with success or error information
 */
nexus_result nlink_token_create_config_section_with_result(const char* name,
                                                         size_t line, size_t column,
                                                         nlink_token_config_section** out_token);

/**
 * Create a configuration property token
 * @param key Property key
 * @param value Property value
 * @param is_required Whether the property is required
 * @param line Line number
 * @param column Column number
 * @return New token or NULL on failure
 */
nlink_token_config_property* nlink_token_create_config_property(const char* key,
                                                              void* value,
                                                              bool is_required,
                                                              size_t line, size_t column);

/**
 * Create a configuration property token with result handling
 * @param key Property key
 * @param value Property value
 * @param is_required Whether the property is required
 * @param line Line number
 * @param column Column number
 * @param out_token Pointer to store the created token
 * @return Result with success or error information
 */
nexus_result nlink_token_create_config_property_with_result(const char* key,
                                                          void* value,
                                                          bool is_required,
                                                          size_t line, size_t column,
                                                          nlink_token_config_property** out_token);

/**
 * Create a configuration context
 * @param error_context Error context
 * @return New context or NULL on failure
 */
nlink_config_context* nlink_config_context_create(NexusContext* error_context);

/**
 * Create a configuration context with result handling
 * @param error_context Error context
 * @param out_context Pointer to store the created context
 * @return Result with success or error information
 */
nexus_result nlink_config_context_create_with_result(NexusContext* error_context,
                                                   nlink_config_context** out_context);

/**
 * Validate a configuration token
 * @param token Token to validate
 * @param context Configuration context
 * @return Result with success or error information
 */
nexus_result nlink_validate_config_token(nlink_token_base* token,
                                        nlink_config_context* context);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_TOKEN_VALUE_H */