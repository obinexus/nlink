// okpala_ast.c - AST optimization implementation for NexusLink
// Author: Nnamdi Michael Okpala
#include "nlink/core/minimizer/okpala_ast.h"

// Create a new AST
OkpalaAST* okpala_ast_create(void) {
    OkpalaAST* ast = (OkpalaAST*)malloc(sizeof(OkpalaAST));
    ast->root = (OkpalaNode*)malloc(sizeof(OkpalaNode));
    ast->root->value = strdup("root");
    ast->root->children = NULL;
    ast->root->child_count = 0;
    ast->root->parent = NULL;
    ast->node_count = 1;
    return ast;
}

// Add a node to the AST
NexusResult okpala_ast_add_node(OkpalaAST* ast, OkpalaNode* parent, 
                              const char* value) {
    if (!ast || !parent || !value) {
        return NEXUS_ERROR_INVALID_ARGUMENT;
    }
    
    // Create the new node
    OkpalaNode* node = (OkpalaNode*)malloc(sizeof(OkpalaNode));
    if (!node) {
        return NEXUS_ERROR_OUT_OF_MEMORY;
    }
    
    node->value = strdup(value);
    node->children = NULL;
    node->child_count = 0;
    node->parent = parent;
    
    // Add the node to the parent's children
    parent->children = (OkpalaNode**)realloc(parent->children, 
                                          (parent->child_count + 1) * sizeof(OkpalaNode*));
    if (!parent->children) {
        free(node->value);
        free(node);
        return NEXUS_ERROR_OUT_OF_MEMORY;
    }
    
    parent->children[parent->child_count++] = node;
    ast->node_count++;
    
    return NEXUS_SUCCESS;
}

// Check if a node is redundant
static bool is_node_redundant(OkpalaNode* node, bool use_boolean_reduction) {
    if (!node) return false;
    
    // A node is redundant if it has exactly one child and no meaningful value
    if (node->child_count == 1 && (strcmp(node->value, "") == 0 || 
                                  strcmp(node->value, "pass") == 0)) {
        return true;
    }
    
    // If boolean reduction is enabled, apply additional rules
    if (use_boolean_reduction) {
        // For example, consider nodes with identical children as redundant
        if (node->child_count >= 2) {
            bool all_same = true;
            for (size_t i = 1; i < node->child_count; i++) {
                if (strcmp(node->children[0]->value, node->children[i]->value) != 0) {
                    all_same = false;
                    break;
                }
            }
            if (all_same) {
                return true;
            }
        }
    }
    
    return false;
}

// Replace a node with its child
static void replace_with_child(OkpalaAST* ast, OkpalaNode* node) {
    if (!node || node->child_count != 1) return;
    
    OkpalaNode* child = node->children[0];
    OkpalaNode* parent = node->parent;
    
    // Update the child's parent
    child->parent = parent;
    
    // Replace the node in its parent's children
    if (parent) {
        for (size_t i = 0; i < parent->child_count; i++) {
            if (parent->children[i] == node) {
                parent->children[i] = child;
                break;
            }
        }
    } else {
        // This is the root node
        ast->root = child;
    }
    
    // Free the node
    free(node->value);
    free(node->children);
    free(node);
    
    // Decrease node count
    ast->node_count--;
}

// Create a deep copy of a node
static OkpalaNode* deep_copy_node(OkpalaNode* node, OkpalaNode* parent) {
    if (!node) return NULL;
    
    OkpalaNode* copy = (OkpalaNode*)malloc(sizeof(OkpalaNode));
    copy->value = strdup(node->value);
    copy->parent = parent;
    copy->child_count = node->child_count;
    
    if (node->child_count > 0) {
        copy->children = (OkpalaNode**)malloc(node->child_count * sizeof(OkpalaNode*));
        for (size_t i = 0; i < node->child_count; i++) {
            copy->children[i] = deep_copy_node(node->children[i], copy);
        }
    } else {
        copy->children = NULL;
    }
    
    return copy;
}

// Optimize the AST
OkpalaAST* okpala_optimize_ast(OkpalaAST* ast, bool use_boolean_reduction) {
    if (!ast) return NULL;
    
    // Create a copy of the AST to avoid modifying the original
    OkpalaAST* optimized = (OkpalaAST*)malloc(sizeof(OkpalaAST));
    optimized->root = deep_copy_node(ast->root, NULL);
    optimized->node_count = ast->node_count;
    
    // Traverse the AST and optimize it
    bool changed;
    do {
        changed = false;
        
        // Use a pre-order traversal to optimize the AST
        OkpalaNode** stack = (OkpalaNode**)malloc(optimized->node_count * sizeof(OkpalaNode*));
        size_t stack_size = 0;
        
        stack[stack_size++] = optimized->root;
        
        while (stack_size > 0) {
            OkpalaNode* node = stack[--stack_size];
            
            if (is_node_redundant(node, use_boolean_reduction)) {
                replace_with_child(optimized, node);
                changed = true;
                break;  // Start over as the tree structure has changed
            }
            
            // Push children onto the stack
            for (int i = node->child_count - 1; i >= 0; i--) {
                stack[stack_size++] = node->children[i];
            }
        }
        
        free(stack);
    } while (changed);
    
    return optimized;
}

// Free an AST node
static void free_node(OkpalaNode* node) {
    if (!node) return;
    
    for (size_t i = 0; i < node->child_count; i++) {
        free_node(node->children[i]);
    }
    
    free(node->value);
    free(node->children);
    free(node);
}

// Free an AST
void okpala_ast_free(OkpalaAST* ast) {
    if (!ast) return;
    
    free_node(ast->root);
    free(ast);
}
