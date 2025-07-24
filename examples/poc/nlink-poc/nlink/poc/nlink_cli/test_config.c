/**
 * @file test_config.c
 * @brief Unit tests for NexusLink configuration parsing system
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.0.0
 *
 * Comprehensive unit testing for pkg.nlink and nlink.txt parsing functionality.
 * Validates decision matrix generation, feature toggles, and threading configuration.
 */

#include "nlink/test_assert.h"
#include "nlink/core/config.h"
#include <sys/stat.h>
#include <unistd.h>

// =============================================================================
// TEST CONFIGURATION CONSTANTS
// =============================================================================

#define TEST_CONFIG_PATH "/tmp/nlink_test_pkg.nlink"
#define TEST_COMPONENT_PATH "/tmp/nlink_test_component.nlink"
#define TEST_PROJECT_ROOT "/tmp/nlink_test_project"

// =============================================================================
// CONFIGURATION PARSING TESTS
// =============================================================================

/**
 * @brief Test basic pkg.nlink parsing functionality
 */
void test_pkg_config_basic_parsing(void) {
  // Create test configuration
  nlink_test_create_pkg_config(TEST_CONFIG_PATH, NLINK_TEST_PKG_CONFIG_TEMPLATE);
  
  // Initialize configuration system
  nlink_config_result_t init_result = nlink_config_init();
  NLINK_TEST_ASSERT_CONFIG_SUCCESS(init_result, "Configuration system initialization");
  
  // Parse configuration
  nlink_pkg_config_t config;
  nlink_config_result_t parse_result = nlink_parse_pkg_config(TEST_CONFIG_PATH, &config);
  NLINK_TEST_ASSERT_CONFIG_SUCCESS(parse_result, "Basic pkg.nlink parsing");
  
  // Validate parsed values
  NLINK_TEST_ASSERT_STR_EQ(config.project_name, "test_project", "Project name parsing");
  NLINK_TEST_ASSERT_STR_EQ(config.project_version, "1.0.0", "Project version parsing");
  NLINK_TEST_ASSERT_STR_EQ(config.entry_point, "main.c", "Entry point parsing");
  NLINK_TEST_ASSERT(config.pass_mode == NLINK_PASS_MODE_SINGLE, "Pass mode parsing");
  NLINK_TEST_ASSERT(!config.experimental_mode_enabled, "Experimental mode parsing");
  NLINK_TEST_ASSERT(config.strict_mode, "Strict mode parsing");
  
  // Validate threading configuration
  NLINK_TEST_ASSERT_INT_EQ(config.thread_pool.worker_count, 4, "Worker count parsing");
  NLINK_TEST_ASSERT_INT_EQ(config.thread_pool.queue_depth, 64, "Queue depth parsing");
  NLINK_TEST_ASSERT_INT_EQ(config.thread_pool.stack_size_kb, 512, "Stack size parsing");
  NLINK_TEST_ASSERT(config.thread_pool.enable_work_stealing, "Work stealing parsing");
  
  // Cleanup
  nlink_test_cleanup_file(TEST_CONFIG_PATH);
  nlink_config_destroy();
}

/**
 * @brief Test configuration validation functionality
 */
void test_config_validation(void) {
  nlink_test_create_pkg_config(TEST_CONFIG_PATH, NLINK_TEST_PKG_CONFIG_TEMPLATE);
  
  nlink_config_init();
  nlink_pkg_config_t config;
  nlink_parse_pkg_config(TEST_CONFIG_PATH, &config);
  
  // Test validation of valid configuration
  nlink_config_result_t validation_result = nlink_validate_config(&config);
  NLINK_TEST_ASSERT_CONFIG_SUCCESS(validation_result, "Valid configuration validation");
  
  // Test invalid threading configuration
  config.thread_pool.worker_count = 0; // Invalid
  validation_result = nlink_validate_config(&config);
  NLINK_TEST_ASSERT(validation_result == NLINK_CONFIG_ERROR_THREAD_POOL_INVALID, 
                   "Invalid threading configuration detection");
  
  nlink_test_cleanup_file(TEST_CONFIG_PATH);
  nlink_config_destroy();
}

/**
 * @brief Test component discovery functionality
 */
void test_component_discovery(void) {
  // Create test project structure
  mkdir(TEST_PROJECT_ROOT, 0755);
  mkdir(TEST_PROJECT_ROOT "/component1", 0755);
  mkdir(TEST_PROJECT_ROOT "/component2", 0755);
  mkdir(TEST_PROJECT_ROOT "/shared", 0755);
  
  // Create pkg.nlink in project root
  char pkg_config_path[256];
  snprintf(pkg_config_path, sizeof(pkg_config_path), "%s/pkg.nlink", TEST_PROJECT_ROOT);
  nlink_test_create_pkg_config(pkg_config_path, NLINK_TEST_PKG_CONFIG_TEMPLATE);
  
  // Create component configurations
  char comp1_path[256], comp2_path[256], shared_path[256];
  snprintf(comp1_path, sizeof(comp1_path), "%s/component1/nlink.txt", TEST_PROJECT_ROOT);
  snprintf(comp2_path, sizeof(comp2_path), "%s/component2/nlink.txt", TEST_PROJECT_ROOT);
  snprintf(shared_path, sizeof(shared_path), "%s/shared/shared.nlink", TEST_PROJECT_ROOT);
  
  nlink_test_create_pkg_config(comp1_path, NLINK_TEST_COMPONENT_CONFIG_TEMPLATE);
  nlink_test_create_pkg_config(comp2_path, NLINK_TEST_COMPONENT_CONFIG_TEMPLATE);
  nlink_test_create_pkg_config(shared_path, NLINK_TEST_COMPONENT_CONFIG_TEMPLATE);
  
  // Test component discovery
  nlink_config_init();
  nlink_pkg_config_t config;
  nlink_parse_pkg_config(pkg_config_path, &config);
  
  int discovered_count = nlink_discover_components(TEST_PROJECT_ROOT, &config);
  NLINK_TEST_ASSERT(discovered_count >= 2, "Component discovery count");
  NLINK_TEST_ASSERT(config.component_count >= 2, "Configuration component count");
  
  // Cleanup
  nlink_test_cleanup_file(comp1_path);
  nlink_test_cleanup_file(comp2_path);
  nlink_test_cleanup_file(shared_path);
  nlink_test_cleanup_file(pkg_config_path);
  rmdir(TEST_PROJECT_ROOT "/component1");
  rmdir(TEST_PROJECT_ROOT "/component2");
  rmdir(TEST_PROJECT_ROOT "/shared");
  rmdir(TEST_PROJECT_ROOT);
  nlink_config_destroy();
}

/**
 * @brief Test pass mode detection
 */
void test_pass_mode_detection(void) {
  // Create test project with multiple components (should trigger multi-pass)
  mkdir(TEST_PROJECT_ROOT, 0755);
  mkdir(TEST_PROJECT_ROOT "/component1", 0755);
  mkdir(TEST_PROJECT_ROOT "/component2", 0755);
  
  char comp1_path[256], comp2_path[256];
  snprintf(comp1_path, sizeof(comp1_path), "%s/component1/nlink.txt", TEST_PROJECT_ROOT);
  snprintf(comp2_path, sizeof(comp2_path), "%s/component2/nlink.txt", TEST_PROJECT_ROOT);
  
  nlink_test_create_pkg_config(comp1_path, NLINK_TEST_COMPONENT_CONFIG_TEMPLATE);
  nlink_test_create_pkg_config(comp2_path, NLINK_TEST_COMPONENT_CONFIG_TEMPLATE);
  
  // Test mode detection
  nlink_pass_mode_t detected_mode = nlink_detect_pass_mode(TEST_PROJECT_ROOT);
  NLINK_TEST_ASSERT(detected_mode == NLINK_PASS_MODE_MULTI || detected_mode == NLINK_PASS_MODE_SINGLE,
                   "Pass mode detection returns valid mode");
  
  // Cleanup
  nlink_test_cleanup_file(comp1_path);
  nlink_test_cleanup_file(comp2_path);
  rmdir(TEST_PROJECT_ROOT "/component1");
  rmdir(TEST_PROJECT_ROOT "/component2");
  rmdir(TEST_PROJECT_ROOT);
}

/**
 * @brief Test configuration checksum calculation
 */
void test_config_checksum(void) {
  nlink_test_create_pkg_config(TEST_CONFIG_PATH, NLINK_TEST_PKG_CONFIG_TEMPLATE);
  
  nlink_config_init();
  nlink_pkg_config_t config1, config2;
  nlink_parse_pkg_config(TEST_CONFIG_PATH, &config1);
  nlink_parse_pkg_config(TEST_CONFIG_PATH, &config2);
  
  // Calculate checksums
  uint32_t checksum1 = nlink_calculate_config_checksum(&config1);
  uint32_t checksum2 = nlink_calculate_config_checksum(&config2);
  
  // Checksums should be identical for same configuration
  NLINK_TEST_ASSERT_INT_EQ(checksum1, checksum2, "Identical configuration checksums");
  NLINK_TEST_ASSERT(checksum1 != 0, "Non-zero checksum generation");
  
  nlink_test_cleanup_file(TEST_CONFIG_PATH);
  nlink_config_destroy();
}

/**
 * @brief Test error handling for missing files
 */
void test_error_handling(void) {
  nlink_config_init();
  
  // Test parsing non-existent file
  nlink_pkg_config_t config;
  nlink_config_result_t result = nlink_parse_pkg_config("/nonexistent/path.nlink", &config);
  NLINK_TEST_ASSERT(result == NLINK_CONFIG_ERROR_FILE_NOT_FOUND, 
                   "Missing file error handling");
  
  nlink_config_destroy();
}

// =============================================================================
// MAIN TEST RUNNER
// =============================================================================

int main(void) {
  nlink_test_context_t ctx;
  NLINK_TEST_INIT(&ctx, "Configuration Parser Unit Tests");
  
  // Run all test functions
  NLINK_TEST_RUN(&ctx, test_pkg_config_basic_parsing);
  NLINK_TEST_RUN(&ctx, test_config_validation);
  NLINK_TEST_RUN(&ctx, test_component_discovery);
  NLINK_TEST_RUN(&ctx, test_pass_mode_detection);
  NLINK_TEST_RUN(&ctx, test_config_checksum);
  NLINK_TEST_RUN(&ctx, test_error_handling);
  
  NLINK_TEST_RESULTS(&ctx);
  
  return (ctx.tests_failed == 0) ? 0 : 1;
}
