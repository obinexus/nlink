// nlink_cli.h - Main CLI interface for NexusLink
#ifndef NLINK_CLI_H
#define NLINK_CLI_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>

// Command option structure
typedef struct {
    const char* name;           // Long option name
    char shorthand;             // Short option character
    bool has_arg;               // Whether option takes an argument
    const char* description;    // Description for help text
    regex_t regex;              // Compiled regex for pattern matching
    const char* pattern;        // Regex pattern string
} NLinkOption;

// CLI context
typedef struct {
    int argc;                   // Original argc
    char** argv;                // Original argv
    char* output_file;          // -o output file
    char** lib_paths;           // -L library paths
    size_t lib_paths_count;
    char** libraries;           // -l libraries
    size_t libraries_count;
    bool relocatable;           // -r flag
    bool gc_sections;           // --gc-sections
    bool whole_archive;         // --whole-archive state
    char* script_file;          // -T/--script
    char* entry_symbol;         // -e/--entry
    bool in_group;              // --start-group/--end-group state
    char* version_script;       // --version-script
    NexusContext* nctx;         // Nexus context
} NLinkContext;

// Initialize CLI context
NLinkContext* nlink_init_context(int argc, char** argv);

// Parse command line arguments
bool nlink_parse_args(NLinkContext* ctx);

// Execute the linker operation
int nlink_execute(NLinkContext* ctx);

// Print help text
void nlink_print_help(void);

// Free CLI context
void nlink_free_context(NLinkContext* ctx);

#endif // NLINK_CLI_H