// include/nlink/core/cli/parse.h
// OBINexus CLI Parse: Constitutional Interface
#ifndef OBINEXUS_NLINK_CORE_CLI_PARSE_H
#define OBINEXUS_NLINK_CORE_CLI_PARSE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constitutional PARSE Operations
int PARSE_initialize(void);
void PARSE_shutdown(void);
bool PARSE_is_initialized(void);
int PARSE_process(void);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_CLI_PARSE_H
