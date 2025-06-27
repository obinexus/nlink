/**
 * @file config.h
 * @brief NexusLink Configuration Parser and Mode Resolution System
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.0.0
 *
 * Core configuration data structures for pkg.nlink and nlink.txt parsing.
 * Implements deterministic build mode resolution and threading pool
 * configuration.
 *
 * Architecture:
 * - pkg.nlink: Root manifest with global constraints and pass-mode declaration
 * - nlink.txt: Optional subcomponent coordination for multi-pass builds
 * - Single-pass mode: Linear execution chain without subcomponent discovery
 * - Multi-pass mode: Dependency-orchestrated coordination across components
 */

#ifndef NLINK_CONFIG_H
#define NLINK_CONFIG_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

// =============================================================================
// CONFIGURATION CONSTANTS AND LIMITS
// =============================================================================

#define NLINK_MAX_PATH_LENGTH 512
#define NLINK_MAX_FEATURES 32
#define NLINK_MAX_COMPONENTS 64
#define NLINK_MAX_SYMBOL_NAME 128
#define NLINK_VERSION_STRING_MAX 32

/**
 * @brief Build pass mode enumeration for mode resolution
 */
typedef enum {
  NLINK_PASS_MODE_UNKNOWN = 0,
  NLINK_PASS_MODE_SINGLE, // Linear execution, no subcomponent coordination
  NLINK_PASS_MODE_MULTI   // Dependency-orchestrated multi-component builds
} nlink_pass_mode_t;

/**
 * @brief Configuration parsing result codes
 */
typedef enum {
  NLINK_CONFIG_SUCCESS = 0,
  NLINK_CONFIG_ERROR_FILE_NOT_FOUND = -1,
  NLINK_CONFIG_ERROR_PARSE_FAILED = -2,
  NLINK_CONFIG_ERROR_INVALID_FORMAT = -3,
  NLINK_CONFIG_ERROR_MISSING_REQUIRED_FIELD = -4,
  NLINK_CONFIG_ERROR_THREAD_POOL_INVALID = -5,
  NLINK_CONFIG_ERROR_MEMORY_ALLOCATION = -6
} nlink_config_result_t;

// =============================================================================
// THREADING POOL CONFIGURATION
// =============================================================================

/**
 * @brief Threading pool configuration for concurrent processing
 */
typedef struct {
  uint32_t worker_count;        // Number of worker threads
  uint32_t queue_depth;         // Maximum queued tasks
  uint32_t stack_size_kb;       // Stack size per thread in KB
  bool enable_thread_affinity;  // CPU affinity binding
  bool enable_work_stealing;    // Work-stealing scheduler
  struct timespec idle_timeout; // Thread idle timeout
} nlink_thread_pool_config_t;

// =============================================================================
// FEATURE TOGGLE SYSTEM
// =============================================================================

/**
 * @brief Feature toggle configuration for build customization
 */
typedef struct {
  char feature_name[64];       // Feature identifier
  bool is_enabled;             // Enable/disable flag
  char version_constraint[32]; // Semver constraint for feature
  uint32_t priority_level;     // Feature priority (0 = highest)
} nlink_feature_toggle_t;

// =============================================================================
// SUBCOMPONENT COORDINATION
// =============================================================================

/**
 * @brief Subcomponent metadata for multi-pass coordination
 */
typedef struct {
  char component_name[64];                    // Component identifier
  char component_path[NLINK_MAX_PATH_LENGTH]; // Relative path to component
  char version[NLINK_VERSION_STRING_MAX];     // Component version
  bool has_nlink_txt;                         // Whether component has nlink.txt
  uint32_t dependency_count;                  // Number of dependencies
  char **dependencies;                        // Array of dependency names
} nlink_component_metadata_t;

// =============================================================================
// PKG.NLINK ROOT CONFIGURATION
// =============================================================================

/**
 * @brief Root configuration parsed from pkg.nlink manifest
 */
typedef struct {
  // Project metadata
  char project_name[128];
  char project_version[NLINK_VERSION_STRING_MAX];
  char entry_point[NLINK_MAX_PATH_LENGTH];

  // Build mode configuration
  nlink_pass_mode_t pass_mode;
  bool experimental_mode_enabled;
  bool unicode_normalization_enabled;
  bool isomorphic_reduction_enabled;

  // Threading configuration
  nlink_thread_pool_config_t thread_pool;

  // Feature toggles
  uint32_t feature_count;
  nlink_feature_toggle_t features[NLINK_MAX_FEATURES];

  // Global constraints
  uint32_t max_memory_mb;
  uint32_t compilation_timeout_seconds;
  bool strict_mode;

  // Component discovery (populated during parsing)
  uint32_t component_count;
  nlink_component_metadata_t components[NLINK_MAX_COMPONENTS];

  // Parse metadata
  struct timespec parse_timestamp;
  char config_file_path[NLINK_MAX_PATH_LENGTH];
  uint32_t config_checksum;
} nlink_pkg_config_t;

// =============================================================================
// NLINK.TXT SUBCOMPONENT CONFIGURATION
// =============================================================================

/**
 * @brief Subcomponent configuration parsed from nlink.txt files
 */
typedef struct {
  // Component identification
  char component_name[64];
  char component_version[NLINK_VERSION_STRING_MAX];
  char parent_component[64];

  // Symbol imports and exports
  uint32_t import_count;
  char imports[32][NLINK_MAX_SYMBOL_NAME];
  uint32_t export_count;
  char exports[32][NLINK_MAX_SYMBOL_NAME];

  // Compilation directives
  bool requires_preprocessing;
  bool enable_optimizations;
  uint32_t optimization_level;

  // Dependencies
  uint32_t dependency_count;
  char dependencies[16][64];

  // Build constraints
  uint32_t max_compile_time_seconds;
  bool parallel_compilation_allowed;
} nlink_component_config_t;

// =============================================================================
// GLOBAL CONFIGURATION STATE
// =============================================================================

/**
 * @brief Global configuration state singleton
 */
typedef struct {
  nlink_pkg_config_t pkg_config;
  bool is_initialized;
  bool is_single_pass_mode;
  pthread_mutex_t config_mutex;
  uint32_t active_component_count;
  nlink_component_config_t *component_configs;
} nlink_global_config_t;

// =============================================================================
// CONFIGURATION PARSING FUNCTIONS
// =============================================================================

/**
 * @brief Initialize global configuration system
 * @return NLINK_CONFIG_SUCCESS on success, error code on failure
 */
nlink_config_result_t nlink_config_init(void);

/**
 * @brief Parse pkg.nlink root configuration file
 * @param config_path Path to pkg.nlink file
 * @param config Output configuration structure
 * @return NLINK_CONFIG_SUCCESS on success, error code on failure
 */
nlink_config_result_t nlink_parse_pkg_config(const char *config_path,
                                             nlink_pkg_config_t *config);

/**
 * @brief Parse nlink.txt subcomponent configuration
 * @param config_path Path to nlink.txt file
 * @param component_config Output component configuration
 * @return NLINK_CONFIG_SUCCESS on success, error code on failure
 */
nlink_config_result_t
nlink_parse_component_config(const char *config_path,
                             nlink_component_config_t *component_config);

/**
 * @brief Detect build pass mode based on project structure
 * @param project_root_path Root directory of project
 * @return Detected pass mode
 */
nlink_pass_mode_t nlink_detect_pass_mode(const char *project_root_path);

/**
 * @brief Discover and enumerate subcomponents in project
 * @param project_root_path Root directory to scan
 * @param config Configuration to populate with discovered components
 * @return Number of components discovered, -1 on error
 */
int nlink_discover_components(const char *project_root_path,
                              nlink_pkg_config_t *config);

/**
 * @brief Validate configuration consistency and constraints
 * @param config Configuration to validate
 * @return NLINK_CONFIG_SUCCESS if valid, error code on validation failure
 */
nlink_config_result_t nlink_validate_config(const nlink_pkg_config_t *config);

/**
 * @brief Get global configuration singleton
 * @return Pointer to global configuration, NULL if not initialized
 */
nlink_global_config_t *nlink_get_global_config(void);

/**
 * @brief Calculate configuration checksum for integrity validation
 * @param config Configuration structure
 * @return CRC32 checksum of configuration
 */
uint32_t nlink_calculate_config_checksum(const nlink_pkg_config_t *config);

/**
 * @brief Print configuration decision matrix for debugging
 * @param config Configuration to analyze
 */
void nlink_print_decision_matrix(const nlink_pkg_config_t *config);

/**
 * @brief Clean up and destroy global configuration
 */
void nlink_config_destroy(void);

// =============================================================================
// CONFIGURATION VALIDATION MACROS
// =============================================================================

/**
 * @brief Macro for validating required configuration fields
 */
#define NLINK_VALIDATE_REQUIRED_FIELD(field, error_msg)                        \
  do {                                                                         \
    if (!(field)) {                                                            \
      fprintf(stderr, "[CONFIG ERROR] %s\n", error_msg);                       \
      return NLINK_CONFIG_ERROR_MISSING_REQUIRED_FIELD;                        \
    }                                                                          \
  } while (0)

/**
 * @brief Macro for thread-safe configuration access
 */
#define NLINK_CONFIG_LOCK()                                                    \
  do {                                                                         \
    nlink_global_config_t *global = nlink_get_global_config();                 \
    if (global)                                                                \
      pthread_mutex_lock(&global->config_mutex);                               \
  } while (0)

#define NLINK_CONFIG_UNLOCK()                                                  \
  do {                                                                         \
    nlink_global_config_t *global = nlink_get_global_config();                 \
    if (global)                                                                \
      pthread_mutex_unlock(&global->config_mutex);                             \
  } while (0)

#endif // NLINK_CONFIG_H
