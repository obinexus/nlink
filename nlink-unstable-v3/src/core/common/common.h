/**
 * @file common.h
 * @brief Header for the NexusLink common subsystem
 * @copyright OBINexus Computing, 2025
 */

#ifndef NLINK_COMMON_H
#define NLINK_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the common subsystem
 * @return Status code (0 = success)
 */
int common_initialize(void);

/**
 * @brief Shutdown the common subsystem
 * @return Status code (0 = success)
 */
int common_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_COMMON_H */
