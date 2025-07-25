/**
 * @file types.h
 * @brief Core type definitions for NexusLink SemVerX
 */

#ifndef NLINK_SEMVERX_CORE_TYPES_H
#define NLINK_SEMVERX_CORE_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdarg.h> // needed for va_list

// Basic configuration constants
#define NLINK_MAX_PATH_LENGTH 512
#define NLINK_MAX_FEATURES 32
#define NLINK_MAX_COMPONENTS 64
#define NLINK_VERSION_STRING_MAX 32

// Pass mode enumeration
typedef enum {
    NLINK_PASS_MODE_UNKNOWN = 0,
    NLINK_PASS_MODE_SINGLE,
    NLINK_PASS_MODE_MULTI
} nlink_pass_mode_t;

// Threading configuration
typedef struct {
    uint32_t worker_count;
    uint32_t queue_depth;
    uint32_t stack_size_kb;
    bool enable_thread_affinity;
    bool enable_work_stealing;
    struct timespec idle_timeout;
} nlink_thread_pool_config_t;

/* FIXING BUILD ERRORS CHECKLIST */

// Define missing types (used in result.h and nexus_core.h)
typedef int NexusResult;
typedef int NexusFlags;
typedef int NexusLogLevel;

// Define callback and symbol registry
typedef void (*NexusLogCallback)(NexusLogLevel level, const char* format, va_list args);

typedef struct NexusSymbolRegistry {
    int placeholder;
} NexusSymbolRegistry;

#endif /* NLINK_SEMVERX_CORE_TYPES_H */
