/**
 * NexusLink QA POC - Main Entry Point
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "nlink_qa_poc/core/config.h"
#include "nlink_qa_poc/etps/telemetry.h"

int main(int argc, char* argv[]) {
    printf("NexusLink QA POC v1.0.0\n");
    printf("OBINexus Aegis Engineering\n");
    
    // Initialize ETPS
    etps_context_t* ctx = etps_context_create("main");
    if (!ctx) {
        fprintf(stderr, "Failed to initialize ETPS context\n");
        return 1;
    }
    
    printf("ETPS initialized successfully\n");
    
    // Cleanup
    etps_context_destroy(ctx);
    
    return 0;
}
