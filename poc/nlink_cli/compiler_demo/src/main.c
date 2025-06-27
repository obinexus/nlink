/**
 * @file main.c
 * @brief NexusLink Compiler Demo - Symbol Table Manager
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * 
 * Demonstration of real-world compiler symbol table management
 * integrated with NexusLink configuration system.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab/symbol_table.h"

// Simple lexer for parsing variable declarations
typedef struct {
    const char *input;
    size_t position;
    size_t length;
    uint32_t line;
    uint32_t column;
} lexer_t;

typedef enum {
    TOKEN_INT, TOKEN_FLOAT, TOKEN_CHAR,
    TOKEN_IDENTIFIER, TOKEN_SEMICOLON,
    TOKEN_ASSIGN, TOKEN_NUMBER, TOKEN_EOF
} token_type_t;

typedef struct {
    token_type_t type;
    char *value;
    uint32_t line;
    uint32_t column;
} token_t;

// =============================================================================
// SIMPLE LEXER IMPLEMENTATION
// =============================================================================

static bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

static bool is_alnum(char c) {
    return is_alpha(c) || is_digit(c);
}

static void skip_whitespace(lexer_t *lexer) {
    while (lexer->position < lexer->length) {
        char c = lexer->input[lexer->position];
        if (c == ' ' || c == '\t') {
            lexer->position++;
            lexer->column++;
        } else if (c == '\n') {
            lexer->position++;
            lexer->line++;
            lexer->column = 1;
        } else {
            break;
        }
    }
}

static token_t next_token(lexer_t *lexer) {
    token_t token = {TOKEN_EOF, NULL, lexer->line, lexer->column};
    
    skip_whitespace(lexer);
    
    if (lexer->position >= lexer->length) {
        return token;
    }
    
    char c = lexer->input[lexer->position];
    
    if (is_alpha(c)) {
        // Identifier or keyword
        size_t start = lexer->position;
        while (lexer->position < lexer->length && is_alnum(lexer->input[lexer->position])) {
            lexer->position++;
            lexer->column++;
        }
        
        size_t len = lexer->position - start;
        token.value = malloc(len + 1);
        strncpy(token.value, &lexer->input[start], len);
        token.value[len] = '\0';
        
        // Check for keywords
        if (strcmp(token.value, "int") == 0) token.type = TOKEN_INT;
        else if (strcmp(token.value, "float") == 0) token.type = TOKEN_FLOAT;
        else if (strcmp(token.value, "char") == 0) token.type = TOKEN_CHAR;
        else token.type = TOKEN_IDENTIFIER;
        
    } else if (is_digit(c)) {
        // Number literal
        size_t start = lexer->position;
        while (lexer->position < lexer->length && is_digit(lexer->input[lexer->position])) {
            lexer->position++;
            lexer->column++;
        }
        
        size_t len = lexer->position - start;
        token.value = malloc(len + 1);
        strncpy(token.value, &lexer->input[start], len);
        token.value[len] = '\0';
        token.type = TOKEN_NUMBER;
        
    } else if (c == ';') {
        token.type = TOKEN_SEMICOLON;
        lexer->position++;
        lexer->column++;
    } else if (c == '=') {
        token.type = TOKEN_ASSIGN;
        lexer->position++;
        lexer->column++;
    } else {
        // Skip unknown character
        lexer->position++;
        lexer->column++;
        return next_token(lexer);
    }
    
    return token;
}

// =============================================================================
// DEMONSTRATION FUNCTIONS
// =============================================================================

static void demonstrate_symbol_table() {
    printf("=== NexusLink Compiler Demo: Symbol Table Manager ===\n\n");
    
    // Create symbol table
    symbol_table_t *global_scope = symtab_create(64);
    if (!global_scope) {
        printf("Failed to create symbol table\n");
        return;
    }
    
    printf("1. Adding symbols to global scope:\n");
    
    // Add some test symbols
    symtab_add_symbol(global_scope, "main_counter", SYM_TYPE_INT, SYM_SCOPE_GLOBAL, 1, 1);
    symtab_add_symbol(global_scope, "pi_value", SYM_TYPE_FLOAT, SYM_SCOPE_GLOBAL, 2, 1);
    symtab_add_symbol(global_scope, "user_name", SYM_TYPE_STRING, SYM_SCOPE_GLOBAL, 3, 1);
    
    symtab_display(global_scope);
    
    printf("2. Testing symbol lookup:\n");
    symbol_entry_t *found = symtab_lookup(global_scope, "main_counter");
    if (found) {
        printf("   Found: %s (type: %d, line: %d)\n", 
               found->name, found->type, found->line_number);
    }
    
    printf("3. Testing type compatibility:\n");
    bool compatible = symtab_check_type_compatibility(SYM_TYPE_INT, SYM_TYPE_FLOAT);
    printf("   int -> float: %s\n", compatible ? "Compatible" : "Incompatible");
    
    compatible = symtab_check_type_compatibility(SYM_TYPE_STRING, SYM_TYPE_INT);
    printf("   string -> int: %s\n", compatible ? "Compatible" : "Incompatible");
    
    printf("4. Testing duplicate detection:\n");
    symtab_result_t result = symtab_add_symbol(global_scope, "main_counter", SYM_TYPE_FLOAT, SYM_SCOPE_GLOBAL, 5, 1);
    printf("   Adding duplicate 'main_counter': %s\n", 
           result == SYMTAB_ERROR_DUPLICATE ? "Detected duplicate (correct)" : "Unexpected result");
    
    symtab_destroy(global_scope);
    printf("\n=== Demo completed successfully ===\n");
}

static void parse_simple_declarations(const char *source_code) {
    printf("\n=== Parsing Variable Declarations ===\n");
    printf("Source: %s\n\n", source_code);
    
    lexer_t lexer = {source_code, 0, strlen(source_code), 1, 1};
    symbol_table_t *table = symtab_create(32);
    
    token_t token = next_token(&lexer);
    
    while (token.type != TOKEN_EOF) {
        if (token.type == TOKEN_INT || token.type == TOKEN_FLOAT || token.type == TOKEN_CHAR) {
            symbol_type_t type = (token.type == TOKEN_INT) ? SYM_TYPE_INT :
                               (token.type == TOKEN_FLOAT) ? SYM_TYPE_FLOAT : SYM_TYPE_CHAR;
            
            free(token.value);
            token = next_token(&lexer);
            
            if (token.type == TOKEN_IDENTIFIER) {
                symtab_result_t result = symtab_add_symbol(table, token.value, type, 
                                                         SYM_SCOPE_LOCAL, token.line, token.column);
                
                if (result == SYMTAB_SUCCESS) {
                    printf("Parsed declaration: %s (type: %d)\n", token.value, type);
                } else {
                    printf("Error adding symbol: %s\n", token.value);
                }
            }
        }
        
        if (token.value) free(token.value);
        token = next_token(&lexer);
    }
    
    printf("\nFinal symbol table:\n");
    symtab_display(table);
    symtab_destroy(table);
}

// =============================================================================
// MAIN DEMONSTRATION PROGRAM
// =============================================================================

int main(int argc, char *argv[]) {
    printf("NexusLink Compiler Demo - Phase 1 Symbol Table Implementation\n");
    printf("Aegis Project: Real-World Compiler Engineering\n");
    printf("Author: Nnamdi Michael Okpala & Development Team\n\n");
    
    // Basic symbol table demonstration
    demonstrate_symbol_table();
    
    // Parse some simple variable declarations
    const char *sample_code = "int counter; float rate; char grade; int value;";
    parse_simple_declarations(sample_code);
    
    if (argc > 1) {
        printf("\n=== Processing command line input ===\n");
        parse_simple_declarations(argv[1]);
    }
    
    printf("\n=== Compiler Demo Completed Successfully ===\n");
    printf("This demonstrates real-world symbol table management\n");
    printf("suitable for production compiler implementation.\n\n");
    
    return 0;
}
