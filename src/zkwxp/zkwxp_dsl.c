/*
 * Zero-Knowledge Weighted XOR Proofs - Domain Specific Language Implementation
 * OBINexus Standard Compliant
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "nlink/zkwxp/zkwxp_core.h"
#include "nlink/core/etps/telemetry.h"

/* DSL Token types */
typedef enum {
    TOKEN_RULE,
    TOKEN_WHEN,
    TOKEN_WEIGHT,
    TOKEN_PRIORITY,
    TOKEN_EVENT,
    TOKEN_PATTERN,
    TOKEN_THRESHOLD,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,
    TOKEN_GT,
    TOKEN_LT,
    TOKEN_EQ,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,
    TOKEN_EOF
} dsl_token_type_t;

/* Token structure */
typedef struct {
    dsl_token_type_t type;
    char* value;
    uint32_t line;
    uint32_t column;
} dsl_token_t;

/* Lexer state */
typedef struct {
    const char* input;
    uint32_t position;
    uint32_t line;
    uint32_t column;
} dsl_lexer_t;

/* Parser state */
typedef struct {
    dsl_token_t* tokens;
    uint32_t token_count;
    uint32_t current;
} dsl_parser_t;

/* AST Node types */
typedef enum {
    AST_RULE_DEF,
    AST_CONDITION,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_EVENT_MATCH,
    AST_PATTERN_MATCH,
    AST_THRESHOLD,
    AST_LITERAL,
    AST_IDENTIFIER
} ast_node_type_t;

/* AST Node structure */
typedef struct ast_node {
    ast_node_type_t type;
    union {
        struct {
            char* name;
            uint32_t weight;
            zkwxp_priority_t priority;
            struct ast_node* condition;
        } rule_def;
        
        struct {
            char* op;
            struct ast_node* left;
            struct ast_node* right;
        } binary_op;
        
        struct {
            char* op;
            struct ast_node* operand;
        } unary_op;
        
        struct {
            zkwxp_event_type_t event_type;
            char* pattern;
        } event_match;
        
        struct {
            char* metric;
            char* op;
            uint64_t value;
        } threshold;
        
        struct {
            char* value;
        } literal;
    } data;
} ast_node_t;

/* Forward declarations */
static dsl_token_t* lex_input(const char* input, uint32_t* token_count);
static ast_node_t* parse_tokens(dsl_token_t* tokens, uint32_t token_count);
static NexusResult compile_ast(ast_node_t* ast, void** bytecode, uint32_t* size);

/* Lexer implementation */
static char* extract_string(dsl_lexer_t* lexer) {
    uint32_t start = lexer->position;
    lexer->position++; /* Skip opening quote */
    
    while (lexer->input[lexer->position] != '"' && 
           lexer->input[lexer->position] != '\0') {
        if (lexer->input[lexer->position] == '\\') {
            lexer->position += 2; /* Skip escape sequence */
        } else {
            lexer->position++;
        }
    }
    
    if (lexer->input[lexer->position] == '"') {
        lexer->position++; /* Skip closing quote */
    }
    
    uint32_t len = lexer->position - start - 2;
    char* str = malloc(len + 1);
    memcpy(str, &lexer->input[start + 1], len);
    str[len] = '\0';
    
    return str;
}

static char* extract_identifier(dsl_lexer_t* lexer) {
    uint32_t start = lexer->position;
    
    while (isalnum(lexer->input[lexer->position]) || 
           lexer->input[lexer->position] == '_') {
        lexer->position++;
    }
    
    uint32_t len = lexer->position - start;
    char* id = malloc(len + 1);
    memcpy(id, &lexer->input[start], len);
    id[len] = '\0';
    
    return id;
}

static dsl_token_type_t keyword_to_token(const char* keyword) {
    if (strcmp(keyword, "rule") == 0) return TOKEN_RULE;
    if (strcmp(keyword, "when") == 0) return TOKEN_WHEN;
    if (strcmp(keyword, "weight") == 0) return TOKEN_WEIGHT;
    if (strcmp(keyword, "priority") == 0) return TOKEN_PRIORITY;
    if (strcmp(keyword, "event") == 0) return TOKEN_EVENT;
    if (strcmp(keyword, "pattern") == 0) return TOKEN_PATTERN;
    if (strcmp(keyword, "threshold") == 0) return TOKEN_THRESHOLD;
    if (strcmp(keyword, "and") == 0) return TOKEN_AND;
    if (strcmp(keyword, "or") == 0) return TOKEN_OR;
    if (strcmp(keyword, "not") == 0) return TOKEN_NOT;
    return TOKEN_IDENTIFIER;
}

static dsl_token_t* lex_input(const char* input, uint32_t* token_count) {
    dsl_lexer_t lexer = {
        .input = input,
        .position = 0,
        .line = 1,
        .column = 1
    };
    
    dsl_token_t* tokens = NULL;
    uint32_t capacity = 0;
    *token_count = 0;
    
    while (lexer.input[lexer.position] != '\0') {
        /* Skip whitespace */
        if (isspace(lexer.input[lexer.position])) {
            if (lexer.input[lexer.position] == '\n') {
                lexer.line++;
                lexer.column = 1;
            } else {
                lexer.column++;
            }
            lexer.position++;
            continue;
        }
        
        /* Skip comments */
        if (lexer.input[lexer.position] == '/' && 
            lexer.input[lexer.position + 1] == '/') {
            while (lexer.input[lexer.position] != '\n' && 
                   lexer.input[lexer.position] != '\0') {
                lexer.position++;
            }
            continue;
        }
        
        /* Allocate more tokens if needed */
        if (*token_count >= capacity) {
            capacity = capacity ? capacity * 2 : 16;
            tokens = realloc(tokens, capacity * sizeof(dsl_token_t));
        }
        
        dsl_token_t* token = &tokens[*token_count];
        token->line = lexer.line;
        token->column = lexer.column;
        
        /* Lex different token types */
        char c = lexer.input[lexer.position];
        
        if (c == '"') {
            token->type = TOKEN_STRING;
            token->value = extract_string(&lexer);
        } else if (isdigit(c)) {
            token->type = TOKEN_NUMBER;
            token->value = extract_identifier(&lexer);
        } else if (isalpha(c) || c == '_') {
            token->value = extract_identifier(&lexer);
            token->type = keyword_to_token(token->value);
        } else {
            /* Single character tokens */
            switch (c) {
                case '{': token->type = TOKEN_LBRACE; break;
                case '}': token->type = TOKEN_RBRACE; break;
                case '(': token->type = TOKEN_LPAREN; break;
                case ')': token->type = TOKEN_RPAREN; break;
                case '>': token->type = TOKEN_GT; break;
                case '<': token->type = TOKEN_LT; break;
                case '=': token->type = TOKEN_EQ; break;
                default:
                    lexer.position++;
                    continue;
            }
            token->value = malloc(2);
            token->value[0] = c;
            token->value[1] = '\0';
            lexer.position++;
            lexer.column++;
        }
        
        (*token_count)++;
    }
    
    /* Add EOF token */
    if (*token_count >= capacity) {
        capacity++;
        tokens = realloc(tokens, capacity * sizeof(dsl_token_t));
    }
    tokens[*token_count].type = TOKEN_EOF;
    tokens[*token_count].value = NULL;
    (*token_count)++;
    
    return tokens;
}

/* Parser implementation */
static ast_node_t* create_ast_node(ast_node_type_t type) {
    ast_node_t* node = calloc(1, sizeof(ast_node_t));
    node->type = type;
    return node;
}

static ast_node_t* parse_expression(dsl_parser_t* parser);

static ast_node_t* parse_primary(dsl_parser_t* parser) {
    dsl_token_t* token = &parser->tokens[parser->current];
    
    if (token->type == TOKEN_NUMBER) {
        ast_node_t* node = create_ast_node(AST_LITERAL);
        node->data.literal.value = strdup(token->value);
        parser->current++;
        return node;
    }
    
    if (token->type == TOKEN_STRING) {
        ast_node_t* node = create_ast_node(AST_LITERAL);
        node->data.literal.value = strdup(token->value);
        parser->current++;
        return node;
    }
    
    if (token->type == TOKEN_IDENTIFIER) {
        ast_node_t* node = create_ast_node(AST_IDENTIFIER);
        node->data.literal.value = strdup(token->value);
        parser->current++;
        return node;
    }
    
    if (token->type == TOKEN_LPAREN) {
        parser->current++; /* Skip '(' */
        ast_node_t* expr = parse_expression(parser);
        parser->current++; /* Skip ')' */
        return expr;
    }
    
    return NULL;
}

static ast_node_t* parse_event_match(dsl_parser_t* parser) {
    parser->current++; /* Skip 'event' */
    
    ast_node_t* node = create_ast_node(AST_EVENT_MATCH);
    dsl_token_t* event_token = &parser->tokens[parser->current++];
    
    /* Map event name to type */
    if (strcmp(event_token->value, "context_switch") == 0) {
        node->data.event_match.event_type = ZKWXP_EVENT_CONTEXT_SWITCH;
    } else if (strcmp(event_token->value, "priority_change") == 0) {
        node->data.event_match.event_type = ZKWXP_EVENT_PRIORITY_CHANGE;
    } else if (strcmp(event_token->value, "state_transition") == 0) {
        node->data.event_match.event_type = ZKWXP_EVENT_STATE_TRANSITION;
    }
    
    /* Optional pattern */
    if (parser->tokens[parser->current].type == TOKEN_PATTERN) {
        parser->current++; /* Skip 'pattern' */
        node->data.event_match.pattern = strdup(parser->tokens[parser->current++].value);
    }
    
    return node;
}

static ast_node_t* parse_threshold(dsl_parser_t* parser) {
    parser->current++; /* Skip 'threshold' */
    
    ast_node_t* node = create_ast_node(AST_THRESHOLD);
    node->data.threshold.metric = strdup(parser->tokens[parser->current++].value);
    
    dsl_token_t* op_token = &parser->tokens[parser->current++];
    if (op_token->type == TOKEN_GT) {
        node->data.threshold.op = strdup(">");
    } else if (op_token->type == TOKEN_LT) {
        node->data.threshold.op = strdup("<");
    } else if (op_token->type == TOKEN_EQ) {
        node->data.threshold.op = strdup("=");
    }
    
    node->data.threshold.value = strtoull(parser->tokens[parser->current++].value, NULL, 10);
    
    return node;
}

static ast_node_t* parse_condition(dsl_parser_t* parser) {
    ast_node_t* left = NULL;
    
    if (parser->tokens[parser->current].type == TOKEN_EVENT) {
        left = parse_event_match(parser);
    } else if (parser->tokens[parser->current].type == TOKEN_THRESHOLD) {
        left = parse_threshold(parser);
    } else if (parser->tokens[parser->current].type == TOKEN_NOT) {
        parser->current++; /* Skip 'not' */
        ast_node_t* node = create_ast_node(AST_UNARY_OP);
        node->data.unary_op.op = strdup("not");
        node->data.unary_op.operand = parse_condition(parser);
        return node;
    } else {
        left = parse_primary(parser);
    }
    
    /* Check for binary operators */
    if (parser->tokens[parser->current].type == TOKEN_AND ||
        parser->tokens[parser->current].type == TOKEN_OR) {
        ast_node_t* node = create_ast_node(AST_BINARY_OP);
        node->data.binary_op.left = left;
        node->data.binary_op.op = strdup(parser->tokens[parser->current++].value);
        node->data.binary_op.right = parse_condition(parser);
        return node;
    }
    
    return left;
}

static ast_node_t* parse_expression(dsl_parser_t* parser) {
    return parse_condition(parser);
}

static ast_node_t* parse_rule(dsl_parser_t* parser) {
    parser->current++; /* Skip 'rule' */
    
    ast_node_t* node = create_ast_node(AST_RULE_DEF);
    node->data.rule_def.name = strdup(parser->tokens[parser->current++].value);
    
    parser->current++; /* Skip '{' */
    
    /* Default values */
    node->data.rule_def.weight = 100;
    node->data.rule_def.priority = ZKWXP_PRIORITY_MEDIUM;
    
    while (parser->tokens[parser->current].type != TOKEN_RBRACE &&
           parser->tokens[parser->current].type != TOKEN_EOF) {
        if (parser->tokens[parser->current].type == TOKEN_WEIGHT) {
            parser->current++; /* Skip 'weight' */
            node->data.rule_def.weight = strtoul(parser->tokens[parser->current++].value, NULL, 10);
        } else if (parser->tokens[parser->current].type == TOKEN_PRIORITY) {
            parser->current++; /* Skip 'priority' */
            char* prio = parser->tokens[parser->current++].value;
            if (strcmp(prio, "critical") == 0) {
                node->data.rule_def.priority = ZKWXP_PRIORITY_CRITICAL;
            } else if (strcmp(prio, "high") == 0) {
                node->data.rule_def.priority = ZKWXP_PRIORITY_HIGH;
            } else if (strcmp(prio, "medium") == 0) {
                node->data.rule_def.priority = ZKWXP_PRIORITY_MEDIUM;
            } else if (strcmp(prio, "low") == 0) {
                node->data.rule_def.priority = ZKWXP_PRIORITY_LOW;
            }
        } else if (parser->tokens[parser->current].type == TOKEN_WHEN) {
            parser->current++; /* Skip 'when' */
            node->data.rule_def.condition = parse_expression(parser);
        } else {
            parser->current++;
        }
    }
    
    parser->current++; /* Skip '}' */
    
    return node;
}

static ast_node_t* parse_tokens(dsl_token_t* tokens, uint32_t token_count) {
    dsl_parser_t parser = {
        .tokens = tokens,
        .token_count = token_count,
        .current = 0
    };
    
    /* For now, parse a single rule */
    if (tokens[0].type == TOKEN_RULE) {
        return parse_rule(&parser);
    }
    
    return NULL;
}

/* Bytecode instruction set */
typedef enum {
    OP_PUSH,
    OP_POP,
    OP_EVENT_MATCH,
    OP_PATTERN_MATCH,
    OP_THRESHOLD_CHECK,
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_RETURN
} bytecode_op_t;

/* Bytecode compilation */
static NexusResult compile_condition(ast_node_t* node, uint8_t** code, uint32_t* size);

static NexusResult emit_op(uint8_t** code, uint32_t* size, uint32_t* capacity,
                          bytecode_op_t op, uint64_t operand) {
    uint32_t needed = *size + 1 + sizeof(uint64_t);
    
    if (needed > *capacity) {
        *capacity = needed * 2;
        *code = realloc(*code, *capacity);
        if (!*code) return NEXUS_ERROR_OUT_OF_MEMORY;
    }
    
    (*code)[(*size)++] = (uint8_t)op;
    memcpy(&(*code)[*size], &operand, sizeof(uint64_t));
    *size += sizeof(uint64_t);
    
    return NEXUS_OK;
}

static NexusResult compile_condition(ast_node_t* node, uint8_t** code, uint32_t* size) {
    if (!node) return NEXUS_ERROR_INVALID_ARGUMENT;
    
    uint32_t capacity = 256;
    *code = malloc(capacity);
    *size = 0;
    
    switch (node->type) {
        case AST_EVENT_MATCH:
            return emit_op(code, size, &capacity, OP_EVENT_MATCH, 
                          (uint64_t)node->data.event_match.event_type);
            
        case AST_THRESHOLD:
            return emit_op(code, size, &capacity, OP_THRESHOLD_CHECK,
                          node->data.threshold.value);
            
        case AST_BINARY_OP:
            {
                NexusResult result = compile_condition(node->data.binary_op.left, code, size);
                if (result != NEXUS_OK) return result;
                
                result = compile_condition(node->data.binary_op.right, code, size);
                if (result != NEXUS_OK) return result;
                
                if (strcmp(node->data.binary_op.op, "and") == 0) {
                    return emit_op(code, size, &capacity, OP_AND, 0);
                } else if (strcmp(node->data.binary_op.op, "or") == 0) {
                    return emit_op(code, size, &capacity, OP_OR, 0);
                }
            }
            break;
            
        case AST_UNARY_OP:
            {
                NexusResult result = compile_condition(node->data.unary_op.operand, code, size);
                if (result != NEXUS_OK) return result;
                
                if (strcmp(node->data.unary_op.op, "not") == 0) {
                    return emit_op(code, size, &capacity, OP_NOT, 0);
                }
            }
            break;
            
        default:
            return NEXUS_ERROR_NOT_SUPPORTED;
    }
    
    return NEXUS_OK;
}

static NexusResult compile_ast(ast_node_t* ast, void** bytecode, uint32_t* size) {
    if (!ast || ast->type != AST_RULE_DEF) {
        return NEXUS_ERROR_INVALID_ARGUMENT;
    }
    
    /* Compile the condition part of the rule */
    if (ast->data.rule_def.condition) {
        uint8_t* code = NULL;
        NexusResult result = compile_condition(ast->data.rule_def.condition, 
                                               &code, size);
        if (result == NEXUS_OK) {
            /* Add return instruction */
            uint32_t capacity = *size + 1;
            code = realloc(code, capacity);
            code[(*size)++] = OP_RETURN;
            
            *bytecode = code;
        }
        return result;
    }
    
    return NEXUS_ERROR_INVALID_ARGUMENT;
}

/* Public API implementation */
NexusResult zkwxp_dsl_compile(const char* expression,
                              void** bytecode,
                              uint32_t* bytecode_size) {
    ETPS_TRACE_FUNCTION();
    
    if (!expression || !bytecode || !bytecode_size) {
        return NEXUS_ERROR_INVALID_ARGUMENT;
    }
    
    /* Lex the input */
    uint32_t token_count = 0;
    dsl_token_t* tokens = lex_input(expression, &token_count);
    if (!tokens) {
        return NEXUS_ERROR_OUT_OF_MEMORY;
    }
    
    /* Parse tokens into AST */
    ast_node_t* ast = parse_tokens(tokens, token_count);
    if (!ast) {
        /* Free tokens */
        for (uint32_t i = 0; i < token_count; i++) {
            free(tokens[i].value);
        }
        free(tokens);
        return NEXUS_ERROR_PARSE_FAILED;
    }
    
    /* Compile AST to bytecode */
    NexusResult result = compile_ast(ast, bytecode, bytecode_size);
    
    /* Cleanup */
    for (uint32_t i = 0; i < token_count; i++) {
        free(tokens[i].value);
    }
    free(tokens);
    
    /* TODO: Free AST nodes recursively */
    
    return result;
}

void zkwxp_dsl_free(void* bytecode) {
    if (bytecode) {
        free(bytecode);
    }
}
