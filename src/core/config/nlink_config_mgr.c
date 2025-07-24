/**
 * @file config_manager.c
 * @brief NexusLink Configuration Management System
 * @methodology Waterfall - Phase 1 Implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <nlink/core/config/types.h"
#include <nlink/core/etps/telemetry.h"

// Configuration node structure
typedef struct config_node {
    char* key;
    char* value;
    struct config_node* next;
} config_node_t;

// Configuration section structure
typedef struct config_section {
    char* name;
    config_node_t* nodes;
    struct config_section* next;
} config_section_t;

// Main configuration manager
typedef struct {
    config_section_t* sections;
    etps_context_t* etps_ctx;
    char config_path[256];
    bool is_loaded;
} config_manager_t;

// Global instance
static config_manager_t* g_config_manager = NULL;

// Utility: Trim whitespace
static char* trim_whitespace(char* str) {
    char* end;
    
    // Trim leading space
    while(isspace((unsigned char)*str)) str++;
    
    if(*str == 0) return str;
    
    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    
    end[1] = '\0';
    return str;
}

// Create configuration manager
config_manager_t* config_manager_create(const char* name) {
    config_manager_t* mgr = calloc(1, sizeof(config_manager_t));
    if (!mgr) return NULL;
    
    mgr->etps_ctx = etps_context_create(name ? name : "config_manager");
    mgr->is_loaded = false;
    
    etps_log_info(mgr->etps_ctx, ETPS_COMPONENT_CONFIG,
                  "config_manager_create", "Configuration manager initialized");
    
    return mgr;
}

// Find or create section
static config_section_t* find_or_create_section(config_manager_t* mgr, const char* name) {
    config_section_t* section = mgr->sections;
    
    // Find existing
    while (section) {
        if (strcmp(section->name, name) == 0) {
            return section;
        }
        section = section->next;
    }
    
    // Create new
    section = calloc(1, sizeof(config_section_t));
    section->name = strdup(name);
    section->next = mgr->sections;
    mgr->sections = section;
    
    return section;
}

// Add key-value to section
static void add_config_value(config_section_t* section, const char* key, const char* value) {
    config_node_t* node = calloc(1, sizeof(config_node_t));
    node->key = strdup(key);
    node->value = strdup(value);
    node->next = section->nodes;
    section->nodes = node;
}

// Load configuration from file
int config_manager_load(config_manager_t* mgr, const char* filepath) {
    if (!mgr || !filepath) return -1;
    
    FILE* file = fopen(filepath, "r");
    if (!file) {
        etps_log_error(mgr->etps_ctx, ETPS_COMPONENT_CONFIG,
                      ETPS_ERROR_FILE_NOT_FOUND, "config_manager_load",
                      "Configuration file not found");
        return -1;
    }
    
    char line[512];
    config_section_t* current_section = find_or_create_section(mgr, "global");
    
    while (fgets(line, sizeof(line), file)) {
        char* trimmed = trim_whitespace(line);
        
        // Skip comments and empty lines
        if (trimmed[0] == '#' || trimmed[0] == '\0') continue;
        
        // Section header
        if (trimmed[0] == '[') {
            char* end = strchr(trimmed, ']');
            if (end) {
                *end = '\0';
                current_section = find_or_create_section(mgr, trimmed + 1);
            }
            continue;
        }
        
        // Key-value pair
        char* equals = strchr(trimmed, '=');
        if (equals) {
            *equals = '\0';
            char* key = trim_whitespace(trimmed);
            char* value = trim_whitespace(equals + 1);
            
            if (strlen(key) > 0 && strlen(value) > 0) {
                add_config_value(current_section, key, value);
            }
        }
    }
    
    fclose(file);
    
    strncpy(mgr->config_path, filepath, sizeof(mgr->config_path) - 1);
    mgr->is_loaded = true;
    
    etps_log_info(mgr->etps_ctx, ETPS_COMPONENT_CONFIG,
                  "config_manager_load", "Configuration loaded successfully");
    
    return 0;
}

// Get configuration value
const char* config_manager_get(config_manager_t* mgr, const char* section, const char* key) {
    if (!mgr || !section || !key) return NULL;
    
    config_section_t* sec = mgr->sections;
    while (sec) {
        if (strcmp(sec->name, section) == 0) {
            config_node_t* node = sec->nodes;
            while (node) {
                if (strcmp(node->key, key) == 0) {
                    return node->value;
                }
                node = node->next;
            }
        }
        sec = sec->next;
    }
    
    return NULL;
}

// Get integer value
int config_manager_get_int(config_manager_t* mgr, const char* section, 
                          const char* key, int default_value) {
    const char* value = config_manager_get(mgr, section, key);
    if (!value) return default_value;
    
    char* endptr;
    long result = strtol(value, &endptr, 10);
    
    if (*endptr != '\0') return default_value;
    return (int)result;
}

// Get boolean value
bool config_manager_get_bool(config_manager_t* mgr, const char* section,
                            const char* key, bool default_value) {
    const char* value = config_manager_get(mgr, section, key);
    if (!value) return default_value;
    
    return (strcasecmp(value, "true") == 0 || 
            strcasecmp(value, "yes") == 0 ||
            strcasecmp(value, "1") == 0);
}

// Destroy configuration manager
void config_manager_destroy(config_manager_t* mgr) {
    if (!mgr) return;
    
    // Free all sections
    config_section_t* section = mgr->sections;
    while (section) {
        config_section_t* next_section = section->next;
        
        // Free all nodes in section
        config_node_t* node = section->nodes;
        while (node) {
            config_node_t* next_node = node->next;
            free(node->key);
            free(node->value);
            free(node);
            node = next_node;
        }
        
        free(section->name);
        free(section);
        section = next_section;
    }
    
    etps_context_destroy(mgr->etps_ctx);
    free(mgr);
}

// Global initialization
int nlink_config_init(void) {
    if (g_config_manager) return 0;
    
    g_config_manager = config_manager_create("global_config");
    if (!g_config_manager) return -1;
    
    // Try to load default config
    if (config_manager_load(g_config_manager, "config/nlink.conf") != 0) {
        // Try alternate locations
        config_manager_load(g_config_manager, "/etc/nlink/nlink.conf");
    }
    
    return 0;
}

// Global cleanup
void nlink_config_cleanup(void) {
    if (g_config_manager) {
        config_manager_destroy(g_config_manager);
        g_config_manager = NULL;
    }
}
