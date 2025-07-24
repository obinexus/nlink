// nlink/core/tatit/nlink_tatit_traverse.h
// OBINexus Constitutional Header Guard: OBINEXUS_NLINK_CORE_TATIT_NLINK_TATIT_TRAVERSE_H
#ifndef OBINEXUS_NLINK_CORE_TATIT_NLINK_TATIT_TRAVERSE_H
#define OBINEXUS_NLINK_CORE_TATIT_NLINK_TATIT_TRAVERSE_H

/**
 * @file traversal.h
 * @brief Traversal tactics for NexusLink
 * @copyright Copyright © 2025 OBINexus Computing
 *
 * This module implements the traversal tactical pattern,
 * allowing systematic navigation through complex data structures
 * while applying operations.
 */

#ifndef NLINK_TACTIC_TRAVERSAL_H
#define NLINK_TACTIC_TRAVERSAL_H

#include <stddef.h>
#include <stdbool.h>

/**
 * Function pointer types for traversal operations
 */
typedef void* (*nlink_visitor_fn)(void* node, void* context);
typedef void* (*nlink_node_getter_fn)(void* node, size_t index, void* context);
typedef size_t (*nlink_child_count_fn)(void* node, void* context);
typedef bool (*nlink_traversal_control_fn)(void* node, void* context);

/**
 * Traversal order enumeration
 */
typedef enum nlink_traversal_order {
    NLINK_TRAVERSAL_PRE_ORDER,    // Process node before children
    NLINK_TRAVERSAL_POST_ORDER,   // Process node after children
    NLINK_TRAVERSAL_LEVEL_ORDER,  // Process nodes level by level
    NLINK_TRAVERSAL_IN_ORDER      // For binary trees: left, node, right
} nlink_traversal_order;

/**
 * Traversal configuration structure
 */
typedef struct nlink_traversal_config {
    nlink_traversal_order order;          // Traversal order
    nlink_visitor_fn visitor;             // Function to apply to each node
    nlink_node_getter_fn get_child;       // Function to get a child node
    nlink_child_count_fn get_child_count; // Function to get child count
    nlink_traversal_control_fn should_traverse; // Function to determine if traversal should continue
    void* context;                         // Context for traversal functions
    bool collect_results;                 // Whether to collect visitor results
} nlink_traversal_config;

/**
 * Traversal result structure
 */
typedef struct nlink_traversal_result {
    void** nodes;     // Array of visited nodes
    void** results;   // Array of visitor results
    size_t count;     // Number of visited nodes
    size_t capacity;  // Capacity of result arrays
} nlink_traversal_result;

/**
 * @brief Create a new traversal configuration
 *
 * @param order Traversal order
 * @param visitor Visitor function to apply to each node
 * @param get_child Function to get a child node
 * @param get_child_count Function to get the number of children
 * @param context Context for traversal functions
 * @return New traversal configuration
 */
nlink_traversal_config* nlink_traversal_config_create(
    nlink_traversal_order order,
    nlink_visitor_fn visitor,
    nlink_node_getter_fn get_child,
    nlink_child_count_fn get_child_count,
    void* context
);

/**
 * @brief Set traversal control function
 *
 * @param config Traversal configuration
 * @param should_traverse Function that determines if traversal should continue
 */
void nlink_traversal_config_set_control(
    nlink_traversal_config* config,
    nlink_traversal_control_fn should_traverse
);

/**
 * @brief Set result collection option
 *
 * @param config Traversal configuration
 * @param collect_results Whether to collect visitor results
 */
void nlink_traversal_config_set_collection(
    nlink_traversal_config* config,
    bool collect_results
);

/**
 * @brief Free traversal configuration
 *
 * @param config Configuration to free
 */
void nlink_traversal_config_free(nlink_traversal_config* config);

/**
 * @brief Traverse a tree structure
 *
 * @param root Root node of the tree
 * @param config Traversal configuration
 * @return Traversal result structure or NULL if traversal failed
 */
nlink_traversal_result* nlink_traverse_tree(void* root, nlink_traversal_config* config);

/**
 * @brief Free traversal result
 *
 * @param result Result to free
 */
void nlink_traversal_result_free(nlink_traversal_result* result);

/**
 * Specialized traversal functions for common data structures
 */

/**
 * @brief Traverse a binary tree structure
 *
 * @param root Root node of the binary tree
 * @param get_left Function to get left child
 * @param get_right Function to get right child
 * @param visitor Visitor function
 * @param order Traversal order
 * @param context Context for traversal functions
 * @return Traversal result structure
 */
nlink_traversal_result* nlink_traverse_binary_tree(
    void* root,
    void* (*get_left)(void* node, void* context),
    void* (*get_right)(void* node, void* context),
    nlink_visitor_fn visitor,
    nlink_traversal_order order,
    void* context
);

/**
 * @brief Traverse a linked list
 *
 * @param head Head of the linked list
 * @param get_next Function to get next node
 * @param visitor Visitor function
 * @param context Context for traversal functions
 * @return Traversal result structure
 */
nlink_traversal_result* nlink_traverse_linked_list(
    void* head,
    void* (*get_next)(void* node, void* context),
    nlink_visitor_fn visitor,
    void* context
);

/**
 * @brief Traverse a directed graph using depth-first search
 *
 * @param start Starting node
 * @param get_adjacents Function to get adjacent nodes
 * @param get_adjacent_count Function to get adjacent node count
 * @param visitor Visitor function
 * @param context Context for traversal functions
 * @return Traversal result structure
 */
nlink_traversal_result* nlink_traverse_graph_dfs(
    void* start,
    nlink_node_getter_fn get_adjacents,
    nlink_child_count_fn get_adjacent_count,
    nlink_visitor_fn visitor,
    void* context
);

/**
 * @brief Traverse a directed graph using breadth-first search
 *
 * @param start Starting node
 * @param get_adjacents Function to get adjacent nodes
 * @param get_adjacent_count Function to get adjacent node count
 * @param visitor Visitor function
 * @param context Context for traversal functions
 * @return Traversal result structure
 */
nlink_traversal_result* nlink_traverse_graph_bfs(
    void* start,
    nlink_node_getter_fn get_adjacents,
    nlink_child_count_fn get_adjacent_count,
    nlink_visitor_fn visitor,
    void* context
);

/**
 * @brief Check if a traversal has visited a specific node
 *
 * @param result Traversal result
 * @param node Node to check
 * @return true if node was visited, false otherwise
 */
bool nlink_traversal_visited(nlink_traversal_result* result, void* node);

/**
 * @brief Get the result of visiting a specific node
 *
 * @param result Traversal result
 * @param node Node to get result for
 * @return Result of visiting the node, or NULL if not found
 */
void* nlink_traversal_get_result(nlink_traversal_result* result, void* node);


#endif // OBINEXUS_NLINK_CORE_TATIT_NLINK_TATIT_TRAVERSE_H
