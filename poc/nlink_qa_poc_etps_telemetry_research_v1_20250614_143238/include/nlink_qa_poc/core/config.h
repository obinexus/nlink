/**
 * @file config.h
 * @brief NexusLink QA POC Core Configuration Header
 * @author Nnamdi Michael Okpala & Aegis Development Team
 * @version 1.5.1
 */

#ifndef NLINK_QA_POC_CORE_CONFIG_H
#define NLINK_QA_POC_CORE_CONFIG_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Configuration structure
typedef struct {
    char project_name[128];
    char version[32];
    char entry_point[256];
    bool strict_mode;
    bool experimental_mode;
    bool debug_symbols;
    bool ast_optimization;
    bool quality_assurance;
} nlink_config_t;

// Function declarations
int nlink_parse_project_name(const char* input, char* output, size_t max_len);
int nlink_parse_version(const char* input, char* output, size_t max_len);
int nlink_load_config(const char* filename, nlink_config_t* config);
int nlink_validate_config(nlink_config_t* config);
void nlink_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif // NLINK_QA_POC_CORE_CONFIG_H
