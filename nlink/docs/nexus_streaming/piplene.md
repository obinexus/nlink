1. **Core infrastructure**:
   - Dynamic loader (`nexus_loader.c`)
   - Command system with parameter extraction
   - Pattern matching capabilities
   - Symbol resolution and versioning

2. **Minimizer subsystem**:
   - State machine representation (`okpala_automaton.c`)
   - AST processing (`okpala_ast.c`)
   - Automaton minimization algorithms

3. **CLI interface**:
   - Command registry and routing
   - Commands for loading, minimizing, versioning
   - Support for both standard and "minimal" syntax modes

## Component Linking Approach
- Components are loaded dynamically at runtime
- Dependencies are resolved on-demand
- Symbol resolution occurs through a registry system
- Version compatibility is managed systematically

For pipeline architectures (tokenizer → parser → AST), this allows components to be linked with only what they need, rather than monolithically.

## Implementation Plan for Single-Pass System Support

For single-pass systems (where components form a linear pipeline):

1. **Enhanced Registry Dependency Management**:
   - Implement a dependency graph in the registry
   - Allow components to declare upstream/downstream dependencies
   - Support directional dependency resolution

2. **Pipeline Configuration Definition**:
   - Create a pipeline configuration format (JSON/YAML) defining component flow
   - Support linear component chaining: tokenizer → parser → AST
   - Enable configuration-driven instantiation

3. **Streaming Data Transfer Protocol**:
   - Define a standard interface for data streaming between components
   - Implement buffer management for efficient data transfer
   - Support both synchronous and asynchronous processing modes

4. **Lifecycle Management**:
   - Add pipeline initialization/termination hooks
   - Implement proper cleanup for aborted pipelines
   - Add support for runtime pipeline reconfiguration

## Multi-Pass System Enhancements

To extend for multi-pass systems (with feedback loops and complex dependencies):

1. **Bidirectional Component Communication**:
   - Support bidirectional connections between components
   - Implement event notification for backward communication
   - Add request/response patterns for component queries

2. **Dependency Cycle Resolution**:
   - Detect and manage circular dependencies
   - Implement component isolation through proxy interfaces
   - Support lazy initialization of circular dependencies

3. **Diamond Dependency Injection**:
   - Create a dependency injection container
   - Support multiple interface implementations
   - Allow runtime selection of implementation variants

4. **Staged Processing Model**:
   - Support multi-stage processing within a pipeline
   - Enable partial processing and resumption
   - Implement checkpointing for long-running pipelines

## Technical Implementation Specifics

For immediate implementation, I recommend:

1. **Enhance the NexusHandleRegistry**:
   ```c
   // Add dependency tracking to registry
   typedef struct {
       const char* required_component;
       const char* required_interface;
       bool optional;
   } NexusDependency;
   
   // Extend component registration
   NexusResult nexus_register_component_with_deps(
       NexusHandleRegistry* registry,
       void* handle,
       const char* path,
       const char* component_id,
       NexusDependency* dependencies,
       size_t dep_count);
   ```

2. **Create Pipeline Configuration Interface**:
   ```c
   // Define pipeline configuration
   typedef struct {
       const char** component_ids;
       size_t component_count;
       const char* input_format;
       const char* output_format;
       bool allow_partial_processing;
   } NexusPipelineConfig;
   
   // Initialize a pipeline from configuration
   NexusPipeline* nexus_pipeline_create(
       NexusContext* ctx,
       NexusPipelineConfig* config);
   ```

3. **Implement Stream Processing Protocol**:
   ```c
   // Data stream abstraction
   typedef struct {
       void* data;
       size_t size;
       const char* format;
       void* metadata;
   } NexusDataStream;
   
   // Component processing function signature
   typedef NexusResult (*NexusProcessFunc)(
       NexusComponent* component,
       NexusDataStream* input,
       NexusDataStream* output);
   ```

