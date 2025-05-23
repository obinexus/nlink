# NexusLink Integration Guide

## Introduction

This guide provides practical instructions for integrating NexusLink into your development workflow. NexusLink offers significant binary size reduction and runtime optimization through dynamic loading and state machine minimization.

## Prerequisites

- C/C++ development environment
- CMake 3.13+ or Make
- GCC or Clang compiler
- Basic understanding of shared library concepts

## Installation

### From Source

```bash
# Clone the repository
git clone https://github.com/obinexus/nlink.git
cd nlink

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
make

# Install (optional)
sudo make install
```

### Binary Packages

For supported platforms, binary packages are available:

```bash
# Debian/Ubuntu
sudo apt install nexuslink

# Red Hat/Fedora
sudo dnf install nexuslink

# macOS
brew install nexuslink
```

## Basic Usage

### Command Line Interface

NexusLink provides a command-line interface for common operations:

```bash
# View available commands
nlink --help

# Load a component
nlink load libmycomponent.so

# Minimize a component
nlink minimize libmycomponent.so --level 2

# View version information
nlink version
```

### Minimizing Components

To minimize a component's size:

```bash
# Basic minimization
nlink minimize mycomponent.so

# Aggressive minimization with boolean reduction
nlink minimize mycomponent.so --level 3 --verbose

# Save minimized component to a new file
nlink minimize mycomponent.so --output mycomponent.min.so
```

## Integration with Build Systems

### CMake Integration

Add the following to your `CMakeLists.txt`:

```cmake
# Find NexusLink package
find_package(NexusLink REQUIRED)

# Add include directories
include_directories(${NEXUSLINK_INCLUDE_DIRS})

# Link against NexusLink libraries
target_link_libraries(your_target PRIVATE ${NEXUSLINK_LIBRARIES})

# Optional: Add post-build minimization step
add_custom_command(TARGET your_target POST_BUILD
    COMMAND nlink minimize $<TARGET_FILE:your_target> --level 2
    COMMENT "Minimizing component with NexusLink"
)
```

### Make Integration

Add the following to your `Makefile`:

```makefile
# NexusLink configuration
NEXUSLINK_CFLAGS = $(shell pkg-config --cflags nexuslink)
NEXUSLINK_LIBS = $(shell pkg-config --libs nexuslink)

# Add to compilation flags
CFLAGS += $(NEXUSLINK_CFLAGS)
LDFLAGS += $(NEXUSLINK_LIBS)

# Add post-build step
.PHONY: minimize
minimize: $(TARGET)
	nlink minimize $(TARGET) --level 2
```

## Programmatic Usage

### Initializing NexusLink

```c
#include <nexuslink/nexus_core.h>

int main() {
    // Initialize NexusLink
    NexusContext* ctx = nexus_create_context(NULL);
    if (!ctx) {
        fprintf(stderr, "Failed to initialize NexusLink\n");
        return 1;
    }
    
    // Use NexusLink...
    
    // Clean up
    nexus_destroy_context(ctx);
    
    return 0;
}
```

### Loading Components

```c
#include <nexuslink/nexus_loader.h>

// Load a component
NexusComponent* component = nexus_load_component(
    ctx,
    "path/to/libmycomponent.so",
    "mycomponent"
);

if (!component) {
    fprintf(stderr, "Failed to load component\n");
    return 1;
}

// Use the component...

// Unload when done
nexus_unload_component(ctx, component);
```

### Symbol Resolution

```c
#include <nexuslink/nexus_symbols.h>

// Resolve a symbol
void* symbol = nexus_resolve_symbol(ctx->symbols, "my_function");
if (symbol) {
    // Cast to appropriate function type and call
    typedef void (*MyFunction)(int);
    MyFunction func = (MyFunction)symbol;
    func(42);
}
```

### Lazy Function Loading

Define lazy-loaded functions using the `NEXUS_LAZY` macro:

```c
#include <nexuslink/nexus_lazy.h>

// Declare lazy-loaded function
NEXUS_LAZY(process_image, void, const char* filename) {
    // Ensure implementation is loaded
    load_process_image();
    
    // Call implementation
    process_image_impl(filename);
}

// Usage
int main() {
    // First call triggers lazy loading
    process_image("example.jpg");
    
    // Subsequent calls use cached implementation
    process_image("another.jpg");
    
    return 0;
}
```

### Version-Aware Loading

For components with version requirements:

```c
#include <nexuslink/nexus_versioned_symbols.h>

// Resolve a versioned symbol
void* symbol = nexus_resolve_versioned_symbol(
    registry,
    "process_image",
    "^1.0.0",     // Version constraint (compatible with 1.x.y)
    "my_component"
);
```

## Minimization API

To programmatically apply minimization:

```c
#include <nexuslink/nexus_minimizer.h>

// Initialize minimizer
nexus_minimizer_initialize(ctx);

// Configure minimization
NexusMinimizerConfig config = nexus_minimizer_default_config();
config.level = NEXUS_MINIMIZE_STANDARD;  // Standard optimization
config.verbose = true;
config.enable_metrics = true;

// Allocate metrics structure
NexusMinimizationMetrics metrics;

// Perform minimization
NexusResult result = nexus_minimize_component(
    ctx,
    "path/to/component.so",
    config,
    &metrics
);

if (result == NEXUS_SUCCESS) {
    // Print metrics
    nexus_print_minimization_metrics(&metrics);
}

// Clean up
nexus_minimizer_cleanup(ctx);
```

## Best Practices

### Component Organization

1. **Single Responsibility**: Each component should focus on a specific functionality
2. **Minimize Dependencies**: Keep the dependency graph shallow and directed
3. **Version Constraints**: Use precise version constraints to avoid conflicts
4. **Export Declarations**: Explicitly declare which symbols are exported

### Optimizing for Minimization

1. **State Design**: Design state machines with minimization in mind
   - Avoid redundant states with identical behaviors
   - Use boolean logic to simplify state transitions

2. **Function Organization**: Group related functions in the same component
   - Put frequently used functions together
   - Separate cold code (rarely used) from hot code (frequently used)

3. **Testing**: Verify behavior after minimization
   - Create comprehensive tests for functionality
   - Ensure state transitions work as expected after minimization

4. **Incremental Optimization**: Start with lower optimization levels
   - Begin with Level 1 (basic) to ensure compatibility
   - Progress to higher levels as confidence increases

## Debugging

### Minimization Issues

If minimization causes unexpected behavior:

1. Use `--verbose` flag to see detailed minimization steps
2. Examine the state transitions before and after minimization
3. Temporarily disable boolean reduction (`--level 1`)
4. Check for assumptions about state identity that may be violated by merging

### Loading Issues

If components fail to load:

1. Check for missing dependencies
2. Verify version constraints are satisfiable
3. Ensure all required symbols are available
4. Use `LD_DEBUG=all` to trace dynamic loading

## Advanced Topics

### Custom Minimization Rules

For specialized minimization requirements, you can extend the system:

```c
// Define custom state equivalence check
bool my_state_equivalence(OkpalaState* state1, OkpalaState* state2) {
    // Custom logic to determine if states are equivalent
    // ...
    
    return are_equivalent;
}

// Use in minimization process
OkpalaAutomaton* minimized = okpala_minimize_automaton_custom(
    automaton,
    true,
    my_state_equivalence
);
```

### Continuous Integration

Incorporate NexusLink into your CI pipeline:

```yaml
# Example GitHub Actions workflow
optimize:
  runs-on: ubuntu-latest
  steps:
    - uses: actions/checkout@v2
    
    - name: Install NexusLink
      run: |
        apt-get update
        apt-get install -y nexuslink
    
    - name: Build project
      run: |
        mkdir build && cd build
        cmake ..
        make
    
    - name: Optimize components
      run: |
        cd build
        nlink minimize lib/libcomponent1.so --level 2
        nlink minimize lib/libcomponent2.so --level 2
    
    - name: Verify size reduction
      run: |
        python ../scripts/verify_size_reduction.py
```

### Memory-Mapped Components

For high-performance applications, consider memory-mapped components:

```c
#include <nexuslink/nexus_mmap.h>

// Map a component into memory
NexusMappedComponent* mapped = nexus_mmap_component(
    ctx,
    "path/to/component.so"
);

// Use the mapped component
// ...

// Unmap when done
nexus_unmap_component(ctx, mapped);
```

## Case Studies

### Web Server Size Reduction

A typical web server implementation saw a 83.6% size reduction (12.8MB to 2.1MB) by:

1. Splitting functionality into focused components
2. Using lazy loading for rarely used handlers
3. Applying aggressive minimization to state machines
4. Implementing version-aware symbol resolution

### Embedded System Optimization

An embedded system achieved 94.4% reduction (3.2MB to 180KB) through:

1. Careful state machine design
2. Component isolation with minimal dependencies
3. Boolean reduction for state transitions
4. Custom minimization rules for resource constraints

## Conclusion

NexusLink offers a powerful approach to reducing binary size and optimizing runtime behavior. By thoughtfully organizing components and leveraging the dynamic loading and minimization capabilities, you can achieve significant improvements in resource utilization while maintaining functionality.

For further assistance, consult the full documentation or contact OBINexus Computing support.

## Further Reading

- NexusLink API Reference
- Okpala's "State Machine Minimization and AST Optimization"
- "Automaton Theory for Software Engineers"
- "Component-Oriented Programming with Dynamic Linking"