// okpala_minimizer.h - State machine minimization for NexusLink
// Author: Nnamdi Michael Okpala
#ifndef OKPALA_MINIMIZER_H
#define OKPALA_MINIMIZER_H

#include "nlink/core/common/types.h"
#include "nlink/core/common/result.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

/* Type definitions */
typedef struct OkpalaAutomaton {
    void* internal;  // Opaque pointer to internal implementation
} OkpalaAutomaton;

typedef struct OkpalaNode {
    void* internal;  // Opaque pointer to internal implementation
} OkpalaNode;

typedef struct OkpalaAST {
    void* internal;  // Opaque pointer to internal implementation
} OkpalaAST;
/* Function declarations */
OkpalaAutomaton* okpala_automaton_create(void);

NexusResult okpala_automaton_add_state(OkpalaAutomaton* automaton, 
                                     const char* id, bool is_final);

NexusResult okpala_automaton_add_transition(OkpalaAutomaton* automaton, 
                                         const char* from_id, 
                                         const char* to_id, 
                                         const char* input_symbol);

OkpalaAutomaton* okpala_minimize_automaton(OkpalaAutomaton* automaton, 
                                        bool use_boolean_reduction);

void okpala_automaton_free(OkpalaAutomaton* automaton);

OkpalaAST* okpala_ast_create(void);

NexusResult okpala_ast_add_node(OkpalaAST* ast, OkpalaNode* parent, 
                              const char* value);

OkpalaAST* okpala_optimize_ast(OkpalaAST* ast, bool use_boolean_reduction);

void okpala_ast_free(OkpalaAST* ast);

/* Initialization function for automaton subsystem */
void nexus_automaton_initialize(void);

#endif // OKPALA_MINIMIZER_H