// include/nlink/core/cli/cli.h
// OBINexus CLI: Constitutional Command Interface
#ifndef OBINEXUS_NLINK_CORE_CLI_CLI_H
#define OBINEXUS_NLINK_CORE_CLI_CLI_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CLI_SUCCESS = 0,
    CLI_ERROR_INVALID_COMMAND = -1,
    CLI_ERROR_INVALID_PARAMETER = -2,
    CLI_ERROR_EXECUTION_FAILED = -3
} cli_result_t;

// Constitutional CLI Operations
int CLI_initialize(void);
void CLI_shutdown(void);
bool CLI_is_initialized(void);
int CLI_process(void);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_CLI_CLI_H
