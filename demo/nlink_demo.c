/**
 * @file nlink_demo.c
 * @brief NexusLink Demo Application
 * @methodology Waterfall - Phase 3 Demonstration
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "nlink/core/etps/telemetry.h"
#include "nlink/core/config/config_manager.c"
#include "nlink/core/marshal/marshal.c"

// Demo component information
typedef struct {
    char name[64];
    char version[32];
    semverx_range_state_t state;
    bool hot_swap_enabled;
} demo_component_t;

// Demo application context
typedef struct {
    etps_context_t* etps_ctx;
    config_manager_t* config;
    demo_component_t components[10];
    size_t component_count;
} demo_app_t;

// Initialize demo application
demo_app_t* demo_app_create(void) {
    demo_app_t* app = calloc(1, sizeof(demo_app_t));
    if (!app) return NULL;
    
    // Initialize ETPS
    if (!etps_is_initialized()) {
        etps_init();
    }
    
    app->etps_ctx = etps_context_create("nlink_demo");
    app->config = config_manager_create("demo_config");
    
    // Load configuration
    if (config_manager_load(app->config, "config/nlink.conf") != 0) {
        printf("[DEMO] Warning: Could not load config file, using defaults\n");
    }
    
    return app;
}

// Register demo components
void demo_register_components(demo_app_t* app) {
    printf("\n=== Registering Demo Components ===\n");
    
    // Component 1: Core Calculator
    demo_component_t calc = {
        .name = "calculator",
        .version = "1.2.0",
        .state = SEMVERX_RANGE_STABLE,
        .hot_swap_enabled = true
    };
    app->components[app->component_count++] = calc;
    
    // Component 2: Scientific Extension
    demo_component_t sci = {
        .name = "scientific",
        .version = "0.3.0",
        .state = SEMVERX_RANGE_EXPERIMENTAL,
        .hot_swap_enabled = false
    };
    app->components[app->component_count++] = sci;
    
    // Component 3: Legacy Parser
    demo_component_t parser = {
        .name = "parser",
        .version = "0.9.5",
        .state = SEMVERX_RANGE_LEGACY,
        .hot_swap_enabled = true
    };
    app->components[app->component_count++] = parser;
    
    // Register with ETPS
    for (size_t i = 0; i < app->component_count; i++) {
        semverx_component_t comp = {0};
        strncpy(comp.name, app->components[i].name, sizeof(comp.name) - 1);
        strncpy(comp.version, app->components[i].version, sizeof(comp.version) - 1);
        comp.range_state = app->components[i].state;
        comp.hot_swap_enabled = app->components[i].hot_swap_enabled;
        comp.component_id = etps_get_current_timestamp();
        
        etps_register_component(app->etps_ctx, &comp);
        
        printf("✓ Registered: %s v%s (%s)\n", 
               comp.name, comp.version, 
               etps_range_state_to_string(comp.range_state));
    }
}

// Demo: Configuration usage
void demo_configuration(demo_app_t* app) {
    printf("\n=== Configuration Demo ===\n");
    
    // Read various configuration values
    const char* project_name = config_manager_get(app->config, "global", "project_name");
    const char* version = config_manager_get(app->config, "global", "version");
    bool etps_enabled = config_manager_get_bool(app->config, "etps", "enabled", false);
    int buffer_size = config_manager_get_int(app->config, "etps", "buffer_size", 4096);
    
    printf("Project: %s\n", project_name ? project_name : "Unknown");
    printf("Version: %s\n", version ? version : "Unknown");
    printf("ETPS Enabled: %s\n", etps_enabled ? "Yes" : "No");
    printf("Buffer Size: %d bytes\n", buffer_size);
}

// Demo: Compatibility validation
void demo_compatibility_check(demo_app_t* app) {
    printf("\n=== Compatibility Validation Demo ===\n");
    
    // Convert demo components to semverx components
    semverx_component_t components[10];
    for (size_t i = 0; i < app->component_count; i++) {
        strncpy(components[i].name, app->components[i].name, sizeof(components[i].name) - 1);
        strncpy(components[i].version, app->components[i].version, sizeof(components[i].version) - 1);
        components[i].range_state = app->components[i].state;
        components[i].hot_swap_enabled = app->components[i].hot_swap_enabled;
        components[i].component_id = etps_get_current_timestamp();
    }
    
    // Check all compatibility pairs
    for (size_t i = 0; i < app->component_count; i++) {
        for (size_t j = 0; j < app->component_count; j++) {
            if (i == j) continue;
            
            etps_semverx_event_t event;
            compatibility_result_t result = etps_validate_component_compatibility(
                app->etps_ctx, &components[i], &components[j], &event
            );
            
            printf("\n%s → %s: %s\n", 
                   components[i].name, 
                   components[j].name,
                   etps_compatibility_result_to_string(result));
            
            if (result != COMPAT_ALLOWED) {
                printf("  ⚠ %s\n", event.migration_recommendation);
            }
        }
    }
}

// Demo: Data marshaling
void demo_marshaling(demo_app_t* app) {
    printf("\n=== Data Marshaling Demo ===\n");
    
    // Create marshal contexts for different formats
    marshal_context_t* json_ctx = marshal_create(MARSHAL_FORMAT_JSON);
    marshal_context_t* binary_ctx = marshal_create(MARSHAL_FORMAT_BINARY);
    
    // Marshal component data
    for (size_t i = 0; i < app->component_count; i++) {
        marshal_string(json_ctx, "name", app->components[i].name);
        marshal_string(json_ctx, "version", app->components[i].version);
        marshal_int32(json_ctx, "state", app->components[i].state);
        
        marshal_string(binary_ctx, "name", app->components[i].name);
        marshal_string(binary_ctx, "version", app->components[i].version);
        marshal_int32(binary_ctx, "state", app->components[i].state);
    }
    
    // Finalize and display JSON
    uint8_t* json_output;
    size_t json_size;
    if (marshal_finalize(json_ctx, &json_output, &json_size) == 0) {
        printf("\nJSON Output:\n%.*s\n", (int)json_size, json_output);
        free(json_output);
    }
    
    // Finalize binary
    uint8_t* binary_output;
    size_t binary_size;
    if (marshal_finalize(binary_ctx, &binary_output, &binary_size) == 0) {
        printf("\nBinary Output Size: %zu bytes\n", binary_size);
        
        // Display hex dump (first 64 bytes)
        printf("Hex Dump:\n");
        for (size_t i = 0; i < binary_size && i < 64; i++) {
            printf("%02X ", binary_output[i]);
            if ((i + 1) % 16 == 0) printf("\n");
        }
        printf("\n");
        
        free(binary_output);
    }
    
    marshal_destroy(json_ctx);
    marshal_destroy(binary_ctx);
}

// Demo: ETPS telemetry
void demo_telemetry(demo_app_t* app) {
    printf("\n=== ETPS Telemetry Demo ===\n");
    
    // Generate various telemetry events
    etps_log_info(app->etps_ctx, ETPS_COMPONENT_CORE, 
                  "demo_telemetry", "Starting telemetry demonstration");
    
    // Simulate some operations with timing
    for (int i = 0; i < 3; i++) {
        uint64_t start = etps_get_current_timestamp();
        
        // Simulate work
        struct timespec ts = {0, 100000000}; // 100ms
        nanosleep(&ts, NULL);
        
        uint64_t end = etps_get_current_timestamp();
        uint64_t duration = end - start;
        
        char message[256];
        snprintf(message, sizeof(message), 
                 "Operation %d completed in %llu ns", i + 1, 
                 (unsigned long long)duration);
        
        etps_log_info(app->etps_ctx, ETPS_COMPONENT_CORE,
                      "demo_operation", message);
    }
    
    // Generate GUID
    char guid_str[ETPS_GUID_STRING_LENGTH];
    etps_generate_guid_string(guid_str);
    printf("Generated GUID: %s\n", guid_str);
    
    // Generate timestamp
    char timestamp[32];
    etps_generate_iso8601_timestamp(timestamp, sizeof(timestamp));
    printf("Current Timestamp: %s\n", timestamp);
}

// Clean up demo application
void demo_app_destroy(demo_app_t* app) {
    if (!app) return;
    
    config_manager_destroy(app->config);
    etps_context_destroy(app->etps_ctx);
    free(app);
}

// Main demo entry point
int main(int argc, char* argv[]) {
    printf("===========================================\n");
    printf("    NexusLink Demo Application v1.0.0\n");
    printf("    OBINexus Computing - Aegis Project\n");
    printf("===========================================\n");
    
    // Create demo application
    demo_app_t* app = demo_app_create();
    if (!app) {
        fprintf(stderr, "Failed to create demo application\n");
        return 1;
    }
    
    // Run demonstrations
    demo_register_components(app);
    demo_configuration(app);
    demo_compatibility_check(app);
    demo_marshaling(app);
    demo_telemetry(app);
    
    // Export events if requested
    if (argc > 1 && strcmp(argv[1], "--export") == 0) {
        const char* output_file = argc > 2 ? argv[2] : "demo_events.json";
        printf("\n=== Exporting Events ===\n");
        
        if (etps_export_events_json(app->etps_ctx, output_file) == 0) {
            printf("✓ Events exported to: %s\n", output_file);
        } else {
            printf("✗ Failed to export events\n");
        }
    }
    
    // Cleanup
    demo_app_destroy(app);
    etps_shutdown();
    
    printf("\n=== Demo Complete ===\n");
    return 0;
}
