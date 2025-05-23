```mermaid
classDiagram
    class NexusContext {
        +void* handle
        +NexusSymbolRegistry* symbols
        +NexusLogLevel log_level
        +NexusFlags flags
    }
    
    class NlinkPipelineConfig {
        +NlinkPipelineMode mode
        +bool enable_optimization
        +bool enable_caching
        +unsigned max_iterations
        +const char* schema_path
    }
    
    class NlinkPipeline {
        -NlinkPipelineConfig config
        -NlinkPipelineMode active_mode
        -NlinkPipelineStage* first_stage
        -unsigned stage_count
        -NexusContext* ctx
        +create(NexusContext*, NlinkPipelineConfig*)
        +add_stage(name, func, user_data)
        +execute(input, output)
        +get_mode()
        +get_stats(iterations, time_ms)
        +destroy()
    }
    
    class NlinkPipelineStage {
        -char* name
        -NlinkPipelineStageFunc func
        -void* user_data
        -NlinkPipelineStage* next
    }
    
    class NlinkPipelinePass {
        -char* name
        -NlinkPassType type
        -NlinkPassFunc func
        -void* user_data
    }
    
    class NlinkPassManager {
        -NexusContext* ctx
        -NlinkPipelinePass** passes
        -unsigned pass_count
        -unsigned capacity
        +create(NexusContext*)
        +add_pass(NlinkPipelinePass*)
        +run(NlinkPipeline*)
        +destroy()
    }
    
    %% Relationships
    NlinkPipeline "1" -- "1" NexusContext : uses
    NlinkPipeline "1" -- "1" NlinkPipelineConfig : configuredBy
    NlinkPipeline "1" -- "0..*" NlinkPipelineStage : contains
    NlinkPipelineStage "1" -- "0..1" NlinkPipelineStage : next
    NlinkPassManager "1" -- "1" NexusContext : uses
    NlinkPassManager "1" -- "0..*" NlinkPipelinePass : manages
    NlinkPassManager "1" -- "0..*" NlinkPipeline : optimizes
    ```