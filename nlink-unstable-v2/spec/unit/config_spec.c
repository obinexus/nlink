/**
 * @file config_spec.c
 * @brief Configuration Parser Unit Specifications
 */

#include "../helpers/spec_runner.c"
#include "nlink/core/config/config_parser.c"

spec_result_t spec_config_parser_create(void) {
    config_parser_t* parser = config_parser_create("test_parser");
    SPEC_ASSERT(parser != NULL, "Failed to create config parser");
    config_parser_destroy(parser);
    return SPEC_PASS;
}

spec_result_t spec_config_load_valid_file(void) {
    // Create test config
    FILE* f = fopen("spec/fixtures/test.conf", "w");
    fprintf(f, "[test]\nkey=value\n");
    fclose(f);
    
    config_parser_t* parser = config_parser_create("test");
    int result = config_parser_load(parser, "spec/fixtures/test.conf");
    SPEC_EXPECT_EQ(result, 0);
    
    const char* value = config_get_value(parser, "test", "key");
    SPEC_EXPECT_STR_EQ(value, "value");
    
    config_parser_destroy(parser);
    return SPEC_PASS;
}

spec_result_t spec_config_etps_integration(void) {
    config_parser_t* parser = config_parser_create("etps_test");
    SPEC_ASSERT(parser != NULL, "Parser creation failed");
    SPEC_ASSERT(parser->etps_ctx != NULL, "ETPS context not initialized");
    
    config_parser_destroy(parser);
    return SPEC_PASS;
}

int main() {
    etps_init();
    
    spec_suite_t* suite = spec_suite_create("Config_Parser_Specs");
    
    spec_add_test(suite, "Config parser creation", spec_config_parser_create);
    spec_add_test(suite, "Load valid config file", spec_config_load_valid_file);
    spec_add_test(suite, "ETPS integration", spec_config_etps_integration);
    
    int result = spec_suite_run(suite);
    
    spec_suite_destroy(suite);
    etps_shutdown();
    
    return result;
}
