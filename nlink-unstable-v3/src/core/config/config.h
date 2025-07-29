/**
 * @file config.h
 * @brief Header for the NexusLink config subsystem
 * @copyright OBINexus Computing, 2025
 */

#ifndef NLINK_CONFIG_H
#define NLINK_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the config subsystem
 * @return Status code (0 = success)
 */
int config_initialize(void);

/**
 * @brief Shutdown the config subsystem
 * @return Status code (0 = success)
 */
int config_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_CONFIG_H */
