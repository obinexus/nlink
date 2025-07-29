/**
 * @file parser.h
 * @brief Parsing system for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 * 
 * This module provides parsing operations based on tactical programming patterns,
 * converting token streams into abstract syntax trees.
 */

#ifndef NLINK_PARSER_H
#define NLINK_PARSER_H

#include <stddef.h>
#include <stdbool.h>
#include "../tactic/tactic.h"
#include "../type/type.h"
#include "../tokenizer/tokenizer.h"

/**
 * Node type for abstract syntax trees
 */
typedef enum {
    NLINK_NODE_PROGRAM,
    NLINK_NODE_DECLARATION,
    NLINK_NODE_EXPRESSION,
    NLINK_NODE_STATEMENT,
    NLINK_NODE_IDENTIFIER,
    NLINK_NODE_LITERAL,
    NLINK_NODE_OPERATOR,
    NLINK_NODE_FUNCTION,
    NLINK_NODE_BLOCK
} nlink_node_type;

/**
 * AST node structure
 */
typedef struct nlink_ast_node {
    nlink_node_type type;
    char* value;
    nlink_token* token;
    struct nlink_ast_node* parent;
    struct nlink_ast_node** children;
    size_t child_count;
    void* metadata;
} nlink_ast_node;

/**
 * Parser context structure
 */
typedef struct {
    nlink_token** tokens;
    size_t position;
    nlink_ast_node* root;
    void* state;
} nlink_parser_context;

/**
 * Parser rule function type
 * @param context Parser context
 * @return AST node created by the rule
 */
typedef nlink_ast_node* (*nlink_parser_rule_fn)(nlink_parser_context* context);

/**
 * Parser configuration
 */
typedef struct {
    nlink_parser_rule_fn* rules;
    size_t rule_count;
    void* custom_context;
} nlink_parser_config;

/**
 * Create a new parser context
 * @param tokens Token array
 * @param config Parser configuration
 * @return Parser context
 */
nlink_parser_context* nlink_parser_create(nlink_token** tokens, nlink_parser_config* config);

/**
 * Free parser context
 * @param context Parser context to free
 */
void nlink_parser_free(nlink_parser_context* context);

/**
 * Parse tokens into an AST
 * @param context Parser context
 * @return Root AST node
 */
nlink_ast_node* nlink_parser_parse(nlink_parser_context* context);

/**
 * Create an AST node
 * @param type Node type
 * @param value Node value
 * @param token Source token
 * @return New AST node
 */
nlink_ast_node* nlink_ast_node_create(nlink_node_type type, const char* value, nlink_token* token);

/**
 * Add a child node to a parent node
 * @param parent Parent node
 * @param child Child node
 * @return true if successful
 */
bool nlink_ast_node_add_child(nlink_ast_node* parent, nlink_ast_node* child);

/**
 * Free an AST node and all its children
 * @param node Node to free
 */
void nlink_ast_node_free(nlink_ast_node* node);

/**
 * Apply a transformation tactic to an AST node
 * @param node Node to transform
 * @param transform Transformation function
 * @param context Transformation context
 * @return Transformed node
 */
nlink_ast_node* nlink_ast_node_transform(nlink_ast_node* node, 
                                        nlink_transform_fn transform, 
                                        void* context);

/**
 * Traverse an AST with a visitor function
 * @param root Root node
 * @param visit Visitor function
 * @param context Visitor context
 */
void nlink_ast_traverse(nlink_ast_node* root, nlink_consumer_fn visit, void* context);

#endif /* NLINK_PARSER_H */
