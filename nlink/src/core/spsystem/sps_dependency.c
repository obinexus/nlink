/**
 * @file sps_dependency.c
 * @brief Dependency resolution for single-pass systems
 *
 * Implements dependency resolution for components in a single-pass
 * pipeline system, ensuring components are executed in the correct order.
 *
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/spsystem/sps_dependency.h"
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/nexus_loader.h"
 #include <stdlib.h>
 #include <string.h>
 
 /* Forward declaration of internal functions */
 static NexusResult build_dependency_nodes(NexusContext* ctx, 
                                          NexusDependencyGraph* graph,
                                          const NexusPipelineConfig* config);
 static NexusResult topological_sort(NexusContext* ctx, 
                                    NexusDependencyGraph* graph,
                                    const char*** ordered_components,
                                    size_t* component_count);
 static NexusResult visit_node(NexusContext* ctx, 
                              NexusDependencyGraph* graph,
                              size_t node_index,
                              const char*** sorted,
                              size_t* sorted_count,
                              size_t* capacity);
 static bool component_has_dependency(NexusDependencyNode* node, const char* dependency_id);
 static NexusResult detect_cycles(NexusContext* ctx, NexusDependencyGraph* graph);
 
 /**
  * Create a dependency graph from component metadata
  */
 NexusDependencyGraph* sps_create_dependency_graph(NexusContext* ctx, 
                                                  const NexusPipelineConfig* config) {
     if (!ctx || !config) {
         return NULL;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Creating dependency graph for pipeline '%s'", 
              config->pipeline_id);
     
     NexusDependencyGraph* graph = (NexusDependencyGraph*)calloc(1, sizeof(NexusDependencyGraph));
     if (!graph) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate dependency graph");
         return NULL;
     }
     
     graph->config = config;
     
     // Allocate nodes array
     if (config->component_count > 0) {
         graph->nodes = (NexusDependencyNode**)calloc(
             config->component_count, sizeof(NexusDependencyNode*)
         );
         
         if (!graph->nodes) {
             nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate nodes array");
             free(graph);
             return NULL;
         }
     }
     
     // Build dependency nodes
     NexusResult result = build_dependency_nodes(ctx, graph, config);
     if (result != NEXUS_SUCCESS) {
         sps_free_dependency_graph(graph);
         return NULL;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Dependency graph created with %zu nodes", graph->node_count);
     return graph;
 }
 
 /**
  * Build dependency nodes from configuration
  */
 static NexusResult build_dependency_nodes(NexusContext* ctx, 
                                          NexusDependencyGraph* graph,
                                          const NexusPipelineConfig* config) {
     for (size_t i = 0; i < config->component_count; i++) {
         NexusPipelineComponentConfig* comp_config = config->components[i];
         if (!comp_config || !comp_config->component_id) {
             continue;
         }
         
         // Create a new node
         NexusDependencyNode* node = (NexusDependencyNode*)calloc(1, sizeof(NexusDependencyNode));
         if (!node) {
             nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate dependency node");
             return NEXUS_OUT_OF_MEMORY;
         }
         
         // Set node properties
         node->component_id = comp_config->component_id;
         node->visited = false;
         node->temporary_mark = false;
         node->metadata = comp_config;  // Store a reference to the component config
         
         // Determine dependencies based on component metadata
         // This would typically load the component and query its dependencies
         // For now, we'll use a simplified approach where we load component info
         
         // Add node to graph
         graph->nodes[graph->node_count++] = node;
         
         nexus_log(ctx, NEXUS_LOG_DEBUG, "Added node for component '%s'", comp_config->component_id);
     }
     
     // Now that we have all nodes, establish dependencies
     // In a real system, this would load each component and query its dependencies
     // For simplicity, we'll just establish linear dependencies for now
     for (size_t i = 0; i < graph->node_count; i++) {
         NexusDependencyNode* node = graph->nodes[i];
         
         // For demonstration, create dependencies between adjacent components
         if (i > 0) {
             // This component depends on the previous one
             node->dependencies = (const char**)calloc(1, sizeof(char*));
             if (!node->dependencies) {
                 nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate dependencies array");
                 return NEXUS_OUT_OF_MEMORY;
             }
             
             node->dependencies[0] = graph->nodes[i-1]->component_id;
             node->dependency_count = 1;
             
             nexus_log(ctx, NEXUS_LOG_DEBUG, "Component '%s' depends on '%s'", 
                      node->component_id, node->dependencies[0]);
         }
     }
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * Resolve dependencies and provide ordered loading sequence
  */
 NexusResult sps_resolve_dependencies(NexusContext* ctx, 
                                     NexusDependencyGraph* graph,
                                     const char*** ordered_components,
                                     size_t* component_count) {
     if (!ctx || !graph || !ordered_components || !component_count) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Resolving dependencies for %zu components", graph->node_count);
     
     // Initialize output
     *ordered_components = NULL;
     *component_count = 0;
     
     // Check for cycles
     NexusResult result = detect_cycles(ctx, graph);
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     // Perform topological sort
     result = topological_sort(ctx, graph, ordered_components, component_count);
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Dependencies resolved successfully, found order for %zu components", 
              *component_count);
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * Topological sort algorithm
  */
 static NexusResult topological_sort(NexusContext* ctx, 
                                    NexusDependencyGraph* graph,
                                    const char*** ordered_components,
                                    size_t* component_count) {
     // Reset visited flags
     for (size_t i = 0; i < graph->node_count; i++) {
         graph->nodes[i]->visited = false;
         graph->nodes[i]->temporary_mark = false;
     }
     
     // Allocate initial result array
     size_t capacity = graph->node_count;
     const char** sorted = (const char**)calloc(capacity, sizeof(char*));
     if (!sorted) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate sorted components array");
         return NEXUS_OUT_OF_MEMORY;
     }
     
     size_t sorted_count = 0;
     
     // Visit each unvisited node
     for (size_t i = 0; i < graph->node_count; i++) {
         if (!graph->nodes[i]->visited) {
             NexusResult result = visit_node(ctx, graph, i, &sorted, &sorted_count, &capacity);
             if (result != NEXUS_SUCCESS) {
                 free(sorted);
                 return result;
             }
         }
     }
     
     // Reverse the array to get dependency order (not reverse topological order)
     for (size_t i = 0; i < sorted_count / 2; i++) {
         const char* temp = sorted[i];
         sorted[i] = sorted[sorted_count - i - 1];
         sorted[sorted_count - i - 1] = temp;
     }
     
     // Set output
     *ordered_components = sorted;
     *component_count = sorted_count;
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * Visit a node in the dependency graph (DFS)
  */
 static NexusResult visit_node(NexusContext* ctx, 
                              NexusDependencyGraph* graph,
                              size_t node_index,
                              const char*** sorted,
                              size_t* sorted_count,
                              size_t* capacity) {
     NexusDependencyNode* node = graph->nodes[node_index];
     
     // Check for cycle
     if (node->temporary_mark) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Dependency cycle detected involving component '%s'",
                  node->component_id);
         return NEXUS_DEPENDENCY_CYCLE;
     }
     
     // Skip if already visited
     if (node->visited) {
         return NEXUS_SUCCESS;
     }
     
     // Mark temporarily for cycle detection
     node->temporary_mark = true;
     
     // Visit dependencies
     for (size_t i = 0; i < node->dependency_count; i++) {
         const char* dep_id = node->dependencies[i];
         
         // Find the dependency node
         for (size_t j = 0; j < graph->node_count; j++) {
             if (strcmp(graph->nodes[j]->component_id, dep_id) == 0) {
                 NexusResult result = visit_node(ctx, graph, j, sorted, sorted_count, capacity);
                 if (result != NEXUS_SUCCESS) {
                     return result;
                 }
                 break;
             }
         }
     }
     
     // Mark as visited
     node->visited = true;
     node->temporary_mark = false;
     
     // Check if we need to resize the sorted array
     if (*sorted_count >= *capacity) {
         *capacity *= 2;
         const char** new_sorted = (const char**)realloc(*sorted, *capacity * sizeof(char*));
         if (!new_sorted) {
             nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to resize sorted array");
             return NEXUS_OUT_OF_MEMORY;
         }
         *sorted = new_sorted;
     }
     
     // Add to sorted list
     (*sorted)[(*sorted_count)++] = node->component_id;
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * Check if a component has a specific dependency
  */
 static bool component_has_dependency(NexusDependencyNode* node, const char* dependency_id) {
     for (size_t i = 0; i < node->dependency_count; i++) {
         if (strcmp(node->dependencies[i], dependency_id) == 0) {
             return true;
         }
     }
     return false;
 }
 
 /**
  * Detect cycles in the dependency graph
  */
 static NexusResult detect_cycles(NexusContext* ctx, NexusDependencyGraph* graph) {
     // Reset visited flags
     for (size_t i = 0; i < graph->node_count; i++) {
         graph->nodes[i]->visited = false;
         graph->nodes[i]->temporary_mark = false;
     }
     
     // Check each node for cycles
     for (size_t i = 0; i < graph->node_count; i++) {
         if (!graph->nodes[i]->visited) {
             // Use DFS to detect cycles
             graph->nodes[i]->temporary_mark = true;
             
             for (size_t j = 0; j < graph->nodes[i]->dependency_count; j++) {
                 const char* dep_id = graph->nodes[i]->dependencies[j];
                 
                 for (size_t k = 0; k < graph->node_count; k++) {
                     if (strcmp(graph->nodes[k]->component_id, dep_id) == 0) {
                         if (graph->nodes[k]->temporary_mark) {
                             nexus_log(ctx, NEXUS_LOG_ERROR, 
                                     "Dependency cycle detected: %s -> %s",
                                     graph->nodes[i]->component_id, dep_id);
                             return NEXUS_DEPENDENCY_CYCLE;
                         }
                         
                         if (!graph->nodes[k]->visited) {
                             NexusResult result = detect_cycles(ctx, graph);
                             if (result != NEXUS_SUCCESS) {
                                 return result;
                             }
                         }
                     }
                 }
             }
             
             graph->nodes[i]->temporary_mark = false;
             graph->nodes[i]->visited = true;
         }
     }
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * Check if there are any missing dependencies
  */
 NexusResult sps_check_missing_dependencies(NexusContext* ctx, 
                                           NexusDependencyGraph* graph,
                                           NexusMissingDependency** missing_deps,
                                           size_t* missing_count) {
     if (!ctx || !graph || !missing_deps || !missing_count) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Checking for missing dependencies");
     
     // Reset counters
     *missing_deps = NULL;
     *missing_count = 0;
     
     // Count missing dependencies first
     size_t total_missing = 0;
     for (size_t i = 0; i < graph->node_count; i++) {
         NexusDependencyNode* node = graph->nodes[i];
         
         for (size_t j = 0; j < node->dependency_count; j++) {
             const char* dep_id = node->dependencies[j];
             bool found = false;
             
             // Check if the dependency exists in the graph
             for (size_t k = 0; k < graph->node_count; k++) {
                 if (strcmp(graph->nodes[k]->component_id, dep_id) == 0) {
                     found = true;
                     break;
                 }
             }
             
             if (!found) {
                 total_missing++;
             }
         }
     }
     
     // If no missing dependencies, we're done
     if (total_missing == 0) {
         nexus_log(ctx, NEXUS_LOG_INFO, "No missing dependencies found");
         return NEXUS_SUCCESS;
     }
     
     // Allocate the missing dependencies array
     *missing_deps = (NexusMissingDependency*)calloc(total_missing, sizeof(NexusMissingDependency));
     if (!*missing_deps) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to allocate missing dependencies array");
         return NEXUS_OUT_OF_MEMORY;
     }
     
     // Fill in the missing dependencies
     size_t index = 0;
     for (size_t i = 0; i < graph->node_count; i++) {
         NexusDependencyNode* node = graph->nodes[i];
         NexusPipelineComponentConfig* comp_config = 
             (NexusPipelineComponentConfig*)node->metadata;
         
         for (size_t j = 0; j < node->dependency_count; j++) {
             const char* dep_id = node->dependencies[j];
             bool found = false;
             
             // Check if the dependency exists in the graph
             for (size_t k = 0; k < graph->node_count; k++) {
                 if (strcmp(graph->nodes[k]->component_id, dep_id) == 0) {
                     found = true;
                     break;
                 }
             }
             
             if (!found) {
                 (*missing_deps)[index].component_id = node->component_id;
                 (*missing_deps)[index].missing_dependency = dep_id;
                 
                 // Get version constraint if available
                 (*missing_deps)[index].version_constraint = 
                     comp_config ? comp_config->version_constraint : NULL;
                 
                 // Determine if optional based on component config
                 (*missing_deps)[index].is_optional = 
                     comp_config ? comp_config->optional : false;
                 
                 index++;
             }
         }
     }
     
     *missing_count = total_missing;
     
     nexus_log(ctx, NEXUS_LOG_WARNING, "Found %zu missing dependencies", total_missing);
     return NEXUS_SUCCESS;
 }
 
 /**
  * Free dependency graph resources
  */
 void sps_free_dependency_graph(NexusDependencyGraph* graph) {
     if (!graph) {
         return;
     }
     
     // Free nodes
     if (graph->nodes) {
         for (size_t i = 0; i < graph->node_count; i++) {
             if (graph->nodes[i]) {
                 // Free dependencies array
                 if (graph->nodes[i]->dependencies) {
                     free((void*)graph->nodes[i]->dependencies);
                 }
                 
                 // Don't free component_id and metadata as they are references
                 
                 free(graph->nodes[i]);
             }
         }
         
         free(graph->nodes);
     }
     
     // Free the graph itself
     free(graph);
 }