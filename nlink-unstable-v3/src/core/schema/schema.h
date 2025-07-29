/**
 * @file schema.h
 * @brief Header for the NexusLink schema subsystem
 * @copyright OBINexus Computing, 2025
 */

#ifndef NLINK_SCHEMA_H
#define NLINK_SCHEMA_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the schema subsystem
 * @return Status code (0 = success)
 */
int schema_initialize(void);

/**
 * @brief Shutdown the schema subsystem
 * @return Status code (0 = success)
 */
int schema_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_SCHEMA_H */
