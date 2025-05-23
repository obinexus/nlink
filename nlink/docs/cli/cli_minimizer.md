```md
classDiagram
    class NexusMinimizer {
        +initialize()
        +minimize_component(component)
        +get_metrics()
        -apply_automaton_minimization()
        -apply_ast_optimization()
    }
    
    class OkpalaAutomaton {
        +create()
        +add_state(id, is_final)
        +add_transition(from_id, to_id, symbol)
        +minimize(use_boolean_reduction)
        +free()
    }
    
    class OkpalaAST {
        +create()
        +add_node(parent, value)
        +optimize(use_boolean_reduction)
        +free()
    }
    
    class MinimizationMetrics {
        +original_states: size_t
        +minimized_states: size_t
        +original_size: size_t
        +minimized_size: size_t
        +time_taken_ms: double
    }
    
    class CLIMinimizeCommand {
        +execute(ctx, argc, argv)
        +print_help()
        +parse_args(argc, argv)
    }
    
    NexusMinimizer --> OkpalaAutomaton : uses
    NexusMinimizer --> OkpalaAST : uses
    NexusMinimizer --> MinimizationMetrics : produces
    CLIMinimizeCommand --> NexusMinimizer : invokes
    ```