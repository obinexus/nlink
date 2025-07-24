/**
 * @file parser.c
 * @brief Implementation of the NexusLink parsing system
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include <nlink/core/parser/nlink_parser_core.h"
#include <stdlib.h>
#include <string.h>

nlink_parser_context* nlink_parser_create(nlink_token** tokens, nlink_parser_config* config) {
    if (tokens == NULL) {
        return NULL;
    }
    
    nlink_parser_context* context = malloc(sizeof(nlink_parser_context));
    if (context == NULL) {
        return NULL;
    }
    
    context->tokens = tokens;
    context->position = 0;
    context->root = NULL;
    context->state = NULL;
    
    return context;
}

void nlink_parser_free(nlink_parser_context* context) {
    if (context == NULL) {
        return;
    }
    
    // Free any allocated state
    free(context->state);
    
    // Note: We don't free tokens or AST here, they should be managed separately
    
    // Free the context itself
    free(context);
}

nlink_ast_node* nlink_parser_parse(nlink_parser_context* context) {
    // Simplified implementation - real parser would be more complex
    if (context == NULL || context->tokens == NULL) {
        return NULL;
    }
    
    // Create a program root node
    nlink_ast_node* root = nlink_ast_node_create(NLINK_NODE_PROGRAM, "program", NULL);
    if (root == NULL) {
        return NULL;
    }
    
    context->root = root;
    
    // Apply parser rules if available
    if (context->position == 0 && context->tokens[0] != NULL) {
        // Very simplified parsing logic for demonstration
        // Process each token and create a corresponding AST node
        while (context->tokens[context->position] != NULL && 
               context->tokens[context->position]->type != NLINK_TOKEN_EOF) {
            
            nlink_token* token = context->tokens[context->position];
            nlink_ast_node* node = NULL;
            
            // Basic token-to-node conversion
            switch (token->type) {
                case NLINK_TOKEN_IDENTIFIER:
                    node = nlink_ast_node_create(NLINK_NODE_IDENTIFIER, token->value, token);
                    break;
                case NLINK_TOKEN_KEYWORD:
                    // Placeholder - real parser would have specific handling based on keyword
                    node = nlink_ast_node_create(NLINK_NODE_STATEMENT, token->value, token);
                    break;
                case NLINK_TOKEN_OPERATOR:
                    node = nlink_ast_node_create(NLINK_NODE_OPERATOR, token->value, token);
                    break;
                case NLINK_TOKEN_LITERAL:
                    node = nlink_ast_node_create(NLINK_NODE_LITERAL, token->value, token);
                    break;
                default:
                    // Skip other token types for this simplified example
                    break;
            }
            
            if (node != NULL) {
                nlink_ast_node_add_child(root, node);
            }
            
            context->position++;
        }
    }
    
    return root;
}

nlink_ast_node* nlink_ast_node_create(nlink_node_type type, const char* value, nlink_token* token) {
    nlink_ast_node* node = malloc(sizeof(nlink_ast_node));
    if (node == NULL) {
        return NULL;
    }
    
    node->type = type;
    node->parent = NULL;
    node->children = NULL;
    node->child_count = 0;
    node->token = token; // We don't copy the token, just reference it
    node->metadata = NULL;
    
    if (value != NULL) {
        node->value = strdup(value);
        if (node->value == NULL) {
            free(node);
            return NULL;
        }
    } else {
        node->value = NULL;
    }
    
    return node;
}

bool nlink_ast_node_add_child(nlink_ast_node* parent, nlink_ast_node* child) {
    if (parent == NULL || child == NULL) {
        return false;
    }
    
    // Resize children array
    nlink_ast_node** new_children = realloc(parent->children, 
                                         (parent->child_count + 1) * sizeof(nlink_ast_node*));
    if (new_children == NULL) {
        return false;
    }
    
    parent->children = new_children;
    parent->children[parent->child_count++] = child;
    child->parent = parent;
    
    return true;
}

void nlink_ast_node_free(nlink_ast_node* node) {
    if (node == NULL) {
        return;
    }
    
    // Free all children first
    for (size_t i = 0; i < node->child_count; i++) {
        nlink_ast_node_free(node->children[i]);
    }
    
    // Free child array
    free(node->children);
    
    // Free node value
    free(node->value);
    
    // Free metadata if any
    free(node->metadata);
    
    // Free the node itself
    free(node);
}

nlink_ast_node* nlink_ast_node_transform(nlink_ast_node* node, 
                                      nlink_transform_fn transform, 
                                      void* context) {
    if (node == NULL || transform == NULL) {
        return node;
    }
    
    return (nlink_ast_node*)transform(node, context);
}

void nlink_ast_traverse(nlink_ast_node* root, nlink_consumer_fn visit, void* context) {
    if (root == NULL || visit == NULL) {
        return;
    }
    
    // Visit this node
    visit(root, context);
    
    // Visit all children
    for (size_t i = 0; i < root->child_count; i++) {
        nlink_ast_traverse(root->children[i], visit, context);
    }
}
