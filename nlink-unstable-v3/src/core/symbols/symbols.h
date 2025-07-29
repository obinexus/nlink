/**
 * @file symbols.h
 * @brief Header for the NexusLink symbols subsystem
 * @copyright OBINexus Computing, 2025
 */

#ifndef NLINK_SYMBOLS_H
#define NLINK_SYMBOLS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the symbols subsystem
 * @return Status code (0 = success)
 */
int symbols_initialize(void);

/**
 * @brief Shutdown the symbols subsystem
 * @return Status code (0 = success)
 */
int symbols_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_SYMBOLS_H */
