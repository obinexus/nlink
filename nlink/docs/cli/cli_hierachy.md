```mermaid
classDiagram
    class NexusCommand {
        <<interface>>
        +const char* name
        +const char* description
        +void (*execute)(int argc, char** argv)
        +void (*printHelp)()
        +bool (*parseArgs)(int argc, char** argv)
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
    
    class BuildCommand {
        +execute(int argc, char** argv)
        +printHelp()
        +parseArgs(int argc, char** argv)
    }
    
    class LoadCommand {
        +execute(int argc, char** argv)
        +printHelp()
        +parseArgs(int argc, char** argv)
    }
    
    class StatsCommand {
        +execute(int argc, char** argv)
        +printHelp()
        +parseArgs(int argc, char** argv)
    }
    
    class VersionCommand {
        +execute(int argc, char** argv)
        +printHelp()
        +parseArgs(int argc, char** argv)
    }
    
    class MinimalCommand {
        +execute(int argc, char** argv)
        +printHelp()
        +parseArgs(int argc, char** argv)
    }
    
    class MinimizeCommand {
        +execute(int argc, char** argv)
        +printHelp()
        +parseArgs(int argc, char** argv)
    }
    
    NexusCommand <|.. BuildCommand
    NexusCommand <|.. LoadCommand
    NexusCommand <|.. StatsCommand
    NexusCommand <|.. VersionCommand
    NexusCommand <|.. MinimalCommand
    NexusCommand <|.. MinimizeCommand
    
    CommandRegistry o-- NexusCommand : contains
    ```