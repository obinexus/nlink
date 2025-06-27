#ifndef NLINK_QA_POC_NLINK_H
#define NLINK_QA_POC_NLINK_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

const char* nlink_version(void);
int nlink_init(void);
void nlink_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
