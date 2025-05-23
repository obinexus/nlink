/**
 * @file sps_dependency.h
 * @brief Dependency resolution for single-pass systems
 *
 * Defines structures and functions for resolving dependencies between
 * components in a single-pass pipeline system.
 *
 * Copyright © 2025 OBINexus Computing
 */

#ifndef NLINK_SPS_DEPENDENCY_H
#define NLINK_SPS_DEPENDENCY_H

#include "nlink/core/common/nexus_core.h"
#include "nlink/core/common/result.h"
#include "nlink/spsystem/sps_config.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Dependency node for a single component
 */
typedef struct NexusDependencyNode {
    const char* component_id;            /**< Component identifier */
    const char** dependencies;           /**< Array of dependency component IDs */
    size_t dependency_count;             /**< Number of dependencies */
    bool visited;                        /**< Visited flag for graph traversal */
    bool temporary_mark;                 /**< Temporary mark for cycle detection */
    void* metadata;                      /**< Component metadata */
} NexusDependencyNode;

/**
 * @brief Dependency graph for components
 */
typedef struct NexusDependencyGraph {
    NexusDependencyNode** nodes;         /**< Array of dependency nodes */
    size_t node_count;                   /**< Number of nodes */
    const NexusPipelineConfig* config;   /**< Reference to pipeline configuration */
} NexusDependencyGraph;

/**
 * @brief Missing dependency information
 */
typedef struct NexusMissingDependency {
    const char* component_id;            /**< Component with the missing dependency */
    const char* missing_dependency;      /**< ID of the missing dependency */
    const char* version_constraint;      /**< Version constraint if applicable */
    bool is_optional;                    /**< Whether the dependency is optional */
} NexusMissingDependency;

/**
 * @brief Create a dependency graph from component metadata
 *
 * @param ctx NexusLink context
 * @param config Pipeline configuration
 * @return NexusDependencyGraph* New dependency graph or NULL on failure
 */
NexusDependencyGraph* sps_create_dependency_graph(NexusContext* ctx, 
                                                 const NexusPipelineConfig* config);

/**
 * @brief Resolve dependencies and provide ordered loading sequence
 *
 * @param ctx NexusLink context
 * @param graph Dependency graph
 * @param ordered_components Output parameter for ordered component IDs
 * @param component_count Output parameter for number of components
 * @return NexusResult Operation result
 */
NexusResult sps_resolve_dependencies(NexusContext* ctx, 
                                    NexusDependencyGraph* graph,
                                    const char*** ordered_components,
                                    size_t* component_count);

/**
 * @brief Check if there are any missing dependencies
 *
 * @param ctx NexusLink context
 * @param graph Dependency graph
 * @param missing_deps Output parameter for missing dependencies
 * @param missing_count Output parameter for number of missing dependencies
 * @return NexusResult Operation result
 */
NexusResult sps_check_missing_dependencies(NexusContext* ctx, 
                                          NexusDependencyGraph* graph,
                                          NexusMissingDependency** missing_deps,
                                          size_t* missing_count);

/**
 * @brief Free dependency graph resources
 *
 * @param graph Dependency graph to free
 */
void sps_free_dependency_graph(NexusDependencyGraph* graph);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_SPS_DEPENDENCY_H */
