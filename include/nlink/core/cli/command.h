// include/nlink/core/cli/command.h
// OBINexus CLI Command: Constitutional Interface
#ifndef OBINEXUS_NLINK_CORE_CLI_COMMAND_H
#define OBINEXUS_NLINK_CORE_CLI_COMMAND_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constitutional COMMAND Operations
int COMMAND_initialize(void);
void COMMAND_shutdown(void);
bool COMMAND_is_initialized(void);
int COMMAND_process(void);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_CLI_COMMAND_H
