```mermaid
graph TB
    subgraph Core["Core System"]
        nexus_core["NexusCore\n(Context management)"]
        nexus_loader["Loader\n(Dynamic loading)"]
        nexus_symbols["Symbol Registry\n(3-tier management)"]
        nexus_versioning["Version Manager\n(SemVer support)"]
    end

    subgraph Minimizer["State Machine Minimizer"]
        minimizer["NexusMinimizer\n(API layer)"]
        okpala_ast["OkpalaAST\n(AST optimization)"]
        okpala_automaton["OkpalaAutomaton\n(State minimization)"]
    end

    subgraph CLI["Command Line Interface"]
        cli_main["Main\n(Entry point)"]
        cli_registry["Command Registry"]
        command_load["Load Command"]
        command_minimize["Minimize Command"]
        command_version["Version Command"]
        command_minimal["Minimal Command"]
    end

    subgraph Metadata["Metadata System"]
        metadata["EnhancedMetadata"]
        json["JSON Parser"]
    end

    %% Core connections
    nexus_core --> nexus_loader
    nexus_core --> nexus_symbols
    nexus_core --> nexus_versioning

    %% Minimizer connections
    minimizer --> okpala_ast
    minimizer --> okpala_automaton
    minimizer --> nexus_core

    %% CLI connections
    cli_main --> cli_registry
    cli_registry --> command_load
    cli_registry --> command_minimize
    cli_registry --> command_version
    cli_registry --> command_minimal
    
    command_load --> nexus_loader
    command_load --> minimizer
    command_minimize --> minimizer
    
    %% Metadata connections
    metadata --> json
    nexus_loader --> metadata
    
    %% Cross-module connections
    nexus_symbols -.-> nexus_versioning
    nexus_loader -.-> nexus_symbols
    ```