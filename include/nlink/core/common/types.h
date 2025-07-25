// types.h — Cleaned up to avoid duplicate struct definitions and header inclusion issues

#ifndef NLINK_SEMVERX_CORE_TYPES_H
#define NLINK_SEMVERX_CORE_TYPES_H

#define NLINK_MAX_FEATURES 32
#define NLINK_MAX_COMPONENTS 64
#define NLINK_VERSION_STRING_MAX 32
#define NLINK_MAX_PATH_LENGTH 512

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdarg.h>

// Forward declare structs to prevent redefinition issues
#ifndef NEXUS_CONTEXT_DEFINED
#define NEXUS_CONTEXT_DEFINED

typedef struct NexusContext {
    int placeholder;
} NexusContext;

#endif

#ifndef NEXUS_SYMBOL_REGISTRY_DEFINED
#define NEXUS_SYMBOL_REGISTRY_DEFINED

typedef struct NexusSymbolRegistry {
    int placeholder;
} NexusSymbolRegistry;

#endif

// Basic type definitions
typedef int NexusResult;
typedef int NexusFlags;
typedef int NexusLogLevel;

typedef void (*NexusLogCallback)(NexusLogLevel level, const char* format, va_list args);

// Pass mode enumeration
typedef enum {
    NLINK_PASS_MODE_UNKNOWN = 0,
    NLINK_PASS_MODE_SINGLE,
    NLINK_PASS_MODE_MULTI
} nlink_pass_mode_t;

// Thread pool config
typedef struct {
    uint32_t worker_count;
    uint32_t queue_depth;
    uint32_t stack_size_kb;
    bool enable_thread_affinity;
    bool enable_work_stealing;
    struct timespec idle_timeout;
} nlink_thread_pool_config_t;

#endif /* NLINK_SEMVERX_CORE_TYPES_H */