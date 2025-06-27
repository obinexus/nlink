/**
 * @file minimize_command_integration.c
 * @brief Example implementation for integrating minimizer with CLI
 * 
 * This file demonstrates how the CLI command interfaces with the 
 * minimization subsystem to provide a user-friendly command interface.
 * 
 * Copyright © 2025 (Demonstration code)
 */

#include "commands/minimize.h"
#include "nlink/core/minimizer/nexus_minimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * @brief Structure for minimize command data
 */
typedef struct {
    char* component_path;      /**< Path to the component */
    NexusMinimizerConfig config;  /**< Minimization configuration */
    bool output_metrics;       /**< Whether to output metrics */
    char* output_file;         /**< Output file path (optional) */
} MinimizeCommandData;

/**
 * @brief Execute the minimize command
 */
static int minimize_execute(NexusContext* ctx, int argc, char** argv) {
    if (!ctx || argc < 1) {
        fprintf(stderr, "Error: No component specified\n");
        return 1;
    }
    
    // Retrieve command data from the global command instance
    MinimizeCommandData* data = (MinimizeCommandData*)minimize_command.data;
    
    // Component path is either from command data or first argument
    char* component_path = data ? data->component_path : argv[0];
    if (!component_path) {
        fprintf(stderr, "Error: No component path specified\n");
        return 1;
    }
    
    // Get minimization configuration from data or defaults
    NexusMinimizerConfig config;
    if (data) {
        config = data->config;
    } else {
        config = nexus_minimizer_default_config();
        
        // Parse command-line options
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--level") == 0 && i + 1 < argc) {
                int level = atoi(argv[++i]);
                if (level >= NEXUS_MINIMIZE_BASIC && level <= NEXUS_MINIMIZE_AGGRESSIVE) {
                    config.level = (NexusMinimizeLevel)level;
                } else {
                    fprintf(stderr, "Warning: Invalid minimization level, using default\n");
                }
            } else if (strcmp(argv[i], "--verbose") == 0) {
                config.verbose = true;
            } else if (strcmp(argv[i], "--no-metrics") == 0) {
                config.enable_metrics = false;
            }
        }
    }
    
    // Allocate metrics structure if metrics are enabled
    NexusMinimizationMetrics metrics;
    NexusMinimizationMetrics* metrics_ptr = config.enable_metrics ? &metrics : NULL;
    
    // Log the operation start
    printf("Minimizing component: %s\n", component_path);
    printf("Minimization level: %d (%s)\n", 
           config.level,
           config.level == NEXUS_MINIMIZE_BASIC ? "Basic" :
           config.level == NEXUS_MINIMIZE_STANDARD ? "Standard" : "Aggressive");
    
    // Start timing the operation
    clock_t start_time = clock();
    
    // Perform minimization
    NexusResult result = nexus_minimize_component(ctx, component_path, config, metrics_ptr);
    
    // Calculate elapsed time
    clock_t end_time = clock();
    double elapsed_seconds = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    if (result != NEXUS_SUCCESS) {
        fprintf(stderr, "Error: Minimization failed: %s\n", nexus_result_to_string(result));
        return 1;
    }
    
    // Print success message
    printf("Minimization completed in %.2f seconds\n", elapsed_seconds);
    
    // Print metrics if enabled
    if (config.enable_metrics && metrics_ptr && (data ? data->output_metrics : true)) {
        printf("\n");
        nexus_print_minimization_metrics(metrics_ptr);
    }
    
    // Save to output file if specified
    if (data && data->output_file) {
        printf("Saving minimized component to: %s\n", data->output_file);
        
        // In a real implementation, this would copy the minimized component
        FILE* src = fopen(component_path, "rb");
        FILE* dst = fopen(data->output_file, "wb");
        
        if (src && dst) {
            char buffer[8192];
            size_t bytes;
            
            while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
                fwrite(buffer, 1, bytes, dst);
            }
            
            fclose(src);
            fclose(dst);
            printf("Minimized component saved successfully\n");
        } else {
            fprintf(stderr, "Error: Failed to save minimized component\n");
            if (src) fclose(src);
            if (dst) fclose(dst);
            return 1;
        }
    }
    
    return 0;
}

/**
 * @brief Free minimize command data
 */
static void minimize_free_data(void* command_data) {
    if (!command_data) {
        return;
    }
    
    MinimizeCommandData* data = (MinimizeCommandData*)command_data;
    free(data->component_path);
    free(data->output_file);
    free(data);
}

/**
 * @brief Integration example showing load command using minimization
 */
int load_with_minimization(NexusContext* ctx, const char* component_path, 
                          bool minimize, int minimize_level) {
    printf("Loading component: %s\n", component_path);
    
    // Measure original size
    FILE* file = fopen(component_path, "rb");
    if (!file) {
        fprintf(stderr, "Error: Unable to open component file\n");
        return 1;
    }
    
    fseek(file, 0, SEEK_END);
    long original_size = ftell(file);
    fclose(file);
    
    printf("Original size: %ld bytes\n", original_size);
    
    // Apply minimization if enabled
    if (minimize) {
        printf("Applying minimization (level %d)...\n", minimize_level);
        
        // Create minimizer configuration
        NexusMinimizerConfig config = nexus_minimizer_default_config();
        config.level = (NexusMinimizeLevel)minimize_level;
        config.verbose = false;
        config.enable_metrics = true;
        
        // Perform minimization
        NexusMinimizationMetrics metrics;
        NexusResult result = nexus_minimize_component(ctx, component_path, config, &metrics);
        
        if (result != NEXUS_SUCCESS) {
            fprintf(stderr, "Error: Minimization failed: %s\n", nexus_result_to_string(result));
            return 1;
        }
        
        // Print minimization results
        printf("Minimization complete\n");
        printf("  States: %zu → %zu\n", metrics.original_states, metrics.minimized_states);
        printf("  Size: %.2f KB → %.2f KB\n", 
               metrics.original_size / 1024.0, metrics.minimized_size / 1024.0);
    }
    
    // Simulate loading the component
    printf("Loading component...\n");
    
    // In a real implementation, this would call nexus_load_component
    
    printf("Component loaded successfully\n");
    return 0;
}

/**
 * @brief Create a test automaton for demonstration
 */
OkpalaAutomaton* create_test_automaton() {
    OkpalaAutomaton* automaton = okpala_automaton_create();
    
    // Add states (q0 through q9)
    for (int i = 0; i < 10; i++) {
        char state_id[8];
        snprintf(state_id, sizeof(state_id), "q%d", i);
        okpala_automaton_add_state(automaton, state_id, i >= 8); // q8, q9 are final
    }
    
    // Add transitions to create redundant states
    // q0 -a-> q1, q1 -b-> q2, q2 -c-> q8 (accepting)
    okpala_automaton_add_transition(automaton, "q0", "q1", "a");
    okpala_automaton_add_transition(automaton, "q1", "q2", "b");
    okpala_automaton_add_transition(automaton, "q2", "q8", "c");
    
    // q3 -a-> q4, q4 -b-> q5, q5 -c-> q8 (accepting)
    // This is equivalent to the path above
    okpala_automaton_add_transition(automaton, "q3", "q4", "a");
    okpala_automaton_add_transition(automaton, "q4", "q5", "b");
    okpala_automaton_add_transition(automaton, "q5", "q8", "c");
    
    // q6 -a-> q7, q7 -d-> q9 (accepting)
    // This is a distinct path
    okpala_automaton_add_transition(automaton, "q6", "q7", "a");
    okpala_automaton_add_transition(automaton, "q7", "q9", "d");
    
    return automaton;
}

/**
 * @brief Demonstrate automaton minimization directly
 */
void demonstrate_automaton_minimization() {
    printf("Demonstrating automaton minimization...\n");
    
    // Create test automaton
    OkpalaAutomaton* automaton = create_test_automaton();
    printf("Original automaton has %zu states\n", automaton->state_count);
    
    // Print states and transitions
    printf("States:\n");
    for (size_t i = 0; i < automaton->state_count; i++) {
        OkpalaState* state = &automaton->states[i];
        printf("  %s (%s)\n", state->id, state->is_final ? "final" : "non-final");
        
        for (size_t j = 0; j < state->transition_count; j++) {
            printf("    --%s--> %s\n", state->input_symbols[j], state->transitions[j]->id);
        }
    }
    
    // Minimize the automaton
    printf("Minimizing automaton...\n");
    OkpalaAutomaton* minimized = okpala_minimize_automaton(automaton, true);
    printf("Minimized automaton has %zu states\n", minimized->state_count);
    
    // Print minimized states and transitions
    printf("Minimized states:\n");
    for (size_t i = 0; i < minimized->state_count; i++) {
        OkpalaState* state = &minimized->states[i];
        printf("  %s (%s)\n", state->id, state->is_final ? "final" : "non-final");
        
        for (size_t j = 0; j < state->transition_count; j++) {
            printf("    --%s--> %s\n", state->input_symbols[j], state->transitions[j]->id);
        }
    }
    
    // Clean up
    okpala_automaton_free(automaton);
    okpala_automaton_free(minimized);
    
    printf("Demonstration complete\n");
}

/**
 * @brief Main function for demonstration
 */
int main(int argc, char** argv) {
    // Initialize context
    NexusContext* ctx = nexus_create_context(NULL);
    if (!ctx) {
        fprintf(stderr, "Error: Failed to create context\n");
        return 1;
    }
    
    // Initialize minimizer
    nexus_minimizer_initialize(ctx);
    
    // Demonstrate automaton minimization
    demonstrate_automaton_minimization();
    
    // Example of loading with minimization
    if (argc > 1) {
        load_with_minimization(ctx, argv[1], true, 2);
    }
    
    // Clean up
    nexus_minimizer_cleanup(ctx);
    nexus_destroy_context(ctx);
    
    return 0;
}