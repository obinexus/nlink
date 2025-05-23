/**
 * @file mps_dependency.h
 * @brief Bidirectional dependency resolution for multi-pass systems
 *
 * Defines structures and functions for resolving dependencies between
 * components in a multi-pass pipeline system.
 *
 * Copyright © 2025 OBINexus Computing
 */

#ifndef NLINK_MPS_DEPENDENCY_H
#define NLINK_MPS_DEPENDENCY_H

#include "nlink/core/common/nexus_core.h"
#include "nlink/core/common/result.h"
#include "nlink/mpsystem/mps_config.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Edge in the dependency graph
 */
typedef struct NexusMPSDependencyEdge {
    size_t source_idx;              /**< Source node index */
    size_t target_idx;              /**< Target node index */
    NexusConnectionDirection direction; /**< Edge direction */
    const char* data_format;        /**< Data format for this edge */
    bool optional;                  /**< Whether this edge is optional */
} NexusMPSDependencyEdge;

/**
 * @brief Node in the dependency graph
 */
typedef struct NexusMPSDependencyNode {
    const char* component_id;       /**< Component identifier */
    NexusMPSComponentConfig* config; /**< Reference to component config */
    size_t* incoming_edges;         /**< Array of incoming edge indices */
    size_t incoming_count;          /**< Number of incoming edges */
    size_t* outgoing_edges;         /**< Array of outgoing edge indices */
    size_t outgoing_count;          /**< Number of outgoing edges */
    int visited;                    /**< Visited status for graph traversal */
    int component_group;            /**< For strongly connected components */
    bool supports_reentrance;       /**< Whether component supports multiple passes */
} NexusMPSDependencyNode;

/**
 * @brief Dependency graph for multi-pass systems
 */
typedef struct NexusMPSDependencyGraph {
    NexusMPSDependencyNode* nodes;  /**< Array of dependency nodes */
    size_t node_count;              /**< Number of nodes */
    NexusMPSDependencyEdge* edges;  /**< Array of dependency edges */
    size_t edge_count;              /**< Number of edges */
    const NexusMPSConfig* config;   /**< Reference to pipeline config */
} NexusMPSDependencyGraph;

/**
 * @brief Group of components for execution
 */
typedef struct NexusExecutionGroup {
    const char** component_ids;     /**< Array of component IDs */
    size_t component_count;         /**< Number of components */
    bool has_cycles;                /**< Whether this group contains cycles */
    bool is_forward_only;           /**< Whether this group is forward-only */
} NexusExecutionGroup;

/**
 * @brief Create a dependency graph from multi-pass component metadata
 *
 * @param ctx NexusLink context
 * @param config Pipeline configuration
 * @return NexusMPSDependencyGraph* New dependency graph or NULL on failure
 */
NexusMPSDependencyGraph* mps_create_dependency_graph(NexusContext* ctx, 
                                                    const NexusMPSConfig* config);

/**
 * @brief Resolve bidirectional dependencies
 *
 * @param ctx NexusLink context
 * @param graph Dependency graph
 * @param execution_groups Output parameter for execution groups
 * @param group_count Output parameter for number of groups
 * @return NexusResult Operation result
 */
NexusResult mps_resolve_bidirectional_dependencies(NexusContext* ctx, 
                                                  NexusMPSDependencyGraph* graph,
                                                  NexusExecutionGroup*** execution_groups,
                                                  size_t* group_count);

/**
 * @brief Find strongly connected components (for cycle detection)
 *
 * @param ctx NexusLink context
 * @param graph Dependency graph
 * @param components Output parameter for component groups
 * @param component_counts Output parameter for number of components in each group
 * @param group_count Output parameter for number of groups
 * @return NexusResult Operation result
 */
NexusResult mps_find_strongly_connected_components(NexusContext* ctx,
                                                  NexusMPSDependencyGraph* graph,
                                                  NexusComponent*** components,
                                                  size_t** component_counts,
                                                  size_t* group_count);

/**
 * @brief Check for issues in graph that would prevent execution
 *
 * @param ctx NexusLink context
 * @param graph Dependency graph to validate
 * @return NexusResult Operation result
 */
NexusResult mps_validate_dependency_graph(NexusContext* ctx, NexusMPSDependencyGraph* graph);

/**
 * @brief Free dependency graph resources
 *
 * @param graph Dependency graph to free
 */
void mps_free_dependency_graph(NexusMPSDependencyGraph* graph);

/**
 * @brief Free execution group resources
 *
 * @param groups Execution groups to free
 * @param group_count Number of groups
 */
void mps_free_execution_groups(NexusExecutionGroup** groups, size_t group_count);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_MPS_DEPENDENCY_H */
