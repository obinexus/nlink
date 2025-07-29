/**
 * @file pipeline.h
 * @brief Header for the NexusLink pipeline subsystem
 * @copyright OBINexus Computing, 2025
 */

#ifndef NLINK_PIPELINE_H
#define NLINK_PIPELINE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the pipeline subsystem
 * @return Status code (0 = success)
 */
int pipeline_initialize(void);

/**
 * @brief Shutdown the pipeline subsystem
 * @return Status code (0 = success)
 */
int pipeline_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_PIPELINE_H */
