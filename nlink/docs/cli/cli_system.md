```mermaid
classDiagram
    class NexusCLI {
        -CommandRegistry registry
        -bool enableMinimal
        -const char* progName
        +initialize(const char* progName)
        +parseAndExecute(int argc, char** argv)
        +printGlobalHelp()
        +cleanup()
    }
    
    class CommandRegistry {
        -NexusCommand** commands
        -size_t commandCount
        -size_t capacity
        +initialize()
        +registerCommand(NexusCommand*)
        +findCommand(const char* name)
        +listCommands()
        +cleanup()
    }
    
    class NexusContext {
        +NexusConfig config
        +NexusSymbolRegistry* symbols
        +void* userData
    }
    
    class MinimalMode {
        <<interface>>
        +bool isEnabled
        +bool parseMinimalFormat(const char* input)
        +execute(const char* command)
    }
    
    class CLIMain {
        +main(int argc, char** argv)
        -setupCommands()
        -setupContext()
    }
    
    NexusCLI *-- CommandRegistry : contains
    NexusCLI o-- NexusContext : uses
    NexusCLI *-- MinimalMode : contains when enabled
    CLIMain --> NexusCLI : creates and uses
    ```