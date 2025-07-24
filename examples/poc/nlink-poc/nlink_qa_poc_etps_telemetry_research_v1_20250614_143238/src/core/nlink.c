/**
 * NexusLink Core Library Implementation - Enhanced with SemVerX ETPS
 * WARNING-FREE VERSION - Production Security Standards
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nlink_qa_poc/core/config.h"
#include "nlink_qa_poc/etps/telemetry.h"

static bool g_nlink_initialized = false;
static etps_context_t* g_global_etps_context = NULL;

const char* nlink_get_version(void) {
    return "1.0.0";
}

const char* nlink_get_build_info(void) {
    return "OBINexus NexusLink QA POC - SemVerX ETPS Integration";
}

int nlink_init(void) {
    if (g_nlink_initialized) return 0;
    
    if (etps_init() != 0) {
        fprintf(stderr, "[NLINK_ERROR] Failed to initialize ETPS\n");
        return -1;
    }
    
    g_global_etps_context = etps_context_create("nlink_global");
    if (!g_global_etps_context) {
        fprintf(stderr, "[NLINK_ERROR] Failed to create ETPS context\n");
        etps_shutdown();
        return -1;
    }
    
    g_nlink_initialized = true;
    printf("[NLINK_INFO] NexusLink initialized with SemVerX ETPS\n");
    return 0;
}

void nlink_shutdown(void) {
    if (!g_nlink_initialized) return;
    
    if (g_global_etps_context) {
        etps_context_destroy(g_global_etps_context);
        g_global_etps_context = NULL;
    }
    
    etps_shutdown();
    g_nlink_initialized = false;
    printf("[NLINK_INFO] NexusLink shutdown complete\n");
}

bool nlink_is_initialized(void) {
    return g_nlink_initialized;
}

int nlink_cli_execute(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: nlink <command> [options]\n");
        return 0;
    }
    
    const char* command = argv[1];
    
    if (strcmp(command, "--version") == 0) {
        printf("NexusLink %s\n", nlink_get_version());
        printf("%s\n", nlink_get_build_info());
        return 0;
    }
    
    if (strcmp(command, "--validate-compatibility") == 0) {
        return nlink_cli_validate_compatibility(argc, argv);
    }
    
    if (strcmp(command, "--semverx-status") == 0) {
        return nlink_cli_semverx_status(argc, argv);
    }
    
    if (strcmp(command, "--migration-plan") == 0) {
        return nlink_cli_migration_plan(argc, argv);
    }
    
    if (strcmp(command, "--etps-test") == 0) {
        printf("🧪 ETPS System Test\n");
        
        if (!g_nlink_initialized) {
            if (nlink_init() != 0) {
                printf("❌ Failed to initialize\n");
                return -1;
            }
        }
        
        // Register test components
        semverx_component_t calc = {0};
        strcpy(calc.name, "calculator");
        strcpy(calc.version, "1.2.0");
        calc.range_state = SEMVERX_RANGE_STABLE;
        etps_register_component(g_global_etps_context, &calc);
        
        semverx_component_t sci = {0};
        strcpy(sci.name, "scientific");
        strcpy(sci.version, "0.3.0");
        sci.range_state = SEMVERX_RANGE_EXPERIMENTAL;
        etps_register_component(g_global_etps_context, &sci);
        
        // Test validation - FIX: Actually use the result variable
        etps_semverx_event_t event;
        compatibility_result_t result = etps_validate_component_compatibility(
            g_global_etps_context, &calc, &sci, &event);
        etps_emit_semverx_event(g_global_etps_context, &event);
        
        // Use the result to avoid unused variable warning
        const char* result_str = etps_compatibility_result_to_string(result);
        printf("Validation result: %s\n", result_str);
        printf("✅ ETPS test completed\n");
        return 0;
    }
    
    printf("Unknown command: %s\n", command);
    return -1;
}
