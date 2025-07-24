// include/nlink/core/config/config.h
// OBINexus Configuration: Constitutional Parameter Management Interface
#ifndef OBINEXUS_NLINK_CORE_CONFIG_CONFIG_H
#define OBINEXUS_NLINK_CORE_CONFIG_CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constitutional Configuration Result System
typedef enum {
    CONFIG_SUCCESS = 0,
    CONFIG_ERROR_INVALID_PARAMETER = -1,
    CONFIG_ERROR_FILE_NOT_FOUND = -2,
    CONFIG_ERROR_PARSE_FAILED = -3,
    CONFIG_ERROR_VALIDATION_FAILED = -4,
    CONFIG_ERROR_MEMORY_ALLOCATION = -5
} config_result_t;

// Constitutional Configuration Context
typedef struct config_context {
    uint32_t magic_marker;          // Constitutional validation marker: 0x434F4E46 "CONF"
    bool initialized;               // Initialization state flag
    char *config_path;              // Configuration file path
    void *parameter_registry;       // Parameter coordination registry
    size_t registry_size;           // Registry allocation size
} config_context_t;

// Constitutional Configuration Interface Functions
config_result_t config_initialize(void);
config_result_t config_shutdown(void);
config_result_t config_load_file(const char *path);
config_result_t config_get_string(const char *key, char **value);
config_result_t config_get_integer(const char *key, int64_t *value);
config_result_t config_get_boolean(const char *key, bool *value);

// Advanced Configuration Coordination
config_result_t config_validate_schema(const char *schema_path);
config_result_t config_reload_configuration(void);
config_result_t config_set_parameter(const char *key, const char *value);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_CONFIG_CONFIG_H
