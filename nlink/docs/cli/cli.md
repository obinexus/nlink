# NexusLink Minimizer

## Overview

The NexusLink Minimizer is a state machine optimization system based on Nnamdi Okpala's automaton minimization algorithm. It reduces the size and improves the performance of components by:

1. Identifying and merging equivalent states in the component's state machine
2. Optimizing the abstract syntax tree (AST) representation
3. Applying boolean reduction for advanced optimizations

## Usage

### Command Line Interface

The minimizer can be accessed through the `minimize` command in the NexusLink CLI:

```bash
# Basic usage with default settings
nlink minimize mycomponent.so

# Aggressive minimization with boolean reduction
nlink minimize mycomponent.so --level 3

# Verbose output with detailed metrics
nlink minimize --verbose mycomponent.so

# Save minimized component to a new file
nlink minimize mycomponent.so --output mycomponent.min.so
```

#### Command Options

- `--level LEVEL`: Set minimization level (1=basic, 2=standard, 3=aggressive)
- `--verbose`: Enable verbose output
- `--no-metrics`: Disable metrics collection
- `--output FILE`: Save minimized component to FILE

### Programmatic API

The minimizer can also be used programmatically through the C API:

```c
#include "nlink/core/minimizer/nexus_minimizer.h"

// Initialize the minimizer
nexus_minimizer_initialize(ctx);

// Create default configuration
NexusMinimizerConfig config = nexus_minimizer_default_config();

// Customize configuration if needed
config.level = NEXUS_MINIMIZE_AGGRESSIVE;
config.verbose = true;

// Allocate metrics structure to receive results
NexusMinimizationMetrics metrics;

// Perform minimization
NexusResult result = nexus_minimize_component(
    ctx,
    "mycomponent.so",
    config,
    &metrics
);

// Print metrics
if (result == NEXUS_SUCCESS) {
    nexus_print_minimization_metrics(&metrics);
}

// Clean up
nexus_minimizer_cleanup(ctx);
```

## Minimization Levels

The minimizer offers three levels of optimization:

1. **Basic (Level 1)**: Performs standard state machine minimization by merging equivalent states. This is the safest option with minimal risk of altering behavior.

2. **Standard (Level 2)**: Applies state machine minimization and basic AST optimizations. This is the default level, offering a good balance between size reduction and safety.

3. **Aggressive (Level 3)**: Enables boolean reduction and advanced optimizations. This level provides the maximum size reduction but may be more aggressive in transforming the component structure.

## Technical Details

### Automaton-Based Minimization

The automaton minimization algorithm identifies states that are behaviorally equivalent. Two states are considered equivalent if:

1. They have the same acceptance status (both accepting or both non-accepting)
2. For every input symbol, they transition to equivalent states

The algorithm uses a partitioning approach that initially groups states by their acceptance status, then iteratively refines these partitions until no further refinements are possible.

### AST Optimization

The Abstract Syntax Tree optimization:

1. Removes redundant nodes (e.g., nodes with a single child and no meaningful value)
2. Merges identical subtrees
3. Simplifies paths in the AST to reduce traversal overhead

### Boolean Reduction

Boolean reduction applies logical minimization techniques to simplify the component's state machine. This includes:

1. Identifying and merging states with identical transitions
2. Eliminating unreachable states
3. Applying boolean algebra to simplify transition conditions

## Performance Considerations

- Minimization is typically done during the build process, not at runtime
- Memory usage is proportional to the square of the number of states in the automaton
- Processing time increases with the complexity of the component

## Examples

### Example 1: Basic Component Minimization

```bash
$ nlink minimize simple_component.so
Minimizing component: simple_component.so
Minimization level: 2
Component minimization completed successfully

Minimization Results:
  State reduction: 24 → 12 (50.0%)
  Size reduction: 12.50 KB → 7.20 KB (42.4%)
  Processing time: 35.42 ms
  Boolean reduction: disabled
```

### Example 2: Aggressive Minimization with Boolean Reduction

```bash
$ nlink minimize complex_component.so --level 3 --verbose
Minimizing component: complex_component.so
Minimization level: 3
Performing automaton minimization (boolean reduction: enabled)
Component minimization completed successfully

Minimization Results:
  State reduction: 156 → 42 (73.1%)
  Size reduction: 45.80 KB → 18.30 KB (60.0%)
  Processing time: 128.75 ms
  Boolean reduction: enabled
```

## Limitations

- The minimizer cannot guarantee identical behavior in all edge cases
- Components with side effects may behave differently after minimization
- Some optimizations may increase memory usage to reduce code size

## References

1. Hopcroft, J. E. (1971). "An n log n algorithm for minimizing states in a finite automaton"
2. Okpala, N. M. (2025). "State Machine Minimization and AST Optimization"
3. NexusLink Programmers Manual, Section 4.2: "Component Optimization"