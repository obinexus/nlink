# NexusLink Integration Guide: Linking .c and .h Files

## Overview

This document provides a comprehensive guide for integrating C source and header files into the NexusLink project. Based on the architecture designed by Nnamdi Michael Okpala, NexusLink employs a modular component-based system with specific file organization patterns, include path conventions, and build system integration approaches.

## Directory Structure

NexusLink follows a hierarchical directory structure that separates public interfaces from implementation details:

```
nexuslink/
├── include/
│   └── nexuslink/
│       ├── core/
│       │   ├── automaton/        # Automaton interfaces
│       │   ├── common/           # Common utility interfaces
│       │   ├── minimizer/        # Minimizer interfaces
│       │   ├── symbols/          # Symbol registry interfaces
│       │   └── versioning/       # Version handling interfaces
│       ├── cli/                  # CLI interfaces
│       └── nexuslink.h           # Main public header
├── src/
│   ├── core/
│   │   ├── automaton/            # Automaton implementations
│   │   ├── common/               # Common utility implementations
│   │   ├── minimizer/            # Minimizer implementations
│   │   ├── symbols/              # Symbol registry implementations
│   │   └── versioning/           # Version handling implementations
│   ├── cli/
│   │   └── commands/             # CLI command implementations
│   └── nexuslink.c               # Main library implementation
├── include/
│   └── internal/                 # Internal headers not exposed publicly
└── build/                        # Build artifacts
```

## Include Path Conventions

NexusLink enforces strict include path conventions:

### Public API Headers

Public headers should be included using the full path from the include directory:

```c
// Core component includes
#include "nexuslink/core/common/result.h"
#include "nexuslink/core/symbols/nexus_symbols.h"
#include "nexuslink/core/versioning/semver.h"
#include "nexuslink/core/minimizer/nexus_minimizer.h"
#include "nexuslink/core/automaton/automaton.h"

// CLI includes
#include "nexuslink/cli/command.h"

// Main include
#include "nexuslink/nexuslink.h"
```

### Internal Headers

Internal headers should be included with the internal prefix:

```c
#include "internal/core/symbols/symbol_internal.h"
#include "internal/cli/command_registry.h"
```

### Avoid Relative Paths

Avoid using relative paths like:

```c
// DO NOT USE
#include "../common/types.h"
#include "./header.h"
```

## Header Declarations

### Header Guards

All header files must use header guards following this naming convention:

```c
#ifndef NEXUSLINK_CORE_COMPONENT_FILENAME_H
#define NEXUSLINK_CORE_COMPONENT_FILENAME_H

// Header content

#endif /* NEXUSLINK_CORE_COMPONENT_FILENAME_H */
```

### External C Declarations

For C++ compatibility, always wrap declarations in an `extern "C"` block:

```c
#ifdef __cplusplus
extern "C" {
#endif

// Function declarations

#ifdef __cplusplus
}
#endif
```

## Implementation Example: Integrating a New Component

Let's demonstrate how to integrate a new component called "metrics" into NexusLink.

### 1. Create the Header File (include/nexuslink/core/metrics/nexus_metrics.h)

```c
/**
 * @file nexus_metrics.h
 * @brief Interface for NexusLink performance metrics subsystem
 * 
 * Copyright © 2025 OBINexus Computing
 */

#ifndef NEXUSLINK_CORE_METRICS_NEXUS_METRICS_H
#define NEXUSLINK_CORE_METRICS_NEXUS_METRICS_H

#include "nexuslink/core/common/types.h"
#include "nexuslink/core/common/result.h"
#include "nexuslink/core/common/nexus_core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Metrics collection configuration
 */
typedef struct NexusMetricsConfig {
    bool enable_timers;      /**< Enable performance timers */
    bool enable_counters;    /**< Enable performance counters */
    bool enable_logging;     /**< Enable metrics logging */
    const char* output_path; /**< Path for metrics output files */
} NexusMetricsConfig;

/**
 * @brief Initialize the metrics subsystem
 * 
 * @param ctx The NexusLink context
 * @param config Configuration for metrics collection
 * @return NexusResult result code
 */
NexusResult nexus_metrics_initialize(NexusContext* ctx, const NexusMetricsConfig* config);

/**
 * @brief Start a performance timer
 * 
 * @param ctx The NexusLink context
 * @param timer_name Name of the timer
 * @return NexusResult result code
 */
NexusResult nexus_metrics_start_timer(NexusContext* ctx, const char* timer_name);

/**
 * @brief Stop a performance timer
 * 
 * @param ctx The NexusLink context
 * @param timer_name Name of the timer
 * @return NexusResult result code
 */
NexusResult nexus_metrics_stop_timer(NexusContext* ctx, const char* timer_name);

/**
 * @brief Clean up the metrics subsystem
 * 
 * @param ctx The NexusLink context
 */
void nexus_metrics_cleanup(NexusContext* ctx);

#ifdef __cplusplus
}
#endif

#endif /* NEXUSLINK_CORE_METRICS_NEXUS_METRICS_H */
```

### 2. Create the Implementation File (src/core/metrics/nexus_metrics.c)

```c
/**
 * @file nexus_metrics.c
 * @brief Implementation of NexusLink performance metrics subsystem
 * 
 * Copyright © 2025 OBINexus Computing
 */

#include "nexuslink/core/metrics/nexus_metrics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Internal metrics context structure
typedef struct MetricsContext {
    NexusMetricsConfig config;
    struct {
        char* name;
        clock_t start_time;
        bool running;
    } *timers;
    size_t timer_count;
} MetricsContext;

// Initialize the metrics subsystem
NexusResult nexus_metrics_initialize(NexusContext* ctx, const NexusMetricsConfig* config) {
    if (!ctx) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    nexus_log(ctx, NEXUS_LOG_INFO, "Initializing metrics subsystem");
    
    // Allocate metrics context
    MetricsContext* metrics_ctx = (MetricsContext*)malloc(sizeof(MetricsContext));
    if (!metrics_ctx) {
        return NEXUS_OUT_OF_MEMORY;
    }
    
    // Initialize metrics context
    memset(metrics_ctx, 0, sizeof(MetricsContext));
    
    // Store configuration
    if (config) {
        metrics_ctx->config = *config;
        
        // Make a copy of the output path if provided
        if (config->output_path) {
            metrics_ctx->config.output_path = strdup(config->output_path);
            if (!metrics_ctx->config.output_path) {
                free(metrics_ctx);
                return NEXUS_OUT_OF_MEMORY;
            }
        }
    } else {
        // Default configuration
        metrics_ctx->config.enable_timers = true;
        metrics_ctx->config.enable_counters = true;
        metrics_ctx->config.enable_logging = false;
        metrics_ctx->config.output_path = NULL;
    }
    
    // Store metrics context in NexusLink context
    // This would use a proper context storage mechanism in the real implementation
    ctx->user_data = metrics_ctx;
    
    return NEXUS_SUCCESS;
}

// Start a performance timer
NexusResult nexus_metrics_start_timer(NexusContext* ctx, const char* timer_name) {
    if (!ctx || !timer_name) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    // Get metrics context
    MetricsContext* metrics_ctx = (MetricsContext*)ctx->user_data;
    if (!metrics_ctx) {
        return NEXUS_ERROR_NOT_INITIALIZED;
    }
    
    // Check if timers are enabled
    if (!metrics_ctx->config.enable_timers) {
        return NEXUS_SUCCESS;  // Silently ignore if timers are disabled
    }
    
    // Check if timer already exists
    for (size_t i = 0; i < metrics_ctx->timer_count; i++) {
        if (strcmp(metrics_ctx->timers[i].name, timer_name) == 0) {
            if (metrics_ctx->timers[i].running) {
                return NEXUS_ERROR_ALREADY_EXISTS;  // Timer already running
            }
            
            // Start the timer
            metrics_ctx->timers[i].start_time = clock();
            metrics_ctx->timers[i].running = true;
            return NEXUS_SUCCESS;
        }
    }
    
    // Create a new timer
    metrics_ctx->timers = (typeof(metrics_ctx->timers))realloc(
        metrics_ctx->timers, 
        (metrics_ctx->timer_count + 1) * sizeof(*metrics_ctx->timers)
    );
    
    if (!metrics_ctx->timers) {
        return NEXUS_OUT_OF_MEMORY;
    }
    
    // Initialize the new timer
    metrics_ctx->timers[metrics_ctx->timer_count].name = strdup(timer_name);
    if (!metrics_ctx->timers[metrics_ctx->timer_count].name) {
        return NEXUS_OUT_OF_MEMORY;
    }
    
    metrics_ctx->timers[metrics_ctx->timer_count].start_time = clock();
    metrics_ctx->timers[metrics_ctx->timer_count].running = true;
    metrics_ctx->timer_count++;
    
    return NEXUS_SUCCESS;
}

// Stop a performance timer
NexusResult nexus_metrics_stop_timer(NexusContext* ctx, const char* timer_name) {
    if (!ctx || !timer_name) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    // Get metrics context
    MetricsContext* metrics_ctx = (MetricsContext*)ctx->user_data;
    if (!metrics_ctx) {
        return NEXUS_ERROR_NOT_INITIALIZED;
    }
    
    // Check if timers are enabled
    if (!metrics_ctx->config.enable_timers) {
        return NEXUS_SUCCESS;  // Silently ignore if timers are disabled
    }
    
    // Find the timer
    for (size_t i = 0; i < metrics_ctx->timer_count; i++) {
        if (strcmp(metrics_ctx->timers[i].name, timer_name) == 0) {
            if (!metrics_ctx->timers[i].running) {
                return NEXUS_ERROR_INVALID_STATE;  // Timer not running
            }
            
            // Stop the timer
            clock_t end_time = clock();
            double elapsed_ms = 1000.0 * (end_time - metrics_ctx->timers[i].start_time) / CLOCKS_PER_SEC;
            metrics_ctx->timers[i].running = false;
            
            // Log the result if logging is enabled
            if (metrics_ctx->config.enable_logging) {
                nexus_log(ctx, NEXUS_LOG_INFO, "Timer '%s': %.2f ms", timer_name, elapsed_ms);
            }
            
            return NEXUS_SUCCESS;
        }
    }
    
    return NEXUS_ERROR_NOT_FOUND;  // Timer not found
}

// Clean up the metrics subsystem
void nexus_metrics_cleanup(NexusContext* ctx) {
    if (!ctx) {
        return;
    }
    
    // Get metrics context
    MetricsContext* metrics_ctx = (MetricsContext*)ctx->user_data;
    if (!metrics_ctx) {
        return;
    }
    
    nexus_log(ctx, NEXUS_LOG_INFO, "Cleaning up metrics subsystem");
    
    // Free timers
    for (size_t i = 0; i < metrics_ctx->timer_count; i++) {
        free(metrics_ctx->timers[i].name);
    }
    free(metrics_ctx->timers);
    
    // Free output path
    if (metrics_ctx->config.output_path) {
        free((void*)metrics_ctx->config.output_path);
    }
    
    // Free metrics context
    free(metrics_ctx);
    ctx->user_data = NULL;
}
```

### 3. Update the Main Header (include/nexuslink/nexuslink.h)

```c
/**
 * @file nexuslink.h
 * @brief Main header for the NexusLink library
 * 
 * Copyright © 2025 OBINexus Computing
 */

#ifndef NEXUSLINK_H
#define NEXUSLINK_H

#ifdef __cplusplus
extern "C" {
#endif

// Core components
#include "nexuslink/core/common/types.h"
#include "nexuslink/core/common/result.h"
#include "nexuslink/core/common/nexus_core.h"
#include "nexuslink/core/symbols/nexus_symbols.h"
#include "nexuslink/core/versioning/nexus_version.h"
#include "nexuslink/core/minimizer/nexus_minimizer.h"
#include "nexuslink/core/automaton/automaton.h"
#include "nexuslink/core/metrics/nexus_metrics.h"  // Add the new component

// CLI components
#include "nexuslink/cli/command.h"

// Library version information
extern const char* NEXUSLINK_VERSION;
extern const char* NEXUSLINK_BUILD_DATE;
extern const char* NEXUSLINK_COPYRIGHT;

/**
 * @brief Initialize the NexusLink library
 * 
 * @param config Configuration for the library (can be NULL for defaults)
 * @return NexusResult result code
 */
NexusResult nexuslink_initialize(const NexusConfig* config);

/**
 * @brief Clean up the NexusLink library
 */
void nexuslink_cleanup(void);

/**
 * @brief Get the global NexusLink context
 * 
 * @return NexusContext* The global context, or NULL if not initialized
 */
NexusContext* nexuslink_get_context(void);

/**
 * @brief Get the library version
 * 
 * @return const char* Version string
 */
const char* nexuslink_get_version(void);

/**
 * @brief Get the build date
 * 
 * @return const char* Build date string
 */
const char* nexuslink_get_build_date(void);

/**
 * @brief Get the copyright information
 * 
 * @return const char* Copyright string
 */
const char* nexuslink_get_copyright(void);

#ifdef __cplusplus
}
#endif

#endif /* NEXUSLINK_H */
```

### 4. Update the Main Implementation (src/nexuslink.c)

Add the new metrics component to the initialization and cleanup:

```c
#include "nexuslink/nexuslink.h"
#include "nexuslink/core/metrics/nexus_metrics.h"  // Include the metrics header

// In nexuslink_initialize()
result = nexus_metrics_initialize(g_global_context, NULL);
if (result != NEXUS_SUCCESS) {
    // Clean up previous components
    nexus_minimizer_cleanup(g_global_context);
    nexus_version_cleanup(g_global_context);
    nexus_symbol_registry_cleanup(g_global_context);
    nexus_destroy_context(g_global_context);
    g_global_context = NULL;
    return result;
}

// In nexuslink_cleanup()
nexus_metrics_cleanup(g_global_context);
```

### 5. Update the Build System

#### For Makefile:

```makefile
# Metrics component
METRICS_DIR = $(CORE_DIR)/metrics
METRICS_SRCS = $(wildcard $(METRICS_DIR)/*.c)
METRICS_OBJS = $(patsubst $(METRICS_DIR)/%.c,$(BUILD_DIR)/core/metrics/%.o,$(METRICS_SRCS))

# Compile metrics sources
$(BUILD_DIR)/core/metrics/%.o: $(METRICS_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link metrics library
$(METRICS_LIB): $(METRICS_OBJS) $(COMMON_LIB)
	$(CC) $(LDFLAGS) $^ -o $@

# Update dependencies
$(CORE_LIB): $(CORE_OBJS) $(COMMON_LIB) $(SYMBOLS_LIB) $(VERSIONING_LIB) $(MINIMIZER_LIB) $(METRICS_LIB)
	$(CC) $(LDFLAGS) $^ -o $@
```

#### For CMake (CMakeLists.txt):

```cmake
# Add metrics component
set(METRICS_SOURCES
    src/core/metrics/nexus_metrics.c
)

add_library(nexus_metrics ${METRICS_SOURCES})
target_link_libraries(nexus_metrics PUBLIC nexus_common)

# Update core dependencies
target_link_libraries(nexus_core
    PUBLIC nexus_common
    PUBLIC nexus_symbols
    PUBLIC nexus_versioning
    PUBLIC nexus_minimizer
    PUBLIC nexus_metrics
)
```

## Testing the Integration

Create a simple test program to verify the new component:

```c
/**
 * @file test_metrics.c
 * @brief Test program for NexusLink metrics component
 */
#include "nexuslink/nexuslink.h"
#include "nexuslink/core/metrics/nexus_metrics.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("NexusLink Metrics Test\n");
    
    // Initialize NexusLink
    NexusResult result = nexuslink_initialize(NULL);
    if (result != NEXUS_SUCCESS) {
        printf("Failed to initialize NexusLink: %d\n", result);
        return 1;
    }
    
    // Get context
    NexusContext* ctx = nexuslink_get_context();
    
    // Configure metrics
    NexusMetricsConfig metrics_config = {
        .enable_timers = true,
        .enable_counters = true,
        .enable_logging = true,
        .output_path = "metrics.log"
    };
    
    // Initialize metrics with configuration
    result = nexus_metrics_initialize(ctx, &metrics_config);
    if (result != NEXUS_SUCCESS) {
        printf("Failed to initialize metrics: %d\n", result);
        nexuslink_cleanup();
        return 1;
    }
    
    // Start a timer
    result = nexus_metrics_start_timer(ctx, "test_operation");
    if (result != NEXUS_SUCCESS) {
        printf("Failed to start timer: %d\n", result);
        nexuslink_cleanup();
        return 1;
    }
    
    // Perform some operation
    printf("Performing test operation...\n");
    for (int i = 0; i < 1000000; i++) {
        // Busy loop
    }
    
    // Stop the timer
    result = nexus_metrics_stop_timer(ctx, "test_operation");
    if (result != NEXUS_SUCCESS) {
        printf("Failed to stop timer: %d\n", result);
        nexuslink_cleanup();
        return 1;
    }
    
    // Clean up
    nexuslink_cleanup();
    
    printf("Test completed successfully\n");
    return 0;
}
```

## Common Issues and Best Practices

### Circular Dependencies

Avoid circular include dependencies by using forward declarations:

```c
// In header A
typedef struct TypeB TypeB;  // Forward declaration

// In header B
#include "a.h"
```

### Include Guards vs. Pragma Once

Always use include guards over `#pragma once` for maximum portability:

```c
// Good
#ifndef HEADER_H
#define HEADER_H
// Content
#endif

// Avoid
#pragma once
// Content
```

### Minimize Public Interfaces

Keep implementation details out of public headers:

```c
// Public header - only what users need
typedef struct NexusMetrics NexusMetrics;  // Opaque struct

// Internal header - full implementation details
struct NexusMetrics {
    // Implementation details
};
```

### Error Handling

Always check return values and provide appropriate error handling:

```c
NexusResult result = nexus_function();
if (result != NEXUS_SUCCESS) {
    // Handle error
    return result;
}
```

## Conclusion

This guide demonstrates how to properly integrate C and header files into the NexusLink project following Nnamdi Okpala's architectural principles. By adhering to these conventions, you'll ensure your code integrates seamlessly with the existing codebase, maintains proper encapsulation, and follows the established patterns for error handling, memory management, and component organization.

Following these patterns will help maintain the quality, readability, and maintainability of the NexusLink codebase as it continues to evolve.