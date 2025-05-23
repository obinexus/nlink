```mermaid
classDiagram
    class NlinkCLI {
        -CommandRegistry* registry
        -Options options
        +initialize(int argc, char** argv)
        +parse_arguments()
        +execute_command()
        +cleanup()
    }

    class CommandRegistry {
        -Command** commands
        -size_t command_count
        +register_command(Command* cmd)
        +find_command(const char* name)
        +get_all_commands()
        +match_commands(const char* pattern)
    }

    class Command {
        <<interface>>
        +const char* name
        +const char* description 
        +execute(Options* opts)
    }

    class Options {
        -HashMap* options
        -List* arguments
        +set_option(const char* key, const char* value)
        +get_option(const char* key)
        +has_option(const char* key)
        +add_argument(const char* arg)
        +get_arguments()
    }

    class RegexMatcher {
        +match(const char* pattern, const char* string)
        +match_commands(const char* pattern, Command** commands, size_t count)
        -compile_pattern(const char* pattern)
    }

    class LoadCommand {
        +execute(Options* opts)
        -validate_options()
    }

    class MinimizeCommand {
        +execute(Options* opts)
        -validate_options()
        -determine_level()
    }

    class VersionCommand {
        +execute(Options* opts)
    }

    class MinimalCommand {
        +execute(Options* opts)
    }

    NlinkCLI --> CommandRegistry : uses
    CommandRegistry --> Command : contains
    NlinkCLI --> Options : contains
    CommandRegistry --> RegexMatcher : uses
    
    Command <|-- LoadCommand
    Command <|-- MinimizeCommand
    Command <|-- VersionCommand
    Command <|-- MinimalCommand
    ```