/**
 * @file pattern_matching.h
 * @brief Header for the NexusLink pattern_matching subsystem
 * @copyright OBINexus Computing, 2025
 */

#ifndef NLINK_PATTERN_MATCHING_H
#define NLINK_PATTERN_MATCHING_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the pattern_matching subsystem
 * @return Status code (0 = success)
 */
int pattern_matching_initialize(void);

/**
 * @brief Shutdown the pattern_matching subsystem
 * @return Status code (0 = success)
 */
int pattern_matching_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_PATTERN_MATCHING_H */
