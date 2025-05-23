/**
 * @file nexus_minimizer.c
 * @brief Implementation of the NexusLink state machine minimizer
 * 
 * Copyright © 2025 OBINexus Computing
 */

#include "nlink/core/minimizer/nexus_minimizer.h"
#include "nlink/core/minimizer/okpala_automaton.h"
#include <sys/stat.h>
#include <time.h>



 
 // Initialize the minimizer subsystem
 NexusResult nexus_minimizer_initialize(NexusContext* ctx) {
     if (!ctx) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Initializing minimizer subsystem");
     
     // Initialize the automaton subsystem
     nexus_automaton_initialize();
     
     return NEXUS_SUCCESS;
 }
 
 // Create default minimizer configuration
 NexusMinimizerConfig nexus_minimizer_default_config(void) {
     NexusMinimizerConfig config;
     config.level = NEXUS_MINIMIZE_STANDARD;
     config.enable_metrics = true;
     config.verbose = false;
     return config;
 }
 
 // Helper function to get file size
 static size_t get_file_size(const char* path) {
     struct stat st;
     if (stat(path, &st) == 0) {
         return (size_t)st.st_size;
     }
     return 0;
 }
 
 // Helper function to measure time
 static double get_current_time_ms(void) {
     struct timespec ts;
     clock_gettime(CLOCK_MONOTONIC, &ts);
     return (ts.tv_sec * 1000.0) + (ts.tv_nsec / 1000000.0);
 }
 
 // Create automaton from component
 OkpalaAutomaton* nexus_create_automaton_from_component(
     NexusContext* ctx,
     const char* component_path
 ) {
     if (!ctx || !component_path) {
         return NULL;
     }
     
     nexus_log(ctx, NEXUS_LOG_DEBUG, "Creating automaton from component: %s", component_path);
     
     // Create a new automaton
     OkpalaAutomaton* automaton = okpala_automaton_create();
     if (!automaton) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to create automaton");
         return NULL;
     }
     
     // For demonstration purposes, we'll create a simple automaton
     // In a real implementation, this would analyze the component structure
     
     // Add states (q0, q1, q2)
     okpala_automaton_add_state(automaton, "q0", false);
     okpala_automaton_add_state(automaton, "q1", false);
     okpala_automaton_add_state(automaton, "q2", true);
     
     // Add transitions (q0 -a-> q1, q1 -b-> q2, q0 -c-> q2)
     okpala_automaton_add_transition(automaton, "q0", "q1", "a");
     okpala_automaton_add_transition(automaton, "q1", "q2", "b");
     okpala_automaton_add_transition(automaton, "q0", "q2", "c");
     
     // Log automaton details
     nexus_log(ctx, NEXUS_LOG_DEBUG, "Created automaton with %zu states", automaton->state_count);
     
     return automaton;
 }
 
 // Apply minimized automaton back to component
 NexusResult nexus_apply_minimized_automaton(
     NexusContext* ctx,
     const char* component_path,
     OkpalaAutomaton* minimized
 ) {
     if (!ctx || !component_path || !minimized) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     nexus_log(ctx, NEXUS_LOG_DEBUG, "Applying minimized automaton to component: %s", component_path);
     
     // In a real implementation, this would update the component structure
     // based on the minimized automaton
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Applied minimized automaton with %zu states", minimized->state_count);
     
     return NEXUS_SUCCESS;
 }
 
 // Minimize a component using automaton-based state minimization
 NexusResult nexus_minimize_component(
     NexusContext* ctx,
     const char* component_path,
     NexusMinimizerConfig config,
     NexusMinimizationMetrics* metrics
 ) {
     if (!ctx || !component_path) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Minimizing component: %s (level: %d)", 
              component_path, config.level);
     
     double start_time = 0.0;
     if (config.enable_metrics) {
         start_time = get_current_time_ms();
     }
     
     // Get original component size
     size_t original_size = get_file_size(component_path);
     
     // Create automaton from component
     OkpalaAutomaton* automaton = nexus_create_automaton_from_component(ctx, component_path);
     if (!automaton) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to create automaton from component");
         return NEXUS_ERROR_INVALID_STATE;
     }
     
     // Store original state count for metrics
     size_t original_states = automaton->state_count;
     
     // Perform minimization
     bool use_boolean_reduction = (config.level >= NEXUS_MINIMIZE_AGGRESSIVE);
     
     if (config.verbose) {
         nexus_log(ctx, NEXUS_LOG_INFO, "Performing automaton minimization (boolean reduction: %s)",
                  use_boolean_reduction ? "enabled" : "disabled");
     }
     
     OkpalaAutomaton* minimized = okpala_minimize_automaton(automaton, use_boolean_reduction);
     if (!minimized) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to minimize automaton");
         okpala_automaton_free(automaton);
         return NEXUS_ERROR_INVALID_STATE;
     }
     
     // Apply minimized automaton back to component
     NexusResult result = nexus_apply_minimized_automaton(ctx, component_path, minimized);
     if (result != NEXUS_SUCCESS) {
         nexus_log(ctx, NEXUS_LOG_ERROR, "Failed to apply minimized automaton to component");
         okpala_automaton_free(automaton);
         okpala_automaton_free(minimized);
         return result;
     }
     
     // Get minimized component size
     size_t minimized_size = get_file_size(component_path);
     
     // Calculate metrics if requested
     if (config.enable_metrics && metrics) {
         double end_time = get_current_time_ms();
         
         metrics->original_states = original_states;
         metrics->minimized_states = minimized->state_count;
         metrics->original_size = original_size;
         metrics->minimized_size = minimized_size;
         metrics->time_taken_ms = end_time - start_time;
         metrics->boolean_reduction = use_boolean_reduction;
         
         if (config.verbose) {
             nexus_print_minimization_metrics(metrics);
         }
     }
     
     // Clean up
     okpala_automaton_free(automaton);
     okpala_automaton_free(minimized);
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Component minimization completed successfully");
     
     return NEXUS_SUCCESS;
 }
 
 // Print minimization metrics
 void nexus_print_minimization_metrics(const NexusMinimizationMetrics* metrics) {
     if (!metrics) {
         return;
     }
     
     double state_reduction = (1.0 - (double)metrics->minimized_states / metrics->original_states) * 100.0;
     double size_reduction = (1.0 - (double)metrics->minimized_size / metrics->original_size) * 100.0;
     
     printf("Minimization Results:\n");
     printf("  State reduction: %zu → %zu (%.1f%%)\n", 
            metrics->original_states, metrics->minimized_states, state_reduction);
     printf("  Size reduction: %.2f KB → %.2f KB (%.1f%%)\n", 
            metrics->original_size / 1024.0, metrics->minimized_size / 1024.0, size_reduction);
     printf("  Processing time: %.2f ms\n", metrics->time_taken_ms);
     printf("  Boolean reduction: %s\n", metrics->boolean_reduction ? "enabled" : "disabled");
 }
 
 // Clean up the minimizer subsystem
 void nexus_minimizer_cleanup(NexusContext* ctx) {
     if (!ctx) {
         return;
     }
     
     nexus_log(ctx, NEXUS_LOG_INFO, "Cleaning up minimizer subsystem");
     
     // Clean up resources
     // In a real implementation, this would free any global resources
 }