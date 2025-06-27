#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

/**
 * @file parser_interface.c
 * @brief NexusLink CLI Parser Interface Implementation
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.0.0
 *
 * Implementation of command-line interface with systematic error handling,
 * dependency injection support, and comprehensive validation following
 * waterfall methodology principles for the Aegis project architecture.
 */

#include "cli/parser_interface.h"
#include "core/config.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


// =============================================================================
// CLI VERSION AND BUILD INFORMATION
// =============================================================================

#define NLINK_VERSION_MAJOR 1
#define NLINK_VERSION_MINOR 0
#define NLINK_VERSION_PATCH 0
#define NLINK_BUILD_DATE __DATE__
#define NLINK_BUILD_TIME __TIME__

// =============================================================================
// CLI UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Resolve project root path with systematic path validation
 */
static nlink_cli_result_t resolve_project_root(nlink_cli_context_t *context,
                                               const char *explicit_path) {
  if (explicit_path) {
    if (access(explicit_path, R_OK) != 0) {
      NLINK_CLI_ERROR(context, "Explicit project root path not accessible: %s",
                      explicit_path);
      return NLINK_CLI_ERROR_CONFIG_NOT_FOUND;
    }
    strncpy(context->project_root_path, explicit_path,
            NLINK_MAX_PATH_LENGTH - 1);
  } else {
    // Default to current working directory
    if (getcwd(context->project_root_path, NLINK_MAX_PATH_LENGTH) == NULL) {
      NLINK_CLI_ERROR(context, "Failed to determine current working directory");
      return NLINK_CLI_ERROR_INTERNAL_ERROR;
    }
  }

  // Enhanced path construction with overflow protection
  int path_result = snprintf(context->config_file_path, 
                          NLINK_MAX_PATH_LENGTH, 
                          "%s/pkg.nlink", 
                          context->project_root_path);

  if (path_result >= NLINK_MAX_PATH_LENGTH) {
      return NLINK_CLI_ERROR_INVALID_ARGUMENTS;
  }

  NLINK_CLI_VERBOSE(context, "Project root resolved to: %s",
                    context->project_root_path);
  NLINK_CLI_VERBOSE(context, "Configuration file path: %s",
                    context->config_file_path);

  return NLINK_CLI_SUCCESS;
}



// =============================================================================
// CLI CONTEXT MANAGEMENT IMPLEMENTATION
// =============================================================================

nlink_cli_result_t nlink_cli_init(nlink_cli_context_t *context) {
  if (!context) {
    return NLINK_CLI_ERROR_INVALID_ARGUMENTS;
  }

  // Initialize context structure with systematic defaults
  memset(context, 0, sizeof(nlink_cli_context_t));

  // Set default execution options following conservative configuration
  context->command = NLINK_CMD_UNKNOWN;
  context->verbose_output = false;
  context->strict_validation = true;
  context->suppress_warnings = false;
  context->json_output_format = false;

  // Initialize dependency injection with production functions
  context->config_parser_func = nlink_parse_pkg_config;
  context->mode_detector_func = nlink_detect_pass_mode;
  context->component_discovery_func = nlink_discover_components;
  context->validation_func = nlink_validate_config;

  // Initialize execution state tracking
  clock_gettime(CLOCK_MONOTONIC, &context->execution_start_time);
  context->warning_count = 0;
  context->error_count = 0;
  context->is_initialized = true;

  return NLINK_CLI_SUCCESS;
}

nlink_cli_result_t nlink_cli_parse_args(int argc, char *argv[],
                                        nlink_cli_args_t *args) {
  if (!args || argc < 1 || !argv) {
    return NLINK_CLI_ERROR_INVALID_ARGUMENTS;
  }

  // Initialize argument structure with systematic defaults
  memset(args, 0, sizeof(nlink_cli_args_t));
  args->argc = argc;
  args->argv = argv;
  args->program_name = argv[0];

  // Define long options for systematic command parsing
  static struct option long_options[] = {
      {"config-check", no_argument, 0, 'c'},
      {"discover-components", no_argument, 0, 'd'},
      {"validate-threading", no_argument, 0, 't'},
      {"parse-only", no_argument, 0, 'p'},
      {"help", no_argument, 0, 'h'},
      {"version", no_argument, 0, 'v'},
      {"verbose", no_argument, 0, 'V'},
      {"quiet", no_argument, 0, 'q'},
      {"config-file", required_argument, 0, 'f'},
      {"project-root", required_argument, 0, 'r'},
      {0, 0, 0, 0}};

  int option_index = 0;
  int c;

  // Systematic argument parsing with comprehensive option handling
  while ((c = getopt_long(argc, argv, "cdtphvVqf:r:", long_options,
                          &option_index)) != -1) {
    switch (c) {
    case 'c':
      args->help_requested = false;
      break;
    case 'd':
      args->help_requested = false;
      break;
    case 't':
      args->help_requested = false;
      break;
    case 'p':
      args->help_requested = false;
      break;
    case 'h':
      args->help_requested = true;
      break;
    case 'v':
      args->version_requested = true;
      break;
    case 'V':
      args->verbose_mode = true;
      break;
    case 'q':
      args->quiet_mode = true;
      break;
    case 'f':
      args->explicit_config_path = optarg;
      break;
    case 'r':
      args->explicit_project_root = optarg;
      break;
    case '?':
      // getopt_long already printed error message
      return NLINK_CLI_ERROR_INVALID_ARGUMENTS;
    default:
      return NLINK_CLI_ERROR_INVALID_ARGUMENTS;
    }
  }

  // Resolve quiet/verbose mode conflicts with conservative precedence
  if (args->quiet_mode && args->verbose_mode) {
    args->verbose_mode = false; // Quiet takes precedence
  }

  return NLINK_CLI_SUCCESS;
}

nlink_cli_result_t nlink_cli_execute(nlink_cli_context_t *context,
                                     const nlink_cli_args_t *args) {
  if (!context || !args || !context->is_initialized) {
    return NLINK_CLI_ERROR_INVALID_ARGUMENTS;
  }

  // Apply command line options to execution context
  context->verbose_output = args->verbose_mode;
  context->suppress_warnings = args->quiet_mode;

  // Handle special commands with immediate execution
  if (args->help_requested) {
    nlink_cli_display_help(args->program_name);
    return NLINK_CLI_SUCCESS;
  }

  if (args->version_requested) {
    nlink_cli_display_version();
    return NLINK_CLI_SUCCESS;
  }

  // Resolve project root and configuration paths
  nlink_cli_result_t result =
      resolve_project_root(context, args->explicit_project_root);
  if (result != NLINK_CLI_SUCCESS) {
    return result;
  }

  // Override configuration file path if explicitly provided
  if (args->explicit_config_path) {
    strncpy(context->config_file_path, args->explicit_config_path,
            NLINK_MAX_PATH_LENGTH - 1);
  }

  // Initialize core configuration system with systematic error handling
  nlink_config_result_t config_init_result = nlink_config_init();
  if (config_init_result != NLINK_CONFIG_SUCCESS) {
    NLINK_CLI_ERROR(context, "Failed to initialize configuration system: %d",
                    config_init_result);
    return NLINK_CLI_ERROR_INTERNAL_ERROR;
  }

  // Determine command from arguments with systematic command resolution
  if (strstr(args->argv[0], "config-check") ||
      (args->argc > 1 && strcmp(args->argv[1], "--config-check") == 0)) {
    context->command = NLINK_CMD_CONFIG_CHECK;
  } else if (strstr(args->argv[0], "discover-components") ||
             (args->argc > 1 &&
              strcmp(args->argv[1], "--discover-components") == 0)) {
    context->command = NLINK_CMD_DISCOVER_COMPONENTS;
  } else if (strstr(args->argv[0], "validate-threading") ||
             (args->argc > 1 &&
              strcmp(args->argv[1], "--validate-threading") == 0)) {
    context->command = NLINK_CMD_VALIDATE_THREADING;
  } else if (strstr(args->argv[0], "parse-only") ||
             (args->argc > 1 && strcmp(args->argv[1], "--parse-only") == 0)) {
    context->command = NLINK_CMD_PARSE_ONLY;
  } else {
    // Default to config-check for systematic validation
    context->command = NLINK_CMD_CONFIG_CHECK;
  }

  // Execute resolved command with systematic error propagation
  switch (context->command) {
  case NLINK_CMD_CONFIG_CHECK:
    return nlink_cli_execute_config_check(context);
  case NLINK_CMD_DISCOVER_COMPONENTS:
    return nlink_cli_execute_component_discovery(context);
  case NLINK_CMD_VALIDATE_THREADING:
    return nlink_cli_execute_threading_validation(context);
  case NLINK_CMD_PARSE_ONLY:
    return nlink_cli_execute_parse_only(context);
  default:
    NLINK_CLI_ERROR(context, "Unrecognized command: %d", context->command);
    return NLINK_CLI_ERROR_INVALID_ARGUMENTS;
  }
}

// =============================================================================
// CLI COMMAND EXECUTION IMPLEMENTATIONS
// =============================================================================

nlink_cli_result_t
nlink_cli_execute_config_check(nlink_cli_context_t *context) {
  NLINK_CLI_VERBOSE(context,
                    "Executing comprehensive configuration validation");

  // Parse configuration using injected parser function
  nlink_pkg_config_t config;
  nlink_config_result_t parse_result =
      context->config_parser_func(context->config_file_path, &config);

  if (parse_result != NLINK_CONFIG_SUCCESS) {
    NLINK_CLI_ERROR(context, "Configuration parsing failed: %d", parse_result);
    return NLINK_CLI_ERROR_PARSE_FAILED;
  }

  NLINK_CLI_VERBOSE(context, "Configuration parsed successfully");

  // Detect pass mode using injected detector function
  nlink_pass_mode_t detected_mode =
      context->mode_detector_func(context->project_root_path);
  if (detected_mode == NLINK_PASS_MODE_UNKNOWN) {
    NLINK_CLI_WARNING(context,
                      "Unable to determine pass mode from project structure");
  } else if (detected_mode != config.pass_mode) {
    NLINK_CLI_WARNING(
        context, "Configured pass mode (%d) differs from detected mode (%d)",
        config.pass_mode, detected_mode);
  }

  // Discover components using injected discovery function
  int discovered_count =
      context->component_discovery_func(context->project_root_path, &config);
  if (discovered_count < 0) {
    NLINK_CLI_ERROR(context, "Component discovery failed");
    return NLINK_CLI_ERROR_COMPONENT_DISCOVERY_FAILED;
  }

  NLINK_CLI_VERBOSE(context, "Discovered %d components", discovered_count);

  // Validate configuration using injected validation function
  nlink_config_result_t validation_result = context->validation_func(&config);
  if (validation_result != NLINK_CONFIG_SUCCESS) {
    NLINK_CLI_ERROR(context, "Configuration validation failed: %d",
                    validation_result);
    return NLINK_CLI_ERROR_VALIDATION_FAILED;
  }

  // Display comprehensive configuration summary
  nlink_cli_display_config_summary(&config, context->json_output_format);

  // Print decision matrix for systematic analysis
  if (context->verbose_output) {
    nlink_print_decision_matrix(&config);
  }

  printf("[NLINK SUCCESS] Configuration validation completed successfully\n");
  printf("Warnings: %d, Errors: %d\n", context->warning_count,
         context->error_count);

  return NLINK_CLI_SUCCESS;
}

nlink_cli_result_t
nlink_cli_execute_component_discovery(nlink_cli_context_t *context) {
  NLINK_CLI_VERBOSE(context, "Executing systematic component discovery");

  // Parse minimal configuration for component context
  nlink_pkg_config_t config;
  nlink_config_result_t parse_result =
      context->config_parser_func(context->config_file_path, &config);

  if (parse_result != NLINK_CONFIG_SUCCESS) {
    NLINK_CLI_ERROR(context, "Configuration parsing failed: %d", parse_result);
    return NLINK_CLI_ERROR_PARSE_FAILED;
  }

  // Execute comprehensive component discovery
  int discovered_count =
      context->component_discovery_func(context->project_root_path, &config);
  if (discovered_count < 0) {
    NLINK_CLI_ERROR(context, "Component discovery failed");
    return NLINK_CLI_ERROR_COMPONENT_DISCOVERY_FAILED;
  }

  // Display discovery results with systematic formatting
  nlink_cli_display_component_results(&config, context->verbose_output);

  printf("[NLINK SUCCESS] Component discovery completed: %d components found\n",
         discovered_count);

  return NLINK_CLI_SUCCESS;
}

nlink_cli_result_t
nlink_cli_execute_threading_validation(nlink_cli_context_t *context) {
  NLINK_CLI_VERBOSE(context, "Executing threading configuration validation");

  // Parse configuration with focus on threading parameters
  nlink_pkg_config_t config;
  nlink_config_result_t parse_result =
      context->config_parser_func(context->config_file_path, &config);

  if (parse_result != NLINK_CONFIG_SUCCESS) {
    NLINK_CLI_ERROR(context, "Configuration parsing failed: %d", parse_result);
    return NLINK_CLI_ERROR_PARSE_FAILED;
  }

  // Validate threading configuration with systematic checks
  if (config.thread_pool.worker_count == 0 ||
      config.thread_pool.worker_count > 64) {
    NLINK_CLI_ERROR(context, "Invalid worker count: %d (must be 1-64)",
                    config.thread_pool.worker_count);
    return NLINK_CLI_ERROR_THREADING_INVALID;
  }

  if (config.thread_pool.queue_depth == 0 ||
      config.thread_pool.queue_depth > 1024) {
    NLINK_CLI_ERROR(context, "Invalid queue depth: %d (must be 1-1024)",
                    config.thread_pool.queue_depth);
    return NLINK_CLI_ERROR_THREADING_INVALID;
  }

  if (config.thread_pool.stack_size_kb < 64 ||
      config.thread_pool.stack_size_kb > 8192) {
    NLINK_CLI_WARNING(
        context,
        "Stack size %d KB may be suboptimal (recommended: 512-2048 KB)",
        config.thread_pool.stack_size_kb);
  }

  // Display threading analysis with performance projections
  nlink_cli_display_threading_analysis(&config);

  printf("[NLINK SUCCESS] Threading configuration validation completed\n");

  return NLINK_CLI_SUCCESS;
}

nlink_cli_result_t nlink_cli_execute_parse_only(nlink_cli_context_t *context) {
  NLINK_CLI_VERBOSE(context,
                    "Executing parse-only operation without validation");

  // Parse configuration without validation overhead
  nlink_pkg_config_t config;
  nlink_config_result_t parse_result =
      context->config_parser_func(context->config_file_path, &config);

  if (parse_result != NLINK_CONFIG_SUCCESS) {
    NLINK_CLI_ERROR(context, "Configuration parsing failed: %d", parse_result);
    return NLINK_CLI_ERROR_PARSE_FAILED;
  }

  // Display minimal configuration information
  printf("Project: %s (v%s)\n", config.project_name, config.project_version);
  printf("Entry Point: %s\n", config.entry_point);
  printf("Pass Mode: %s\n",
         config.pass_mode == NLINK_PASS_MODE_SINGLE  ? "Single-Pass"
         : config.pass_mode == NLINK_PASS_MODE_MULTI ? "Multi-Pass"
                                                     : "Unknown");
  printf("Features: %d configured\n", config.feature_count);

  printf("[NLINK SUCCESS] Configuration parsing completed\n");

  return NLINK_CLI_SUCCESS;
}

// =============================================================================
// CLI DISPLAY FUNCTIONS IMPLEMENTATION
// =============================================================================

void nlink_cli_display_help(const char *program_name) {
  printf("NexusLink Configuration Parser - Aegis Project v%d.%d.%d\n",
         NLINK_VERSION_MAJOR, NLINK_VERSION_MINOR, NLINK_VERSION_PATCH);
  printf("Systematic configuration parsing and validation for modular build "
         "systems\n\n");

  printf("USAGE:\n");
  printf("  %s [COMMAND] [OPTIONS]\n\n", program_name);

  printf("COMMANDS:\n");
  printf("  --config-check        Validate configuration and display decision "
         "matrix\n");
  printf("  --discover-components Enumerate project components and "
         "substructure\n");
  printf(
      "  --validate-threading  Verify thread pool configuration consistency\n");
  printf("  --parse-only          Parse configuration without validation\n");
  printf("  --help                Display this help information\n");
  printf("  --version             Display version and build information\n\n");

  printf("OPTIONS:\n");
  printf("  -v, --verbose         Enable verbose output for systematic "
         "analysis\n");
  printf(
      "  -q, --quiet           Suppress warnings and non-critical messages\n");
  printf("  -f, --config-file     Specify explicit configuration file path\n");
  printf("  -r, --project-root    Specify explicit project root directory\n\n");

  printf("EXAMPLES:\n");
  printf("  %s --config-check --verbose\n", program_name);
  printf("  %s --discover-components --project-root /path/to/project\n",
         program_name);
  printf("  %s --validate-threading --config-file custom.nlink\n",
         program_name);
  printf("\nFor technical documentation, consult the Aegis project "
         "specifications.\n");
}

void nlink_cli_display_version(void) {
  printf("NexusLink Configuration Parser\n");
  printf("Version: %d.%d.%d\n", NLINK_VERSION_MAJOR, NLINK_VERSION_MINOR,
         NLINK_VERSION_PATCH);
  printf("Build Date: %s %s\n", NLINK_BUILD_DATE, NLINK_BUILD_TIME);
  printf("Project: Aegis Development Framework\n");
  printf("Author: Nnamdi Michael Okpala & Development Team\n");
  printf("Architecture: Waterfall Methodology with Systematic Validation\n");
}

void nlink_cli_display_config_summary(const nlink_pkg_config_t *config,
                                      bool json_format) {
  if (!config)
    return;

  if (json_format) {
    printf("{\n");
    printf("  \"project_name\": \"%s\",\n", config->project_name);
    printf("  \"project_version\": \"%s\",\n", config->project_version);
    printf("  \"entry_point\": \"%s\",\n", config->entry_point);
    printf("  \"pass_mode\": \"%s\",\n",
           config->pass_mode == NLINK_PASS_MODE_SINGLE ? "single" : "multi");
    printf("  \"component_count\": %d,\n", config->component_count);
    printf("  \"thread_pool\": {\n");
    printf("    \"worker_count\": %d,\n", config->thread_pool.worker_count);
    printf("    \"queue_depth\": %d\n", config->thread_pool.queue_depth);
    printf("  },\n");
    printf("  \"features_enabled\": %d,\n", config->feature_count);
    printf("  \"checksum\": \"0x%08X\"\n", config->config_checksum);
    printf("}\n");
  } else {
    printf("\n=== Configuration Summary ===\n");
    printf("Project: %s (v%s)\n", config->project_name,
           config->project_version);
    printf("Entry Point: %s\n", config->entry_point);
    printf("Build Mode: %s\n", config->pass_mode == NLINK_PASS_MODE_SINGLE
                                   ? "Single-Pass"
                                   : "Multi-Pass");
    printf("Components: %d discovered\n", config->component_count);
    printf("Thread Pool: %d workers, %d queue depth\n",
           config->thread_pool.worker_count, config->thread_pool.queue_depth);
    printf("Features: %d enabled\n", config->feature_count);
    printf("Configuration Checksum: 0x%08X\n", config->config_checksum);
    printf("=============================\n\n");
  }
}

void nlink_cli_display_component_results(const nlink_pkg_config_t *config,
                                         bool verbose_output) {
  if (!config)
    return;

  printf("\n=== Component Discovery Results ===\n");
  printf("Total Components: %d\n\n", config->component_count);

  for (uint32_t i = 0; i < config->component_count; i++) {
    const nlink_component_metadata_t *component = &config->components[i];
    printf("Component %d: %s\n", i + 1, component->component_name);

    if (verbose_output) {
      printf("  Path: %s\n", component->component_path);
      printf("  Version: %s\n", component->version);
      printf("  Has nlink.txt: %s\n", component->has_nlink_txt ? "Yes" : "No");
      printf("  Dependencies: %d\n", component->dependency_count);
    }
    printf("\n");
  }

  printf("===================================\n\n");
}

void nlink_cli_display_threading_analysis(const nlink_pkg_config_t *config) {
  if (!config)
    return;

  printf("\n=== Threading Configuration Analysis ===\n");
  printf("Worker Threads: %d\n", config->thread_pool.worker_count);
  printf("Queue Depth: %d\n", config->thread_pool.queue_depth);
  printf("Stack Size: %d KB per thread\n", config->thread_pool.stack_size_kb);
  printf("Work Stealing: %s\n",
         config->thread_pool.enable_work_stealing ? "Enabled" : "Disabled");
  printf("Thread Affinity: %s\n",
         config->thread_pool.enable_thread_affinity ? "Enabled" : "Disabled");

  // Calculate performance projections
  uint32_t total_memory_kb =
      config->thread_pool.worker_count * config->thread_pool.stack_size_kb;
  printf("\nPerformance Projections:\n");
  printf("  Total Stack Memory: %d KB (%.2f MB)\n", total_memory_kb,
         total_memory_kb / 1024.0);
  printf("  Theoretical Throughput: %d concurrent tasks\n",
         config->thread_pool.queue_depth);
  printf("  Parallelization Factor: %dx\n", config->thread_pool.worker_count);

  printf("========================================\n\n");
}

// =============================================================================
// DEPENDENCY INJECTION IMPLEMENTATION
// =============================================================================

void nlink_cli_inject_config_parser(nlink_cli_context_t *context,
                                    nlink_config_parser_func_t parser_func) {
  if (context && parser_func) {
    context->config_parser_func = parser_func;
  }
}

void nlink_cli_inject_mode_detector(nlink_cli_context_t *context,
                                    nlink_mode_detector_func_t detector_func) {
  if (context && detector_func) {
    context->mode_detector_func = detector_func;
  }
}

void nlink_cli_inject_component_discovery(
    nlink_cli_context_t *context,
    nlink_component_discovery_func_t discovery_func) {
  if (context && discovery_func) {
    context->component_discovery_func = discovery_func;
  }
}

void nlink_cli_inject_validation(nlink_cli_context_t *context,
                                 nlink_validation_func_t validation_func) {
  if (context && validation_func) {
    context->validation_func = validation_func;
  }
}

bool nlink_cli_validate_context(const nlink_cli_context_t *context) {
  return context && context->is_initialized && context->config_parser_func &&
         context->mode_detector_func && context->component_discovery_func &&
         context->validation_func;
}

void nlink_cli_cleanup(nlink_cli_context_t *context) {
  if (!context)
    return;

  // Reset execution state with systematic cleanup
  memset(context, 0, sizeof(nlink_cli_context_t));
}
