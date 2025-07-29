/**
 * @file versioning.h
 * @brief Header for the NexusLink versioning subsystem
 * @copyright OBINexus Computing, 2025
 */

#ifndef NLINK_VERSIONING_H
#define NLINK_VERSIONING_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the versioning subsystem
 * @return Status code (0 = success)
 */
int versioning_initialize(void);

/**
 * @brief Shutdown the versioning subsystem
 * @return Status code (0 = success)
 */
int versioning_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_VERSIONING_H */
