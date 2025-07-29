/**
 * @file tactic.h
 * @brief Header for the NexusLink tactic subsystem
 * @author NexusLink Implementation Team
 * @copyright OBINexus Computing, 2025
 */

#ifndef NLINK_TACTIC_H
#define NLINK_TACTIC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration structure for tactic subsystem
 */
typedef struct {
    int mode;                   /**< Operating mode */
    unsigned int flags;         /**< Configuration flags */
    void* context;              /**< User-defined context */
} tactic_config_t;

/**
 * @brief Initialize the tactic subsystem
 * @param config Configuration parameters
 * @return Status code (0 = success)
 */
int tactic_initialize(tactic_config_t* config);

/**
 * @brief Shutdown the tactic subsystem
 * @return Status code (0 = success)
 */
int tactic_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* NLINK_TACTIC_H */
