/**
 * @file main.c
 * @brief Main entry point for the NexusLink CLI
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/cli/cli.h"
 #include "nlink/core/nlink.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 int main(int argc, char** argv) {
     // Initialize NexusLink library
     NexusResult result = nlink_initialize(NULL);
     if (result != NEXUS_SUCCESS) {
         fprintf(stderr, "Error: Failed to initialize NexusLink library\n");
         return 1;
     }
     
     // Get context from library
     NexusContext* context = nlink_get_context();
     if (!context) {
         fprintf(stderr, "Error: Failed to get NexusLink context\n");
         nlink_cleanup();
         return 1;
     }
     
     // Initialize CLI
     NexusCLI cli;
     if (!nexus_cli_init(&cli, argv[0], context)) {
         fprintf(stderr, "Error: Failed to initialize CLI system\n");
         nlink_cleanup();
         return 1;
     }
     
     // Check for minimal mode environment variable
     if (getenv("NEXUS_MINIMAL") != NULL) {
         nexus_cli_set_minimal_mode(&cli, true);
     }
     
     // Parse and execute command
     int exit_code = nexus_cli_parse_and_execute(&cli, argc, argv);
     
     // Clean up
     nexus_cli_cleanup(&cli);
     nlink_cleanup();
     
     return exit_code;
 }