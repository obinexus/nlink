/**
 * @file config_parser.c
 * @brief Enhanced configuration parser with ETPS integration
 */

#include <core/config/types.h"
#include <core/etps/telemetry.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char section[64];
    char key[64];
    char value[256];
} config_entry_t;

typedef struct {
    config_entry_t* entries;
    size_t count;
    size_t capacity;
    etps_context_t* etps_ctx;
} config_parser_t;

config_parser_t* config_parser_create(const char* name) {
    config_parser_t* parser = calloc(1, sizeof(config_parser_t));
    if (!parser) return NULL;
    
    parser->capacity = 128;
    parser->entries = calloc(parser->capacity, sizeof(config_entry_t));
    parser->etps_ctx = etps_context_create(name);
    
    etps_log_info(parser->etps_ctx, ETPS_COMPONENT_CONFIG,
                  "config_parser_create", "Configuration parser initialized");
    
    return parser;
}

int config_parser_load(config_parser_t* parser, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        etps_log_error(parser->etps_ctx, ETPS_COMPONENT_CONFIG,
                      ETPS_ERROR_FILE_NOT_FOUND, "config_parser_load",
                      "Failed to open config file");
        return -1;
    }
    
    char line[512];
    char current_section[64] = "global";
    
    while (fgets(line, sizeof(line), file)) {
        // Remove comments and whitespace
        char* comment = strchr(line, '#');
        if (comment) *comment = '\0';
        
        // Skip empty lines
        if (strlen(line) == 0 || line[0] == '\n') continue;
        
        // Check for section
        if (line[0] == '[') {
            sscanf(line, "[%63[^]]]", current_section);
            continue;
        }
        
        // Parse key=value
        char key[64], value[256];
        if (sscanf(line, "%63[^=]=%255[^\n]", key, value) == 2) {
            // Add to entries
            if (parser->count < parser->capacity) {
                config_entry_t* entry = &parser->entries[parser->count++];
                strncpy(entry->section, current_section, sizeof(entry->section) - 1);
                strncpy(entry->key, key, sizeof(entry->key) - 1);
                strncpy(entry->value, value, sizeof(entry->value) - 1);
            }
        }
    }
    
    fclose(file);
    
    etps_log_info(parser->etps_ctx, ETPS_COMPONENT_CONFIG,
                  "config_parser_load", "Configuration loaded successfully");
    
    return 0;
}

const char* config_get_value(config_parser_t* parser, 
                            const char* section, 
                            const char* key) {
    for (size_t i = 0; i < parser->count; i++) {
        config_entry_t* entry = &parser->entries[i];
        if (strcmp(entry->section, section) == 0 &&
            strcmp(entry->key, key) == 0) {
            return entry->value;
        }
    }
    return NULL;
}

void config_parser_destroy(config_parser_t* parser) {
    if (!parser) return;
    
    etps_log_info(parser->etps_ctx, ETPS_COMPONENT_CONFIG,
                  "config_parser_destroy", "Destroying configuration parser");
    
    free(parser->entries);
    etps_context_destroy(parser->etps_ctx);
    free(parser);
}
