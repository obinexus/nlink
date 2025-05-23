# NexusLink System Architecture Integration Guide

## Overview

This document describes the integration of the Single-Pass System (SPS) and Multi-Pass System (MPS) modules into the NexusLink architecture. These modules enhance NexusLink with pipeline processing capabilities, dependency management, and streamlined data transfer between components.

## System Architecture

The NexusLink architecture now includes two complementary pipeline systems:

1. **Single-Pass System (SPS)**: A linear pipeline where data flows in one direction through components.
2. **Multi-Pass System (MPS)**: A complex pipeline supporting bidirectional data flow and multiple iterations through components.

### Component Integration

Both systems integrate with the core NexusLink components:

```
NexusLink
├── Core
│   ├── Common (types, loader, etc.)
│   ├── Symbols
│   └── Versioning
├── Single-Pass System (SPS)
│   ├── Config
│   ├── Dependency
│   ├── Pipeline
│   ├── Stream
│   └── Lifecycle
└── Multi-Pass System (MPS)
    ├── Config
    ├── Dependency
    ├── Pipeline
    ├── Stream
    └── Lifecycle
```

## CMake Integration

Add the following to your root `CMakeLists.txt`:

```cmake
# Add SPS system
add_subdirectory(src/spsystem)

# Add MPS system
add_subdirectory(src/mpsystem)
```

Create `src/spsystem/CMakeLists.txt`:

```cmake
# SPS system CMakeLists.txt
set(SPS_SOURCES
    sps_config.c
    sps_dependency.c
    sps_pipeline.c
    sps_stream.c
    sps_lifecycle.c
)

# Create SPS library
add_library(nexus_sps ${SPS_SOURCES})

# Set include directories
target_include_directories(nexus_sps
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

# Link dependencies
target_link_libraries(nexus_sps
    PUBLIC
        nexus_common
)

# Install headers and library
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/nlink/spsystem/
        DESTINATION include/nlink/spsystem
        FILES_MATCHING PATTERN "*.h")

install(TARGETS nexus_sps
        EXPORT NexusLinkTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)
```

Create `src/mpsystem/CMakeLists.txt` with similar content for MPS.

## Usage Examples

### Single-Pass Pipeline

```c
#include "nlink/core/common/nexus_core.h"
#include "nlink/spsystem/sps_pipeline.h"
#include "nlink/spsystem/sps_config.h"
#include "nlink/spsystem/sps_stream.h"

int main() {
    // Initialize NexusLink context
    NexusContext* ctx = nexus_create_context(NULL);
    
    // Parse pipeline configuration from JSON
    NexusPipelineConfig* config = sps_parse_pipeline_config(ctx, "pipeline_config.json");
    
    // Create and initialize pipeline
    NexusPipeline* pipeline = sps_pipeline_create(ctx, config);
    sps_pipeline_initialize(ctx, pipeline);
    
    // Create input and output streams
    NexusDataStream* input = sps_stream_create(1024);
    NexusDataStream* output = sps_stream_create(1024);
    
    // Write input data
    const char* input_data = "Input data for processing";
    sps_stream_write(input, input_data, strlen(input_data));
    
    // Execute pipeline
    sps_pipeline_execute(ctx, pipeline, input, output);
    
    // Read output data
    char output_buffer[1024];
    size_t bytes_read;
    sps_stream_read(output, output_buffer, sizeof(output_buffer), &bytes_read);
    output_buffer[bytes_read] = '\0';
    printf("Output: %s\n", output_buffer);
    
    // Cleanup
    sps_stream_destroy(input);
    sps_stream_destroy(output);
    sps_pipeline_destroy(ctx, pipeline);
    sps_free_pipeline_config(config);
    nexus_destroy_context(ctx);
    
    return 0;
}
```

### Multi-Pass Pipeline

```c
#include "nlink/core/common/nexus_core.h"
#include "nlink/mpsystem/mps_pipeline.h"
#include "nlink/mpsystem/mps_config.h"
#include "nlink/mpsystem/mps_stream.h"

int main() {
    // Initialize NexusLink context
    NexusContext* ctx = nexus_create_context(NULL);
    
    // Parse pipeline configuration from JSON
    NexusMPSConfig* config = mps_parse_pipeline_config(ctx, "mps_pipeline_config.json");
    
    // Create and initialize pipeline
    NexusMPSPipeline* pipeline = mps_pipeline_create(ctx, config);
    mps_pipeline_initialize(ctx, pipeline);
    
    // Set iteration limit (3 passes maximum)
    mps_pipeline_set_iteration_limit(pipeline, 3);
    
    // Create input and output streams
    NexusMPSDataStream* input = mps_stream_create(1024);
    NexusMPSDataStream* output = mps_stream_create(1024);
    
    // Write input data
    const char* input_data = "Input data for multi-pass processing";
    mps_stream_write(input, input_data, strlen(input_data));
    
    // Execute pipeline
    mps_pipeline_execute(ctx, pipeline, input, output);
    
    // Get execution statistics
    NexusMPSPipelineStats stats;
    mps_pipeline_get_stats(pipeline, &stats);
    printf("Pipeline executed %d iterations\n", stats.total_iterations);
    
    // Read output data
    char output_buffer[1024];
    size_t bytes_read;
    mps_stream_read(output, output_buffer, sizeof(output_buffer), &bytes_read);
    output_buffer[bytes_read] = '\0';
    printf("Output: %s\n", output_buffer);
    
    // Cleanup
    mps_stream_destroy(input);
    mps_stream_destroy(output);
    mps_pipeline_destroy(ctx, pipeline);
    mps_free_pipeline_config(config);
    nexus_destroy_context(ctx);
    
    return 0;
}
```

## Pipeline Configuration Format

### Single-Pass System Configuration (JSON)

```json
{
  "pipeline_id": "text_processing",
  "description": "Text tokenization and parsing pipeline",
  "components": [
    {
      "component_id": "tokenizer",
      "version_constraint": "^1.0.0"
    },
    {
      "component_id": "parser",
      "version_constraint": "^2.0.0"
    },
    {
      "component_id": "ast_builder",
      "version_constraint": "^1.2.0"
    }
  ],
  "input_format": "text/plain",
  "output_format": "application/ast+json",
  "allow_partial_processing": false
}
```

### Multi-Pass System Configuration (JSON)

```json
{
  "pipeline_id": "compiler_pipeline",
  "description": "Compiler with optimization passes",
  "components": [
    {
      "component_id": "lexer",
      "version_constraint": "^1.0.0"
    },
    {
      "component_id": "parser",
      "version_constraint": "^2.0.0",
      "supports_reentrance": true
    },
    {
      "component_id": "optimizer",
      "version_constraint": "^1.2.0",
      "supports_reentrance": true,
      "max_passes": 3
    },
    {
      "component_id": "code_generator",
      "version_constraint": "^1.1.0"
    }
  ],
  "connections": [
    {
      "source_id": "lexer",
      "target_id": "parser",
      "direction": "forward",
      "data_format": "token_stream"
    },
    {
      "source_id": "parser",
      "target_id": "optimizer",
      "direction": "forward",
      "data_format": "ast"
    },
    {
      "source_id": "optimizer",
      "target_id": "parser",
      "direction": "backward",
      "data_format": "modified_ast"
    },
    {
      "source_id": "optimizer",
      "target_id": "code_generator",
      "direction": "forward",
      "data_format": "optimized_ast"
    }
  ],
  "allow_cycles": true,
  "max_iteration_count": 10,
  "allow_partial_processing": false
}
```

## Dependency Resolution

Both systems handle dependencies differently:

1. **SPS**: Uses topological sorting to determine the correct execution order.
2. **MPS**: Uses strongly connected components algorithm to identify cycles and group components for execution.

## Testing

Test your integration with the provided shell scripts:

```bash
# Create SPS modules
./create_sps_modules.sh

# Create MPS modules
./create_mps_modules.sh

# Build the project
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# Run tests
ctest --output-on-failure
```

## Deployment Considerations

1. **Library Size**: The SPS modules add approximately 100KB to the library size, while MPS adds around 150KB.
2. **Performance**: MPS has higher overhead due to cycle detection and multiple iterations.
3. **Dependencies**: Both systems depend on the core NexusLink components.

## Future Enhancements

1. **Visualization**: Add pipeline visualization tools for debugging.
2. **Parallelism**: Add support for parallel component execution.
3. **Dynamic Reconfiguration**: Enhance runtime pipeline reconfiguration.