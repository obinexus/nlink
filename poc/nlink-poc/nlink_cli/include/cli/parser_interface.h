/**
 * @file parser_interface.h
 * @brief NexusLink CLI Parser Interface for Configuration Validation
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.0.0
 *
 * Command-line interface wrapper for configuration parsing and validation.
 * Implements inversion-of-control pattern for systematic testing and modular
 * design.
 *
 * Supported Commands:
 * - nlink --config-check: Validate configuration and display decision matrix
 * - nlink --discover-components: Enumerate project components and substructure
 * - nlink --validate-threading: Verify thread pool configuration consistency
 * - nlink --parse-only: Parse configuration without validation or execution
 */

#ifndef NLINK_CLI_PARSER_INTERFACE_H
#define NLINK_CLI_PARSER_INTERFACE_H

#include "core/config.h"
#include <stdbool.h>
#include <stdint.h>

// =============================================================================
// CLI COMMAND ENUMERATION AND RESULT CODES
// =============================================================================

/**
 * @brief CLI command enumeration for systematic command parsing
 */
typedef enum {
  NLINK_CMD_UNKNOWN = 0,
  NLINK_CMD_CONFIG_CHECK,        // Validate and display configuration
  NLINK_CMD_DISCOVER_COMPONENTS, // Enumerate project components
  NLINK_CMD_VALIDATE_THREADING,  // Verify thread pool configuration
  NLINK_CMD_PARSE_ONLY,          // Parse without validation
  NLINK_CMD_HELP,                // Display usage information
  NLINK_CMD_VERSION              // Display version information
} nlink_cli_command_t;

/**
 * @brief CLI execution result codes following waterfall error propagation
 */
typedef enum {
  NLINK_CLI_SUCCESS = 0,
  NLINK_CLI_ERROR_INVALID_ARGUMENTS = 1,
  NLINK_CLI_ERROR_CONFIG_NOT_FOUND = 2,
  NLINK_CLI_ERROR_PARSE_FAILED = 3,
  NLINK_CLI_ERROR_VALIDATION_FAILED = 4,
  NLINK_CLI_ERROR_THREADING_INVALID = 5,
  NLINK_CLI_ERROR_COMPONENT_DISCOVERY_FAILED = 6,
  NLINK_CLI_ERROR_INTERNAL_ERROR = 7
} nlink_cli_result_t;

// =============================================================================
// CLI CONTEXT AND CONFIGURATION STRUCTURES
// =============================================================================

/**
 * @brief CLI execution context with dependency injection support
 */
typedef struct {
  // Command configuration
  nlink_cli_command_t command;
  char project_root_path[NLINK_MAX_PATH_LENGTH];
  char config_file_path[NLINK_MAX_PATH_LENGTH];

  // Execution options
  bool verbose_output;
  bool strict_validation;
  bool suppress_warnings;
  bool json_output_format;

  // Dependency injection hooks for testing
  nlink_config_result_t (*config_parser_func)(const char *,
                                              nlink_pkg_config_t *);
  nlink_pass_mode_t (*mode_detector_func)(const char *);
  int (*component_discovery_func)(const char *, nlink_pkg_config_t *);
  nlink_config_result_t (*validation_func)(const nlink_pkg_config_t *);

  // Execution state
  bool is_initialized;
  struct timespec execution_start_time;
  uint32_t warning_count;
  uint32_t error_count;
} nlink_cli_context_t;

/**
 * @brief CLI command argument structure for systematic parameter handling
 */
typedef struct {
  int argc;
  char **argv;
  char *program_name;

  // Parsed command line options
  bool help_requested;
  bool version_requested;
  bool verbose_mode;
  bool quiet_mode;
  char *explicit_config_path;
  char *explicit_project_root;
} nlink_cli_args_t;

// =============================================================================
// CLI INTERFACE FUNCTION DECLARATIONS
// =============================================================================

/**
 * @brief Initialize CLI context with default dependency injection functions
 * @param context CLI context structure to initialize
 * @return NLINK_CLI_SUCCESS on successful initialization
 */
nlink_cli_result_t nlink_cli_init(nlink_cli_context_t *context);

/**
 * @brief Parse command line arguments using systematic argument processing
 * @param argc Argument count from main()
 * @param argv Argument vector from main()
 * @param args Output structure for parsed arguments
 * @return NLINK_CLI_SUCCESS if arguments parsed successfully
 */
nlink_cli_result_t nlink_cli_parse_args(int argc, char *argv[],
                                        nlink_cli_args_t *args);

/**
 * @brief Execute CLI command with full error handling and validation
 * @param context Initialized CLI context
 * @param args Parsed command line arguments
 * @return CLI result code following waterfall error propagation
 */
nlink_cli_result_t nlink_cli_execute(nlink_cli_context_t *context,
                                     const nlink_cli_args_t *args);

/**
 * @brief Execute configuration check command with comprehensive validation
 * @param context CLI execution context
 * @return NLINK_CLI_SUCCESS if configuration is valid and complete
 */
nlink_cli_result_t nlink_cli_execute_config_check(nlink_cli_context_t *context);

/**
 * @brief Execute component discovery with systematic enumeration
 * @param context CLI execution context
 * @return NLINK_CLI_SUCCESS if components discovered successfully
 */
nlink_cli_result_t
nlink_cli_execute_component_discovery(nlink_cli_context_t *context);

/**
 * @brief Execute threading validation with pool configuration analysis
 * @param context CLI execution context
 * @return NLINK_CLI_SUCCESS if thread configuration is valid
 */
nlink_cli_result_t
nlink_cli_execute_threading_validation(nlink_cli_context_t *context);

/**
 * @brief Execute parse-only operation without validation overhead
 * @param context CLI execution context
 * @return NLINK_CLI_SUCCESS if parsing completed without errors
 */
nlink_cli_result_t nlink_cli_execute_parse_only(nlink_cli_context_t *context);

/**
 * @brief Display comprehensive help information for all commands
 * @param program_name Name of the executable for usage display
 */
void nlink_cli_display_help(const char *program_name);

/**
 * @brief Display version information and build metadata
 */
void nlink_cli_display_version(void);

/**
 * @brief Display detailed configuration summary in structured format
 * @param config Configuration structure to display
 * @param json_format Whether to output in JSON format
 */
void nlink_cli_display_config_summary(const nlink_pkg_config_t *config,
                                      bool json_format);

/**
 * @brief Display component discovery results with hierarchical structure
 * @param config Configuration with discovered components
 * @param verbose_output Whether to include detailed component information
 */
void nlink_cli_display_component_results(const nlink_pkg_config_t *config,
                                         bool verbose_output);

/**
 * @brief Display threading configuration analysis with performance projections
 * @param config Configuration with thread pool settings
 */
void nlink_cli_display_threading_analysis(const nlink_pkg_config_t *config);

/**
 * @brief Clean up CLI context and release allocated resources
 * @param context CLI context to clean up
 */
void nlink_cli_cleanup(nlink_cli_context_t *context);

// =============================================================================
// DEPENDENCY INJECTION FUNCTION TYPEDEFS
// =============================================================================

/**
 * @brief Function typedef for configuration parser dependency injection
 */
typedef nlink_config_result_t (*nlink_config_parser_func_t)(
    const char *, nlink_pkg_config_t *);

/**
 * @brief Function typedef for mode detection dependency injection
 */
typedef nlink_pass_mode_t (*nlink_mode_detector_func_t)(const char *);

/**
 * @brief Function typedef for component discovery dependency injection
 */
typedef int (*nlink_component_discovery_func_t)(const char *,
                                                nlink_pkg_config_t *);

/**
 * @brief Function typedef for validation dependency injection
 */
typedef nlink_config_result_t (*nlink_validation_func_t)(
    const nlink_pkg_config_t *);

// =============================================================================
// CLI TESTING AND VALIDATION SUPPORT
// =============================================================================

/**
 * @brief Inject custom configuration parser for systematic testing
 * @param context CLI context to modify
 * @param parser_func Custom parser function for testing scenarios
 */
void nlink_cli_inject_config_parser(nlink_cli_context_t *context,
                                    nlink_config_parser_func_t parser_func);

/**
 * @brief Inject custom mode detector for pass-mode testing
 * @param context CLI context to modify
 * @param detector_func Custom mode detector for testing scenarios
 */
void nlink_cli_inject_mode_detector(nlink_cli_context_t *context,
                                    nlink_mode_detector_func_t detector_func);

/**
 * @brief Inject custom component discovery for isolation testing
 * @param context CLI context to modify
 * @param discovery_func Custom discovery function for testing scenarios
 */
void nlink_cli_inject_component_discovery(
    nlink_cli_context_t *context,
    nlink_component_discovery_func_t discovery_func);

/**
 * @brief Inject custom validation function for error condition testing
 * @param context CLI context to modify
 * @param validation_func Custom validation function for testing scenarios
 */
void nlink_cli_inject_validation(nlink_cli_context_t *context,
                                 nlink_validation_func_t validation_func);

/**
 * @brief Validate CLI context integrity for systematic testing verification
 * @param context CLI context to validate
 * @return true if context is properly initialized and consistent
 */
bool nlink_cli_validate_context(const nlink_cli_context_t *context);

// =============================================================================
// CLI UTILITY MACROS FOR SYSTEMATIC ERROR HANDLING
// =============================================================================

/**
 * @brief Macro for standardized CLI error reporting with context preservation
 */
#define NLINK_CLI_ERROR(context, format, ...)                                  \
  do {                                                                         \
    if (!(context)->suppress_warnings) {                                       \
      fprintf(stderr, "[NLINK ERROR] " format "\n", ##__VA_ARGS__);            \
    }                                                                          \
    (context)->error_count++;                                                  \
  } while (0)

/**
 * @brief Macro for standardized CLI warning reporting with optional suppression
 */
#define NLINK_CLI_WARNING(context, format, ...)                                \
  do {                                                                         \
    if (!(context)->suppress_warnings && (context)->verbose_output) {          \
      fprintf(stderr, "[NLINK WARNING] " format "\n", ##__VA_ARGS__);          \
    }                                                                          \
    (context)->warning_count++;                                                \
  } while (0)

/**
 * @brief Macro for verbose CLI output with conditional display
 */
#define NLINK_CLI_VERBOSE(context, format, ...)                                \
  do {                                                                         \
    if ((context)->verbose_output) {                                           \
      printf("[NLINK VERBOSE] " format "\n", ##__VA_ARGS__);                   \
    }                                                                          \
  } while (0)

#endif // NLINK_CLI_PARSER_INTERFACE_H
