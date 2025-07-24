// include/nlink/core/config/config.h
// OBINexus Configuration: Constitutional Parameter Management
#ifndef OBINEXUS_NLINK_CORE_CONFIG_CONFIG_H
#define OBINEXUS_NLINK_CORE_CONFIG_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Configuration Result System
typedef enum {
    CONFIG_SUCCESS = 0,
    CONFIG_ERROR_INVALID_PARAMETER = -1,
    CONFIG_ERROR_FILE_NOT_FOUND = -2,
    CONFIG_ERROR_PARSE_FAILED = -3,
    CONFIG_ERROR_VALIDATION_FAILED = -4
} config_result_t;

// Configuration Context: Constitutional Parameter Coordination
typedef struct config_context {
    uint32_t magic_marker;
    bool initialized;
    char *config_path;
    void *parameter_registry;
} config_context_t;

// Constitutional Configuration Interface
config_result_t config_initialize(void);
config_result_t config_shutdown(void);
config_result_t config_load_file(const char *path);
config_result_t config_get_string(const char *key, char **value);
config_result_t config_get_integer(const char *key, int64_t *value);
config_result_t config_get_boolean(const char *key, bool *value);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_CONFIG_CONFIG_H
