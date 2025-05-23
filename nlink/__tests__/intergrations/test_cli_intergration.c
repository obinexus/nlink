/**
 * @file test_cli_integration.c
 * @brief Integration test for NexusLink CLI
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <assert.h>
 #include "nlink/cli/cli.h"
 #include "nlink/core/common/nexus_core.h"
 
 // Capture stdout for testing
 static char captured_output[4096];
 static FILE* original_stdout = NULL;
 static FILE* temp_out = NULL;
 
 // Setup and teardown for stdout capture
 static void setup_capture(void) {
     fflush(stdout);
     original_stdout = stdout;
     temp_out = tmpfile();
     stdout = temp_out;
 }
 
 static void end_capture(void) {
     fflush(stdout);
     
     // Reset stdout
     stdout = original_stdout;
     
     // Read captured output
     if (temp_out) {
         rewind(temp_out);
         size_t bytes_read = fread(captured_output, 1, sizeof(captured_output) - 1, temp_out);
         captured_output[bytes_read] = '\0';
         fclose(temp_out);
         temp_out = NULL;
     }
 }
 
 // Helper function to check if a string contains a substring
 static int contains_string(const char* haystack, const char* needle) {
     return strstr(haystack, needle) != NULL;
 }
 
 // Test CLI initialization
 static void test_cli_init(void) {
     NexusCLI cli;
     bool result = nexus_cli_init(&cli, "nlink_test", NULL);
     
     assert(result);
     assert(cli.context != NULL);
     assert(strcmp(cli.prog_name, "nlink_test") == 0);
     
     // Verify command registry initialization
     assert(cli.registry.commands != NULL || cli.registry.capacity == 0);
     
     nexus_cli_cleanup(&cli);
     printf("✓ CLI initialization test passed\n");
 }
 
 // Test help command
 static void test_cli_help(void) {
     NexusCLI cli;
     nexus_cli_init(&cli, "nlink_test", NULL);
     nexus_cli_register_standard_commands(&cli);
     
     setup_capture();
     nexus_cli_print_help(&cli);
     end_capture();
     
     // Check for standard command descriptions
     assert(contains_string(captured_output, "Available commands"));
     assert(contains_string(captured_output, "version"));
     assert(contains_string(captured_output, "load"));
     assert(contains_string(captured_output, "minimize"));
     
     nexus_cli_cleanup(&cli);
     printf("✓ CLI help test passed\n");
 }
 
 // Test version command
 static void test_cli_version(void) {
     NexusCLI cli;
     nexus_cli_init(&cli, "nlink_test", NULL);
     nexus_cli_register_standard_commands(&cli);
     
     char* argv[] = {"version"};
     
     setup_capture();
     int result = nexus_cli_parse_and_execute(&cli, 1, argv);
     end_capture();
     
     assert(result == 0);
     assert(contains_string(captured_output, "NexusLink"));
     assert(contains_string(captured_output, "version"));
     
     nexus_cli_cleanup(&cli);
     printf("✓ CLI version command test passed\n");
 }
 
 // Test minimal mode
 static void test_cli_minimal_mode(void) {
     NexusCLI cli;
     nexus_cli_init(&cli, "nlink_test", NULL);
     nexus_cli_register_standard_commands(&cli);
     nexus_cli_set_minimal_mode(&cli, true);
     
     assert(cli.minimal_mode_enabled);
     
     // Simulate minimal command input
     char* minimal_input = "version";
     
     setup_capture();
     bool result = nexus_cli_parse_minimal(&cli, minimal_input);
     end_capture();
     
     assert(result);
     assert(contains_string(captured_output, "NexusLink"));
     assert(contains_string(captured_output, "version"));
     
     nexus_cli_cleanup(&cli);
     printf("✓ CLI minimal mode test passed\n");
 }
 
 // Test nonexistent command
 static void test_cli_nonexistent_command(void) {
     NexusCLI cli;
     nexus_cli_init(&cli, "nlink_test", NULL);
     nexus_cli_register_standard_commands(&cli);
     
     char* argv[] = {"nonexistent"};
     
     setup_capture();
     int result = nexus_cli_parse_and_execute(&cli, 1, argv);
     end_capture();
     
     assert(result != 0);
     assert(contains_string(captured_output, "Unknown command"));
     
     nexus_cli_cleanup(&cli);
     printf("✓ CLI nonexistent command test passed\n");
 }
 
 // Main function
 int main(void) {
     printf("Running CLI integration tests...\n");
     
     // Run tests
     test_cli_init();
     test_cli_help();
     test_cli_version();
     test_cli_minimal_mode();
     test_cli_nonexistent_command();
     
     printf("All CLI integration tests passed!\n");
     return 0;
 }