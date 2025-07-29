/**
 * @file token_value.c
 * @brief Implementation of token operations for NexusLink scripting
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include "token_value.h"
#include <stdlib.h>
#include <string.h>

nlink_token_base* nlink_token_create(nlink_token_type_id type_id, 
                                    const char* source,
                                    size_t line, size_t column) {
    // Get type information to determine token size
    const nlink_token_type_info* type_info = nlink_get_token_type_info(type_id);
    if (type_info == NULL) {
        return NULL;  // Unknown type
    }
    
    // Allocate memory for the token
    nlink_token_base* token = (nlink_token_base*)calloc(1, type_info->size);
    if (token == NULL) {
        return NULL;  // Memory allocation failed
    }
    
    // Initialize base fields
    token->type_id = type_id;
    token->line = line;
    token->column = column;
    token->next = NULL;
    
    if (source != NULL) {
        token->source = strdup(source);
        if (token->source == NULL) {
            free(token);
            return NULL;  // Memory allocation failed
        }
    } else {
        token->source = NULL;
    }
    
    return token;
}

void nlink_token_free(nlink_token_base* token) {
    if (token == NULL) {
        return;
    }
    
    // Free specific token type resources
    nlink_token_type_id type_id = token->type_id;
    
    switch (type_id) {
        case NLINK_TYPE_IDENTIFIER: {
            nlink_token_identifier* id_token = (nlink_token_identifier*)token;
            free(id_token->name);
            break;
        }
        case NLINK_TYPE_LITERAL: {
            nlink_token_literal* lit_token = (nlink_token_literal*)token;
            if (lit_token->value_type == 3) {  // Assuming 3 is string type
                free(lit_token->string_value);
            } else if (lit_token->value_type > 10) {  // Complex types
                // Free complex value (implementation depends on type)
                // This is a placeholder for type-specific cleanup
            }
            break;
        }
        case NLINK_TYPE_EXPRESSION: {
            nlink_token_expression* expr_token = (nlink_token_expression*)token;
            // Free expression-specific resources
            switch (expr_token->expr_type) {
                case NLINK_EXPR_BINARY:
                    nlink_token_free((nlink_token_base*)expr_token->binary.left);
                    nlink_token_free((nlink_token_base*)expr_token->binary.right);
                    nlink_token_free((nlink_token_base*)expr_token->binary.op);
                    break;
                case NLINK_EXPR_UNARY:
                    nlink_token_free((nlink_token_base*)expr_token->unary.operand);
                    nlink_token_free((nlink_token_base*)expr_token->unary.op);
                    break;
                case NLINK_EXPR_CALL:
                    nlink_token_free((nlink_token_base*)expr_token->call.callee);
                    for (size_t i = 0; i < expr_token->call.arg_count; i++) {
                        nlink_token_free((nlink_token_base*)expr_token->call.args[i]);
                    }
                    free(expr_token->call.args);
                    break;
                case NLINK_EXPR_GROUP:
                    nlink_token_free((nlink_token_base*)expr_token->group.expr);
                    break;
                case NLINK_EXPR_MEMBER:
                    nlink_token_free((nlink_token_base*)expr_token->member.object);
                    nlink_token_free((nlink_token_base*)expr_token->member.property);
                    break;
                case NLINK_EXPR_INDEX:
                    nlink_token_free((nlink_token_base*)expr_token->index.object);
                    nlink_token_free((nlink_token_base*)expr_token->index.index);
                    break;
                case NLINK_EXPR_LAMBDA:
                    for (size_t i = 0; i < expr_token->lambda.param_count; i++) {
                        nlink_token_free((nlink_token_base*)expr_token->lambda.params[i]);
                    }
                    free(expr_token->lambda.params);
                    nlink_token_free((nlink_token_base*)expr_token->lambda.body);
                    break;
                case NLINK_EXPR_LITERAL:
                    nlink_token_free((nlink_token_base*)expr_token->literal);
                    break;
                case NLINK_EXPR_IDENTIFIER:
                    nlink_token_free((nlink_token_base*)expr_token->identifier);
                    break;
                default:
                    break;
            }
            break;
        }
        case NLINK_TYPE_STATEMENT: {
            nlink_token_statement* stmt_token = (nlink_token_statement*)token;
            // Free statement-specific resources
            switch (stmt_token->stmt_type) {
                case NLINK_STMT_DECLARATION:
                    nlink_token_free((nlink_token_base*)stmt_token->declaration.id);
                    nlink_token_free((nlink_token_base*)stmt_token->declaration.init);
                    break;
                case NLINK_STMT_ASSIGNMENT:
                    nlink_token_free((nlink_token_base*)stmt_token->assignment.target);
                    nlink_token_free((nlink_token_base*)stmt_token->assignment.value);
                    break;
                case NLINK_STMT_CONDITIONAL:
                    nlink_token_free((nlink_token_base*)stmt_token->conditional.condition);
                    nlink_token_free((nlink_token_base*)stmt_token->conditional.consequent);
                    nlink_token_free((nlink_token_base*)stmt_token->conditional.alternate);
                    break;
                case NLINK_STMT_LOOP:
                    nlink_token_free((nlink_token_base*)stmt_token->loop.init);
                    nlink_token_free((nlink_token_base*)stmt_token->loop.condition);
                    nlink_token_free((nlink_token_base*)stmt_token->loop.update);
                    nlink_token_free((nlink_token_base*)stmt_token->loop.body);
                    break;
                case NLINK_STMT_RETURN:
                    nlink_token_free((nlink_token_base*)stmt_token->return_stmt.value);
                    break;
                case NLINK_STMT_BLOCK:
                    for (size_t i = 0; i < stmt_token->block.count; i++) {
                        nlink_token_free((nlink_token_base*)stmt_token->block.statements[i]);
                    }
                    free(stmt_token->block.statements);
                    break;
                case NLINK_STMT_EXPRESSION:
                    nlink_token_free((nlink_token_base*)stmt_token->expression.expr);
                    break;
                case NLINK_STMT_IMPORT:
                    free(stmt_token->import.module_name);
                    for (size_t i = 0; i < stmt_token->import.import_count; i++) {
                        nlink_token_free((nlink_token_base*)stmt_token->import.imports[i]);
                    }
                    free(stmt_token->import.imports);
                    break;
                case NLINK_STMT_EXPORT:
                    for (size_t i = 0; i < stmt_token->export.export_count; i++) {
                        nlink_token_free((nlink_token_base*)stmt_token->export.exports[i]);
                    }
                    free(stmt_token->export.exports);
                    break;
                case NLINK_STMT_PIPELINE:
                    free(stmt_token->pipeline.pipeline_name);
                    for (size_t i = 0; i < stmt_token->pipeline.stage_count; i++) {
                        nlink_token_free((nlink_token_base*)stmt_token->pipeline.stages[i]);
                    }
                    free(stmt_token->pipeline.stages);
                    break;
                default:
                    break;
            }
            break;
        }
        case NLINK_TYPE_PROGRAM: {
            nlink_token_program* prog_token = (nlink_token_program*)token;
            for (size_t i = 0; i < prog_token->statement_count; i++) {
                nlink_token_free((nlink_token_base*)prog_token->statements[i]);
            }
            free(prog_token->statements);
            free(prog_token->module_name);
            // Free symbol table (implementation depends on symbol table structure)
            break;
        }
        default:
            break;
    }
    
    // Free source text and token itself
    free((void*)token->source);
    free(token);
}

void nlink_token_list_free(nlink_token_base* token) {
    while (token != NULL) {
        nlink_token_base* next = token->next;
        nlink_token_free(token);
        token = next;
    }
}

nlink_token_base* nlink_token_transform(nlink_token_base* token,
                                       nlink_transform_fn transform,
                                       void* context) {
    if (token == NULL || transform == NULL) {
        return token;
    }
    
    return (nlink_token_base*)transform(token, context);
}

nlink_token_identifier* nlink_token_create_identifier(const char* name, 
                                                   size_t line, size_t column) {
    nlink_token_identifier* token = (nlink_token_identifier*)nlink_token_create(
        NLINK_TYPE_IDENTIFIER, name, line, column);
    
    if (token == NULL) {
        return NULL;
    }
    
    if (name != NULL) {
        token->name = strdup(name);
        if (token->name == NULL) {
            nlink_token_free((nlink_token_base*)token);
            return NULL;
        }
    } else {
        token->name = NULL;
    }
    
    token->symbol = NULL;
    
    return token;
}

nlink_token_literal* nlink_token_create_literal(uint32_t value_type, 
                                             const void* value,
                                             size_t line, size_t column) {
    nlink_token_literal* token = (nlink_token_literal*)nlink_token_create(
        NLINK_TYPE_LITERAL, NULL, line, column);
    
    if (token == NULL) {
        return NULL;
    }
    
    token->value_type = value_type;
    
    // Initialize value based on type
    if (value != NULL) {
        switch (value_type) {
            case 1:  // Integer
                token->int_value = *(const int64_t*)value;
                break;
            case 2:  // Float
                token->float_value = *(const double*)value;
                break;
            case 3:  // String
                token->string_value = strdup((const char*)value);
                if (token->string_value == NULL) {
                    nlink_token_free((nlink_token_base*)token);
                    return NULL;
                }
                break;
            case 4:  // Boolean
                token->bool_value = *(const bool*)value;
                break;
            default:  // Complex or unknown type
                token->complex_value = NULL;
                break;
        }
    }
    
    return token;
}

nlink_token_operator* nlink_token_create_operator(uint32_t op_type, 
                                                const char* text,
                                                size_t line, size_t column) {
    nlink_token_operator* token = (nlink_token_operator*)nlink_token_create(
        NLINK_TYPE_OPERATOR, text, line, column);
    
    if (token == NULL) {
        return NULL;
    }
    
    token->op_type = op_type;
    
    // Set precedence based on operator type
    switch (op_type) {
        case 1:  // Assignment (=)
            token->precedence = 1;
            token->is_unary = false;
            break;
        case 2:  // Logical OR (||)
            token->precedence = 2;
            token->is_unary = false;
            break;
        case 3:  // Logical AND (&&)
            token->precedence = 3;
            token->is_unary = false;
            break;
        case 4:  // Equality (==, !=)
            token->precedence = 4;
            token->is_unary = false;
            break;
        case 5:  // Comparison (<, >, <=, >=)
            token->precedence = 5;
            token->is_unary = false;
            break;
        case 6:  // Addition, subtraction (+, -)
            token->precedence = 6;
            token->is_unary = false;
            break;
        case 7:  // Multiplication, division, modulo (*, /, %)
            token->precedence = 7;
            token->is_unary = false;
            break;
        case 8:  // Unary (!, -, +, ++, --)
            token->precedence = 8;
            token->is_unary = true;
            break;
        case 9:  // Member access (., ->)
            token->precedence = 9;
            token->is_unary = false;
            break;
        default:
            token->precedence = 0;
            token->is_unary = false;
            break;
    }
    
    return token;
}

nlink_token_keyword* nlink_token_create_keyword(uint32_t keyword_type, 
                                              const char* text,
                                              size_t line, size_t column) {
    nlink_token_keyword* token = (nlink_token_keyword*)nlink_token_create(
        NLINK_TYPE_KEYWORD, text, line, column);
    
    if (token == NULL) {
        return NULL;
    }
    
    token->keyword_type = keyword_type;
    
    return token;
}

nlink_token_expression* nlink_token_create_expression(nlink_expression_type expr_type,
                                                    size_t line, size_t column) {
    nlink_token_expression* token = (nlink_token_expression*)nlink_token_create(
        NLINK_TYPE_EXPRESSION, NULL, line, column);
    
    if (token == NULL) {
        return NULL;
    }
    
    token->expr_type = expr_type;
    
    // Initialize expression-specific fields to NULL
    switch (expr_type) {
        case NLINK_EXPR_BINARY:
            token->binary.left = NULL;
            token->binary.right = NULL;
            token->binary.op = NULL;
            break;
        case NLINK_EXPR_UNARY:
            token->unary.operand = NULL;
            token->unary.op = NULL;
            token->unary.prefix = true;
            break;
        case NLINK_EXPR_CALL:
            token->call.callee = NULL;
            token->call.args = NULL;
            token->call.arg_count = 0;
            break;
        case NLINK_EXPR_GROUP:
            token->group.expr = NULL;
            break;
        case NLINK_EXPR_MEMBER:
            token->member.object = NULL;
            token->member.property = NULL;
            break;
        case NLINK_EXPR_INDEX:
            token->index.object = NULL;
            token->index.index = NULL;
            break;
        case NLINK_EXPR_LAMBDA:
            token->lambda.params = NULL;
            token->lambda.param_count = 0;
            token->lambda.body = NULL;
            break;
        case NLINK_EXPR_LITERAL:
            token->literal = NULL;
            break;
        case NLINK_EXPR_IDENTIFIER:
            token->identifier = NULL;
            break;
        default:
            break;
    }
    
    return token;
}

nlink_token_statement* nlink_token_create_statement(nlink_statement_type stmt_type,
                                                  size_t line, size_t column) {
    nlink_token_statement* token = (nlink_token_statement*)nlink_token_create(
        NLINK_TYPE_STATEMENT, NULL, line, column);
    
    if (token == NULL) {
        return NULL;
    }
    
    token->stmt_type = stmt_type;
    
    // Initialize statement-specific fields to NULL
    switch (stmt_type) {
        case NLINK_STMT_DECLARATION:
            token->declaration.id = NULL;
            token->declaration.init = NULL;
            token->declaration.type_hint = 0;
            break;
        case NLINK_STMT_ASSIGNMENT:
            token->assignment.target = NULL;
            token->assignment.value = NULL;
            break;
        case NLINK_STMT_CONDITIONAL:
            token->conditional.condition = NULL;
            token->conditional.consequent = NULL;
            token->conditional.alternate = NULL;
            break;
        case NLINK_STMT_LOOP:
            token->loop.init = NULL;
            token->loop.condition = NULL;
            token->loop.update = NULL;
            token->loop.body = NULL;
            break;
        case NLINK_STMT_RETURN:
            token->return_stmt.value = NULL;
            break;
        case NLINK_STMT_BLOCK:
            token->block.statements = NULL;
            token->block.count = 0;
            break;
        case NLINK_STMT_EXPRESSION:
            token->expression.expr = NULL;
            break;
        case NLINK_STMT_IMPORT:
            token->import.module_name = NULL;
            token->import.imports = NULL;
            token->import.import_count = 0;
            break;
        case NLINK_STMT_EXPORT:
            token->export.exports = NULL;
            token->export.export_count = 0;
            break;
        case NLINK_STMT_PIPELINE:
            token->pipeline.pipeline_name = NULL;
            token->pipeline.stages = NULL;
            token->pipeline.stage_count = 0;
            break;
        default:
            break;
    }
    
    return token;
}

nlink_token_program* nlink_token_create_program(const char* module_name,
                                              size_t line, size_t column) {
    nlink_token_program* token = (nlink_token_program*)nlink_token_create(
        NLINK_TYPE_PROGRAM, NULL, line, column);
    
    if (token == NULL) {
        return NULL;
    }
    
    token->statements = NULL;
    token->statement_count = 0;
    
    if (module_name != NULL) {
        token->module_name = strdup(module_name);
        if (token->module_name == NULL) {
            nlink_token_free((nlink_token_base*)token);
            return NULL;
        }
    } else {
        token->module_name = NULL;
    }
    
    token->symbol_table = NULL;
    
    return token;
}

nlink_token_literal* nlink_token_create_literal(uint32_t value_type, 
                                             const void* value,
                                             size_t line, size_t column) {
    nlink_token_literal* token = (nlink_token_literal*)nlink_token_create(
        NLINK_TYPE_LITERAL, NULL, line, column);
    
    if (token == NULL) {
        return NULL;
    }
    
    token->value_type = value_type;
    
    // Initialize value based on type
    if (value != NULL) {
        switch (value_type) {
            case 1:  // Integer
                token->int_value = *(const int64_t*)value;
                break;
            case 2:  // Float
                token->float_value = *(const double*)value;
                break;
            case 3:  // String
                token->string_value = strdup((const char*)value);
                if (token->string_value == NULL) {
                    nlink_token_free((nlink_token_base*)token);
                    return NULL;
                }
                break;
            case 4:  // Boolean
                token->bool_value = *(const bool*)value;
                break;
            default:  // Complex or unknown type
                token->complex_value = NULL;
                break;
        }
    }
    
    return token;
}

nexus_result nlink_token_create_literal_with_result(uint32_t value_type, 
                                                  const void* value,
                                                  size_t line, size_t column,
                                                  nlink_token_literal** out_token) {
    if (out_token == NULL) {
        nexus_error* error = nexus_error_create(
            NEXUS_ERROR_INVALID_ARGUMENT,
            "Output token pointer cannot be NULL",
            __FILE__, __LINE__
        );
        return nexus_error_result(error, NULL);
    }

    nlink_token_base* base_token;
    nexus_result result = nlink_token_create_with_result(
        NLINK_TYPE_LITERAL, NULL, line, column, &base_token);
    
    if (result.status != NEXUS_STATUS_SUCCESS) {
        return result;
    }
    
    nlink_token_literal* token = (nlink_token_literal*)base_token;
    token->value_type = value_type;
    
    // Initialize value based on type
    if (value != NULL) {
        switch (value_type) {
            case 1:  // Integer
                token->int_value = *(const int64_t*)value;
                break;
            case 2:  // Float
                token->float_value = *(const double*)value;
                break;
            case 3:  // String
                token->string_value = strdup((const char*)value);
                if (token->string_value == NULL) {
                    nlink_token_free(base_token);
                    nexus_error* error = nexus_error_create(
                        NEXUS_ERROR_OUT_OF_MEMORY,
                        "Failed to allocate memory for string value",
                        __FILE__, __LINE__
                    );
                    return nexus_error_result(error, NULL);
                }
                break;
            case 4:  // Boolean
                token->bool_value = *(const bool*)value;
                break;
            default:  // Complex or unknown type
                token->complex_value = NULL;
                break;
        }
    }
    
    *out_token = token;
    return nexus_success(token, NULL);
}