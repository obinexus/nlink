/**
 * @file tokenizer.h
 * @brief Tokenization system for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 * 
 * This module provides token-based operations for lexical analysis,
 * implementing tactical programming patterns for token manipulation.
 */

#ifndef NLINK_TOKENIZER_H
#define NLINK_TOKENIZER_H

#include <stddef.h>
#include <stdbool.h>
#include "../tactic/tactic.h"
#include "../type/type.h"

/**
 * Token type enumeration
 */
typedef enum {
    NLINK_TOKEN_IDENTIFIER,
    NLINK_TOKEN_KEYWORD,
    NLINK_TOKEN_OPERATOR,
    NLINK_TOKEN_LITERAL,
    NLINK_TOKEN_SEPARATOR,
    NLINK_TOKEN_COMMENT,
    NLINK_TOKEN_EOF,
    NLINK_TOKEN_ERROR
} nlink_token_type;

/**
 * Token structure
 */
typedef struct {
    nlink_token_type type;
    char* value;
    size_t line;
    size_t column;
    void* metadata;
} nlink_token;

/**
 * Tokenizer context
 */
typedef struct nlink_tokenizer_context {
    const char* source;
    size_t position;
    size_t line;
    size_t column;
    void* state;
} nlink_tokenizer_context;

/**
 * Tokenizer configuration
 */
typedef struct {
    const char* keywords[64];
    const char* operators[32];
    const char* separators[16];
    nlink_transform_fn custom_token_handler;
    void* custom_context;
} nlink_tokenizer_config;

/**
 * Create a new tokenizer context
 * @param source Source text to tokenize
 * @param config Tokenizer configuration
 * @return Tokenizer context
 */
nlink_tokenizer_context* nlink_tokenizer_create(const char* source, nlink_tokenizer_config* config);

/**
 * Free tokenizer context
 * @param context Tokenizer context to free
 */
void nlink_tokenizer_free(nlink_tokenizer_context* context);

/**
 * Get the next token from the source
 * @param context Tokenizer context
 * @return Next token
 */
nlink_token* nlink_tokenizer_next(nlink_tokenizer_context* context);

/**
 * Peek at the next token without advancing
 * @param context Tokenizer context
 * @return Next token or NULL if at end
 */
nlink_token* nlink_tokenizer_peek(nlink_tokenizer_context* context);

/**
 * Create a token object
 * @param type Token type
 * @param value Token value
 * @param line Source line number
 * @param column Source column number
 * @return New token object
 */
nlink_token* nlink_token_create(nlink_token_type type, const char* value, 
                               size_t line, size_t column);

/**
 * Free a token object
 * @param token Token to free
 */
void nlink_token_free(nlink_token* token);

/**
 * Apply a transformation tactic to a token
 * @param token Token to transform
 * @param transform Transformation function
 * @param context Transformation context
 * @return Transformed token
 */
nlink_token* nlink_token_transform(nlink_token* token, 
                                  nlink_transform_fn transform, 
                                  void* context);

/**
 * Tokenize an entire source string
 * @param source Source text
 * @param config Tokenizer configuration
 * @return Array of tokens, terminated with TOKEN_EOF
 */
nlink_token** nlink_tokenize_source(const char* source, nlink_tokenizer_config* config);

#endif /* NLINK_TOKENIZER_H */
