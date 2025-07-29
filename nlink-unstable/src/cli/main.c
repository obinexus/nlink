// nlink_main.c - Main entry point for NexusLink CLI
#include "cli/nlink_cli.h"

int main(int argc, char** argv) {
    // Initialize CLI context
    NLinkContext* ctx = nlink_init_context(argc, argv);
    if (!ctx) {
        fprintf(stderr, "Failed to initialize NexusLink\n");
        return 1;
    }
    
    // Parse command line arguments
    if (!nlink_parse_args(ctx)) {
        nlink_free_context(ctx);
        return 0;  // Help or version was displayed
    }
    
    // Execute the linker operation
    int result = nlink_execute(ctx);
    
    // Clean up
    nlink_free_context(ctx);
    
    return result;
}