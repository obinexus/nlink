#!/bin/bash
#
# NexusLink Development Environment Setup Script
# =============================================
#
# This script sets up the development environment for the NexusLink project.
# It installs necessary dependencies, configures build environments,
# and prepares the repository for development.
#
# Copyright © 2025 OBINexus Computing - Nnamdi Michael Okpala

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
echo "███╗   ██╗███████╗██╗  ██╗██╗   ██╗███████╗██╗     ██╗███╗   ██╗██╗  ██╗"
echo "████╗  ██║██╔════╝╚██╗██╔╝██║   ██║██╔════╝██║     ██║████╗  ██║██║ ██╔╝"
echo "██╔██╗ ██║█████╗   ╚███╔╝ ██║   ██║███████╗██║     ██║██╔██╗ ██║█████╔╝ "
echo "██║╚██╗██║██╔══╝   ██╔██╗ ██║   ██║╚════██║██║     ██║██║╚██╗██║██╔═██╗ "
echo "██║ ╚████║███████╗██╔╝ ██╗╚██████╔╝███████║███████╗██║██║ ╚████║██║  ██╗"
echo "╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝╚══════╝╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝"
echo -e "${NC}"
echo -e "${BLUE}Dynamic Component Linkage System${NC}"
echo -e "${MAGENTA}Development Environment Setup (v${SCRIPT_VERSION})${NC}"
echo -e "${MAGENTA}Copyright © 2025 OBINexus Computing - Nnamdi Michael Okpala${NC}"
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

# Function to check OS type
check_os() {
    section "Detecting Operating System"
    
    if [ "$(uname)" == "Darwin" ]; then
        OS="macos"
        info "macOS detected"
    elif [ "$(uname)" == "Linux" ]; then
        OS="linux"
        if [ -f /etc/debian_version ]; then
            DISTRO="debian"
            info "Debian-based Linux detected (Debian/Ubuntu/Mint)"
        elif [ -f /etc/redhat-release ]; then
            DISTRO="redhat"
            info "RedHat-based Linux detected (RHEL/CentOS/Fedora)"
        elif [ -f /etc/arch-release ]; then
            DISTRO="arch"
            info "Arch Linux detected"
        else
            DISTRO="unknown"
            warning "Unknown Linux distribution. You might need to install dependencies manually."
        fi
    elif [ "$(uname -s | cut -c 1-10)" == "MINGW32_NT" ] || [ "$(uname -s | cut -c 1-10)" == "MINGW64_NT" ]; then
        OS="windows"
        info "Windows detected (MinGW/Git Bash)"
    else
        OS="unknown"
        error "Unsupported operating system. This script is designed for Linux, macOS, and Windows (with MinGW/Git Bash)."
        exit 1
    fi
}

# Function to install dependencies based on OS
install_dependencies() {
    section "Installing Dependencies"
    
    info "Installing build tools and dependencies..."
    
    case $OS in
        linux)
            case $DISTRO in
                debian)
                    info "Installing packages for Debian/Ubuntu..."
                    sudo apt-get update
                    sudo apt-get install -y \
                        build-essential \
                        cmake \
                        git \
                        gcc \
                        g++ \
                        make \
                        pkg-config \
                        libpcre3-dev \
                        libssl-dev \
                        valgrind \
                        doxygen \
                        graphviz \
                        cppcheck \
                        clang-format \
                        lcov
                    ;;
                redhat)
                    info "Installing packages for RedHat/CentOS/Fedora..."
                    if command_exists dnf; then
                        sudo dnf install -y \
                            gcc \
                            gcc-c++ \
                            cmake \
                            make \
                            git \
                            pkgconfig \
                            pcre-devel \
                            openssl-devel \
                            valgrind \
                            doxygen \
                            graphviz \
                            cppcheck \
                            clang-tools-extra \
                            lcov
                    else
                        sudo yum install -y \
                            gcc \
                            gcc-c++ \
                            cmake \
                            make \
                            git \
                            pkgconfig \
                            pcre-devel \
                            openssl-devel \
                            valgrind \
                            doxygen \
                            graphviz \
                            cppcheck \
                            clang-tools-extra \
                            lcov
                    fi
                    ;;
                arch)
                    info "Installing packages for Arch Linux..."
                    sudo pacman -Sy --noconfirm \
                        base-devel \
                        cmake \
                        git \
                        pcre \
                        openssl \
                        valgrind \
                        doxygen \
                        graphviz \
                        cppcheck \
                        clang \
                        lcov
                    ;;
                *)
                    warning "Unknown Linux distribution. Please install the following packages manually:"
                    info "- CMake (3.16 or higher)"
                    info "- GCC/G++ (supporting C11)"
                    info "- make"
                    info "- git"
                    info "- valgrind"
                    info "- Doxygen and Graphviz"
                    info "- Static analysis tools (cppcheck, clang-format)"
                    info "- LCOV (for code coverage)"
                    ;;
            esac
            ;;
        macos)
            info "Installing packages for macOS..."
            if ! command_exists brew; then
                info "Homebrew not found. Installing Homebrew..."
                /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
            fi
            
            brew update
            brew install \
                cmake \
                pcre \
                openssl \
                doxygen \
                graphviz \
                cppcheck \
                lcov
            
            # On macOS, clang is already installed with Xcode tools
            if ! command_exists clang; then
                info "Xcode Command Line Tools not found. Installing..."
                xcode-select --install
            fi
            ;;
        windows)
            info "For Windows development, we recommend using WSL (Windows Subsystem for Linux)"
            info "or setting up dependencies via MSYS2/MinGW."
            
            if command_exists pacman; then
                info "MSYS2 detected. Installing dependencies..."
                pacman -Sy --noconfirm \
                    mingw-w64-x86_64-toolchain \
                    mingw-w64-x86_64-cmake \
                    mingw-w64-x86_64-pcre \
                    mingw-w64-x86_64-openssl \
                    mingw-w64-x86_64-doxygen \
                    mingw-w64-x86_64-graphviz \
                    mingw-w64-x86_64-cppcheck
            else
                warning "MSYS2 package manager not found. Please install dependencies manually."
            fi
            ;;
    esac
    
    success "Dependencies installed successfully!"
}

# Function to check if required tools are installed
check_tools() {
    section "Checking Required Tools"
    
    # List of required tools
    REQUIRED_TOOLS=("cmake" "gcc" "make" "git")
    MISSING_TOOLS=()
    
    for tool in "${REQUIRED_TOOLS[@]}"; do
        if ! command_exists "$tool"; then
            MISSING_TOOLS+=("$tool")
            error "$tool not found"
        else
            success "$tool found"
        fi
    done
    
    if [ ${#MISSING_TOOLS[@]} -ne 0 ]; then
        error "Some required tools are missing. Please install them and run this script again."
        exit 1
    fi
    
    # Check CMake version
    CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
    CMAKE_MAJOR=$(echo $CMAKE_VERSION | cut -d. -f1)
    CMAKE_MINOR=$(echo $CMAKE_VERSION | cut -d. -f2)
    
    if [ "$CMAKE_MAJOR" -lt 3 ] || ([ "$CMAKE_MAJOR" -eq 3 ] && [ "$CMAKE_MINOR" -lt 16 ]); then
        warning "CMake version $CMAKE_VERSION detected. NexusLink requires CMake 3.16 or higher."
        warning "Please update CMake to continue."
        exit 1
    else
        success "CMake version $CMAKE_VERSION meets requirements"
    fi
    
    # Check GCC version
    GCC_VERSION=$(gcc --version | head -n1 | grep -o -E '[0-9]+\.[0-9]+\.[0-9]+' | head -n1)
    GCC_MAJOR=$(echo $GCC_VERSION | cut -d. -f1)
    
    if [ "$GCC_MAJOR" -lt 7 ]; then
        warning "GCC version $GCC_VERSION detected. NexusLink recommends GCC 7.0 or higher for full C11 support."
    else
        success "GCC version $GCC_VERSION meets requirements"
    fi
    
    success "All required tools are available!"
}

# Function to set up build directories
setup_build_dirs() {
    section "Setting Up Build Directories"
    
    # Create build directories if they don't exist
    mkdir -p build/debug
    mkdir -p build/release
    mkdir -p build/coverage
    
    success "Build directories created"
}

# Function to configure Git hooks
setup_git_hooks() {
    section "Setting Up Git Hooks"
    
    # Check if .git directory exists (we're in a Git repository)
    if [ -d ".git" ]; then
        # Create pre-commit hook
        PRE_COMMIT_HOOK=".git/hooks/pre-commit"
        
        info "Creating pre-commit hook..."
        cat > "$PRE_COMMIT_HOOK" << 'EOF'
#!/bin/bash
# Pre-commit hook for NexusLink project
# This hook runs static analysis and formatting checks before allowing commits

echo "Running pre-commit checks..."

# Store the files that are about to be committed
files=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(c|h)$')

if [ -z "$files" ]; then
    # No C files to check
    exit 0
fi

# Run clang-format on the files
if command -v clang-format >/dev/null 2>&1; then
    echo "Running clang-format..."
    for file in $files; do
        clang-format -i "$file"
        git add "$file"
    done
else
    echo "Warning: clang-format not found, skipping code formatting check"
fi

# Run cppcheck on the files
if command -v cppcheck >/dev/null 2>&1; then
    echo "Running cppcheck..."
    cppcheck --quiet --error-exitcode=1 $files
    if [ $? -ne 0 ]; then
        echo "Error: cppcheck found issues, commit aborted"
        exit 1
    fi
else
    echo "Warning: cppcheck not found, skipping static analysis check"
fi

echo "Pre-commit checks passed!"
exit 0
EOF
        
        chmod +x "$PRE_COMMIT_HOOK"
        success "Git pre-commit hook installed"
        
        # Create commit-msg hook for conventional commits
        COMMIT_MSG_HOOK=".git/hooks/commit-msg"
        
info "Creating commit-msg hook for conventional commits..."
        cat > "$COMMIT_MSG_HOOK" << 'EOF'
#!/bin/bash
# commit-msg hook for NexusLink project
# This hook enforces conventional commit message format

# The commit message file is passed as the first argument
commit_msg_file="$1"
commit_msg=$(cat "$commit_msg_file")

# Regex pattern for conventional commits
# format: type(scope): description
conventional_pattern='^(feat|fix|docs|style|refactor|perf|test|build|ci|chore|revert)(\([a-z0-9-]+\))?: .{1,}'

if ! [[ "$commit_msg" =~ $conventional_pattern ]]; then
    echo "Error: Commit message does not follow conventional format."
    echo "Required format: type(scope): description"
    echo "Where type is one of: feat, fix, docs, style, refactor, perf, test, build, ci, chore, revert"
    echo "Example: feat(minimizer): implement automaton state reduction algorithm"
    exit 1
fi

exit 0
EOF
        
        chmod +x "$COMMIT_MSG_HOOK"
        success "Git commit-msg hook installed"
    else
        warning "Not a Git repository. Skipping Git hooks setup."
    fi
}

# Function to configure CMake builds
configure_cmake() {
    section "Configuring CMake Builds"
    
    # Configure debug build
    info "Configuring debug build..."
    cd build/debug
    cmake ../.. -DCMAKE_BUILD_TYPE=Debug -DNEXUS_BUILD_TESTS=ON
    cd ../..
    success "Debug build configured"
    
    # Configure release build
    info "Configuring release build..."
    cd build/release
    cmake ../.. -DCMAKE_BUILD_TYPE=Release
    cd ../..
    success "Release build configured"
    
    # Configure coverage build
    info "Configuring coverage build..."
    cd build/coverage
    cmake ../.. -DCMAKE_BUILD_TYPE=Debug -DNEXUS_BUILD_TESTS=ON -DNEXUS_ENABLE_ASAN=ON
    cd ../..
    success "Coverage build configured"
}

# Function to set up environment variables
setup_environment() {
    section "Setting Up Environment Variables"
    
    # Set up environment variables based on OS
    case $OS in
        linux|macos)
            ENV_FILE="$HOME/.nexuslink_env"
            
            # Create or update environment file
            info "Creating environment file at $ENV_FILE"
            cat > "$ENV_FILE" << EOF
# NexusLink environment variables
export NEXUSLINK_ROOT="$(pwd)"
export PATH="\$NEXUSLINK_ROOT/build/debug/bin:\$NEXUSLINK_ROOT/build/release/bin:\$PATH"
export LD_LIBRARY_PATH="\$NEXUSLINK_ROOT/build/debug/lib:\$NEXUSLINK_ROOT/build/release/lib:\$LD_LIBRARY_PATH"
export CPPCHECK_SUPPRESS="\$NEXUSLINK_ROOT/.cppcheck_suppressions"
EOF
            
            # Add source command to shell profile
            SHELL_PROFILE=""
            if [ "$OS" == "macos" ]; then
                if [ -f "$HOME/.zshrc" ]; then
                    SHELL_PROFILE="$HOME/.zshrc"
                else
                    SHELL_PROFILE="$HOME/.bash_profile"
                fi
            else
                if [ -f "$HOME/.bashrc" ]; then
                    SHELL_PROFILE="$HOME/.bashrc"
                elif [ -f "$HOME/.zshrc" ]; then
                    SHELL_PROFILE="$HOME/.zshrc"
                fi
            fi
            
            if [ -n "$SHELL_PROFILE" ]; then
                # Check if the line already exists to avoid duplication
                if ! grep -q "source $ENV_FILE" "$SHELL_PROFILE"; then
                    echo "# NexusLink environment setup" >> "$SHELL_PROFILE"
                    echo "source $ENV_FILE" >> "$SHELL_PROFILE"
                    info "Added environment setup to $SHELL_PROFILE"
                else
                    info "Environment setup already exists in $SHELL_PROFILE"
                fi
            else
                warning "Could not find shell profile. Please add the following line to your shell profile manually:"
                echo "source $ENV_FILE"
            fi
            ;;
        windows)
            info "For Windows environment setup:"
            info "1. Add the following directories to your PATH:"
            info "   - %CD%\\build\\debug\\bin"
            info "   - %CD%\\build\\release\\bin"
            info "2. Set NEXUSLINK_ROOT environment variable to: %CD%"
            ;;
    esac
    
    success "Environment setup completed"
}

# Function to create code formatting configuration
setup_formatting() {
    section "Setting Up Code Formatting"
    
    # Create .clang-format file if it doesn't exist
    if [ ! -f ".clang-format" ]; then
        info "Creating .clang-format file..."
        cat > ".clang-format" << 'EOF'
---
Language: Cpp
# NexusLink uses C11 standard
Standard: c++11
BasedOnStyle: LLVM
IndentWidth: 4
TabWidth: 4
UseTab: Never
ColumnLimit: 100
AllowShortFunctionsOnASingleLine: None
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false
AlwaysBreakAfterDefinitionReturnType: All
BreakBeforeBraces: Linux
IndentCaseLabels: false
PointerAlignment: Right
SortIncludes: true
IncludeBlocks: Regroup
IncludeCategories:
  - Regex:           '^<nlink/.*\.h>'
    Priority:        2
  - Regex:           '^<.*\.h>'
    Priority:        1
  - Regex:           '.*'
    Priority:        3
...
EOF
        success ".clang-format file created"
    else
        info ".clang-format file already exists"
    fi
    
    # Create .cppcheck_suppressions file
    if [ ! -f ".cppcheck_suppressions" ]; then
        info "Creating .cppcheck_suppressions file..."
        cat > ".cppcheck_suppressions" << 'EOF'
// Suppress warnings for variadic arguments in our API functions
variableScope
unusedFunction
missingInclude
EOF
        success ".cppcheck_suppressions file created"
    else
        info ".cppcheck_suppressions file already exists"
    fi
    
    # Create or update format.sh script
    mkdir -p tools
    info "Creating format.sh script..."
    cat > "tools/format.sh" << 'EOF'
#!/bin/bash
# Script to format all C code in the NexusLink project

# Find all C files in include and src directories
find include src -name "*.c" -o -name "*.h" | while read -r file; do
    echo "Formatting $file"
    clang-format -i "$file"
done

echo "Formatting complete!"
EOF
    chmod +x "tools/format.sh"
    success "format.sh script created"
}

# Function to create a build helper script
create_build_script() {
    section "Creating Build Helper Script"
    
    info "Creating build.sh script..."
    cat > "build.sh" << 'EOF'
#!/bin/bash
# NexusLink build script
# Provides a convenient interface for building the project

set -e  # Exit immediately if a command exits with a non-zero status

# ANSI color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print usage information
usage() {
    echo "NexusLink Build Script"
    echo ""
    echo "Usage: $0 [OPTIONS] [TARGET]"
    echo ""
    echo "Options:"
    echo "  --cmake           Build using CMake (default)"
    echo "  --make            Build using Makefile directly"
    echo "  --debug           Build debug version (default)"
    echo "  --release         Build release version"
    echo "  --coverage        Build with coverage instrumentation"
    echo "  --clean           Clean before building"
    echo "  --install         Install after building"
    echo "  --help            Show this help message"
    echo ""
    echo "Targets (for Makefile build):"
    echo "  all               Build all (default)"
    echo "  nlink             Build NexusLink library"
    echo "  nlink_cli         Build CLI executable"
    echo "  test              Run tests"
    echo ""
    echo "Examples:"
    echo "  $0 --cmake --debug      Build debug version using CMake"
    echo "  $0 --make --release     Build release version using Makefile"
    echo "  $0 --clean --make       Clean and build using Makefile"
}

# Default options
BUILD_SYSTEM="cmake"
BUILD_TYPE="debug"
CLEAN=false
INSTALL=false
TARGET="all"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        --cmake)
            BUILD_SYSTEM="cmake"
            shift
            ;;
        --make)
            BUILD_SYSTEM="make"
            shift
            ;;
        --debug)
            BUILD_TYPE="debug"
            shift
            ;;
        --release)
            BUILD_TYPE="release"
            shift
            ;;
        --coverage)
            BUILD_TYPE="coverage"
            shift
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --install)
            INSTALL=true
            shift
            ;;
        --help)
            usage
            exit 0
            ;;
        -*)
            echo -e "${RED}Error: Unknown option $1${NC}"
            usage
            exit 1
            ;;
        *)
            TARGET="$1"
            shift
            ;;
    esac
done

# Header
echo -e "${BLUE}=== NexusLink Build (${BUILD_SYSTEM}, ${BUILD_TYPE}) ===${NC}"

# Clean if requested
if [ "$CLEAN" = true ]; then
    echo -e "${YELLOW}Cleaning build artifacts...${NC}"
    if [ "$BUILD_SYSTEM" = "cmake" ]; then
        rm -rf build/${BUILD_TYPE}/*
    else
        make clean
    fi
fi

# Build using selected build system
if [ "$BUILD_SYSTEM" = "cmake" ]; then
    # CMake build
    echo -e "${YELLOW}Building with CMake (${BUILD_TYPE})...${NC}"
    mkdir -p build/${BUILD_TYPE}
    cd build/${BUILD_TYPE}
    
    # Configure based on build type
    if [ "$BUILD_TYPE" = "debug" ]; then
        cmake ../.. -DCMAKE_BUILD_TYPE=Debug -DNEXUS_BUILD_TESTS=ON
    elif [ "$BUILD_TYPE" = "release" ]; then
        cmake ../.. -DCMAKE_BUILD_TYPE=Release
    elif [ "$BUILD_TYPE" = "coverage" ]; then
        cmake ../.. -DCMAKE_BUILD_TYPE=Debug -DNEXUS_BUILD_TESTS=ON -DNEXUS_ENABLE_ASAN=ON
    fi
    
    # Build
    make -j$(nproc)
    
    # Install if requested
    if [ "$INSTALL" = true ]; then
        echo -e "${YELLOW}Installing...${NC}"
        sudo make install
    fi
else
    # Direct Makefile build
    echo -e "${YELLOW}Building with Makefile (${TARGET})...${NC}"
    make ${TARGET}
    
    # Install if requested
    if [ "$INSTALL" = true ]; then
        echo -e "${YELLOW}Installing...${NC}"
        sudo make install
    fi
fi

echo -e "${GREEN}Build completed successfully!${NC}"
EOF
    chmod +x "build.sh"
    success "build.sh script created"
}

# Main script execution
check_os
install_dependencies
check_tools
setup_build_dirs
setup_git_hooks
setup_formatting
configure_cmake
setup_environment
create_build_script

section "Setup Complete"
success "NexusLink development environment setup completed successfully!"
info "To get started:"
info "1. Source your environment: source ~/.nexuslink_env"
info "2. Build the project: ./build.sh"
info "3. Run the CLI: ./build/debug/bin/nlink --help"