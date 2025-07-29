/**
 * @file minimizer.h
 * @brief Header for the NexusLink minimizer subsystem
 * @copyright OBINexus Computing, 2025
 */

#ifndef NLINK_MINIMIZER_H
#define NLINK_MINIMIZER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the minimizer subsystem
 * @return Status code (0 = success)
 */
int minimizer_initialize(void);

/**
 * @brief Shutdown the minimizer subsystem
 * @return Status code (0 = success)
 */
int minimizer_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_MINIMIZER_H */
