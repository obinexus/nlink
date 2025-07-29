// include/nlink/core/config/config_manager.h
// OBINexus Configuration Manager: Advanced Parameter Coordination Interface
#ifndef OBINEXUS_NLINK_CORE_CONFIG_CONFIG_MANAGER_H
#define OBINEXUS_NLINK_CORE_CONFIG_CONFIG_MANAGER_H

#include <nlink/core/config/config.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Advanced Configuration Management Interface
typedef struct config_manager_context config_manager_context_t;

// Constitutional Manager Operations
config_result_t config_manager_initialize(void);
config_result_t config_manager_shutdown(void);
config_result_t config_manager_create_context(config_manager_context_t **ctx);
config_result_t config_manager_destroy_context(config_manager_context_t *ctx);

// Advanced Configuration Coordination
config_result_t config_manager_load_configuration(const char *config_path);
config_result_t config_manager_validate_schema(const char *schema_path);
config_result_t config_manager_reload_configuration(void);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_CONFIG_CONFIG_MANAGER_H
