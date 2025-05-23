/**
 * @file test_command_registry.c
 * @brief Unit tests for command registry
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include <check.h>
 #include <stdlib.h>
 #include "nlink/cli/command_registry.h"
 #include "nlink/core/common/nexus_core.h"
 
 // Mock command execution functions
 static int mock_command1_executed = 0;
 static int mock_command2_executed = 0;
 
 static int mock_command1_execute(NexusContext* ctx, int argc, char** argv) {
     mock_command1_executed++;
     return 0;
 }
 
 static int mock_command2_execute(NexusContext* ctx, int argc, char** argv) {
     mock_command2_executed++;
     return 0;
 }
 
 // Test fixtures
 static CommandRegistry* registry;
 static NexusCommand* command1;
 static NexusCommand* command2;
 static NexusContext* context;
 
 static void setup(void) {
     // Create registry
     registry = (CommandRegistry*)malloc(sizeof(CommandRegistry));
     command_registry_init(registry);
     
     // Create commands
     command1 = (NexusCommand*)malloc(sizeof(NexusCommand));
     command1->name = "command1";
     command1->short_name = "c1";
     command1->description = "Test command 1";
     command1->execute = mock_command1_execute;
     
     command2 = (NexusCommand*)malloc(sizeof(NexusCommand));
     command2->name = "command2";
     command2->short_name = NULL;
     command2->description = "Test command 2";
     command2->execute = mock_command2_execute;
     
     // Create context
     context = nexus_create_context(NULL);
     
     // Reset execution counters
     mock_command1_executed = 0;
     mock_command2_executed = 0;
 }
 
 static void teardown(void) {
     command_registry_cleanup(registry);
     free(registry);
     free(command1);
     free(command2);
     nexus_destroy_context(context);
 }
 
 // Test cases
 START_TEST(test_registry_init) {
     CommandRegistry test_registry;
     bool result = command_registry_init(&test_registry);
     
     ck_assert(result);
     ck_assert_ptr_null(test_registry.commands);
     ck_assert_uint_eq(test_registry.count, 0);
     ck_assert_uint_eq(test_registry.capacity, 0);
 }
 END_TEST
 
 START_TEST(test_registry_register) {
     bool result1 = command_registry_register(registry, command1);
     bool result2 = command_registry_register(registry, command2);
     
     ck_assert(result1);
     ck_assert(result2);
     ck_assert_uint_eq(registry->count, 2);
     ck_assert(registry->capacity >= 2);
     
     // Test duplicate registration
     bool result3 = command_registry_register(registry, command1);
     ck_assert(!result3);
     ck_assert_uint_eq(registry->count, 2);
 }
 END_TEST
 
 START_TEST(test_registry_find) {
     command_registry_register(registry, command1);
     command_registry_register(registry, command2);
     
     NexusCommand* found1 = command_registry_find(registry, "command1");
     NexusCommand* found2 = command_registry_find(registry, "command2");
     NexusCommand* found3 = command_registry_find(registry, "c1");
     NexusCommand* found4 = command_registry_find(registry, "nonexistent");
     
     ck_assert_ptr_eq(found1, command1);
     ck_assert_ptr_eq(found2, command2);
     ck_assert_ptr_eq(found3, command1); // Found by short name
     ck_assert_ptr_null(found4);
 }
 END_TEST
 
 START_TEST(test_registry_execute) {
     command_registry_register(registry, command1);
     command_registry_register(registry, command2);
     
     int result1 = command_registry_execute(registry, context, "command1", 0, NULL);
     int result2 = command_registry_execute(registry, context, "command2", 0, NULL);
     int result3 = command_registry_execute(registry, context, "c1", 0, NULL);
     int result4 = command_registry_execute(registry, context, "nonexistent", 0, NULL);
     
     ck_assert_int_eq(result1, 0);
     ck_assert_int_eq(result2, 0);
     ck_assert_int_eq(result3, 0);
     ck_assert_int_eq(result4, -1);
     
     ck_assert_int_eq(mock_command1_executed, 2); // Called twice (once by name, once by short name)
     ck_assert_int_eq(mock_command2_executed, 1);
 }
 END_TEST
 
 // Test suite
 Suite* command_registry_suite(void) {
     Suite* s = suite_create("CommandRegistry");
     
     // Core functionality tests
     TCase* tc_core = tcase_create("Core");
     tcase_add_checked_fixture(tc_core, setup, teardown);
     tcase_add_test(tc_core, test_registry_init);
     tcase_add_test(tc_core, test_registry_register);
     tcase_add_test(tc_core, test_registry_find);
     tcase_add_test(tc_core, test_registry_execute);
     suite_add_tcase(s, tc_core);
     
     return s;
 }
 
 // Main function
 int main(void) {
     int number_failed;
     Suite* s = command_registry_suite();
     SRunner* sr = srunner_create(s);
     
     srunner_run_all(sr, CK_NORMAL);
     number_failed = srunner_ntests_failed(sr);
     srunner_free(sr);
     
     return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
 }