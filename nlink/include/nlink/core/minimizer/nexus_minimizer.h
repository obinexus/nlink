/**
 * @file nexus_minimizer.h
 * @brief Interface for the NexusLink state machine minimizer
 * 
 * This header defines the interface for the NexusLink state machine minimizer,
 * which uses automaton-based optimization techniques to reduce the size and
 * complexity of state machines in NexusLink components.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_CORE_MINIMIZER_NEXUS_MINIMIZER_H
 #define NLINK_CORE_MINIMIZER_NEXUS_MINIMIZER_H
 
 #include "nlink/core/common/types.h"
 #include "nlink/core/common/result.h"
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/minimizer/okpala_automaton.h"  /* Include automaton definitions */
 #include <stdbool.h>
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 

 /**
  * @brief Minimization levels for the NexusLink minimizer
  */
 typedef enum NexusMinimizerLevel {
     NEXUS_MINIMIZE_NONE = 0,       /**< No minimization */
     NEXUS_MINIMIZE_BASIC = 1,      /**< Basic minimization (redundancy removal) */
     NEXUS_MINIMIZE_STANDARD = 2,   /**< Standard minimization (Hopcroft algorithm) */
     NEXUS_MINIMIZE_AGGRESSIVE = 3, /**< Aggressive minimization (with boolean reduction) */
     NEXUS_MINIMIZE_MAX = 4         /**< Maximum minimization (all techniques) */
 } NexusMinimizerLevel;
 
 /**
  * @brief Configuration for the NexusLink minimizer
  */
 typedef struct NexusMinimizerConfig {
     NexusMinimizerLevel level;     /**< Minimization level */
     bool enable_metrics;           /**< Whether to collect metrics during minimization */
     bool verbose;                  /**< Whether to output verbose information */
 } NexusMinimizerConfig;
 
 /**
  * @brief Metrics collected during minimization
  */
 typedef struct NexusMinimizationMetrics {
     size_t original_states;        /**< Number of states before minimization */
     size_t minimized_states;       /**< Number of states after minimization */
     size_t original_size;          /**< Size in bytes before minimization */
     size_t minimized_size;         /**< Size in bytes after minimization */
     double time_taken_ms;          /**< Time taken for minimization in milliseconds */
     bool boolean_reduction;        /**< Whether boolean reduction was used */
 } NexusMinimizationMetrics;
 
 /**
  * @brief Initialize the minimizer subsystem
  * 
  * This function initializes the minimizer subsystem and must be called
  * before using any other minimizer functions.
  * 
  * @param ctx The NexusLink context
  * @return NexusResult result code (NEXUS_SUCCESS on success)
  */
 NexusResult nexus_minimizer_initialize(NexusContext* ctx);
 
 /**
  * @brief Create default minimizer configuration
  * 
  * @return Default configuration with standard minimization level
  */
 NexusMinimizerConfig nexus_minimizer_default_config(void);

 /**
  * @brief Create minimizer configuration with specified level
  *
  * @param level The minimization level to use
  * @return Configuration with the specified minimization level
  */
 NexusMinimizerConfig nexus_minimizer_config_from_level(NexusMinimizerLevel level);
 
 /**
  * @brief Create an automaton representation from a component
  * 
  * @param ctx The NexusLink context
  * @param component_path Path to the component file
  * @return Pointer to created automaton, or NULL on failure
  */
 OkpalaAutomaton* nexus_create_automaton_from_component(
     NexusContext* ctx,
     const char* component_path
 );
 
 /**
  * @brief Apply a minimized automaton back to a component
  * 
  * @param ctx The NexusLink context
  * @param component_path Path to the component file
  * @param minimized The minimized automaton to apply
  * @return NexusResult result code (NEXUS_SUCCESS on success)
  */
 NexusResult nexus_apply_minimized_automaton(
     NexusContext* ctx,
     const char* component_path,
     OkpalaAutomaton* minimized
 );
 
 /**
  * @brief Minimize a component using automaton-based state minimization
  * 
  * This function minimizes a NexusLink component by creating an automaton
  * representation, minimizing the automaton, and then applying the minimized
  * automaton back to the component.
  * 
  * @param ctx The NexusLink context
  * @param component_path Path to the component file
  * @param config Minimization configuration
  * @param metrics Optional pointer to store minimization metrics (can be NULL)
  * @return NexusResult result code (NEXUS_SUCCESS on success)
  */
 NexusResult nexus_minimize_component(
     NexusContext* ctx,
     const char* component_path,
     NexusMinimizerConfig config,
     NexusMinimizationMetrics* metrics
 );
 /**
  * @brief Clean up the minimizer subsystem
  * 
  * This function cleans up any resources allocated by the minimizer subsystem.
  * 
  * @param ctx The NexusLink context
  */
 void nexus_minimizer_cleanup(NexusContext* ctx);

 /**
  * @brief Convenience function to minimize a component using just a level
  *
  * @param ctx The NexusLink context
  * @param component_path Path to the component file
  * @param level Minimization level
  * @param metrics Optional pointer to store minimization metrics (can be NULL)
  * @return NexusResult result code (NEXUS_SUCCESS on success)
  */
 NexusResult nexus_minimize_component_with_level(
     NexusContext* ctx,
     const char* component_path,
     NexusMinimizerLevel level,
     NexusMinimizationMetrics* metrics
 );
 /**
  * @brief Clean up the minimizer subsystem
  * 
  * This function cleans up any resources allocated by the minimizer subsystem.
  * 
  * @param ctx The NexusLink context
  */
 void nexus_minimizer_cleanup(NexusContext* ctx);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_CORE_MINIMIZER_NEXUS_MINIMIZER_H */