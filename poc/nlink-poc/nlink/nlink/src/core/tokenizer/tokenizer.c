/**
 * @file tokenizer.c
 * @brief Implementation of the NexusLink tokenization system
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include "tokenizer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Helper function to check if a character is a separator
static bool is_separator(char c, const nlink_tokenizer_config* config) {
    if (config == NULL || config->separators == NULL) {
        // Default separators
        return c == '(' || c == ')' || c == '{' || c == '}' || 
               c == '[' || c == ']' || c == ';' || c == ',' || c == '.';
    }
    
    for (size_t i = 0; config->separators[i] != NULL; i++) {
        if (config->separators[i][0] == c && config->separators[i][1] == '\0') {
            return true;
        }
    }
    
    return false;
}

// Helper function to check if a string is a keyword
static bool is_keyword(const char* str, const nlink_tokenizer_config* config) {
    if (config == NULL || config->keywords == NULL || str == NULL) {
        return false;
    }
    
    for (size_t i = 0; config->keywords[i] != NULL; i++) {
        if (strcmp(str, config->keywords[i]) == 0) {
            return true;
        }
    }
    
    return false;
}

// Helper function to check if a string starts with an operator
static int is_operator(const char* str, const nlink_tokenizer_config* config) {
    if (config == NULL || config->operators == NULL || str == NULL) {
        // Default operators
        static const char* default_ops[] = {
            "+", "-", "*", "/", "%", "=", "==", "!=", "<", ">", "<=", ">=", 
            "&&", "||", "!", "&", "|", "^", "~", "<<", ">>", "++", "--", NULL
        };
        
        for (size_t i = 0; default_ops[i] != NULL; i++) {
            size_t len = strlen(default_ops[i]);
            if (strncmp(str, default_ops[i], len) == 0) {
                return (int)len;
            }
        }
        
        return 0;
    }
    
    for (size_t i = 0; config->operators[i] != NULL; i++) {
        size_t len = strlen(config->operators[i]);
        if (strncmp(str, config->operators[i], len) == 0) {
            return (int)len;
        }
    }
    
    return 0;
}

nlink_tokenizer_context* nlink_tokenizer_create(const char* source, nlink_tokenizer_config* config) {
    if (source == NULL) {
        return NULL;
    }
    
    nlink_tokenizer_context* context = malloc(sizeof(nlink_tokenizer_context));
    if (context == NULL) {
        return NULL;
    }
    
    context->source = source;
    context->position = 0;
    context->line = 1;
    context->column = 1;
    context->state = NULL;
    
    return context;
}

void nlink_tokenizer_free(nlink_tokenizer_context* context) {
    if (context == NULL) {
        return;
    }
    
    // Free any allocated state
    free(context->state);
    
    // Free the context itself
    free(context);
}

nlink_token* nlink_tokenizer_next(nlink_tokenizer_context* context) {
    // Simplified implementation - real tokenizer would be more complex
    if (context == NULL || context->source == NULL) {
        return NULL;
    }
    
    // Skip whitespace
    while (context->source[context->position] != '\0' && 
           isspace(context->source[context->position])) {
        if (context->source[context->position] == '\n') {
            context->line++;
            context->column = 1;
        } else {
            context->column++;
        }
        context->position++;
    }
    
    // Check for end of input
    if (context->source[context->position] == '\0') {
        return nlink_token_create(NLINK_TOKEN_EOF, "", context->line, context->column);
    }
    
    // Placeholder simple implementation - token extraction logic would go here
    // For now, just create a simple identifier token for demonstration
    size_t start = context->position;
    size_t line = context->line;
    size_t col = context->column;
    
    // Placeholder: just read until whitespace for demonstration
    while (context->source[context->position] != '\0' && 
           !isspace(context->source[context->position])) {
        context->position++;
        context->column++;
    }
    
    // Create a substring for the token value
    size_t len = context->position - start;
    char* value = malloc(len + 1);
    if (value == NULL) {
        return nlink_token_create(NLINK_TOKEN_ERROR, "Memory allocation failed", line, col);
    }
    
    strncpy(value, context->source + start, len);
    value[len] = '\0';
    
    nlink_token* token = nlink_token_create(NLINK_TOKEN_IDENTIFIER, value, line, col);
    free(value);
    
    return token;
}

nlink_token* nlink_tokenizer_peek(nlink_tokenizer_context* context) {
    if (context == NULL) {
        return NULL;
    }
    
    // Save current position
    size_t pos = context->position;
    size_t line = context->line;
    size_t col = context->column;
    
    // Get next token
    nlink_token* token = nlink_tokenizer_next(context);
    
    // Restore position
    context->position = pos;
    context->line = line;
    context->column = col;
    
    return token;
}

nlink_token* nlink_token_create(nlink_token_type type, const char* value, 
                               size_t line, size_t column) {
    nlink_token* token = malloc(sizeof(nlink_token));
    if (token == NULL) {
        return NULL;
    }
    
    token->type = type;
    token->line = line;
    token->column = column;
    token->metadata = NULL;
    
    if (value != NULL) {
        token->value = strdup(value);
        if (token->value == NULL) {
            free(token);
            return NULL;
        }
    } else {
        token->value = NULL;
    }
    
    return token;
}

void nlink_token_free(nlink_token* token) {
    if (token == NULL) {
        return;
    }
    
    free(token->value);
    free(token->metadata);
    free(token);
}

nlink_token* nlink_token_transform(nlink_token* token, 
                                  nlink_transform_fn transform, 
                                  void* context) {
    if (token == NULL || transform == NULL) {
        return token;
    }
    
    return (nlink_token*)transform(token, context);
}

nlink_token** nlink_tokenize_source(const char* source, nlink_tokenizer_config* config) {
    if (source == NULL) {
        return NULL;
    }
    
    // Create tokenizer context
    nlink_tokenizer_context* context = nlink_tokenizer_create(source, config);
    if (context == NULL) {
        return NULL;
    }
    
    // Allocate initial token array
    size_t capacity = 16;
    size_t count = 0;
    nlink_token** tokens = malloc(capacity * sizeof(nlink_token*));
    if (tokens == NULL) {
        nlink_tokenizer_free(context);
        return NULL;
    }
    
    // Tokenize the source
    nlink_token* token;
    do {
        token = nlink_tokenizer_next(context);
        if (token == NULL) {
            break;
        }
        
        // Resize token array if needed
        if (count >= capacity) {
            capacity *= 2;
            nlink_token** new_tokens = realloc(tokens, capacity * sizeof(nlink_token*));
            if (new_tokens == NULL) {
                // Free all tokens and the array
                for (size_t i = 0; i < count; i++) {
                    nlink_token_free(tokens[i]);
                }
                free(tokens);
                nlink_tokenizer_free(context);
                nlink_token_free(token);
                return NULL;
            }
            tokens = new_tokens;
        }
        
        // Add token to array
        tokens[count++] = token;
        
    } while (token->type != NLINK_TOKEN_EOF);
    
    // Clean up tokenizer context
    nlink_tokenizer_free(context);
    
    return tokens;
}
