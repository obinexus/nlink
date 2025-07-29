/**
 * @file mpsystem.h
 * @brief Header for the NexusLink mpsystem subsystem
 * @copyright OBINexus Computing, 2025
 */

#ifndef NLINK_MPSYSTEM_H
#define NLINK_MPSYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the mpsystem subsystem
 * @return Status code (0 = success)
 */
int mpsystem_initialize(void);

/**
 * @brief Shutdown the mpsystem subsystem
 * @return Status code (0 = success)
 */
int mpsystem_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_MPSYSTEM_H */
