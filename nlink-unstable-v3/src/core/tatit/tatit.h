/**
 * @file tatit.h
 * @brief Header for the NexusLink tatit subsystem
 * @copyright OBINexus Computing, 2025
 */

#ifndef NLINK_TATIT_H
#define NLINK_TATIT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the tatit subsystem
 * @return Status code (0 = success)
 */
int tatit_initialize(void);

/**
 * @brief Shutdown the tatit subsystem
 * @return Status code (0 = success)
 */
int tatit_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_TATIT_H */
