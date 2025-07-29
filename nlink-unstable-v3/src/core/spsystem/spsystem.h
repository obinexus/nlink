/**
 * @file spsystem.h
 * @brief Header for the NexusLink spsystem subsystem
 * @copyright OBINexus Computing, 2025
 */

#ifndef NLINK_SPSYSTEM_H
#define NLINK_SPSYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the spsystem subsystem
 * @return Status code (0 = success)
 */
int spsystem_initialize(void);

/**
 * @brief Shutdown the spsystem subsystem
 * @return Status code (0 = success)
 */
int spsystem_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_SPSYSTEM_H */
