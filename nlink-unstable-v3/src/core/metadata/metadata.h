/**
 * @file metadata.h
 * @brief Header for the NexusLink metadata subsystem
 * @copyright OBINexus Computing, 2025
 */

#ifndef NLINK_METADATA_H
#define NLINK_METADATA_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the metadata subsystem
 * @return Status code (0 = success)
 */
int metadata_initialize(void);

/**
 * @brief Shutdown the metadata subsystem
 * @return Status code (0 = success)
 */
int metadata_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_METADATA_H */
