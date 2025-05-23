# NexusLink Migration Plan

## Overview

This document outlines the detailed migration path for transforming the current `nlink` codebase to the refactored `nexuslink_refactored` structure. It provides a systematic approach to file migration, include path standardization, and dependency management to ensure a clean transition with minimal disruption to functionality.

## Component Migration Strategy

Migration will proceed in component-by-component phases to allow for incremental validation. The migration order has been optimized to address dependencies from the bottom up:

1. Core Common (foundation layer)
2. Symbols and Versioning (base components)
3. Metadata (dependent on Common)
4. Automaton and Minimizer (specialized components)
5. CLI Commands (depends on all core components)
6. Main Integration points (unified interfaces)

## File Migration Mapping

### Phase 2.1: Core Common Component

| Source File | Destination File | Include Path Updates |
|-------------|------------------|----------------------|
| `nlink/src/core/common/nexus_core.c` | `nexuslink_refactored/src/core/common/nexus_core.c` | Update includes from `"nlink/core/common/*.h"` to `"nexuslink/core/common/*.h"` |
| `nlink/src/core/common/nexus_loader.c` | `nexuslink_refactored/src/core/common/nexus_loader.c` | Update includes from `"nlink/core/common/*.h"` to `"nexuslink/core/common/*.h"` |
| `nlink/src/core/common/result.c` | `nexuslink_refactored/src/core/common/result.c` | Update includes from `"nlink/core/common/*.h"` to `"nexuslink/core/common/*.h"` |
| `nlink/src/core/common/types.c` | `nexuslink_refactored/src/core/common/types.c` | Update includes from `"nlink/core/common/*.h"` to `"nexuslink/core/common/*.h"` |
| `nlink/include/nlink/core/common/forward_decl.h` | `nexuslink_refactored/include/nexuslink/core/common/forward_decl.h` | Update include guards and internal references |
| `nlink/include/nlink/core/common/json.h` | `nexuslink_refactored/include/nexuslink/core/common/json.h` | Update include guards and internal references |
| `nlink/include/nlink/core/common/lazy.h` | `nexuslink_refactored/include/nexuslink/core/common/lazy.h` | Update include guards and internal references |
| `nlink/include/nlink/core/common/lazy_legacy.h` | `nexuslink_refactored/include/nexuslink/core/common/lazy_legacy.h` | Update include guards and internal references |
| `nlink/include/nlink/core/common/lazy_versioned.h` | `nexuslink_refactored/include/nexuslink/core/common/lazy_versioned.h` | Update include guards and internal references |
| `nlink/include/nlink/core/common/nexus_core.h` | `nexuslink_refactored/include/nexuslink/core/common/nexus_core.h` | Update include guards and internal references |
| `nlink/include/nlink/core/common/nexus_json.h` | `nexuslink_refactored/include/nexuslink/core/common/nexus_json.h` | Update include guards and internal references |
| `nlink/include/nlink/core/common/nexus_lazy_versioned.h` | `nexuslink_refactored/include/nexuslink/core/common/nexus_lazy_versioned.h` | Update include guards and internal references |
| `nlink/include/nlink/core/common/nexus_loader.h` | `nexuslink_refactored/include/nexuslink/core/common/nexus_loader.h` | Update include guards and internal references |
| `nlink/include/nlink/core/common/result.h` | `nexuslink_refactored/include/nexuslink/core/common/result.h` | Update include guards and internal references |
| `nlink/include/nlink/core/common/types.h` | `nexuslink_refactored/include/nexuslink/core/common/types.h` | Update include guards and internal references |

### Phase 2.2: Symbols Component

| Source File | Destination File | Include Path Updates |
|-------------|------------------|----------------------|
| `nlink/src/core/symbols/cold_symbol.c` | `nexuslink_refactored/src/core/symbols/cold_symbol.c` | Update includes, reference new common paths |
| `nlink/src/core/symbols/nexus_symbols.c` | `nexuslink_refactored/src/core/symbols/nexus_symbols.c` | Update includes, reference new common paths |
| `nlink/src/core/symbols/versioned_symbols.c` | `nexuslink_refactored/src/core/symbols/versioned_symbols.c` | Update includes, reference new common paths |
| `nlink/include/nlink/core/symbols/nexus_symbols.h` | `nexuslink_refactored/include/nexuslink/core/symbols/nexus_symbols.h` | Update include guards and internal references |
| `nlink/include/nlink/core/symbols/nexus_versioned_symbols.h` | `nexuslink_refactored/include/nexuslink/core/symbols/nexus_versioned_symbols.h` | Update include guards and internal references |
| `nlink/include/nlink/core/symbols/registry.h` | `nexuslink_refactored/include/nexuslink/core/symbols/registry.h` | Update include guards and internal references |
| `nlink/include/nlink/core/symbols/symbols.h` | `nexuslink_refactored/include/nexuslink/core/symbols/symbols.h` | Update include guards and internal references |

### Phase 2.3: Versioning Component

| Source File | Destination File | Include Path Updates |
|-------------|------------------|----------------------|
| `nlink/src/core/versioning/lazy_versioned.c` | `nexuslink_refactored/src/core/versioning/lazy_versioned.c` | Update includes, reference new common and symbols paths |
| `nlink/src/core/versioning/semver.c` | `nexuslink_refactored/src/core/versioning/semver.c` | Update includes, reference new common paths |
| `nlink/include/nlink/core/versioning/nexus_version.h` | `nexuslink_refactored/include/nexuslink/core/versioning/nexus_version.h` | Update include guards and internal references |
| `nlink/include/nlink/core/versioning/semver.h` | `nexuslink_refactored/include/nexuslink/core/versioning/semver.h` | Update include guards and internal references |
| `nlink/include/nlink/core/versioning/versioned_symbols.h` | `nexuslink_refactored/include/nexuslink/core/versioning/versioned_symbols.h` | Update include guards and internal references |

### Phase 2.4: Metadata Component

| Source File | Destination File | Include Path Updates |
|-------------|------------------|----------------------|
| `nlink/src/core/metadata/enhanced_metadata.c` | `nexuslink_refactored/src/core/metadata/enhanced_metadata.c` | Update includes, reference new common paths |
| `nlink/include/nlink/core/metadata/enhanced_metadata.h` | `nexuslink_refactored/include/nexuslink/core/metadata/enhanced_metadata.h` | Update include guards and internal references |
| `nlink/include/nlink/core/metadata/metadata.h` | `nexuslink_refactored/include/nexuslink/core/metadata/metadata.h` | Update include guards and internal references |

### Phase 2.5: Automaton and Minimizer Components

| Source File | Destination File | Include Path Updates |
|-------------|------------------|----------------------|
| `nlink/src/core/minimizer/minimizer.c` | `nexuslink_refactored/src/core/minimizer/minimizer.c` | Update includes, reference new common and automaton paths |
| `nlink/src/core/minimizer/nexus_automaton.c` | `nexuslink_refactored/src/core/automaton/nexus_automaton.c` | Update includes, reference new common paths |
| `nlink/src/core/minimizer/okpala_ast.c` | `nexuslink_refactored/src/core/minimizer/okpala_ast.c` | Update includes, reference new common paths |
| `nlink/src/core/minimizer/okpala_automaton.c` | `nexuslink_refactored/src/core/automaton/okpala_automaton.c` | Update includes, reference new common paths |
| `nlink/include/nlink/core/minimizer/automaton.h` | `nexuslink_refactored/include/nexuslink/core/automaton/automaton.h` | Update include guards and internal references |
| `nlink/include/nlink/core/minimizer/nexus_minimizer.h` | `nexuslink_refactored/include/nexuslink/core/minimizer/nexus_minimizer.h` | Update include guards and internal references |
| `nlink/include/nlink/core/minimizer/okpala_ast.h` | `nexuslink_refactored/include/nexuslink/core/minimizer/okpala_ast.h` | Update include guards and internal references |
| `nlink/include/nlink/core/minimizer/okpala_automaton.h` | `nexuslink_refactored/include/nexuslink/core/automaton/okpala_automaton.h` | Update include guards and internal references |
| `nlink/include/nlink/core/minimizer/okpala_minimizer.h` | `nexuslink_refactored/include/nexuslink/core/minimizer/okpala_minimizer.h` | Update include guards and internal references |

### Phase 2.6: CLI Component and Commands

| Source File | Destination File | Include Path Updates |
|-------------|------------------|----------------------|
| `nlink/src/cli/cli.c` | `nexuslink_refactored/src/cli/cli.c` | Update includes, reference new core paths |
| `nlink/src/cli/command_registry.c` | `nexuslink_refactored/src/cli/command_registry.c` | Update includes, reference new core paths |
| `nlink/src/cli/commands/load.c` | `nexuslink_refactored/src/cli/commands/load.c` | Update includes, reference new core paths |
| `nlink/src/cli/commands/minimal.c` | `nexuslink_refactored/src/cli/commands/minimal.c` | Update includes, reference new core paths |
| `nlink/src/cli/commands/minimize.c` | `nexuslink_refactored/src/cli/commands/minimize.c` | Update includes, reference new core and minimizer paths |
| `nlink/src/cli/commands/version.c` | `nexuslink_refactored/src/cli/commands/version.c` | Update includes, reference new core paths |
| `nlink/src/cli/main.c` | `nexuslink_refactored/src/cli/main.c` | Update includes, reference new paths |
| `nlink/src/cli/nlink_cli.c` | `nexuslink_refactored/src/cli/nexuslink_cli.c` | Update includes, rename file to match new naming convention |
| `nlink/src/cli/cli.h` | `nexuslink_refactored/include/nexuslink/cli/cli.h` | Move to public include dir, update include guards and internal references |
| `nlink/src/cli/commands/command.h` | `nexuslink_refactored/include/nexuslink/cli/command.h` | Move to public include dir, update include guards and internal references |
| `nlink/src/cli/commands/load.h` | `nexuslink_refactored/include/nexuslink/cli/load.h` | Move to public include dir, update include guards and internal references |
| `nlink/src/cli/commands/minimal.h` | `nexuslink_refactored/include/nexuslink/cli/minimal.h` | Move to public include dir, update include guards and internal references |
| `nlink/src/cli/commands/minimize.h` | `nexuslink_refactored/include/nexuslink/cli/minimize.h` | Move to public include dir, update include guards and internal references |
| `nlink/src/cli/commands/version.h` | `nexuslink_refactored/include/nexuslink/cli/version.h` | Move to public include dir, update include guards and internal references |
| `nlink/src/cli/nlink_cli.h` | `nexuslink_refactored/include/nexuslink/cli/nexuslink_cli.h` | Move to public include dir, update include guards and internal references |

### Phase 2.7: Main Integration

| Source File | Destination File | Include Path Updates |
|-------------|------------------|----------------------|
| `nlink/src/core/nlink.c` | `nexuslink_refactored/src/nexuslink.c` | Update all includes, reference new core component paths |
| `nlink/include/nlink/nlink.h` | `nexuslink_refactored/include/nexuslink/nexuslink.h` | Update include guards, create umbrella header that includes all public component APIs |
| `nlink/src/core/schema/nlink_schema.json` | `nexuslink_refactored/include/nexuslink/core/schema/nexuslink_schema.json` | Rename to match new naming convention |

## Include Path Standardization

All include paths will be updated according to the following standard:

### Public API Includes

```c
// Core component includes
#include "nexuslink/core/common/result.h"
#include "nexuslink/core/symbols/nexus_symbols.h"
#include "nexuslink/core/versioning/semver.h"
#include "nexuslink/core/minimizer/nexus_minimizer.h"
#include "nexuslink/core/automaton/automaton.h"

// CLI includes
#include "nexuslink/cli/command.h"
#include "nexuslink/cli/nexuslink_cli.h"

// Main includes
#include "nexuslink/nexuslink.h"
```

### Include Guards

Include guards will be updated to match the path convention:

```c
// Before
#ifndef NLINK_CORE_SYMBOLS_H
#define NLINK_CORE_SYMBOLS_H
// ...
#endif /* NLINK_CORE_SYMBOLS_H */

// After
#ifndef NEXUSLINK_CORE_SYMBOLS_NEXUS_SYMBOLS_H
#define NEXUSLINK_CORE_SYMBOLS_NEXUS_SYMBOLS_H
// ...
#endif /* NEXUSLINK_CORE_SYMBOLS_NEXUS_SYMBOLS_H */
```

## Internal Headers Strategy

Certain implementation-specific headers will be moved to the internal include directory:

| Original Header | Internal Header |
|-----------------|-----------------|
| `nlink/src/cli/command_registry.h` | `nexuslink_refactored/include/internal/cli/command_registry.h` |
| Implementation-specific headers | `nexuslink_refactored/include/internal/core/*/internal_*.h` |

## Build System Migration

### CMakeLists.txt Updates

Each component directory will have its own CMakeLists.txt file that:

1. Defines component-specific sources
2. Establishes include directories
3. Defines component-specific dependencies
4. Sets up installation rules

Example for the symbols component:

```cmake
# Symbols component build configuration
set(SYMBOLS_SOURCES
    nexus_symbols.c
    cold_symbol.c
    versioned_symbols.c
)

add_library(nexuslink_symbols SHARED ${SYMBOLS_SOURCES})

target_include_directories(nexuslink_symbols
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include/internal
)

target_link_libraries(nexuslink_symbols
    PUBLIC nexuslink_common
)

install(TARGETS nexuslink_symbols
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
```

## Migration Tools and Scripts

The following scripts will be implemented to assist with the migration process:

1. `migrate_file.py`: Copies a file from source to destination while updating include paths
2. `update_include_paths.py`: Updates include paths in a single file according to standards
3. `validate_migration.py`: Validates that all files have been migrated correctly
4. `generate_include_report.py`: Generates a report of include path compliance

## Testing Strategy

Each phase of the migration will be validated through:

1. **Compilation Checks**: Ensure the component compiles correctly
2. **Unit Tests**: Validate component functionality with existing tests
3. **Include Path Validation**: Verify all include paths follow the standard
4. **Dependency Checks**: Ensure all required dependencies are properly resolved

## Migration Timeline

| Phase | Component | Estimated Duration | Dependencies |
|-------|-----------|---------------------|-------------|
| 2.1 | Core Common | 2 days | None |
| 2.2 | Symbols | 1 day | Core Common |
| 2.3 | Versioning | 1 day | Core Common, Symbols |
| 2.4 | Metadata | 1 day | Core Common |
| 2.5 | Automaton/Minimizer | 2 days | Core Common |
| 2.6 | CLI | 2 days | All Core Components |
| 2.7 | Main Integration | 1 day | All Components |

Total Estimated Duration: 10 working days

## Risk Assessment and Mitigation

| Risk | Probability | Impact | Mitigation Strategy |
|------|------------|--------|---------------------|
| Include path inconsistencies | High | Medium | Automated validation and fixing tools |
| Build system integration issues | Medium | High | Incremental component validation |
| Circular dependencies | Medium | High | Dependency graph analysis, forward declarations |
| API compatibility breaks | Low | Critical | Thorough interface testing before and after migration |
| Missing files | Low | Medium | Automated validation to ensure all files are accounted for |

## Validation Criteria for Phase 2 Completion

- All source files successfully migrated to the new structure
- All include paths updated to follow the standardized format
- Complete build system configuration for all components
- Successful compilation of the entire codebase
- Passing of all existing tests
- No regression in functionality

## Next Steps for Phase 3

Upon successful completion of the file migration phase, we will proceed to:

1. API harmonization to ensure consistent naming and parameter conventions
2. Component interface documentation updates
3. Build system optimization for better dependency tracking
4. Implementation of additional validation tools