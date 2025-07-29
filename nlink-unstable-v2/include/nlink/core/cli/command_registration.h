// include/nlink/core/cli/command_registration.h
// OBINexus CLI Command_registration: Constitutional Interface
#ifndef OBINEXUS_NLINK_CORE_CLI_COMMAND_REGISTRATION_H
#define OBINEXUS_NLINK_CORE_CLI_COMMAND_REGISTRATION_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constitutional COMMAND_REGISTRATION Operations
int COMMAND_REGISTRATION_initialize(void);
void COMMAND_REGISTRATION_shutdown(void);
bool COMMAND_REGISTRATION_is_initialized(void);
int COMMAND_REGISTRATION_process(void);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_CLI_COMMAND_REGISTRATION_H
