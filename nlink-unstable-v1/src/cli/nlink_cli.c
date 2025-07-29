// nlink_cli.c - CLI implementation for NexusLink
#include "nlink_cli.h"

// Define option patterns
static NLinkOption options[] = {
    {"output", 'o', true, "Set output file name", {0}, "-o[[:space:]]+([^[:space:]]+)|--output[[:space:]]+([^[:space:]]+)"},
    {"library-path", 'L', true, "Add directory to library search path", {0}, "-L[[:space:]]*([^[:space:]]+)"},
    {"library", 'l', true, "Search for library", {0}, "-l([^[:space:]]+)"},
    {"relocatable", 'r', false, "Generate relocatable output", {0}, "-r|--relocatable"},
    {"gc-sections", '\0', false, "Remove unused sections", {0}, "--gc-sections"},
    {"whole-archive", '\0', false, "Include all objects from archives", {0}, "--whole-archive"},
    {"no-whole-archive", '\0', false, "Turn off --whole-archive", {0}, "--no-whole-archive"},
    {"script", 'T', true, "Read linker script", {0}, "-T[[:space:]]*([^[:space:]]+)|--script[[:space:]]+([^[:space:]]+)"},
    {"entry", 'e', true, "Set entry point", {0}, "-e[[:space:]]*([^[:space:]]+)|--entry[[:space:]]+([^[:space:]]+)"},
    {"start-group", '(', false, "Start a group", {0}, "--start-group|-\\("},
    {"end-group", ')', false, "End a group", {0}, "--end-group|-\\)"},
    {"version-script", '\0', true, "Read version script", {0}, "--version-script[[:space:]]+([^[:space:]]+)"},
    {"help", 'h', false, "Display this help", {0}, "--help|-h"},
    {"version", 'v', false, "Display version information", {0}, "--version|-v"},
    {NULL, '\0', false, NULL, {0}, NULL}
};

// Initialize CLI context
NLinkContext* nlink_init_context(int argc, char** argv) {
    NLinkContext* ctx = (NLinkContext*)malloc(sizeof(NLinkContext));
    if (!ctx) return NULL;
    
    memset(ctx, 0, sizeof(NLinkContext));
    ctx->argc = argc;
    ctx->argv = argv;
    
    // Initialize regex patterns
    for (size_t i = 0; options[i].name != NULL; i++) {
        if (regcomp(&options[i].regex, options[i].pattern, REG_EXTENDED) != 0) {
            fprintf(stderr, "Failed to compile regex pattern for option: %s\n", options[i].name);
            nlink_free_context(ctx);
            return NULL;
        }
    }
    
    // Create Nexus context
    NexusConfig config = {0};
    config.log_level = NEXUS_LOG_INFO;
    ctx->nctx = nexus_create_context(&config);
    
    return ctx;
}

// Parse command line arguments
bool nlink_parse_args(NLinkContext* ctx) {
    if (!ctx) return false;
    
    // Skip program name (argv[0])
    for (int i = 1; i < ctx->argc; i++) {
        const char* arg = ctx->argv[i];
        bool matched = false;
        
        // Try to match each option
        for (size_t j = 0; options[j].name != NULL; j++) {
            regmatch_t matches[2];
            if (regexec(&options[j].regex, arg, 2, matches, 0) == 0) {
                matched = true;
                
                // Extract argument if needed
                char* opt_arg = NULL;
                if (options[j].has_arg) {
                    if (matches[1].rm_so != -1) {
                        // Argument is part of the current arg
                        size_t len = matches[1].rm_eo - matches[1].rm_so;
                        opt_arg = (char*)malloc(len + 1);
                        strncpy(opt_arg, arg + matches[1].rm_so, len);
                        opt_arg[len] = '\0';
                    } else if (i + 1 < ctx->argc) {
                        // Argument is the next arg
                        opt_arg = strdup(ctx->argv[++i]);
                    }
                }
                
                // Process the option
                switch (options[j].shorthand) {
                    case 'o':
                        ctx->output_file = opt_arg;
                        break;
                    case 'L':
                        // Add library path
                        ctx->lib_paths = (char**)realloc(ctx->lib_paths, 
                                                        (ctx->lib_paths_count + 1) * sizeof(char*));
                        ctx->lib_paths[ctx->lib_paths_count++] = opt_arg;
                        break;
                    case 'l':
                        // Add library
                        ctx->libraries = (char**)realloc(ctx->libraries, 
                                                        (ctx->libraries_count + 1) * sizeof(char*));
                        ctx->libraries[ctx->libraries_count++] = opt_arg;
                        break;
                    case 'r':
                        ctx->relocatable = true;
                        break;
                    case 'T':
                        ctx->script_file = opt_arg;
                        break;
                    case 'e':
                        ctx->entry_symbol = opt_arg;
                        break;
                    case '(':
                        ctx->in_group = true;
                        break;
                    case ')':
                        ctx->in_group = false;
                        break;
                    case 'h':
                        nlink_print_help();
                        return false;
                    case 'v':
                        printf("NexusLink (nlink) version 0.1.0\n");
                        printf("Copyright © 2025 OBINexus Computing\n");
                        return false;
                    default:
                        // Handle flag options
                        if (strcmp(options[j].name, "gc-sections") == 0) {
                            ctx->gc_sections = true;
                        } else if (strcmp(options[j].name, "whole-archive") == 0) {
                            ctx->whole_archive = true;
                        } else if (strcmp(options[j].name, "no-whole-archive") == 0) {
                            ctx->whole_archive = false;
                        } else if (strcmp(options[j].name, "version-script") == 0) {
                            ctx->version_script = opt_arg;
                        }
                        break;
                }
                
                break;
            }
        }
        
        if (!matched) {
            // Unrecognized argument, assume it's an input file
            // In a complete implementation, you'd store these for processing
            printf("Input file: %s\n", arg);
        }
    }
    
    return true;
}

// Execute the linker operation
int nlink_execute(NLinkContext* ctx) {
    if (!ctx) return 1;
    
    // Validate required options
    if (!ctx->output_file) {
        fprintf(stderr, "Error: No output file specified (-o)\n");
        return 1;
    }
    
    printf("Linking to output file: %s\n", ctx->output_file);
    
    // Library paths
    for (size_t i = 0; i < ctx->lib_paths_count; i++) {
        printf("Library path: %s\n", ctx->lib_paths[i]);
    }
    
    // Libraries
    for (size_t i = 0; i < ctx->libraries_count; i++) {
        printf("Library: %s\n", ctx->libraries[i]);
    }
    
    // TODO: Implement actual linking process
    // This is where we'd call into the NexusLink core functionality
    
    return 0;
}

// Print help text
void nlink_print_help(void) {
    printf("Usage: nlink [options] file...\n");
    printf("Options:\n");
    
    for (size_t i = 0; options[i].name != NULL; i++) {
        if (options[i].shorthand != '\0') {
            printf("  -%c, --%s%s %s\n", 
                  options[i].shorthand, 
                  options[i].name,
                  options[i].has_arg ? " ARG" : "",
                  options[i].description);
        } else {
            printf("  --%s%s %s\n", 
                  options[i].name,
                  options[i].has_arg ? " ARG" : "",
                  options[i].description);
        }
    }
}

// Free CLI context
void nlink_free_context(NLinkContext* ctx) {
    if (!ctx) return;
    
    // Free regex patterns
    for (size_t i = 0; options[i].name != NULL; i++) {
        regfree(&options[i].regex);
    }
    
    // Free allocated strings (don't free output_file, script_file, etc. if they point to argv)
    // In a real implementation, you'd need to track which strings were dynamically allocated
    
    // Free arrays
    free(ctx->lib_paths);
    free(ctx->libraries);
    
    // Destroy Nexus context
    nexus_destroy_context(ctx->nctx);
    
    free(ctx);
}