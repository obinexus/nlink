/**
 * @file minimal.c
 * @brief Implementation of minimal command for NexusLink CLI
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/cli/commands/minimal.h"

 
 // Forward declaration of load command execute function
 extern int load_command_execute(NexusContext* ctx, int argc, char** argv);
 
 /**
  * @brief Structure for minimal command data
  */
 typedef struct {
     char* component;
     char* version;
     char* function;
     char* args;
 } MinimalCommandData;
 
 /**
  * @brief Free minimal command data
  */
 static void minimal_data_free(MinimalCommandData* data) {
     if (!data) {
         return;
     }
     
     free(data->component);
     free(data->version);
     free(data->function);
     free(data->args);
     free(data);
 }
 
 /**
  * @brief Execute the minimal command
  */
 static int minimal_execute(NexusContext* ctx, int argc, char** argv) {
     if (!ctx || argc < 1 || !argv) {
         return 1;
     }
     
     // Get the minimal command string
     const char* input = argv[0];
     
     // Parse the minimal command string
     char* component = NULL;
     char* version = NULL;
     char* function = NULL;
     char* args = NULL;
     
     if (!minimal_parse(input, &component, &version, &function, &args)) {
         fprintf(stderr, "Error: Invalid minimal command format: %s\n", input);
         fprintf(stderr, "Format: [component][@version][:function][=args]\n");
         return 1;
     }
     
     // Log what we're doing
     printf("Minimal command: component='%s'", component);
     if (version) printf(", version='%s'", version);
     if (function) printf(", function='%s'", function);
     if (args) printf(", args='%s'", args);
     printf("\n");
     
     // Prepare arguments for the load command
     int load_argc = 1;
     char* load_argv[4]; // component, --version, version, NULL
     
     load_argv[0] = component;
     
     if (version) {
         load_argv[1] = "--version";
         load_argv[2] = version;
         load_argc += 2;
     }
     
     // Execute the load command
     int result = load_command_execute(ctx, load_argc, load_argv);
     if (result != 0) {
         fprintf(stderr, "Error: Failed to load component '%s'\n", component);
         free(component);
         free(version);
         free(function);
         free(args);
         return result;
     }
     
     // If a function was specified, call it
     if (function) {
         // In a real implementation, we would look up the function and call it
         printf("Would call function '%s'", function);
         if (args) {
             printf(" with arguments '%s'", args);
         }
         printf("\n");
     }
     
     // Clean up
     free(component);
     free(version);
     free(function);
     free(args);
     
     return 0;
 }
 
 /**
  * @brief Print help for the minimal command
  */
 static void minimal_print_help(void) {
     printf("Usage: minimal [component][@version][:function][=args]\n\n");
     printf("Examples:\n");
     printf("  logger           - Load the logger component\n");
     printf("  logger@1.2.3     - Load logger version 1.2.3\n");
     printf("  logger:log       - Load logger and call log function\n");
     printf("  logger@1.2.3:log=Hello World\n");
     printf("                   - Load logger 1.2.3, call log with \"Hello World\"\n");
 }
 
 /**
  * @brief Parse arguments for the minimal command
  */
 static bool minimal_parse_args(int argc, char** argv, void** command_data) {
     if (argc < 1 || !argv || !command_data) {
         return false;
     }
     
     // Create command data
     MinimalCommandData* data = (MinimalCommandData*)malloc(sizeof(MinimalCommandData));
     if (!data) {
         return false;
     }
     
     // Initialize data
     data->component = NULL;
     data->version = NULL;
     data->function = NULL;
     data->args = NULL;
     
     // Parse the minimal command string
     if (!minimal_parse(argv[0], 
                       &data->component, 
                       &data->version, 
                       &data->function, 
                       &data->args)) {
         minimal_data_free(data);
         return false;
     }
     
     *command_data = data;
     return true;
 }
 
 // Minimal command definition
 NexusCommand minimal_command = {
     .name = "minimal",
     .short_name = "m",
     .description = "Execute a command in minimal syntax",
     .help = "Provides a concise syntax for common operations.",
     .execute = minimal_execute,
     .print_help = minimal_print_help,
     .parse_args = minimal_parse_args,
     .data = NULL
 };
 
 bool minimal_parse(const char* input, 
                   char** component, 
                   char** version, 
                   char** function, 
                   char** args) {
     if (!input || !component) {
         return false;
     }
     
     // Initialize output parameters
     *component = NULL;
     if (version) *version = NULL;
     if (function) *function = NULL;
     if (args) *args = NULL;
     
     // Make a copy of the input for parsing
     char* input_copy = strdup(input);
     if (!input_copy) {
         return false;
     }
     
     // Find delimiters
     char* at_sign = strchr(input_copy, '@');
     char* colon = strchr(input_copy, ':');
     char* equals = strchr(input_copy, '=');
     
     // Extract component name
     if (at_sign) {
         *at_sign = '\0';
     }
     
     if (colon && (at_sign == NULL || colon > at_sign)) {
         *colon = '\0';
     }
     
     // Check if we have a component name
     if (input_copy[0] == '\0') {
         free(input_copy);
         return false;
     }
     
     // Set component name
     *component = strdup(input_copy);
     if (!*component) {
         free(input_copy);
         return false;
     }
     
     // Extract version if present
     if (at_sign && version) {
         char* version_start = at_sign + 1;
         
         if (colon) {
             *colon = '\0';
         }
         
         if (*version_start) {
             *version = strdup(version_start);
             if (!*version) {
                 free(*component);
                 *component = NULL;
                 free(input_copy);
                 return false;
             }
         }
     }
     
     // Extract function if present
     if (colon && function) {
         char* function_start = colon + 1;
         
         if (equals) {
             *equals = '\0';
         }
         
         if (*function_start) {
             *function = strdup(function_start);
             if (!*function) {
                 free(*component);
                 *component = NULL;
                 if (version && *version) {
                     free(*version);
                     *version = NULL;
                 }
                 free(input_copy);
                 return false;
             }
         }
     }
     
     // Extract args if present
     if (equals && args) {
         char* args_start = equals + 1;
         
         if (*args_start) {
             *args = strdup(args_start);
             if (!*args) {
                 free(*component);
                 *component = NULL;
                 if (version && *version) {
                     free(*version);
                     *version = NULL;
                 }
                 if (function && *function) {
                     free(*function);
                     *function = NULL;
                 }
                 free(input_copy);
                 return false;
             }
         }
     }
     
     free(input_copy);
     return true;
 }