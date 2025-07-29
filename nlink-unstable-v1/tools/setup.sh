#!/bin/bash
#
# NexusLink Dynamic Component Linkage System
# Setup Script for Development Environment
# 
# Copyright © 2025 OBINexus Computing

set -e  # Exit immediately if a command exits with a non-zero status

# ANSI color codes for better output readability
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Banner and version info
SCRIPT_VERSION="1.0.0"

echo -e "${CYAN}"
echo "  _   _ _     _       _    "
echo " | \ | | |   (_)     | |   "
echo " |  \| | |    _ _ __ | | __"
echo " | . \` | |   | | '_ \| |/ /"
echo " | |\  | |___| | | | |   < "
echo " |_| \_|\_____/_|_| |_|_|\_\\"
echo "                           "
echo -e "${NC}"
echo -e "${BLUE}Dynamic Component Linkage System${NC}"
echo -e "${MAGENTA}Development Environment Setup (v${SCRIPT_VERSION})${NC}"
echo -e "${MAGENTA}Copyright © 2025 OBINexus Computing${NC}"
echo ""

# Function to print section headers
section() {
    echo ""
    echo -e "${BLUE}==== $1 ====${NC}"
}

# Function to print success messages
success() {
    echo -e "${GREEN}✓ $1${NC}"
}

# Function to print warning messages
warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

# Function to print error messages
error() {
    echo -e "${RED}✗ $1${NC}"
}

# Function to print info messages
info() {
    echo -e "${CYAN}ℹ $1${NC}"
}

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check OS and dependencies
check_environment() {
    section "Environment Check"
    
    # Check OS
    if [ "$(uname)" == "Darwin" ]; then
        OS="macos"
        info "macOS detected"
    elif [ "$(uname)" == "Linux" ]; then
        OS="linux"
        info "Linux detected"
    else
        OS="other"
        info "Other OS detected (Windows/BSD/etc.)"
    fi
    
    # Check for required tools
    REQUIRED_TOOLS=("gcc" "make" "cmake" "git" "clang-format")
    MISSING_TOOLS=()
    
    for tool in "${REQUIRED_TOOLS[@]}"; do
        if command_exists "$tool"; then
            success "Found $tool"
        else
            warning "$tool not found"
            MISSING_TOOLS+=("$tool")
        fi
    done
    
    if [ ${#MISSING_TOOLS[@]} -ne 0 ]; then
        warning "Some tools are missing. You may need to install them manually."
    else
        success "All required development tools are available"
    fi
}

# Create initial project structure
create_project_structure() {
    section "Creating Project Structure"
    
    # Create main directories
    mkdir -p include/nexus
    mkdir -p src/core
    mkdir -p src/metadata
    mkdir -p src/symbols
    mkdir -p src/versioning
    mkdir -p src/minimizer
    mkdir -p tests/unit
    mkdir -p tests/integration
    mkdir -p build
    mkdir -p docs/api
    mkdir -p examples/minimal
    mkdir -p tools
    
    success "Basic directory structure created"

    # Create CMake files
    cat > "CMakeLists.txt" << 'EOF'
cmake_minimum_required(VERSION 3.13)
project(NexusLink VERSION 0.1.0 LANGUAGES C)

# Options
option(BUILD_TESTING "Build test suite" ON)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_MICRO "Enable micro command support" ON)
option(ENABLE_EDGE "Enable edge command support" ON)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

# Add coverage flags if enabled
if(ENABLE_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
endif()

# Add include directories
include_directories(include)

# Add subdirectories
add_subdirectory(src)

# Unit tests
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install rules
install(DIRECTORY include/ DESTINATION include)
EOF

    cat > "src/CMakeLists.txt" << 'EOF'
# Define source files
set(CORE_SOURCES
    core/nexus_json.c
    core/nexus_lazy.c
)

set(METADATA_SOURCES
    metadata/nexus_metadata.c
    metadata/nexus_enhanced_metadata.c
)

set(SYMBOLS_SOURCES
    symbols/nexus_symbols.c
    symbols/nexus_versioned_symbols.c
)

set(VERSIONING_SOURCES
    versioning/nexus_semver.c
)

set(MINIMIZER_SOURCES
    minimizer/nexus_automaton.c
)

# Define libraries
add_library(nexus_core SHARED ${CORE_SOURCES})
add_library(nexus_metadata SHARED ${METADATA_SOURCES})
add_library(nexus_symbols SHARED ${SYMBOLS_SOURCES})
add_library(nexus_versioning SHARED ${VERSIONING_SOURCES})
add_library(nexus_minimizer SHARED ${MINIMIZER_SOURCES})

# Link dependencies
target_link_libraries(nexus_metadata PUBLIC nexus_core)
target_link_libraries(nexus_symbols PUBLIC nexus_core nexus_versioning)
target_link_libraries(nexus_minimizer PUBLIC nexus_core)

# Define main library that includes everything
add_library(nlink SHARED 
    $<TARGET_OBJECTS:nexus_core>
    $<TARGET_OBJECTS:nexus_metadata>
    $<TARGET_OBJECTS:nexus_symbols>
    $<TARGET_OBJECTS:nexus_versioning>
    $<TARGET_OBJECTS:nexus_minimizer>
)

target_link_libraries(nlink PRIVATE dl pthread)

# Set library properties
set_target_properties(nlink PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Install libraries
install(TARGETS nlink nexus_core nexus_metadata nexus_symbols nexus_versioning nexus_minimizer
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
EOF

    cat > "tests/CMakeLists.txt" << 'EOF'
add_subdirectory(unit)
add_subdirectory(integration)
EOF

    cat > "tests/unit/CMakeLists.txt" << 'EOF'
# Add unit tests here
include(CTest)

# Example test
add_executable(test_semver test_semver.c)
target_link_libraries(test_semver nexus_versioning)
add_test(NAME SemverTests COMMAND test_semver)

# Example test for symbols
add_executable(test_symbols test_symbols.c)
target_link_libraries(test_symbols nexus_symbols)
add_test(NAME SymbolsTests COMMAND test_symbols)
EOF

    cat > "tests/integration/CMakeLists.txt" << 'EOF'
# Add integration tests here
include(CTest)

# Example test for diamond dependency resolution
add_executable(test_diamond_dependency test_diamond_dependency.c)
target_link_libraries(test_diamond_dependency nlink)
add_test(NAME DiamondDependencyTest COMMAND test_diamond_dependency)

# Example test for versioned integration
add_executable(test_versioned_integration test_versioned_integration.c)
target_link_libraries(test_versioned_integration nlink)
add_test(NAME VersionedIntegrationTest COMMAND test_versioned_integration)
EOF

    success "CMake files created successfully"

    # Create a simple README file
    cat > "README.md" << 'EOF'
# NexusLink

## Dynamic Component Linkage System

NexusLink represents a paradigm shift in component architecture for programming language toolchains. 
Building on the foundation established by LibRift's automaton-based language processing, NexusLink 
introduces dynamic, demand-driven component loading that significantly reduces binary footprints and 
enhances compilation efficiency.

### Core Principles

NexusLink is founded on four fundamental principles:

1. **Load-By-Need**: Components are loaded into memory only when needed and unloaded when no longer required.
2. **Symbol Resolution**: Lazy symbol binding occurs at runtime rather than compile time.
3. **Dependency Pruning**: The system automatically identifies and eliminates unused components.
4. **Component Isolation**: Language components (tokenizers, parsers, type systems) are encapsulated as independent modules.

### Getting Started

```bash
# Clone the repository
git clone https://github.com/obinexus/nlink.git
cd nlink

# Build the project
mkdir build && cd build
cmake ..
make

# Run the tests
make test
```

### Documentation

For more information, see the [documentation](docs/README.md).

### License

Copyright © 2025 OBINexus Computing - All rights reserved.
EOF

    success "README created"

    # Create gitignore file
    cat > ".gitignore" << 'EOF'
# Build artifacts
build/
lib/
*.so
*.a
*.o
*.obj
*.exe
*.dll
*.dylib

# CMake artifacts
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
compile_commands.json
Makefile
*.cmake

# Code coverage artifacts
*.gcda
*.gcno
*.gcov
coverage/

# Generated documentation
docs/api/generated/

# Editor artifacts
.vscode/
.idea/
*.swp
*~

# OS artifacts
.DS_Store
Thumbs.db
EOF

    success "Project structure setup complete"
}

# Setup code formatting
setup_formatting() {
    section "Setting Up Code Formatting"

    # Create .clang-format file
    cat > ".clang-format" << 'EOF'
---
Language: Cpp
BasedOnStyle: LLVM
IndentWidth: 4
TabWidth: 4
UseTab: Never
ColumnLimit: 100
AllowShortFunctionsOnASingleLine: None
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false
BreakBeforeBraces: Linux
IndentCaseLabels: false
PointerAlignment: Right
SortIncludes: true
IncludeBlocks: Regroup
IncludeCategories:
  - Regex:           '^<nexus/.*\.h>'
    Priority:        2
  - Regex:           '^<.*\.h>'
    Priority:        1
  - Regex:           '.*'
    Priority:        3
---
EOF

    # Create formatting script
    cat > "tools/format.sh" << 'EOF'
#!/bin/bash
# Script to format all C code in the NexusLink project

find include src tests -name "*.c" -o -name "*.h" | while read -r file; do
    echo "Formatting $file"
    clang-format -i "$file"
done

echo "Formatting complete!"
EOF
    chmod +x tools/format.sh

    success "Code formatting setup complete"
}

# Setup Git hooks
setup_git_hooks() {
    section "Setting Up Git Hooks"

    # Check if we're in a Git repository
    if [ -d ".git" ]; then
        mkdir -p .git/hooks

        # Create pre-commit hook
        cat > ".git/hooks/pre-commit" << 'EOF'
#!/bin/bash
# Pre-commit hook for NexusLink project

echo "Running pre-commit checks..."

# Store the files that are about to be committed
files=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(c|h)$')

if [ -z "$files" ]; then
    # No C files to check
    exit 0
fi

# Format code
if command -v clang-format >/dev/null 2>&1; then
    echo "Running clang-format..."
    for file in $files; do
        clang-format -i "$file"
        git add "$file"
    done
else
    echo "Warning: clang-format not found, skipping code formatting"
fi

# Run basic static analysis
if command -v cppcheck >/dev/null 2>&1; then
    echo "Running cppcheck..."
    cppcheck --quiet --error-exitcode=1 $files
    if [ $? -ne 0 ]; then
        echo "Error: cppcheck found issues, commit aborted"
        exit 1
    fi
fi

echo "Pre-commit checks passed!"
exit 0
EOF
        chmod +x .git/hooks/pre-commit

        # Create commit-msg hook
        cat > ".git/hooks/commit-msg" << 'EOF'
#!/bin/bash
# Commit message hook for NexusLink project

commit_msg_file="$1"
commit_msg=$(cat "$commit_msg_file")

# Regex pattern for conventional commits
conventional_pattern='^(feat|fix|docs|style|refactor|perf|test|build|ci|chore|revert)(\([a-z0-9-]+\))?: .{1,}'

if ! [[ "$commit_msg" =~ $conventional_pattern ]]; then
    echo "Error: Commit message does not follow conventional format."
    echo "Required format: type(scope): description"
    echo "Where type is one of: feat, fix, docs, style, refactor, perf, test, build, ci, chore, revert"
    echo "Example: feat(symbols): implement versioned symbol registry"
    exit 1
fi

exit 0
EOF
        chmod +x .git/hooks/commit-msg

        success "Git hooks installed"
    else
        warning "Not a Git repository. Skipping Git hooks setup."
        info "Run 'git init' to initialize a repository and then run this script again to set up Git hooks."
    fi
}

main() {
    section "NexusLink Setup"
    info "Initializing NexusLink development environment"
    
    check_environment
    create_project_structure
    setup_formatting
    setup_git_hooks
    
    section "Setup Complete"
    success "NexusLink development environment successfully initialized."
    info "Next steps:"
    info "1. Import source code from POC with './migrate_poc.sh'"
    info "2. Build the project with 'mkdir -p build && cd build && cmake .. && make'"
    info "3. Run tests with 'make test'"
}

# Run the main function
main