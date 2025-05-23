#ifndef NLINK_CORE_MINIMIZER_NEXUS_AUTOMATON_H
#define NLINK_CORE_MINIMIZER_NEXUS_AUTOMATON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct NexusAutomaton NexusAutomaton;

/**
 * @brief Initialize the automaton minimizer
 */
void nexus_automaton_initialize(void);

// Add other function declarations with nexus_ prefix here

#ifdef __cplusplus
}
#endif

#endif /* NLINK_CORE_MINIMIZER_NEXUS_AUTOMATON_H */
