/**
 * @file token_system_test.c
 * @brief Test suite for NexusLink token system
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include "nlink_token_system.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * Test types
 */
static nlink_token_type_id TEST_CUSTOM_TYPE_1 = 0;
static nlink_token_type_id TEST_CUSTOM_TYPE_2 = 0;

/**
 * Test function for token transformation
 */
static void* test_transform(void* token, void* context) {
    nlink_token_base* base = (nlink_token_base*)token;
    if (base == NULL) {
        return NULL;
    }
    
    // Just return the same token for testing purposes
    return token;
}

/**
 * Test initialization
 */
static void test_initialization(void) {
    printf("Testing token system initialization... ");
    
    // Initialize with default configuration
    assert(nlink_token_system_init(NULL));
    
    // Get system state
    const nlink_token_system_state* state = nlink_token_system_get_state();
    assert(state != NULL);
    assert(state->status == NLINK_TOKEN_SYSTEM_INITIALIZED);
    
    // Shutdown
    nlink_token_system_shutdown();
    
    // State should be reset
    assert(state->status == NLINK_TOKEN_SYSTEM_UNINITIALIZED);
    
    // Custom configuration
    nlink_token_system_config config = {
        .enable_compile_time_checks = true,
        .strict_mode = true,
        .allow_implicit_casting = false,
        .max_custom_types = 64,
        .token_pool_size = 8192
    };
    
    assert(nlink_token_system_init(&config));
    assert(state->status == NLINK_TOKEN_SYSTEM_INITIALIZED);
    assert(state->config.enable_compile_time_checks == true);
    assert(state->config.strict_mode == true);
    assert(state->config.allow_implicit_casting == false);
    assert(state->config.max_custom_types == 64);
    assert(state->config.token_pool_size == 8192);
    
    printf("PASSED\n");
}

/**
 * Test custom type creation
 */
static void test_custom_types(void) {
    printf("Testing custom type creation... ");
    
    // Register custom types
    TEST_CUSTOM_TYPE_1 = nlink_token_system_create_custom_type(
        "test_type_1",
        NLINK_TYPE_IDENTIFIER,
        sizeof(nlink_token_identifier),
        NLINK_TYPE_FLAG_ATOMIC | NLINK_TYPE_FLAG_CASTABLE
    );
    
    assert(TEST_CUSTOM_TYPE_1 != 0);
    assert(TEST_CUSTOM_TYPE_1 >= NLINK_TYPE_CUSTOM_BASE);
    
    TEST_CUSTOM_TYPE_2 = nlink_token_system_create_custom_type(
        "test_type_2",
        NLINK_TYPE_EXPRESSION,
        sizeof(nlink_token_expression),
        NLINK_TYPE_FLAG_COMPOSITE | NLINK_TYPE_FLAG_EXPRESSION
    );
    
    assert(TEST_CUSTOM_TYPE_2 != 0);
    assert(TEST_CUSTOM_TYPE_2 >= NLINK_TYPE_CUSTOM_BASE);
    assert(TEST_CUSTOM_TYPE_2 != TEST_CUSTOM_TYPE_1);
    
    // Check type info
    const nlink_token_type_info* info1 = nlink_get_token_type_info(TEST_CUSTOM_TYPE_1);
    assert(info1 != NULL);
    assert(strcmp(info1->name, "test_type_1") == 0);
    assert(info1->size == sizeof(nlink_token_identifier));
    assert(info1->flags == (NLINK_TYPE_FLAG_ATOMIC | NLINK_TYPE_FLAG_CASTABLE));
    
    const nlink_token_type_info* info2 = nlink_get_token_type_info(TEST_CUSTOM_TYPE_2);
    assert(info2 != NULL);
    assert(strcmp(info2->name, "test_type_2") == 0);
    assert(info2->size == sizeof(nlink_token_expression));
    assert(info2->flags == (NLINK_TYPE_FLAG_COMPOSITE | NLINK_TYPE_FLAG_EXPRESSION));
    
    // Register transformation function
    assert(nlink_token_system_register_transform(TEST_CUSTOM_TYPE_1, test_transform));
    
    printf("PASSED\n");
}

/**
 * Test token creation
 */
static void test_token_creation(void) {
    printf("Testing token creation... ");
    
    // Create identifier token
    nlink_token_identifier* id_token = nlink_token_create_identifier(
        "test_identifier",
        1,
        1
    );
    
    assert(id_token != NULL);
    assert(id_token->base.type_id == NLINK_TYPE_IDENTIFIER);
    assert(strcmp(id_token->name, "test_identifier") == 0);
    assert(id_token->base.line == 1);
    assert(id_token->base.column == 1);
    
    // Create literal token
    int64_t int_value = 42;
    nlink_token_literal* lit_token = nlink_token_create_literal(
        1,  // Integer type
        &int_value,
        1,
        2
    );
    
    assert(lit_token != NULL);
    assert(lit_token->base.type_id == NLINK_TYPE_LITERAL);
    assert(lit_token->value_type == 1);
    assert(lit_token->int_value == 42);
    assert(lit_token->base.line == 1);
    assert(lit_token->base.column == 2);
    
    // Create expression token
    nlink_token_expression* expr_token = nlink_token_create_expression(
        NLINK_EXPR_IDENTIFIER,
        1,
        3
    );
    
    assert(expr_token != NULL);
    assert(expr_token->base.type_id == NLINK_TYPE_EXPRESSION);
    assert(expr_token->expr_type == NLINK_EXPR_IDENTIFIER);
    assert(expr_token->base.line == 1);
    assert(expr_token->base.column == 3);
    
    // Set the identifier
    expr_token->identifier = id_token;
    
    // Create statement token
    nlink_token_statement* stmt_token = nlink_token_create_statement(
        NLINK_STMT_EXPRESSION,
        1,
        4
    );
    
    assert(stmt_token != NULL);
    assert(stmt_token->base.type_id == NLINK_TYPE_STATEMENT);
    assert(stmt_token->stmt_type == NLINK_STMT_EXPRESSION);
    assert(stmt_token->base.line == 1);
    assert(stmt_token->base.column == 4);
    
    // Set the expression
    stmt_token->expression.expr = expr_token;
    
    // Create program token
    nlink_token_program* prog_token = nlink_token_create_program(
        "test_module",
        1,
        0
    );
    
    assert(prog_token != NULL);
    assert(prog_token->base.type_id == NLINK_TYPE_PROGRAM);
    assert(strcmp(prog_token->module_name, "test_module") == 0);
    assert(prog_token->base.line == 1);
    assert(prog_token->base.column == 0);
    
    // Add statement to program
    prog_token->statements = malloc(sizeof(nlink_token_statement*));
    assert(prog_token->statements != NULL);
    prog_token->statements[0] = stmt_token;
    prog_token->statement_count = 1;
    
    // Free everything
    nlink_token_free((nlink_token_base*)prog_token);  // This should recursively free everything
    
    printf("PASSED\n");
}

/**
 * Test token type checking
 */
static void test_type_checking(void) {
    printf("Testing token type checking... ");
    
    // Create tokens
    nlink_token_identifier* id_token = nlink_token_create_identifier(
        "test_identifier",
        1,
        1
    );
    
    nlink_token_expression* expr_token = nlink_token_create_expression(
        NLINK_EXPR_IDENTIFIER,
        1,
        2
    );
    
    expr_token->identifier = id_token;
    
    // Type checking
    assert(nlink_is_token_type(id_token, NLINK_TYPE_IDENTIFIER));
    assert(!nlink_is_token_type(id_token, NLINK_TYPE_EXPRESSION));
    
    assert(nlink_is_token_type(expr_token, NLINK_TYPE_EXPRESSION));
    assert(!nlink_is_token_type(expr_token, NLINK_TYPE_IDENTIFIER));
    
    // Assert type
    assert(nlink_assert_token_type(id_token, NLINK_TYPE_IDENTIFIER));
    assert(!nlink_assert_token_type(id_token, NLINK_TYPE_EXPRESSION));
    
    // Type casting
    nlink_token_expression* cast_expr = NLINK_TOKEN_AS_EXPRESSION(expr_token);
    assert(cast_expr == expr_token);
    
    nlink_token_expression* invalid_cast = NLINK_TOKEN_AS_EXPRESSION(id_token);
    assert(invalid_cast == NULL);
    
    // Free tokens
    nlink_token_free((nlink_token_base*)expr_token);  // This will free both tokens
    
    printf("PASSED\n");
}

/**
 * Test tokenization and parsing
 */
static void test_tokenization_parsing(void) {
    printf("Testing tokenization and parsing... ");
    
    const char* source = "identifier = 42;";
    
    // Tokenize
    nlink_token_base* tokens = nlink_token_system_tokenize(source, "test.nlink", NULL);
    assert(tokens != NULL);
    
    // Parse
    nlink_token_program* program = nlink_token_system_parse(tokens, NULL);
    assert(program != NULL);
    
    // Execute
    void* result = nlink_token_system_execute(program, NULL);
    // For this basic test, we don't expect a meaningful result
    
    // Free
    nlink_token_free((nlink_token_base*)program);
    nlink_token_list_free(tokens);
    
    printf("PASSED\n");
}

/**
 * Main test function
 */
int main(void) {
    printf("=== NexusLink Token System Tests ===\n");
    
    // Initialize
    assert(nlink_token_system_init(NULL));
    
    // Run tests
    test_initialization();
    test_custom_types();
    test_token_creation();
    test_type_checking();
    test_tokenization_parsing();
    
    // Shutdown
    nlink_token_system_shutdown();
    
    printf("All tests passed!\n");
    return 0;
}