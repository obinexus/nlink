// include/nlink/core/cli/command_registry.h
// OBINexus CLI Command_registry: Constitutional Interface
#ifndef OBINEXUS_NLINK_CORE_CLI_COMMAND_REGISTRY_H
#define OBINEXUS_NLINK_CORE_CLI_COMMAND_REGISTRY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constitutional COMMAND_REGISTRY Operations
int COMMAND_REGISTRY_initialize(void);
void COMMAND_REGISTRY_shutdown(void);
bool COMMAND_REGISTRY_is_initialized(void);
int COMMAND_REGISTRY_process(void);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_CLI_COMMAND_REGISTRY_H
