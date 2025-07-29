// include/nlink/core/cli/command_router.h
// OBINexus CLI Command_router: Constitutional Interface
#ifndef OBINEXUS_NLINK_CORE_CLI_COMMAND_ROUTER_H
#define OBINEXUS_NLINK_CORE_CLI_COMMAND_ROUTER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constitutional COMMAND_ROUTER Operations
int COMMAND_ROUTER_initialize(void);
void COMMAND_ROUTER_shutdown(void);
bool COMMAND_ROUTER_is_initialized(void);
int COMMAND_ROUTER_process(void);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_CLI_COMMAND_ROUTER_H
