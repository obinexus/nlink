/**
 * @file config.h
 * @brief NexusLink Configuration Parser with SemVerX Range State Extensions
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.5.0 (SemVerX Integration)
 *
 * Extended configuration system supporting SemVerX range state versioning
 * with backward compatibility for existing NexusLink infrastructure.
 */

#ifndef NLINK_CONFIG_H
#define NLINK_CONFIG_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

// =============================================================================
// CORE NLINK CONSTANTS (Backward Compatibility)
// =============================================================================

#define NLINK_MAX_PATH_LENGTH 512
#define NLINK_MAX_FEATURES 32
#define NLINK_MAX_COMPONENTS 64
#define NLINK_MAX_SYMBOL_NAME 128
#define NLINK_VERSION_STRING_MAX 32

// =============================================================================
// SEMVERX RANGE STATE EXTENSIONS
// =============================================================================

#define SEMVERX_MAX_SWAPPABLE_VERSIONS 16
#define SEMVERX_MAX_EXCLUSION_PATTERNS 32
#define SEMVERX_VERSION_STRING_MAX 64
#define SEMVERX_RANGE_STRING_MAX 256

/**
 * @brief SemVerX range state enumeration for lifecycle management
 */
typedef enum {
    SEMVERX_RANGE_STATE_UNKNOWN = 0,
    SEMVERX_RANGE_STATE_LEGACY,     // Deprecated, migration required
    SEMVERX_RANGE_STATE_STABLE,     // Production-ready, hot-swappable
    SEMVERX_RANGE_STATE_EXPERIMENTAL // Opt-in, validation required
} semverx_range_state_t;

/**
 * @brief SemVerX validation level for compatibility checking
 */
typedef enum {
    SEMVERX_VALIDATION_DISABLED = 0,
    SEMVERX_VALIDATION_PERMISSIVE,  // Warnings only
    SEMVERX_VALIDATION_STRICT,      // Enforcement with errors
    SEMVERX_VALIDATION_PARANOID     // Maximum validation
} semverx_validation_level_t;

/**
 * @brief SemVerX registry coordination mode
 */
typedef enum {
    SEMVERX_REGISTRY_CENTRALIZED = 0,
    SEMVERX_REGISTRY_DISTRIBUTED,
    SEMVERX_REGISTRY_HYBRID
} semverx_registry_mode_t;

// =============================================================================
// CORE NLINK ENUMERATIONS (Original)
// =============================================================================

/**
 * @brief Build pass mode enumeration
 */
typedef enum {
    NLINK_PASS_MODE_UNKNOWN = 0,
    NLINK_PASS_MODE_SINGLE,  // Linear execution
    NLINK_PASS_MODE_MULTI    // Dependency-orchestrated builds
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
    NLINK_CONFIG_ERROR_MEMORY_ALLOCATION = -6,
    NLINK_CONFIG_ERROR_SEMVERX_INCOMPATIBLE = -7,
    NLINK_CONFIG_ERROR_RANGE_STATE_INVALID = -8
} nlink_config_result_t;

// =============================================================================
// THREADING POOL CONFIGURATION (Original)
// =============================================================================

/**
 * @brief Threading pool configuration
 */
typedef struct {
    uint32_t worker_count;
    uint32_t queue_depth;
    uint32_t stack_size_kb;
    bool enable_thread_affinity;
    bool enable_work_stealing;
    struct timespec idle_timeout;
} nlink_thread_pool_config_t;

// =============================================================================
// SEMVERX COMPONENT CONFIGURATION
// =============================================================================

/**
 * @brief SemVerX hot-swap configuration
 */
typedef struct {
    bool hot_swap_enabled;
    bool runtime_validation;
    bool allow_cross_range_swap;
    uint32_t validation_timeout_ms;
    bool rollback_on_failure;
    bool pre_swap_validation;
    bool post_swap_verification;
} semverx_hotswap_config_t;

/**
 * @brief SemVerX compatibility rules
 */
typedef struct {
    char compatible_range[SEMVERX_RANGE_STRING_MAX];
    char **swappable_with;
    size_t swappable_count;
    char **exclusion_patterns;
    size_t exclusion_count;
    bool requires_opt_in;
    bool backward_compatibility;
} semverx_compatibility_rules_t;

/**
 * @brief SemVerX component metadata
 */
typedef struct {
    char component_name[64];
    char version[SEMVERX_VERSION_STRING_MAX];
    semverx_range_state_t range_state;
    semverx_compatibility_rules_t compatibility;
    semverx_hotswap_config_t hotswap;
    struct timespec last_validated;
    uint32_t validation_checksum;
    bool is_validated;
} semverx_component_metadata_t;

// =============================================================================
// FEATURE TOGGLE SYSTEM (Extended)
// =============================================================================

/**
 * @brief Enhanced feature toggle with SemVerX support
 */
typedef struct {
    char feature_name[64];
    bool is_enabled;
    char version_constraint[32];
    uint32_t priority_level;
    semverx_range_state_t feature_range_state;
    bool requires_semverx_validation;
} nlink_feature_toggle_t;

// =============================================================================
// COMPONENT METADATA (Extended)
// =============================================================================

/**
 * @brief Enhanced component metadata with SemVerX
 */
typedef struct {
    char component_name[64];
    char component_path[NLINK_MAX_PATH_LENGTH];
    char version[NLINK_VERSION_STRING_MAX];
    bool has_nlink_txt;
    uint32_t dependency_count;
    char **dependencies;
    
    // SemVerX extensions
    semverx_component_metadata_t semverx_metadata;
    bool is_semverx_compliant;
    struct timespec last_compatibility_check;
} nlink_component_metadata_t;

// =============================================================================
// SEMVERX GLOBAL CONFIGURATION
// =============================================================================

/**
 * @brief SemVerX global configuration
 */
typedef struct {
    bool semverx_enabled;
    semverx_range_state_t project_range_state;
    semverx_validation_level_t validation_level;
    semverx_registry_mode_t registry_mode;
    
    // Registry paths
    char shared_registry_path[NLINK_MAX_PATH_LENGTH];
    char compatibility_matrix_path[NLINK_MAX_PATH_LENGTH];
    char range_policies_path[NLINK_MAX_PATH_LENGTH];
    
    // Global policies
    bool allow_legacy_components;
    bool enforce_range_boundaries;
    bool enable_dependency_graph_analysis;
    bool monitor_hot_swap_events;
    
    // Performance tuning
    uint32_t max_validation_depth;
    uint32_t compatibility_cache_size;
    bool lazy_validation;
} nlink_semverx_config_t;

// =============================================================================
// COMPONENT CONFIGURATION (Original + SemVerX)
// =============================================================================

/**
 * @brief Component configuration parsed from nlink.txt files
 */
typedef struct {
    // Original component identification
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
    
    // SemVerX extensions
    semverx_range_state_t range_state;
    char compatible_range[SEMVERX_RANGE_STRING_MAX];
    bool hot_swap_enabled;
    bool runtime_validation;
    bool requires_semverx_validation;
} nlink_component_config_t;

// =============================================================================
// PKG.NLINK ROOT CONFIGURATION (Extended)
// =============================================================================

/**
 * @brief Extended root configuration with SemVerX support
 */
typedef struct {
    // Original project metadata
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

    // Enhanced feature toggles
    uint32_t feature_count;
    nlink_feature_toggle_t features[NLINK_MAX_FEATURES];

    // Global constraints
    uint32_t max_memory_mb;
    uint32_t compilation_timeout_seconds;
    bool strict_mode;

    // Enhanced component discovery
    uint32_t component_count;
    nlink_component_metadata_t components[NLINK_MAX_COMPONENTS];

    // SemVerX configuration
    nlink_semverx_config_t semverx;

    // Parse metadata
    struct timespec parse_timestamp;
    char config_file_path[NLINK_MAX_PATH_LENGTH];
    uint32_t config_checksum;
    
    // SemVerX metadata
    uint32_t semverx_config_version;
    bool semverx_validation_passed;
    struct timespec last_semverx_validation;
} nlink_pkg_config_t;

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
// CORE CONFIGURATION FUNCTIONS
// =============================================================================

/**
 * @brief Initialize global configuration system
 */
nlink_config_result_t nlink_config_init(void);

/**
 * @brief Parse pkg.nlink root configuration file with SemVerX support
 */
nlink_config_result_t nlink_parse_pkg_config(const char *config_path,
                                             nlink_pkg_config_t *config);

/**
 * @brief Parse nlink.txt subcomponent configuration with SemVerX
 */
nlink_config_result_t nlink_parse_component_config(const char *config_path,
                                                   nlink_component_config_t *component_config);

/**
 * @brief Detect build pass mode based on project structure
 */
nlink_pass_mode_t nlink_detect_pass_mode(const char *project_root_path);

/**
 * @brief Discover and enumerate subcomponents with SemVerX metadata
 */
int nlink_discover_components(const char *project_root_path,
                              nlink_pkg_config_t *config);

/**
 * @brief Validate configuration consistency including SemVerX rules
 */
nlink_config_result_t nlink_validate_config(const nlink_pkg_config_t *config);

/**
 * @brief Get global configuration singleton
 */
nlink_global_config_t *nlink_get_global_config(void);

/**
 * @brief Calculate configuration checksum including SemVerX data
 */
uint32_t nlink_calculate_config_checksum(const nlink_pkg_config_t *config);

/**
 * @brief Print enhanced decision matrix with SemVerX information
 */
void nlink_print_decision_matrix(const nlink_pkg_config_t *config);

/**
 * @brief Clean up and destroy global configuration
 */
void nlink_config_destroy(void);

// =============================================================================
// SEMVERX-SPECIFIC FUNCTIONS
// =============================================================================

/**
 * @brief Initialize SemVerX subsystem
 */
nlink_config_result_t nlink_semverx_init(void);

/**
 * @brief Parse SemVerX configuration section from pkg.nlink
 */
nlink_config_result_t nlink_parse_semverx_config(const char *config_path,
                                                 nlink_semverx_config_t *semverx_config);

/**
 * @brief Parse SemVerX metadata from component nlink.txt
 */
nlink_config_result_t nlink_parse_component_semverx(const char *config_path,
                                                    semverx_component_metadata_t *metadata);

/**
 * @brief Validate SemVerX compatibility between components
 */
nlink_config_result_t nlink_validate_semverx_compatibility(
    const semverx_component_metadata_t *comp1,
    const semverx_component_metadata_t *comp2,
    const nlink_semverx_config_t *global_config);

/**
 * @brief Build compatibility matrix for project
 */
nlink_config_result_t nlink_build_compatibility_matrix(const char *project_root,
                                                       nlink_pkg_config_t *config);

/**
 * @brief Validate hot-swap feasibility
 */
bool nlink_can_hot_swap(const semverx_component_metadata_t *current,
                       const semverx_component_metadata_t *target,
                       const nlink_semverx_config_t *global_config);

/**
 * @brief Load shared artifacts registry
 */
nlink_config_result_t nlink_load_shared_registry(const char *registry_path,
                                                 nlink_semverx_config_t *config);

/**
 * @brief Validate project-wide SemVerX compliance
 */
nlink_config_result_t nlink_validate_project_semverx(const char *project_root,
                                                     nlink_pkg_config_t *config);

/**
 * @brief Parse range state from string
 */
semverx_range_state_t nlink_parse_range_state(const char *state_str);

/**
 * @brief Convert range state to string
 */
const char* nlink_range_state_to_string(semverx_range_state_t state);

// =============================================================================
// SEMVERX UTILITY MACROS
// =============================================================================

/**
 * @brief Check if component is in stable range
 */
#define NLINK_IS_STABLE_RANGE(comp) \
    ((comp)->semverx_metadata.range_state == SEMVERX_RANGE_STATE_STABLE)

/**
 * @brief Check if component is experimental
 */
#define NLINK_IS_EXPERIMENTAL_RANGE(comp) \
    ((comp)->semverx_metadata.range_state == SEMVERX_RANGE_STATE_EXPERIMENTAL)

/**
 * @brief Check if component is legacy
 */
#define NLINK_IS_LEGACY_RANGE(comp) \
    ((comp)->semverx_metadata.range_state == SEMVERX_RANGE_STATE_LEGACY)

/**
 * @brief Check if hot-swap is enabled for component
 */
#define NLINK_CAN_HOT_SWAP_COMPONENT(comp) \
    ((comp)->semverx_metadata.hotswap.hot_swap_enabled && \
     (comp)->semverx_metadata.range_state != SEMVERX_RANGE_STATE_LEGACY)

// =============================================================================
// CONFIGURATION VALIDATION MACROS (Original)
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

#endif /* NLINK_CONFIG_H */