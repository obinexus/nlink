// include/nlink/core/config/config_parser.h
// OBINexus Configuration Parser: Syntax Analysis Constitutional Interface
#ifndef OBINEXUS_NLINK_CORE_CONFIG_CONFIG_PARSER_H
#define OBINEXUS_NLINK_CORE_CONFIG_CONFIG_PARSER_H

#include <nlink/core/config/config.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constitutional Parser Operations
config_result_t config_parser_process_file(const char *filepath);
config_result_t config_parser_validate_syntax(const char *config_data);
config_result_t config_parser_parse_buffer(const char *buffer, size_t length);

// Advanced Parsing Coordination
config_result_t config_parser_set_validation_mode(bool strict_mode);
config_result_t config_parser_get_last_error(char *error_buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_CONFIG_CONFIG_PARSER_H
