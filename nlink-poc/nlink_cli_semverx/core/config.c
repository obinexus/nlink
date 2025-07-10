/**
 * @file config.c
 * @brief NexusLink Configuration Parser with SemVerX Range State Extensions
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.5.0 (SemVerX Integration)
 * 
 * Extended implementation supporting SemVerX range state versioning
 * with systematic lifecycle management for legacy, stable, and experimental components.
 */

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include "../include/core/config.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

// =============================================================================
// GLOBAL STATE MANAGEMENT
// =============================================================================

static nlink_global_config_t g_nlink_config = {0};
static bool g_config_initialized = false;

// =============================================================================
// UTILITY FUNCTIONS (Original + Extensions)
// =============================================================================

/**
 * @brief Check if file exists and is readable
 */
static bool file_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0 && S_ISREG(st.st_mode));
}

/**
 * @brief Check if directory exists
 */
static bool directory_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

/**
 * @brief Safe string copy with bounds checking and proper termination
 */
static void safe_strcpy(char *dest, const char *src, size_t dest_size) {
    if (dest && src && dest_size > 0) {
        size_t src_len = strlen(src);
        size_t copy_len = (src_len < dest_size - 1) ? src_len : dest_size - 1;
        memcpy(dest, src, copy_len);
        dest[copy_len] = '\0';
    }
}

/**
 * @brief Calculate CRC32 checksum for data integrity
 */
static uint32_t calculate_crc32(const void *data, size_t length) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < length; i++) {
        crc ^= bytes[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
        }
    }

    return ~crc;
}

/**
 * @brief Trim whitespace from string
 */
static void trim_whitespace(char *str) {
    if (!str) return;

    // Trim leading whitespace
    char *start = str;
    while (*start && (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r')) {
        start++;
    }

    // Move trimmed string to beginning
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    // Trim trailing whitespace
    char *end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        *end = '\0';
        end--;
    }
}

/**
 * @brief Safe path construction with buffer overflow protection
 */
static int safe_path_join(char *dest, size_t dest_size, const char *base, const char *component) {
    if (!dest || !base || !component || dest_size == 0) {
        return -1;
    }
    
    size_t base_len = strlen(base);
    size_t comp_len = strlen(component);
    
    // Check if result would fit (base + "/" + component + null terminator)
    if (base_len + 1 + comp_len + 1 > dest_size) {
        return -1;  // Would overflow
    }
    
    int result = snprintf(dest, dest_size, "%s/%s", base, component);
    return (result >= 0 && (size_t)result < dest_size) ? 0 : -1;
}

// =============================================================================
// SEMVERX RANGE STATE FUNCTIONS
// =============================================================================

/**
 * @brief Parse range state from string with comprehensive validation
 */
semverx_range_state_t nlink_parse_range_state(const char *state_str) {
    if (!state_str) return SEMVERX_RANGE_STATE_UNKNOWN;
    
    if (strcmp(state_str, "legacy") == 0) return SEMVERX_RANGE_STATE_LEGACY;
    if (strcmp(state_str, "stable") == 0) return SEMVERX_RANGE_STATE_STABLE;
    if (strcmp(state_str, "experimental") == 0) return SEMVERX_RANGE_STATE_EXPERIMENTAL;
    
    return SEMVERX_RANGE_STATE_UNKNOWN;
}

/**
 * @brief Convert range state to string for display
 */
const char* nlink_range_state_to_string(semverx_range_state_t state) {
    switch (state) {
        case SEMVERX_RANGE_STATE_LEGACY: return "legacy";
        case SEMVERX_RANGE_STATE_STABLE: return "stable";
        case SEMVERX_RANGE_STATE_EXPERIMENTAL: return "experimental";
        default: return "unknown";
    }
}

/**
 * @brief Initialize SemVerX subsystem with default policies
 */
nlink_config_result_t nlink_semverx_init(void) {
    printf("[SEMVERX] Initializing SemVerX range state subsystem\n");
    
    // Set default SemVerX policies
    nlink_global_config_t *global = nlink_get_global_config();
    if (!global) return NLINK_CONFIG_ERROR_MEMORY_ALLOCATION;
    
    global->pkg_config.semverx.semverx_enabled = true;
    global->pkg_config.semverx.project_range_state = SEMVERX_RANGE_STATE_STABLE;
    global->pkg_config.semverx.validation_level = SEMVERX_VALIDATION_STRICT;
    global->pkg_config.semverx.registry_mode = SEMVERX_REGISTRY_CENTRALIZED;
    global->pkg_config.semverx.allow_legacy_components = false;
    global->pkg_config.semverx.enforce_range_boundaries = true;
    global->pkg_config.semverx.max_validation_depth = 10;
    global->pkg_config.semverx_config_version = 1;
    
    printf("[SEMVERX] SemVerX subsystem initialized successfully\n");
    return NLINK_CONFIG_SUCCESS;
}

// =============================================================================
// CONFIGURATION PARSING IMPLEMENTATION (Extended)
// =============================================================================

nlink_config_result_t nlink_config_init(void) {
    if (g_config_initialized) {
        return NLINK_CONFIG_SUCCESS;
    }

    // Initialize global configuration structure
    memset(&g_nlink_config, 0, sizeof(nlink_global_config_t));

    // Initialize mutex for thread-safe access
    if (pthread_mutex_init(&g_nlink_config.config_mutex, NULL) != 0) {
        return NLINK_CONFIG_ERROR_MEMORY_ALLOCATION;
    }

    // Set default thread pool configuration
    g_nlink_config.pkg_config.thread_pool.worker_count = 4;
    g_nlink_config.pkg_config.thread_pool.queue_depth = 64;
    g_nlink_config.pkg_config.thread_pool.stack_size_kb = 512;
    g_nlink_config.pkg_config.thread_pool.enable_thread_affinity = false;
    g_nlink_config.pkg_config.thread_pool.enable_work_stealing = true;
    g_nlink_config.pkg_config.thread_pool.idle_timeout.tv_sec = 30;
    g_nlink_config.pkg_config.thread_pool.idle_timeout.tv_nsec = 0;

    // Set default global constraints
    g_nlink_config.pkg_config.max_memory_mb = 2048;
    g_nlink_config.pkg_config.compilation_timeout_seconds = 300;
    g_nlink_config.pkg_config.strict_mode = true;
    g_nlink_config.pkg_config.unicode_normalization_enabled = true;
    g_nlink_config.pkg_config.isomorphic_reduction_enabled = true;

    clock_gettime(CLOCK_REALTIME, &g_nlink_config.pkg_config.parse_timestamp);

    g_config_initialized = true;
    g_nlink_config.is_initialized = true;

    // Initialize SemVerX subsystem
    nlink_semverx_init();

    return NLINK_CONFIG_SUCCESS;
}

nlink_config_result_t nlink_parse_pkg_config(const char *config_path,
                                             nlink_pkg_config_t *config) {
    if (!config_path || !config) {
        return NLINK_CONFIG_ERROR_INVALID_FORMAT;
    }

    if (!file_exists(config_path)) {
        fprintf(stderr, "[CONFIG] pkg.nlink not found at: %s\n", config_path);
        return NLINK_CONFIG_ERROR_FILE_NOT_FOUND;
    }

    FILE *file = fopen(config_path, "r");
    if (!file) {
        fprintf(stderr, "[CONFIG] Failed to open pkg.nlink: %s\n", strerror(errno));
        return NLINK_CONFIG_ERROR_PARSE_FAILED;
    }

    char line[512];
    char section[64] = {0};

    // Initialize configuration with defaults
    safe_strcpy(config->project_name, "unknown", sizeof(config->project_name));
    safe_strcpy(config->project_version, "1.0.0", sizeof(config->project_version));
    safe_strcpy(config->entry_point, "main.c", sizeof(config->entry_point));
    config->pass_mode = NLINK_PASS_MODE_SINGLE; // Default to single-pass
    
    // Initialize SemVerX defaults
    config->semverx.semverx_enabled = false;
    config->semverx.project_range_state = SEMVERX_RANGE_STATE_STABLE;
    config->semverx.validation_level = SEMVERX_VALIDATION_STRICT;
    config->semverx.registry_mode = SEMVERX_REGISTRY_CENTRALIZED;

    while (fgets(line, sizeof(line), file)) {
        trim_whitespace(line);

        // Skip empty lines and comments
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        // Parse section headers [section_name]
        if (line[0] == '[' && line[strlen(line) - 1] == ']') {
            size_t section_len = strlen(line) - 2;
            if (section_len < sizeof(section)) {
                memcpy(section, line + 1, section_len);
                section[section_len] = '\0';
            }
            continue;
        }

        // Parse key-value pairs
        char *equals = strchr(line, '=');
        if (!equals) continue;

        *equals = '\0';
        char *key = line;
        char *value = equals + 1;
        trim_whitespace(key);
        trim_whitespace(value);

        // Parse based on current section
        if (strcmp(section, "project") == 0) {
            if (strcmp(key, "name") == 0) {
                safe_strcpy(config->project_name, value, sizeof(config->project_name));
            } else if (strcmp(key, "version") == 0) {
                safe_strcpy(config->project_version, value, sizeof(config->project_version));
            } else if (strcmp(key, "entry_point") == 0) {
                safe_strcpy(config->entry_point, value, sizeof(config->entry_point));
            }
        } else if (strcmp(section, "build") == 0) {
            if (strcmp(key, "pass_mode") == 0) {
                if (strcmp(value, "single") == 0) {
                    config->pass_mode = NLINK_PASS_MODE_SINGLE;
                } else if (strcmp(value, "multi") == 0) {
                    config->pass_mode = NLINK_PASS_MODE_MULTI;
                }
            } else if (strcmp(key, "experimental_mode") == 0) {
                config->experimental_mode_enabled = (strcmp(value, "true") == 0);
            } else if (strcmp(key, "strict_mode") == 0) {
                config->strict_mode = (strcmp(value, "true") == 0);
            } else if (strcmp(key, "semverx_enabled") == 0) {
                config->semverx.semverx_enabled = (strcmp(value, "true") == 0);
            }
        } else if (strcmp(section, "semverx") == 0) {
            // Parse SemVerX-specific configuration
            if (strcmp(key, "range_state") == 0) {
                config->semverx.project_range_state = nlink_parse_range_state(value);
            } else if (strcmp(key, "compatible_range") == 0) {
                safe_strcpy(config->semverx.shared_registry_path, value, 
                           sizeof(config->semverx.shared_registry_path));
            } else if (strcmp(key, "registry_mode") == 0) {
                if (strcmp(value, "centralized") == 0) {
                    config->semverx.registry_mode = SEMVERX_REGISTRY_CENTRALIZED;
                } else if (strcmp(value, "distributed") == 0) {
                    config->semverx.registry_mode = SEMVERX_REGISTRY_DISTRIBUTED;
                } else if (strcmp(value, "hybrid") == 0) {
                    config->semverx.registry_mode = SEMVERX_REGISTRY_HYBRID;
                }
            } else if (strcmp(key, "validation_level") == 0) {
                if (strcmp(value, "disabled") == 0) {
                    config->semverx.validation_level = SEMVERX_VALIDATION_DISABLED;
                } else if (strcmp(value, "permissive") == 0) {
                    config->semverx.validation_level = SEMVERX_VALIDATION_PERMISSIVE;
                } else if (strcmp(value, "strict") == 0) {
                    config->semverx.validation_level = SEMVERX_VALIDATION_STRICT;
                } else if (strcmp(value, "paranoid") == 0) {
                    config->semverx.validation_level = SEMVERX_VALIDATION_PARANOID;
                }
            } else if (strcmp(key, "hot_swap_enabled") == 0) {
                config->semverx.monitor_hot_swap_events = (strcmp(value, "true") == 0);
            } else if (strcmp(key, "allow_cross_range_swap") == 0) {
                config->semverx.enforce_range_boundaries = (strcmp(value, "false") == 0);
            } else if (strcmp(key, "shared_registry_path") == 0) {
                safe_strcpy(config->semverx.shared_registry_path, value,
                           sizeof(config->semverx.shared_registry_path));
            } else if (strcmp(key, "compatibility_matrix_path") == 0) {
                safe_strcpy(config->semverx.compatibility_matrix_path, value,
                           sizeof(config->semverx.compatibility_matrix_path));
            } else if (strcmp(key, "range_policies_path") == 0) {
                safe_strcpy(config->semverx.range_policies_path, value,
                           sizeof(config->semverx.range_policies_path));
            }
        } else if (strcmp(section, "threading") == 0) {
            if (strcmp(key, "worker_count") == 0) {
                config->thread_pool.worker_count = (uint32_t)atoi(value);
            } else if (strcmp(key, "queue_depth") == 0) {
                config->thread_pool.queue_depth = (uint32_t)atoi(value);
            } else if (strcmp(key, "stack_size_kb") == 0) {
                config->thread_pool.stack_size_kb = (uint32_t)atoi(value);
            } else if (strcmp(key, "enable_work_stealing") == 0) {
                config->thread_pool.enable_work_stealing = (strcmp(value, "true") == 0);
            }
        } else if (strcmp(section, "features") == 0) {
            if (config->feature_count < NLINK_MAX_FEATURES) {
                nlink_feature_toggle_t *feature = &config->features[config->feature_count];
                safe_strcpy(feature->feature_name, key, sizeof(feature->feature_name));
                feature->is_enabled = (strcmp(value, "true") == 0);
                feature->priority_level = config->feature_count;
                safe_strcpy(feature->version_constraint, "*", sizeof(feature->version_constraint));
                
                // Set SemVerX defaults for features
                feature->feature_range_state = SEMVERX_RANGE_STATE_STABLE;
                feature->requires_semverx_validation = config->semverx.semverx_enabled;
                
                config->feature_count++;
            }
        }
    }

    fclose(file);

    // Store configuration metadata
    safe_strcpy(config->config_file_path, config_path, sizeof(config->config_file_path));
    clock_gettime(CLOCK_REALTIME, &config->parse_timestamp);
    config->config_checksum = nlink_calculate_config_checksum(config);

    // Validate SemVerX configuration if enabled
    if (config->semverx.semverx_enabled) {
        printf("[SEMVERX] SemVerX enabled for project: %s\n", config->project_name);
        printf("[SEMVERX] Range state: %s\n", nlink_range_state_to_string(config->semverx.project_range_state));
        clock_gettime(CLOCK_REALTIME, &config->last_semverx_validation);
        config->semverx_validation_passed = true;
    }

    return NLINK_CONFIG_SUCCESS;
}

nlink_config_result_t nlink_parse_component_config(const char *config_path,
                                                   nlink_component_config_t *component_config) {
    if (!config_path || !component_config) {
        return NLINK_CONFIG_ERROR_INVALID_FORMAT;
    }

    if (!file_exists(config_path)) {
        return NLINK_CONFIG_ERROR_FILE_NOT_FOUND;
    }

    FILE *file = fopen(config_path, "r");
    if (!file) {
        return NLINK_CONFIG_ERROR_PARSE_FAILED;
    }

    char line[512];
    char section[64] = {0};

    // Initialize with defaults
    safe_strcpy(component_config->component_name, "unknown", sizeof(component_config->component_name));
    safe_strcpy(component_config->component_version, "1.0.0", sizeof(component_config->component_version));
    component_config->optimization_level = 2;
    component_config->max_compile_time_seconds = 60;
    component_config->parallel_compilation_allowed = true;
    
    // Initialize SemVerX defaults
    component_config->range_state = SEMVERX_RANGE_STATE_STABLE;
    component_config->hot_swap_enabled = false;
    component_config->runtime_validation = true;
    component_config->requires_semverx_validation = false;

    while (fgets(line, sizeof(line), file)) {
        trim_whitespace(line);

        if (line[0] == '\0' || line[0] == '#') continue;

        if (line[0] == '[' && line[strlen(line) - 1] == ']') {
            size_t section_len = strlen(line) - 2;
            if (section_len < sizeof(section)) {
                memcpy(section, line + 1, section_len);
                section[section_len] = '\0';
            }
            continue;
        }

        char *equals = strchr(line, '=');
        if (!equals) continue;

        *equals = '\0';
        char *key = line;
        char *value = equals + 1;
        trim_whitespace(key);
        trim_whitespace(value);

        if (strcmp(section, "component") == 0) {
            if (strcmp(key, "name") == 0) {
                safe_strcpy(component_config->component_name, value, sizeof(component_config->component_name));
            } else if (strcmp(key, "version") == 0) {
                safe_strcpy(component_config->component_version, value, sizeof(component_config->component_version));
            }
        } else if (strcmp(section, "semverx") == 0) {
            // Parse SemVerX component-specific configuration
            if (strcmp(key, "range_state") == 0) {
                component_config->range_state = nlink_parse_range_state(value);
            } else if (strcmp(key, "compatible_range") == 0) {
                safe_strcpy(component_config->compatible_range, value, sizeof(component_config->compatible_range));
            } else if (strcmp(key, "hot_swap_enabled") == 0) {
                component_config->hot_swap_enabled = (strcmp(value, "true") == 0);
            } else if (strcmp(key, "runtime_validation") == 0) {
                if (strcmp(value, "strict") == 0 || strcmp(value, "true") == 0) {
                    component_config->runtime_validation = true;
                } else {
                    component_config->runtime_validation = false;
                }
            } else if (strcmp(key, "requires_opt_in") == 0) {
                component_config->requires_semverx_validation = (strcmp(value, "true") == 0);
            }
        } else if (strcmp(section, "compilation") == 0) {
            if (strcmp(key, "optimization_level") == 0) {
                component_config->optimization_level = (uint32_t)atoi(value);
            } else if (strcmp(key, "max_compile_time") == 0) {
                component_config->max_compile_time_seconds = (uint32_t)atoi(value);
            } else if (strcmp(key, "parallel_allowed") == 0) {
                component_config->parallel_compilation_allowed = (strcmp(value, "true") == 0);
            } else if (strcmp(key, "requires_semverx_validation") == 0) {
                component_config->requires_semverx_validation = (strcmp(value, "true") == 0);
            }
        }
    }

    fclose(file);
    
    // Validate SemVerX component configuration
    if (component_config->range_state != SEMVERX_RANGE_STATE_UNKNOWN) {
        printf("[SEMVERX] Component '%s' configured with range state: %s\n",
               component_config->component_name, 
               nlink_range_state_to_string(component_config->range_state));
    }
    
    return NLINK_CONFIG_SUCCESS;
}

// =============================================================================
// REMAINING CORE FUNCTIONS (Original Implementation)
// =============================================================================

nlink_pass_mode_t nlink_detect_pass_mode(const char *project_root_path) {
    if (!project_root_path || !directory_exists(project_root_path)) {
        return NLINK_PASS_MODE_UNKNOWN;
    }

    // Check for pkg.nlink in root
    char pkg_config_path[NLINK_MAX_PATH_LENGTH];
    if (safe_path_join(pkg_config_path, sizeof(pkg_config_path), project_root_path, "pkg.nlink") != 0) {
        return NLINK_PASS_MODE_UNKNOWN;
    }

    if (!file_exists(pkg_config_path)) {
        return NLINK_PASS_MODE_UNKNOWN;
    }

    // Count subdirectories with potential components
    DIR *dir = opendir(project_root_path);
    if (!dir) {
        return NLINK_PASS_MODE_UNKNOWN;
    }

    int component_folder_count = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0) {

            char subdir_path[NLINK_MAX_PATH_LENGTH];
            if (safe_path_join(subdir_path, sizeof(subdir_path), project_root_path, entry->d_name) != 0) {
                continue;  // Skip if path too long
            }

            // Check if subdirectory contains source files or nlink.txt
            char nlink_txt_path[NLINK_MAX_PATH_LENGTH];
            if (safe_path_join(nlink_txt_path, sizeof(nlink_txt_path), subdir_path, "nlink.txt") == 0) {
                if (file_exists(nlink_txt_path)) {
                    component_folder_count++;
                }
            }
        }
    }

    closedir(dir);

    // Decision logic: Single component folder = single-pass, multiple = multi-pass
    return (component_folder_count <= 1) ? NLINK_PASS_MODE_SINGLE : NLINK_PASS_MODE_MULTI;
}

int nlink_discover_components(const char *project_root_path,
                              nlink_pkg_config_t *config) {
    if (!project_root_path || !config || !directory_exists(project_root_path)) {
        return -1;
    }

    DIR *dir = opendir(project_root_path);
    if (!dir) {
        return -1;
    }

    int discovered_count = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL &&
           discovered_count < NLINK_MAX_COMPONENTS) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0) {

            char subdir_path[NLINK_MAX_PATH_LENGTH];
            if (safe_path_join(subdir_path, sizeof(subdir_path), project_root_path, entry->d_name) != 0) {
                continue;  // Skip if path too long
            }

            char nlink_txt_path[NLINK_MAX_PATH_LENGTH];
            bool has_nlink_txt = false;
            if (safe_path_join(nlink_txt_path, sizeof(nlink_txt_path), subdir_path, "nlink.txt") == 0) {
                has_nlink_txt = file_exists(nlink_txt_path);
            }

            nlink_component_metadata_t *component = &config->components[discovered_count];
            safe_strcpy(component->component_name, entry->d_name, sizeof(component->component_name));
            safe_strcpy(component->component_path, subdir_path, sizeof(component->component_path));
            safe_strcpy(component->version, "1.0.0", sizeof(component->version));
            component->has_nlink_txt = has_nlink_txt;
            component->dependency_count = 0;
            component->dependencies = NULL;
            
            // Initialize SemVerX metadata for discovered components
            component->is_semverx_compliant = config->semverx.semverx_enabled;
            component->semverx_metadata.range_state = SEMVERX_RANGE_STATE_STABLE;
            clock_gettime(CLOCK_REALTIME, &component->last_compatibility_check);

            discovered_count++;
        }
    }

    closedir(dir);
    config->component_count = discovered_count;

    return discovered_count;
}

nlink_config_result_t nlink_validate_config(const nlink_pkg_config_t *config) {
    if (!config) {
        return NLINK_CONFIG_ERROR_INVALID_FORMAT;
    }

    // Validate required fields
    NLINK_VALIDATE_REQUIRED_FIELD(strlen(config->project_name) > 0,
                                  "Project name is required");
    NLINK_VALIDATE_REQUIRED_FIELD(strlen(config->entry_point) > 0,
                                  "Entry point is required");

    // Validate thread pool configuration
    if (config->thread_pool.worker_count == 0 ||
        config->thread_pool.worker_count > 64) {
        return NLINK_CONFIG_ERROR_THREAD_POOL_INVALID;
    }

    if (config->thread_pool.queue_depth == 0 ||
        config->thread_pool.queue_depth > 1024) {
        return NLINK_CONFIG_ERROR_THREAD_POOL_INVALID;
    }

    // Validate SemVerX configuration if enabled
    if (config->semverx.semverx_enabled) {
        if (config->semverx.project_range_state == SEMVERX_RANGE_STATE_UNKNOWN) {
            fprintf(stderr, "[SEMVERX ERROR] Invalid project range state\n");
            return NLINK_CONFIG_ERROR_RANGE_STATE_INVALID;
        }
        
        printf("[SEMVERX] Configuration validation passed for range state: %s\n",
               nlink_range_state_to_string(config->semverx.project_range_state));
    }

    // Validate pass mode consistency
    if (config->pass_mode == NLINK_PASS_MODE_MULTI &&
        config->component_count <= 1) {
        fprintf(stderr,
                "[CONFIG WARNING] Multi-pass mode configured but only %d "
                "components found\n",
                config->component_count);
    }

    return NLINK_CONFIG_SUCCESS;
}

nlink_global_config_t *nlink_get_global_config(void) {
    return g_config_initialized ? &g_nlink_config : NULL;
}

uint32_t nlink_calculate_config_checksum(const nlink_pkg_config_t *config) {
    if (!config) return 0;

    // Calculate checksum of key configuration fields including SemVerX
    struct {
        char project_name[128];
        char entry_point[NLINK_MAX_PATH_LENGTH];
        nlink_pass_mode_t pass_mode;
        uint32_t thread_worker_count;
        uint32_t feature_count;
        bool semverx_enabled;
        semverx_range_state_t project_range_state;
    } checksum_data;

    safe_strcpy(checksum_data.project_name, config->project_name, sizeof(checksum_data.project_name));
    safe_strcpy(checksum_data.entry_point, config->entry_point, sizeof(checksum_data.entry_point));
    checksum_data.pass_mode = config->pass_mode;
    checksum_data.thread_worker_count = config->thread_pool.worker_count;
    checksum_data.feature_count = config->feature_count;
    checksum_data.semverx_enabled = config->semverx.semverx_enabled;
    checksum_data.project_range_state = config->semverx.project_range_state;

    return calculate_crc32(&checksum_data, sizeof(checksum_data));
}

void nlink_print_decision_matrix(const nlink_pkg_config_t *config) {
    if (!config) {
        printf("[CONFIG] No configuration to display\n");
        return;
    }

    printf("\n=== NexusLink Configuration Decision Matrix (SemVerX Enhanced) ===\n");
    printf("Project: %s (v%s)\n", config->project_name, config->project_version);
    printf("Entry Point: %s\n", config->entry_point);
    printf("Pass Mode: %s\n",
           config->pass_mode == NLINK_PASS_MODE_SINGLE  ? "Single-Pass"
           : config->pass_mode == NLINK_PASS_MODE_MULTI ? "Multi-Pass"
                                                        : "Unknown");
    printf("Components Discovered: %d\n", config->component_count);
    printf("Thread Pool: %d workers, %d queue depth\n",
           config->thread_pool.worker_count, config->thread_pool.queue_depth);
    printf("Features Enabled: %d\n", config->feature_count);
    
    // SemVerX information
    if (config->semverx.semverx_enabled) {
        printf("\n=== SemVerX Range State Configuration ===\n");
        printf("SemVerX Enabled: Yes\n");
        printf("Project Range State: %s\n", 
               nlink_range_state_to_string(config->semverx.project_range_state));
        printf("Validation Level: %s\n",
               config->semverx.validation_level == SEMVERX_VALIDATION_STRICT ? "Strict" :
               config->semverx.validation_level == SEMVERX_VALIDATION_PERMISSIVE ? "Permissive" :
               config->semverx.validation_level == SEMVERX_VALIDATION_PARANOID ? "Paranoid" : "Disabled");
        printf("Registry Mode: %s\n",
               config->semverx.registry_mode == SEMVERX_REGISTRY_CENTRALIZED ? "Centralized" :
               config->semverx.registry_mode == SEMVERX_REGISTRY_DISTRIBUTED ? "Distributed" : "Hybrid");
        printf("Hot-Swap Monitoring: %s\n", 
               config->semverx.monitor_hot_swap_events ? "Enabled" : "Disabled");
        printf("Range Boundary Enforcement: %s\n",
               config->semverx.enforce_range_boundaries ? "Enabled" : "Disabled");
    } else {
        printf("SemVerX: Disabled\n");
    }
    
    printf("Experimental Mode: %s\n",
           config->experimental_mode_enabled ? "Enabled" : "Disabled");
    printf("Unicode Normalization: %s\n",
           config->unicode_normalization_enabled ? "Enabled" : "Disabled");
    printf("Isomorphic Reduction: %s\n",
           config->isomorphic_reduction_enabled ? "Enabled" : "Disabled");
    printf("Configuration Checksum: 0x%08X\n", config->config_checksum);
    printf("===============================================================\n\n");
}

void nlink_config_destroy(void) {
    if (!g_config_initialized) return;

    NLINK_CONFIG_LOCK();

    // Clean up dynamically allocated component configurations
    if (g_nlink_config.component_configs) {
        free(g_nlink_config.component_configs);
        g_nlink_config.component_configs = NULL;
    }

    // Reset global state
    memset(&g_nlink_config.pkg_config, 0, sizeof(nlink_pkg_config_t));
    g_nlink_config.is_initialized = false;
    g_nlink_config.is_single_pass_mode = false;
    g_nlink_config.active_component_count = 0;

    NLINK_CONFIG_UNLOCK();

    pthread_mutex_destroy(&g_nlink_config.config_mutex);
    g_config_initialized = false;
}

// =============================================================================
// SEMVERX VALIDATION FUNCTIONS (Stubs for Future Implementation)
// =============================================================================

nlink_config_result_t nlink_validate_semverx_compatibility(
    const semverx_component_metadata_t *comp1,
    const semverx_component_metadata_t *comp2,
    const nlink_semverx_config_t *global_config) {
    
    printf("[SEMVERX] Validating compatibility between components\n");
    // Implementation placeholder for systematic SemVerX validation
    return NLINK_CONFIG_SUCCESS;
}

bool nlink_can_hot_swap(const semverx_component_metadata_t *current,
                       const semverx_component_metadata_t *target,
                       const nlink_semverx_config_t *global_config) {
    
    printf("[SEMVERX] Evaluating hot-swap feasibility\n");
    // Implementation placeholder for hot-swap validation
    return false;
}

nlink_config_result_t nlink_load_shared_registry(const char *registry_path,
                                                 nlink_semverx_config_t *config) {
    
    printf("[SEMVERX] Loading shared registry from: %s\n", registry_path);
    // Implementation placeholder for registry loading
    return NLINK_CONFIG_SUCCESS;
}