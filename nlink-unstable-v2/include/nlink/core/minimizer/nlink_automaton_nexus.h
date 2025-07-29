// nlink/core/minimizer/nlink_automaton_nexus.h
// OBINexus Constitutional Header Guard: OBINEXUS_NLINK_CORE_MINIMIZER_NLINK_AUTOMATON_NEXUS_H
#ifndef OBINEXUS_NLINK_CORE_MINIMIZER_NLINK_AUTOMATON_NEXUS_H
#define OBINEXUS_NLINK_CORE_MINIMIZER_NLINK_AUTOMATON_NEXUS_H

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


#endif // OBINEXUS_NLINK_CORE_MINIMIZER_NLINK_AUTOMATON_NEXUS_H
