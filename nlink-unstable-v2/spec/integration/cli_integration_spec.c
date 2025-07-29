/**
 * @file cli_integration_spec.c
 * @brief CLI Integration Specifications
 */

#include "../helpers/spec_runner.c"
#include <stdlib.h>

spec_result_t spec_cli_etps_json_output(void) {
    // Test CLI with JSON output
    int result = system("LD_LIBRARY_PATH=../lib ../bin/nlink --etps-test --json > /tmp/nlink_test.json");
    SPEC_EXPECT_EQ(result, 0);
    
    // Verify JSON output exists
    FILE* f = fopen("/tmp/nlink_test.json", "r");
    SPEC_ASSERT(f != NULL, "JSON output file not created");
    
    // Read and verify JSON structure
    char buffer[1024];
    fgets(buffer, sizeof(buffer), f);
    fclose(f);
    
    SPEC_ASSERT(strstr(buffer, "\"command\"") != NULL, "JSON missing command field");
    SPEC_ASSERT(strstr(buffer, "\"guid\"") != NULL, "JSON missing guid field");
    
    return SPEC_PASS;
}

spec_result_t spec_cli_config_loading(void) {
    // Test CLI with config file
    int result = system("LD_LIBRARY_PATH=../lib ../bin/nlink --config ../config/nlink.conf --version");
    SPEC_EXPECT_EQ(result, 0);
    return SPEC_PASS;
}

int main() {
    etps_init();
    
    spec_suite_t* suite = spec_suite_create("CLI_Integration_Specs");
    
    spec_add_test(suite, "CLI ETPS JSON output", spec_cli_etps_json_output);
    spec_add_test(suite, "CLI config file loading", spec_cli_config_loading);
    
    int result = spec_suite_run(suite);
    
    spec_suite_destroy(suite);
    etps_shutdown();
    
    return result;
}
