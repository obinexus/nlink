/**
 * @file traversal.c
 * @brief Implementation of traversal tactics
 * @copyright Copyright © 2025 OBINexus Computing
 */

#include <nlink/core/tactic/traversal.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

/**
 * Queue implementation for breadth-first traversal
 */
typedef struct node_queue_item {
    void* node;
    struct node_queue_item* next;
} node_queue_item;

typedef struct node_queue {
    node_queue_item* head;
    node_queue_item* tail;
    size_t size;
} node_queue;

static node_queue* queue_create() {
    node_queue* queue = malloc(sizeof(node_queue));
    if (queue == NULL) {
        return NULL;
    }
    
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    
    return queue;
}

static void queue_free(node_queue* queue) {
    if (queue == NULL) {
        return;
    }
    
    // Free all items
    node_queue_item* item = queue->head;
    while (item != NULL) {
        node_queue_item* next = item->next;
        free(item);
        item = next;
    }
    
    free(queue);
}

static bool queue_enqueue(node_queue* queue, void* node) {
    if (queue == NULL) {
        return false;
    }
    
    node_queue_item* item = malloc(sizeof(node_queue_item));
    if (item == NULL) {
        return false;
    }
    
    item->node = node;
    item->next = NULL;
    
    if (queue->tail == NULL) {
        // Empty queue
        queue->head = item;
        queue->tail = item;
    } else {
        // Add to end
        queue->tail->next = item;
        queue->tail = item;
    }
    
    queue->size++;
    
    return true;
}

static void* queue_dequeue(node_queue* queue) {
    if (queue == NULL || queue->head == NULL) {
        return NULL;
    }
    
    node_queue_item* item = queue->head;
    void* node = item->node;
    
    queue->head = item->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }
    
    free(item);
    queue->size--;
    
    return node;
}

static bool queue_is_empty(node_queue* queue) {
    return queue == NULL || queue->head == NULL;
}

/**
 * Stack implementation for depth-first traversal
 */
typedef struct node_stack_item {
    void* node;
    struct node_stack_item* next;
} node_stack_item;

typedef struct node_stack {
    node_stack_item* top;
    size_t size;
} node_stack;

static node_stack* stack_create() {
    node_stack* stack = malloc(sizeof(node_stack));
    if (stack == NULL) {
        return NULL;
    }
    
    stack->top = NULL;
    stack->size = 0;
    
    return stack;
}

static void stack_free(node_stack* stack) {
    if (stack == NULL) {
        return;
    }
    
    // Free all items
    node_stack_item* item = stack->top;
    while (item != NULL) {
        node_stack_item* next = item->next;
        free(item);
        item = next;
    }
    
    free(stack);
}

static bool stack_push(node_stack* stack, void* node) {
    if (stack == NULL) {
        return false;
    }
    
    node_stack_item* item = malloc(sizeof(node_stack_item));
    if (item == NULL) {
        return false;
    }
    
    item->node = node;
    item->next = stack->top;
    stack->top = item;
    stack->size++;
    
    return true;
}

static void* stack_pop(node_stack* stack) {
    if (stack == NULL || stack->top == NULL) {
        return NULL;
    }
    
    node_stack_item* item = stack->top;
    void* node = item->node;
    
    stack->top = item->next;
    free(item);
    stack->size--;
    
    return node;
}

static bool stack_is_empty(node_stack* stack) {
    return stack == NULL || stack->top == NULL;
}

/**
 * Visited node tracking
 */
typedef struct {
    void** nodes;
    size_t count;
    size_t capacity;
} visited_set;

static visited_set* visited_set_create(size_t initial_capacity) {
    visited_set* set = malloc(sizeof(visited_set));
    if (set == NULL) {
        return NULL;
    }
    
    set->nodes = malloc(initial_capacity * sizeof(void*));
    if (set->nodes == NULL) {
        free(set);
        return NULL;
    }
    
    set->count = 0;
    set->capacity = initial_capacity;
    
    return set;
}

static void visited_set_free(visited_set* set) {
    if (set == NULL) {
        return;
    }
    
    free(set->nodes);
    free(set);
}

static bool visited_set_contains(visited_set* set, void* node) {
    if (set == NULL || node == NULL) {
        return false;
    }
    
    for (size_t i = 0; i < set->count; i++) {
        if (set->nodes[i] == node) {
            return true;
        }
    }
    
    return false;
}

static bool visited_set_add(visited_set* set, void* node) {
    if (set == NULL || node == NULL) {
        return false;
    }
    
    // Check if already in set
    if (visited_set_contains(set, node)) {
        return true;
    }
    
    // Check if need to resize
    if (set->count >= set->capacity) {
        size_t new_capacity = set->capacity * 2;
        void** new_nodes = realloc(set->nodes, new_capacity * sizeof(void*));
        if (new_nodes == NULL) {
            return false;
        }
        
        set->nodes = new_nodes;
        set->capacity = new_capacity;
    }
    
    // Add to set
    set->nodes[set->count++] = node;
    
    return true;
}

/**
 * Traversal result implementation
 */
static nlink_traversal_result* traversal_result_create(size_t initial_capacity) {
    nlink_traversal_result* result = malloc(sizeof(nlink_traversal_result));
    if (result == NULL) {
        return NULL;
    }
    
    result->nodes = malloc(initial_capacity * sizeof(void*));
    result->results = malloc(initial_capacity * sizeof(void*));
    
    if (result->nodes == NULL || result->results == NULL) {
        free(result->nodes);
        free(result->results);
        free(result);
        return NULL;
    }
    
    result->count = 0;
    result->capacity = initial_capacity;
    
    return result;
}

static bool traversal_result_add(nlink_traversal_result* result, void* node, void* value) {
    if (result == NULL) {
        return false;
    }
    
    // Check if need to resize
    if (result->count >= result->capacity) {
        size_t new_capacity = result->capacity * 2;
        void** new_nodes = realloc(result->nodes, new_capacity * sizeof(void*));
        void** new_results = realloc(result->results, new_capacity * sizeof(void*));
        
        if (new_nodes == NULL || new_results == NULL) {
            free(new_nodes);  // May be NULL, free handles that
            free(new_results);
            return false;
        }
        
        result->nodes = new_nodes;
        result->results = new_results;
        result->capacity = new_capacity;
    }
    
    // Add node and result
    result->nodes[result->count] = node;
    result->results[result->count] = value;
    result->count++;
    
    return true;
}

nlink_traversal_config* nlink_traversal_config_create(
    nlink_traversal_order order,
    nlink_visitor_fn visitor,
    nlink_node_getter_fn get_child,
    nlink_child_count_fn get_child_count,
    void* context
) {
    if (visitor == NULL || get_child == NULL || get_child_count == NULL) {
        return NULL;
    }
    
    nlink_traversal_config* config = malloc(sizeof(nlink_traversal_config));
    if (config == NULL) {
        return NULL;
    }
    
    config->order = order;
    config->visitor = visitor;
    config->get_child = get_child;
    config->get_child_count = get_child_count;
    config->should_traverse = NULL;  // Default: traverse all nodes
    config->context = context;
    config->collect_results = true;  // Default: collect results
    
    return config;
}

void nlink_traversal_config_set_control(
    nlink_traversal_config* config,
    nlink_traversal_control_fn should_traverse
) {
    if (config == NULL) {
        return;
    }
    
    config->should_traverse = should_traverse;
}

void nlink_traversal_config_set_collection(
    nlink_traversal_config* config,
    bool collect_results
) {
    if (config == NULL) {
        return;
    }
    
    config->collect_results = collect_results;
}

void nlink_traversal_config_free(nlink_traversal_config* config) {
    free(config);
}

void nlink_traversal_result_free(nlink_traversal_result* result) {
    if (result == NULL) {
        return;
    }
    
    free(result->nodes);
    free(result->results);
    free(result);
}

/**
 * Pre-order traversal implementation
 */
static nlink_traversal_result* traverse_pre_order(
    void* root,
    nlink_traversal_config* config
) {
    if (root == NULL || config == NULL) {
        return NULL;
    }
    
    // Initialize result
    nlink_traversal_result* result = traversal_result_create(16);
    if (result == NULL) {
        return NULL;
    }
    
    // Initialize stack
    node_stack* stack = stack_create();
    if (stack == NULL) {
        nlink_traversal_result_free(result);
        return NULL;
    }
    
    // Push root node
    stack_push(stack, root);
    
    // Traverse tree
    while (!stack_is_empty(stack)) {
        void* node = stack_pop(stack);
        
        // Check if we should traverse this node
        if (config->should_traverse != NULL && !config->should_traverse(node, config->context)) {
            continue;
        }
        
        // Visit node
        void* visit_result = config->visitor(node, config->context);
        
        // Collect result if requested
        if (config->collect_results) {
            if (!traversal_result_add(result, node, visit_result)) {
                stack_free(stack);
                nlink_traversal_result_free(result);
                return NULL;
            }
        }
        
        // Push children in reverse order (so they get processed in correct order)
        size_t child_count = config->get_child_count(node, config->context);
        for (size_t i = child_count; i > 0; i--) {
            void* child = config->get_child(node, i - 1, config->context);
            if (child != NULL) {
                stack_push(stack, child);
            }
        }
    }
    
    stack_free(stack);
    return result;
}

/**
 * Post-order traversal implementation
 */
static nlink_traversal_result* traverse_post_order(
    void* root,
    nlink_traversal_config* config
) {
    if (root == NULL || config == NULL) {
        return NULL;
    }
    
    // Initialize result
    nlink_traversal_result* result = traversal_result_create(16);
    if (result == NULL) {
        return NULL;
    }
    
    // We need two stacks for post-order traversal
    node_stack* stack1 = stack_create();
    node_stack* stack2 = stack_create();
    
    if (stack1 == NULL || stack2 == NULL) {
        nlink_traversal_result_free(result);
        stack_free(stack1);
        stack_free(stack2);
        return NULL;
    }
    
    // Push root node
    stack_push(stack1, root);
    
    // Fill stack2 in post-order
    while (!stack_is_empty(stack1)) {
        void* node = stack_pop(stack1);
        
        // Check if we should traverse this node
        if (config->should_traverse != NULL && !config->should_traverse(node, config->context)) {
            continue;
        }
        
        stack_push(stack2, node);
        
        // Push children for processing
        size_t child_count = config->get_child_count(node, config->context);
        for (size_t i = 0; i < child_count; i++) {
            void* child = config->get_child(node, i, config->context);
            if (child != NULL) {
                stack_push(stack1, child);
            }
        }
    }
    
    // Process nodes in post-order
    while (!stack_is_empty(stack2)) {
        void* node = stack_pop(stack2);
        
        // Visit node
        void* visit_result = config->visitor(node, config->context);
        
        // Collect result if requested
        if (config->collect_results) {
            if (!traversal_result_add(result, node, visit_result)) {
                stack_free(stack1);
                stack_free(stack2);
                nlink_traversal_result_free(result);
                return NULL;
            }
        }
    }
    
    stack_free(stack1);
    stack_free(stack2);
    return result;
}

/**
 * Level-order (breadth-first) traversal implementation
 */
static nlink_traversal_result* traverse_level_order(
    void* root,
    nlink_traversal_config* config
) {
    if (root == NULL || config == NULL) {
        return NULL;
    }
    
    // Initialize result
    nlink_traversal_result* result = traversal_result_create(16);
    if (result == NULL) {
        return NULL;
    }
    
    // Initialize queue
    node_queue* queue = queue_create();
    if (queue == NULL) {
        nlink_traversal_result_free(result);
        return NULL;
    }
    
    // Enqueue root node
    queue_enqueue(queue, root);
    
    // Traverse tree
    while (!queue_is_empty(queue)) {
        void* node = queue_dequeue(queue);
        
        // Check if we should traverse this node
        if (config->should_traverse != NULL && !config->should_traverse(node, config->context)) {
            continue;
        }
        
        // Visit node
        void* visit_result = config->visitor(node, config->context);
        
        // Collect result if requested
        if (config->collect_results) {
            if (!traversal_result_add(result, node, visit_result)) {
                queue_free(queue);
                nlink_traversal_result_free(result);
                return NULL;
            }
        }
        
        // Enqueue children
        size_t child_count = config->get_child_count(node, config->context);
        for (size_t i = 0; i < child_count; i++) {
            void* child = config->get_child(node, i, config->context);
            if (child != NULL) {
                queue_enqueue(queue, child);
            }
        }
    }
    
    queue_free(queue);
    return result;
}

/**
 * In-order traversal implementation (for binary trees)
 */
static nlink_traversal_result* traverse_in_order(
    void* root,
    nlink_traversal_config* config
) {
    if (root == NULL || config == NULL) {
        return NULL;
    }
    
    // Initialize result
    nlink_traversal_result* result = traversal_result_create(16);
    if (result == NULL) {
        return NULL;
    }
    
    // Initialize stack
    node_stack* stack = stack_create();
    if (stack == NULL) {
        nlink_traversal_result_free(result);
        return NULL;
    }
    
    // Traverse tree
    void* current = root;
    
    while (current != NULL || !stack_is_empty(stack)) {
        // Reach the leftmost node
        while (current != NULL) {
            // Check if we should traverse this node
            if (config->should_traverse != NULL && !config->should_traverse(current, config->context)) {
                current = NULL;
                break;
            }
            
            stack_push(stack, current);
            
            // For binary trees, we always expect left child at index 0
            if (config->get_child_count(current, config->context) > 0) {
                current = config->get_child(current, 0, config->context);
            } else {
                current = NULL;
            }
        }
        
        if (stack_is_empty(stack)) {
            break;
        }
        
        current = stack_pop(stack);
        
        // Visit node
        void* visit_result = config->visitor(current, config->context);
        
        // Collect result if requested
        if (config->collect_results) {
            if (!traversal_result_add(result, current, visit_result)) {
                stack_free(stack);
                nlink_traversal_result_free(result);
                return NULL;
            }
        }
        
        // Move to right child
        // For binary trees, we always expect right child at index 1
        if (config->get_child_count(current, config->context) > 1) {
            current = config->get_child(current, 1, config->context);
        } else {
            current = NULL;
        }
    }
    
    stack_free(stack);
    return result;
}

nlink_traversal_result* nlink_traverse_tree(void* root, nlink_traversal_config* config) {
    if (root == NULL || config == NULL) {
        return NULL;
    }
    
    // Choose traversal implementation based on order
    switch (config->order) {
        case NLINK_TRAVERSAL_PRE_ORDER:
            return traverse_pre_order(root, config);
            
        case NLINK_TRAVERSAL_POST_ORDER:
            return traverse_post_order(root, config);
            
        case NLINK_TRAVERSAL_LEVEL_ORDER:
            return traverse_level_order(root, config);
            
        case NLINK_TRAVERSAL_IN_ORDER:
            return traverse_in_order(root, config);
            
        default:
            // Invalid order
            return NULL;
    }
}

/**
 * Binary tree traversal helper functions
 */
static void* binary_tree_get_child(void* node, size_t index, void* context) {
    if (node == NULL || context == NULL) {
        return NULL;
    }
    
    // Context is a struct with get_left and get_right functions
    struct {
        void* (*get_left)(void* node, void* ctx);
        void* (*get_right)(void* node, void* ctx);
        void* ctx;
    } *btree_ctx = context;
    
    if (index == 0) {
        return btree_ctx->get_left(node, btree_ctx->ctx);
    } else if (index == 1) {
        return btree_ctx->get_right(node, btree_ctx->ctx);
    } else {
        return NULL;
    }
}

static size_t binary_tree_get_child_count(void* node, void* context) {
    if (node == NULL || context == NULL) {
        return 0;
    }
    
    // Context is a struct with get_left and get_right functions
    struct {
        void* (*get_left)(void* node, void* ctx);
        void* (*get_right)(void* node, void* ctx);
        void* ctx;
    } *btree_ctx = context;
    
    size_t count = 0;
    
    if (btree_ctx->get_left(node, btree_ctx->ctx) != NULL) {
        count++;
    }
    
    if (btree_ctx->get_right(node, btree_ctx->ctx) != NULL) {
        count++;
    }
    
    return count;
}

nlink_traversal_result* nlink_traverse_binary_tree(
    void* root,
    void* (*get_left)(void* node, void* context),
    void* (*get_right)(void* node, void* context),
    nlink_visitor_fn visitor,
    nlink_traversal_order order,
    void* context
) {
    if (root == NULL || get_left == NULL || get_right == NULL || visitor == NULL) {
        return NULL;
    }
    
    // Create context for binary tree traversal
    struct {
        void* (*get_left)(void* node, void* ctx);
        void* (*get_right)(void* node, void* ctx);
        void* ctx;
    } *btree_ctx = malloc(sizeof(*btree_ctx));
    
    if (btree_ctx == NULL) {
        return NULL;
    }
    
    btree_ctx->get_left = get_left;
    btree_ctx->get_right = get_right;
    btree_ctx->ctx = context;
    
    // Create traversal configuration
    nlink_traversal_config* config = nlink_traversal_config_create(
        order,
        visitor,
        binary_tree_get_child,
        binary_tree_get_child_count,
        btree_ctx
    );
    
    if (config == NULL) {
        free(btree_ctx);
        return NULL;
    }
    
    // Traverse tree
    nlink_traversal_result* result = nlink_traverse_tree(root, config);
    
    // Clean up
    free(btree_ctx);
    nlink_traversal_config_free(config);
    
    return result;
}

/**
 * Linked list traversal helper functions
 */
static void* linked_list_get_child(void* node, size_t index, void* context) {
    if (node == NULL || context == NULL || index > 0) {
        return NULL;
    }
    
    // Context is a struct with get_next function
    struct {
        void* (*get_next)(void* node, void* ctx);
        void* ctx;
    } *list_ctx = context;
    
    return list_ctx->get_next(node, list_ctx->ctx);
}

static size_t linked_list_get_child_count(void* node, void* context) {
    if (node == NULL || context == NULL) {
        return 0;
    }
    
    // Context is a struct with get_next function
    struct {
        void* (*get_next)(void* node, void* ctx);
        void* ctx;
    } *list_ctx = context;
    
    return list_ctx->get_next(node, list_ctx->ctx) != NULL ? 1 : 0;
}

nlink_traversal_result* nlink_traverse_linked_list(
    void* head,
    void* (*get_next)(void* node, void* context),
    nlink_visitor_fn visitor,
    void* context
) {
    if (head == NULL || get_next == NULL || visitor == NULL) {
        return NULL;
    }
    
    // Create context for linked list traversal
    struct {
        void* (*get_next)(void* node, void* ctx);
        void* ctx;
    } *list_ctx = malloc(sizeof(*list_ctx));
    
    if (list_ctx == NULL) {
        return NULL;
    }
    
    list_ctx->get_next = get_next;
    list_ctx->ctx = context;
    
    // Create traversal configuration
    nlink_traversal_config* config = nlink_traversal_config_create(
        NLINK_TRAVERSAL_PRE_ORDER,  // For a linked list, order doesn't matter
        visitor,
        linked_list_get_child,
        linked_list_get_child_count,
        list_ctx
    );
    
    if (config == NULL) {
        free(list_ctx);
        return NULL;
    }
    
    // Traverse list
    nlink_traversal_result* result = nlink_traverse_tree(head, config);
    
    // Clean up
    free(list_ctx);
    nlink_traversal_config_free(config);
    
    return result;
}

/**
 * Graph traversal
 */
nlink_traversal_result* nlink_traverse_graph_dfs(
    void* start,
    nlink_node_getter_fn get_adjacents,
    nlink_child_count_fn get_adjacent_count,
    nlink_visitor_fn visitor,
    void* context
) {
    if (start == NULL || get_adjacents == NULL || get_adjacent_count == NULL || visitor == NULL) {
        return NULL;
    }
    
    // Initialize result
    nlink_traversal_result* result = traversal_result_create(16);
    if (result == NULL) {
        return NULL;
    }
    
    // Initialize stack and visited set
    node_stack* stack = stack_create();
    visited_set* visited = visited_set_create(16);
    
    if (stack == NULL || visited == NULL) {
        nlink_traversal_result_free(result);
        stack_free(stack);
        visited_set_free(visited);
        return NULL;
    }
    
    // Push start node
    stack_push(stack, start);
    visited_set_add(visited, start);
    
    // Traverse graph
    while (!stack_is_empty(stack)) {
        void* node = stack_pop(stack);
        
        // Visit node
        void* visit_result = visitor(node, context);
        
        // Collect result
        if (!traversal_result_add(result, node, visit_result)) {
            stack_free(stack);
            visited_set_free(visited);
            nlink_traversal_result_free(result);
            return NULL;
        }
        
        // Push unvisited adjacent nodes
        size_t adjacent_count = get_adjacent_count(node, context);
        for (size_t i = 0; i < adjacent_count; i++) {
            void* adjacent = get_adjacents(node, i, context);
            if (adjacent != NULL && !visited_set_contains(visited, adjacent)) {
                stack_push(stack, adjacent);
                visited_set_add(visited, adjacent);
            }
        }
    }
    
    stack_free(stack);
    visited_set_free(visited);
    return result;
}

nlink_traversal_result* nlink_traverse_graph_bfs(
    void* start,
    nlink_node_getter_fn get_adjacents,
    nlink_child_count_fn get_adjacent_count,
    nlink_visitor_fn visitor,
    void* context
) {
    if (start == NULL || get_adjacents == NULL || get_adjacent_count == NULL || visitor == NULL) {
        return NULL;
    }
    
    // Initialize result
    nlink_traversal_result* result = traversal_result_create(16);
    if (result == NULL) {
        return NULL;
    }
    
    // Initialize queue and visited set
    node_queue* queue = queue_create();
    visited_set* visited = visited_set_create(16);
    
    if (queue == NULL || visited == NULL) {
        nlink_traversal_result_free(result);
        queue_free(queue);
        visited_set_free(visited);
        return NULL;
    }
    
    // Enqueue start node
    queue_enqueue(queue, start);
    visited_set_add(visited, start);
    
    // Traverse graph
    while (!queue_is_empty(queue)) {
        void* node = queue_dequeue(queue);
        
        // Visit node
        void* visit_result = visitor(node, context);
        
        // Collect result
        if (!traversal_result_add(result, node, visit_result)) {
            queue_free(queue);
            visited_set_free(visited);
            nlink_traversal_result_free(result);
            return NULL;
        }
        
        // Enqueue unvisited adjacent nodes
        size_t adjacent_count = get_adjacent_count(node, context);
        for (size_t i = 0; i < adjacent_count; i++) {
            void* adjacent = get_adjacents(node, i, context);
            if (adjacent != NULL && !visited_set_contains(visited, adjacent)) {
                queue_enqueue(queue, adjacent);
                visited_set_add(visited, adjacent);
            }
        }
    }
    
    queue_free(queue);
    visited_set_free(visited);
    return result;
}

bool nlink_traversal_visited(nlink_traversal_result* result, void* node) {
    if (result == NULL || node == NULL) {
        return false;
    }
    
    for (size_t i = 0; i < result->count; i++) {
        if (result->nodes[i] == node) {
            return true;
        }
    }
    
    return false;
}

void* nlink_traversal_get_result(nlink_traversal_result* result, void* node) {
    if (result == NULL || node == NULL) {
        return NULL;
    }
    
    for (size_t i = 0; i < result->count; i++) {
        if (result->nodes[i] == node) {
            return result->results[i];
        }
    }
    
    return NULL;
}