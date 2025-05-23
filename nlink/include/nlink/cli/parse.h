/**
 * @file parse.h
 * @brief Command input parsing system for NexusLink CLI
 * 
 * Provides parsing functionality for command input strings with support
 * for command arguments, options, and pattern matching integration.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_PARSE_H
 #define NLINK_PARSE_H
 
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/common/result.h"
 #include "nlink/cli/command_params.h"
 #include <stdbool.h>
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Parse options structure
  */
 typedef struct NlinkParseOptions {
     bool allow_quoted_strings;    /**< Whether to handle quoted strings */
     bool handle_escapes;          /**< Whether to handle escape sequences */
     bool allow_environment_vars;  /**< Whether to expand environment variables */
     bool case_sensitive;          /**< Whether parsing is case-sensitive */
 } NlinkParseOptions;
 
 /**
  * @brief Parse result structure
  */
 typedef struct NlinkParseResult {
     char* command;              /**< The command name */
     int argc;                   /**< Number of arguments */
     char** argv;                /**< Array of arguments */
     NlinkCommandParams* params; /**< Extracted parameters (if requested) */
     NexusResult result;         /**< Result code */
     char* error_message;        /**< Error message (if any) */
 } NlinkParseResult;
 
 /**
  * @brief Get default parse options
  * 
  * @return NlinkParseOptions Default options
  */
 NlinkParseOptions nlink_parse_default_options(void);
 
 /**
  * @brief Parse a command string into arguments
  * 
  * @param input Input string to parse
  * @param options Parse options
  * @param result Parse result (must be freed with nlink_parse_result_free)
  * @return NexusResult Result code
  */
 NexusResult nlink_parse_command_string(
     const char* input,
     const NlinkParseOptions* options,
     NlinkParseResult** result);
 
 /**
  * @brief Parse a command string and extract parameters
  * 
  * @param input Input string to parse
  * @param pattern Pattern to match (can be NULL for standard parsing)
  * @param param_names Parameter names (can be NULL if pattern is NULL)
  * @param param_count Number of parameter names
  * @param options Parse options
  * @param result Parse result (must be freed with nlink_parse_result_free)
  * @return NexusResult Result code
  */
 NexusResult nlink_parse_command_with_params(
     const char* input,
     const char* pattern,
     const char** param_names,
     size_t param_count,
     const NlinkParseOptions* options,
     NlinkParseResult** result);
 
 /**
  * @brief Parse a file for commands
  * 
  * @param filename File to parse
  * @param options Parse options
  * @param results Array of parse results (must be freed with nlink_parse_result_free)
  * @param result_count Number of results
  * @return NexusResult Result code
  */
 NexusResult nlink_parse_file(
     const char* filename,
     const NlinkParseOptions* options,
     NlinkParseResult*** results,
     size_t* result_count);
 
 /**
  * @brief Free parse result resources
  * 
  * @param result Parse result to free
  */
 void nlink_parse_result_free(NlinkParseResult* result);
 
 /**
  * @brief Convert params to arguments array
  * 
  * @param params Parameter list
  * @param argc Output parameter for argument count
  * @param argv Output parameter for argument array (must be freed by caller)
  * @return NexusResult Result code
  */
 NexusResult nlink_parse_params_to_args(
     const NlinkCommandParams* params,
     int* argc,
     char*** argv);
 
 /**
  * @brief Check if a string needs quoting for shell use
  * 
  * @param str String to check
  * @return bool True if string needs quoting
  */
 bool nlink_parse_needs_quoting(const char* str);
 
 /**
  * @brief Escape a string for shell use
  * 
  * @param str String to escape
  * @return char* Escaped string (must be freed by caller)
  */
 char* nlink_parse_escape_string(const char* str);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_PARSE_H */