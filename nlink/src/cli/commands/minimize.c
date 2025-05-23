/**
 * @file minimize.c
 * @brief Implementation of minimize command for NexusLink CLI
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/cli/commands/minimize.h"
 #include "nlink/core/minimizer/nexus_minimizer.h"
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/result.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 
 // Forward declarations
 static int minimize_execute(NexusContext* ctx, int argc, char** argv);
 static void minimize_print_help(void);
 static bool minimize_parse_args(int argc, char** argv, void** command_data);
 static void minimize_free_data(void* command_data);
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
    * @brief Minimize command definition
    */
 static NexusCommand minimize_command = {
         .name = "minimize",
         .description = "Minimize a component using state machine minimization",
         .handler = NULL,
         .handler_with_params = NULL,
         .execute = minimize_execute,
         .data = NULL
 };
 
 /**
    * @brief Execute the minimize command
    */
 static int minimize_execute(NexusContext* ctx, int argc, char** argv) {
         if (!ctx || argc < 1) {
                 fprintf(stderr, "Error: No component specified\n");
                 return 1;
         }
         
         // Get command data
         MinimizeCommandData* data = (MinimizeCommandData*)minimize_command.data;
         
         // Component path is either from command data or first argument
         char* component_path = data ? data->component_path : argv[0];
         if (!component_path) {
                 fprintf(stderr, "Error: No component path specified\n");
                 return 1;
         }
         
         // Get minimization configuration
         NexusMinimizerConfig config;
         if (data) {
                 config = data->config;
         } else {
                 config = nexus_minimizer_default_config();
                 
                 // Parse options from arguments
                 for (int i = 1; i < argc; i++) {
                         if (strcmp(argv[i], "--level") == 0 && i + 1 < argc) {
                                 int level = atoi(argv[++i]);
                                 if (level >= 1 && level <= 3) {
                                         config.level = (NexusMinimizerLevel)level;
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
         
         // Allocate metrics if needed
         NexusMinimizationMetrics metrics;
         NexusMinimizationMetrics* metrics_ptr = config.enable_metrics ? &metrics : NULL;
         
         // Perform minimization
         printf("Minimizing component: %s\n", component_path);
         printf("Minimization level: %d\n", config.level);
         
         NexusResult result = nexus_minimize_component(ctx, component_path, config, metrics_ptr);
         
         if (result != NEXUS_SUCCESS) {
                 fprintf(stderr, "Error: Minimization failed: %s\n", nexus_result_to_string(result));
                 return 1;
         }
         
         // Print metrics if enabled
         if (config.enable_metrics && (data ? data->output_metrics : true)) {
                 printf("\nMinimization metrics:\n");
                 printf("---------------------\n");
                 printf("Original states: %zu\n", metrics_ptr->original_states);
                 printf("Minimized states: %zu\n", metrics_ptr->minimized_states);
                 printf("Reduction: %.1f%%\n", (1.0 - (double)metrics_ptr->minimized_states / metrics_ptr->original_states) * 100.0);
                 printf("Original size: %.2f KB\n", metrics_ptr->original_size / 1024.0);
                 printf("Minimized size: %.2f KB\n", metrics_ptr->minimized_size / 1024.0);
                 printf("Size reduction: %.1f%%\n", (1.0 - (double)metrics_ptr->minimized_size / metrics_ptr->original_size) * 100.0);
                 printf("Processing time: %.2f ms\n", metrics_ptr->time_taken_ms);
                 printf("Boolean reduction: %s\n", metrics_ptr->boolean_reduction ? "enabled" : "disabled");
         }
         
         // Save to output file if specified
         if (data && data->output_file) {
                 // In a real implementation, this would save the minimized component
                 printf("Saving minimized component to: %s\n", data->output_file);
         }
         
         return 0;
 }
 
 /**
    * @brief Print help for the minimize command
    */
 static void minimize_print_help(void) {
         printf("Usage: minimize [OPTIONS] COMPONENT_PATH\n\n");
         printf("Options:\n");
         printf("  --level LEVEL      Set minimization level (1=basic, 2=standard, 3=aggressive)\n");
         printf("  --verbose          Enable verbose output\n");
         printf("  --no-metrics       Disable metrics collection\n");
         printf("  --output FILE      Save minimized component to FILE\n");
         printf("\n");
         printf("Examples:\n");
         printf("  minimize mycomponent.so              - Minimize component with standard settings\n");
         printf("  minimize mycomponent.so --level 3    - Aggressive minimization with boolean reduction\n");
         printf("  minimize --verbose mycomponent.so    - Verbose output with detailed metrics\n");
 }
 
 /**
    * @brief Parse arguments for the minimize command
    */
 static bool minimize_parse_args(int argc, char** argv, void** command_data) {
         if (argc < 1) {
                 return false;
         }
         
         // Create command data
         MinimizeCommandData* data = (MinimizeCommandData*)malloc(sizeof(MinimizeCommandData));
         if (!data) {
                 return false;
         }
         
         // Initialize with defaults
         data->component_path = NULL;
         data->config = nexus_minimizer_default_config();
         data->output_metrics = true;
         data->output_file = NULL;
         
         // First argument is always the component path
         data->component_path = strdup(argv[0]);
         if (!data->component_path) {
                 free(data);
                 return false;
         }
         
         // Parse remaining arguments
         for (int i = 1; i < argc; i++) {
                 if (strcmp(argv[i], "--level") == 0 && i + 1 < argc) {
                         int level = atoi(argv[++i]);
                         if (level >= 1 && level <= 3) {
                                 data->config.level = (NexusMinimizerLevel)level;
                         }
                 } else if (strcmp(argv[i], "--verbose") == 0) {
                         data->config.verbose = true;
                 } else if (strcmp(argv[i], "--no-metrics") == 0) {
                         data->config.enable_metrics = false;
                         data->output_metrics = false;
                 } else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
                         data->output_file = strdup(argv[++i]);
                         if (!data->output_file) {
                                 free(data->component_path);
                                 free(data);
                                 return false;
                         }
                 }
         }
         
         *command_data = data;
         return true;
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
    * @brief Get the minimize command structure
    * 
    * @return NexusCommand* Pointer to the command structure
    */
 NexusCommand* nlink_get_minimize_command(void) {
         minimize_command.handler = NULL;
         minimize_command.data = NULL;
         return &minimize_command;
 }