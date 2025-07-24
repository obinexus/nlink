// include/nlink/core/cli/command_params.h
// OBINexus CLI Command_params: Constitutional Interface
#ifndef OBINEXUS_NLINK_CORE_CLI_COMMAND_PARAMS_H
#define OBINEXUS_NLINK_CORE_CLI_COMMAND_PARAMS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constitutional COMMAND_PARAMS Operations
int COMMAND_PARAMS_initialize(void);
void COMMAND_PARAMS_shutdown(void);
bool COMMAND_PARAMS_is_initialized(void);
int COMMAND_PARAMS_process(void);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_CLI_COMMAND_PARAMS_H
