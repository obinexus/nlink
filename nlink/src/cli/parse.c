/**
 * @file parse.c
 * @brief Implementation of command input parsing system for NexusLink CLI
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/cli/parse.h"
 #include "nlink/cli/command_params.h"
 #include "nlink/core/pattern/matcher.h"
 #include "nlink/core/common/nexus_core.h"
 #include <string.h>
 #include <stdlib.h>
 #include <ctype.h>
 #include <stdio.h>
 
 /* Maximum length for a command string */
 #define MAX_COMMAND_LENGTH 4096
 
 /* Maximum number of arguments */
 #define MAX_ARGS 256
 
 /**
  * @brief Initialize a parse result structure
  * 
  * @return NlinkParseResult* New parse result or NULL on failure
  */
 static NlinkParseResult* create_parse_result(void) {
     NlinkParseResult* result = (NlinkParseResult*)malloc(sizeof(NlinkParseResult));
     if (!result) {
         return NULL;
     }
     
     /* Initialize with default values */
     result->command = NULL;
     result->argc = 0;
     result->argv = NULL;
     result->params = NULL;
     result->result = NEXUS_SUCCESS;
     result->error_message = NULL;
     
     return result;
 }
 
 /**
  * @brief Set error message in parse result
  * 
  * @param result Parse result
  * @param format Format string (printf-style)
  * @param ... Additional arguments
  */
 static void set_parse_error(NlinkParseResult* result, const char* format, ...) {
     if (!result) {
         return;
     }
     
     /* Clear any existing error message */
     if (result->error_message) {
         free(result->error_message);
         result->error_message = NULL;
     }
     
     /* Format the error message */
     char buffer[512];
     va_list args;
     va_start(args, format);
     vsnprintf(buffer, sizeof(buffer), format, args);
     va_end(args);
     
     /* Copy the error message */
     result->error_message = strdup(buffer);
     result->result = NEXUS_INVALID_PARAMETER;
 }
 
 /**
  * @brief Check if a character is a whitespace character
  * 
  * @param c Character to check
  * @return bool True if whitespace
  */
 static bool is_whitespace(char c) {
     return c == ' ' || c == '\t' || c == '\n' || c == '\r';
 }
 
 /**
  * @brief Get default parse options
  * 
  * @return NlinkParseOptions Default options
  */
 NlinkParseOptions nlink_parse_default_options(void) {
     NlinkParseOptions options;
     options.allow_quoted_strings = true;
     options.handle_escapes = true;
     options.allow_environment_vars = true;
     options.case_sensitive = false;
     return options;
 }
 
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
     NlinkParseResult** result) {
     
     if (!input || !result) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     /* Use default options if not provided */
     NlinkParseOptions default_options;
     if (!options) {
         default_options = nlink_parse_default_options();
         options = &default_options;
     }
     
     /* Create parse result */
     NlinkParseResult* parse_result = create_parse_result();
     if (!parse_result) {
         return NEXUS_OUT_OF_MEMORY;
     }
     
     /* Allocate argv array */
     char** argv = (char**)malloc(MAX_ARGS * sizeof(char*));
     if (!argv) {
         nlink_parse_result_free(parse_result);
         return NEXUS_OUT_OF_MEMORY;
     }
     
     /* Initialize argv array */
     for (int i = 0; i < MAX_ARGS; i++) {
         argv[i] = NULL;
     }
     
     /* Skip leading whitespace */
     const char* p = input;
     while (*p && is_whitespace(*p)) {
         p++;
     }
     
     if (!*p) {
         /* Empty input */
         parse_result->argc = 0;
         parse_result->argv = argv;
         *result = parse_result;
         return NEXUS_SUCCESS;
     }
     
     /* Parse command and arguments */
     int argc = 0;
     
     while (*p && argc < MAX_ARGS) {
         /* Skip whitespace */
         while (*p && is_whitespace(*p)) {
             p++;
         }
         
         if (!*p) {
             break;
         }
         
         /* Start of an argument */
         const char* arg_start = p;
         char* arg = NULL;
         
         if (options->allow_quoted_strings && *p == '"') {
             /* Quoted string */
             p++; /* Skip opening quote */
             arg_start = p;
             
             /* Find closing quote */
             while (*p && *p != '"') {
                 if (options->handle_escapes && *p == '\\' && *(p+1)) {
                     p += 2; /* Skip escape and escaped character */
                 } else {
                     p++;
                 }
             }
             
             if (!*p) {
                 /* Missing closing quote */
                 set_parse_error(parse_result, "Missing closing quote for argument %d", argc);
                 goto error;
             }
             
             /* Copy argument */
             size_t arg_len = p - arg_start;
             arg = (char*)malloc(arg_len + 1);
             if (!arg) {
                 set_parse_error(parse_result, "Memory allocation failed for argument %d", argc);
                 goto error;
             }
             
             /* Copy characters, handling escapes */
             const char* s = arg_start;
             char* d = arg;
             while (s < p) {
                 if (options->handle_escapes && *s == '\\' && *(s+1)) {
                     s++;
                     switch (*s) {
                         case 'n': *d++ = '\n'; break;
                         case 'r': *d++ = '\r'; break;
                         case 't': *d++ = '\t'; break;
                         default: *d++ = *s; break;
                     }
                     s++;
                 } else {
                     *d++ = *s++;
                 }
             }
             *d = '\0';
             
             p++; /* Skip closing quote */
         } else {
             /* Unquoted argument */
             while (*p && !is_whitespace(*p)) {
                 if (options->handle_escapes && *p == '\\' && *(p+1)) {
                     p += 2; /* Skip escape and escaped character */
                 } else {
                     p++;
                 }
             }
             
             /* Copy argument */
             size_t arg_len = p - arg_start;
             arg = (char*)malloc(arg_len + 1);
             if (!arg) {
                 set_parse_error(parse_result, "Memory allocation failed for argument %d", argc);
                 goto error;
             }
             
             /* Copy characters, handling escapes */
             const char* s = arg_start;
             char* d = arg;
             while (s < p) {
                 if (options->handle_escapes && *s == '\\' && *(s+1)) {
                     s++;
                     switch (*s) {
                         case 'n': *d++ = '\n'; break;
                         case 'r': *d++ = '\r'; break;
                         case 't': *d++ = '\t'; break;
                         default: *d++ = *s; break;
                     }
                     s++;
                 } else {
                     *d++ = *s++;
                 }
             }
             *d = '\0';
         }
         
         /* Add argument to argv */
         argv[argc++] = arg;
         
         /* Process environment variables if enabled */
         if (options->allow_environment_vars && arg[0] == '$' && arg[1]) {
             /* Environment variable reference */
             char* env_name = arg + 1;
             char* env_value = getenv(env_name);
             
             if (env_value) {
                 /* Replace the reference with the value */
                 free(arg);
                 argv[argc - 1] = strdup(env_value);
                 if (!argv[argc - 1]) {
                     set_parse_error(parse_result, "Memory allocation failed for environment variable %s", env_name);
                     goto error;
                 }
             }
         }
     }
     
     /* Set the command name (first argument) */
     if (argc > 0) {
         parse_result->command = strdup(argv[0]);
         if (!parse_result->command) {
             set_parse_error(parse_result, "Memory allocation failed for command name");
             goto error;
         }
     }
     
     /* Set the result */
     parse_result->argc = argc;
     parse_result->argv = argv;
     *result = parse_result;
     
     return NEXUS_SUCCESS;
     
 error:
     /* Clean up on error */
     for (int i = 0; i < argc; i++) {
         free(argv[i]);
     }
     free(argv);
     *result = parse_result;
     return parse_result->result;
 }
 
 /**
  * @brief Parse a command string and extract parameters using a pattern
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
     NlinkParseResult** result) {
     
     if (!input || !result) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     /* Use default options if not provided */
     NlinkParseOptions default_options;
     if (!options) {
         default_options = nlink_parse_default_options();
         options = &default_options;
     }
     
     /* Create parse result */
     NlinkParseResult* parse_result = create_parse_result();
     if (!parse_result) {
         return NEXUS_OUT_OF_MEMORY;
     }
     
     /* If no pattern provided, use standard parsing */
     if (!pattern) {
         NexusResult parse_result_code = nlink_parse_command_string(input, options, result);
         
         /* If requested, create an empty params object */
         if (parse_result_code == NEXUS_SUCCESS && *result) {
             (*result)->params = nlink_command_params_create();
             if (!(*result)->params) {
                 (*result)->result = NEXUS_OUT_OF_MEMORY;
                 return NEXUS_OUT_OF_MEMORY;
             }
         }
         
         return parse_result_code;
     }
     
     /* Use pattern matching for parameter extraction */
     NlinkPatternFlags flags = NLINK_PATTERN_FLAG_REGEX;
     if (!options->case_sensitive) {
         flags |= NLINK_PATTERN_FLAG_CASE_INSENSITIVE;
     }
     
     /* Create pattern matcher */
     NlinkPatternMatcher* matcher = nlink_pattern_create(pattern, flags);
     if (!matcher) {
         set_parse_error(parse_result, "Failed to create pattern matcher");
         *result = parse_result;
         return NEXUS_OUT_OF_MEMORY;
     }
     
     /* Match the input against the pattern */
     NlinkMatchInfo* match_info = NULL;
     if (!nlink_pattern_match_with_params(matcher, input, &match_info)) {
         set_parse_error(parse_result, "Input does not match pattern: %s", pattern);
         nlink_pattern_destroy(matcher);
         *result = parse_result;
         return NEXUS_INVALID_PARAMETER;
     }
     
     /* Extract parameters */
     parse_result->params = nlink_command_params_create();
     if (!parse_result->params) {
         set_parse_error(parse_result, "Failed to create parameters object");
         nlink_pattern_destroy(matcher);
         nlink_match_info_destroy(match_info);
         *result = parse_result;
         return NEXUS_OUT_OF_MEMORY;
     }
     
     /* Get command name (first capture group) */
     size_t group_count = nlink_match_info_get_group_count(match_info);
     if (group_count > 0) {
         parse_result->command = strdup(nlink_match_info_get_group(match_info, 0));
         if (!parse_result->command) {
             set_parse_error(parse_result, "Memory allocation failed for command name");
             nlink_pattern_destroy(matcher);
             nlink_match_info_destroy(match_info);
             *result = parse_result;
             return NEXUS_OUT_OF_MEMORY;
         }
     }
     
     /* Add parameters */
     for (size_t i = 1; i < group_count && i <= param_count; i++) {
         const char* value = nlink_match_info_get_group(match_info, i);
         const char* name = (i <= param_count) ? param_names[i - 1] : NULL;
         
         if (name && value) {
             NexusResult param_result = nlink_command_params_add(parse_result->params, name, value);
             if (param_result != NEXUS_SUCCESS) {
                 set_parse_error(parse_result, "Failed to add parameter %s with value %s", name, value);
                 nlink_pattern_destroy(matcher);
                 nlink_match_info_destroy(match_info);
                 *result = parse_result;
                 return param_result;
             }
         }
     }
     
     /* Convert parameters to arguments */
     NexusResult args_result = nlink_parse_params_to_args(
         parse_result->params,
         &parse_result->argc,
         &parse_result->argv
     );
     
     if (args_result != NEXUS_SUCCESS) {
         set_parse_error(parse_result, "Failed to convert parameters to arguments");
         nlink_pattern_destroy(matcher);
         nlink_match_info_destroy(match_info);
         *result = parse_result;
         return args_result;
     }
     
     /* Clean up */
     nlink_pattern_destroy(matcher);
     nlink_match_info_destroy(match_info);
     
     /* Set the result */
     *result = parse_result;
     return NEXUS_SUCCESS;
 }
 
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
     size_t* result_count) {
     
     if (!filename || !results || !result_count) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     /* Use default options if not provided */
     NlinkParseOptions default_options;
     if (!options) {
         default_options = nlink_parse_default_options();
         options = &default_options;
     }
     
     /* Open the file */
     FILE* file = fopen(filename, "r");
     if (!file) {
         return NEXUS_NOT_FOUND;
     }
     
     /* Allocate result array */
     size_t capacity = 16;
     NlinkParseResult** result_array = (NlinkParseResult**)malloc(capacity * sizeof(NlinkParseResult*));
     if (!result_array) {
         fclose(file);
         return NEXUS_OUT_OF_MEMORY;
     }
     
     /* Initialize count */
     size_t count = 0;
     
     /* Read each line */
     char line[MAX_COMMAND_LENGTH];
     while (fgets(line, sizeof(line), file)) {
         /* Skip empty lines and comments */
         char* p = line;
         while (*p && is_whitespace(*p)) {
             p++;
         }
         
         if (!*p || *p == '#') {
             continue;
         }
         
         /* Remove trailing newline */
         size_t len = strlen(line);
         if (len > 0 && line[len - 1] == '\n') {
             line[len - 1] = '\0';
         }
         
         /* Parse the line */
         NlinkParseResult* parse_result = NULL;
         NexusResult parse_result_code = nlink_parse_command_string(line, options, &parse_result);
         
         if (parse_result_code != NEXUS_SUCCESS || !parse_result) {
             continue;
         }
         
         /* Add to result array */
         if (count >= capacity) {
             /* Resize array */
             capacity *= 2;
             NlinkParseResult** new_array = (NlinkParseResult**)realloc(
                 result_array, capacity * sizeof(NlinkParseResult*)
             );
             
             if (!new_array) {
                 /* Clean up */
                 for (size_t i = 0; i < count; i++) {
                     nlink_parse_result_free(result_array[i]);
                 }
                 free(result_array);
                 fclose(file);
                 return NEXUS_OUT_OF_MEMORY;
             }
             
             result_array = new_array;
         }
         
         result_array[count++] = parse_result;
     }
     
     /* Close the file */
     fclose(file);
     
     /* Set the results */
     *results = result_array;
     *result_count = count;
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * @brief Free parse result resources
  * 
  * @param result Parse result to free
  */
 void nlink_parse_result_free(NlinkParseResult* result) {
     if (!result) {
         return;
     }
     
     /* Free command */
     if (result->command) {
         free(result->command);
     }
     
     /* Free arguments */
     if (result->argv) {
         for (int i = 0; i < result->argc; i++) {
             if (result->argv[i]) {
                 free(result->argv[i]);
             }
         }
         free(result->argv);
     }
     
     /* Free parameters */
     if (result->params) {
         nlink_command_params_destroy(result->params);
     }
     
     /* Free error message */
     if (result->error_message) {
         free(result->error_message);
     }
     
     /* Free result structure */
     free(result);
 }
 
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
     char*** argv) {
     
     if (!params || !argc || !argv) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     /* Get parameter count */
     size_t param_count = nlink_command_params_count(params);
     
     /* Allocate argv array (we need 2 args per param, plus the command itself) */
     char** args = (char**)malloc((param_count * 2 + 1) * sizeof(char*));
     if (!args) {
         return NEXUS_OUT_OF_MEMORY;
     }
     
     /* Initialize arguments */
     int arg_count = 0;
     
     /* Add parameter arguments */
     for (size_t i = 0; i < param_count; i++) {
         const char* name;
         const char* value;
         
         if (nlink_command_params_get_at(params, i, &name, &value)) {
             /* Add parameter name as an option (--name) */
             args[arg_count] = (char*)malloc(strlen(name) + 3);
             if (!args[arg_count]) {
                 /* Clean up */
                 for (int j = 0; j < arg_count; j++) {
                     free(args[j]);
                 }
                 free(args);
                 return NEXUS_OUT_OF_MEMORY;
             }
             
             sprintf(args[arg_count], "--%s", name);
             arg_count++;
             
             /* Add parameter value if present */
             if (value) {
                 args[arg_count] = strdup(value);
                 if (!args[arg_count]) {
                     /* Clean up */
                     for (int j = 0; j < arg_count; j++) {
                         free(args[j]);
                     }
                     free(args);
                     return NEXUS_OUT_OF_MEMORY;
                 }
                 arg_count++;
             }
         }
     }
     
     /* Set the results */
     *argc = arg_count;
     *argv = args;
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * @brief Check if a string needs quoting for shell use
  * 
  * @param str String to check
  * @return bool True if string needs quoting
  */
 bool nlink_parse_needs_quoting(const char* str) {
     if (!str) {
         return false;
     }
     
     /* Check for characters that need quoting */
     const char* p = str;
     while (*p) {
         if (is_whitespace(*p) || *p == '"' || *p == '\'' || *p == '\\' ||
             *p == '&' || *p == '|' || *p == ';' || *p == '<' || *p == '>' ||
             *p == '(' || *p == ')' || *p == '$' || *p == '`' || *p == '*' ||
             *p == '?' || *p == '[' || *p == ']' || *p == '#' || *p == '~') {
             return true;
         }
         p++;
     }
     
     return false;
 }
 
 /**
  * @brief Escape a string for shell use
  * 
  * @param str String to escape
  * @return char* Escaped string (must be freed by caller)
  */
 char* nlink_parse_escape_string(const char* str) {
     if (!str) {
         return NULL;
     }
     
     /* Allocate enough space for worst case (every character escaped) */
     size_t len = strlen(str);
     char* escaped = (char*)malloc(len * 2 + 3);
     if (!escaped) {
         return NULL;
     }
     
     /* Check if quoting is needed */
     if (nlink_parse_needs_quoting(str)) {
         /* Add quotes and escape special characters */
         char* p = escaped;
         *p++ = '"';
         
         const char* s = str;
         while (*s) {
             if (*s == '"' || *s == '\\' || *s == '$' || *s == '`') {
                 *p++ = '\\';
             }
             *p++ = *s++;
         }
         
         *p++ = '"';
         *p = '\0';
     } else {
         /* No quoting needed, just copy */
         strcpy(escaped, str);
     }
     
     return escaped;
 }