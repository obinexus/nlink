/**
 * @file pattern.h
 * @brief Header for the NexusLink pattern subsystem
 * @copyright OBINexus Computing, 2025
 */

#ifndef NLINK_PATTERN_H
#define NLINK_PATTERN_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the pattern subsystem
 * @return Status code (0 = success)
 */
int pattern_initialize(void);

/**
 * @brief Shutdown the pattern subsystem
 * @return Status code (0 = success)
 */
int pattern_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_PATTERN_H */
