// okpala_minimizer.h - State machine minimization for NexusLink
// Author: Nnamdi Michael Okpala
#ifndef OKPALA_MINIMIZER_H
#define OKPALA_MINIMIZER_H

#include "nexus/core.h"

// State structure
typedef struct OkpalaState {
    char* id;
    bool is_final;
    struct OkpalaState** transitions;
    char** input_symbols;
    size_t transition_count;
} OkpalaState;

// Automaton structure
typedef struct {
    OkpalaState* states;
    size_t state_count;
    OkpalaState* initial_state;
    OkpalaState** final_states;
    size_t final_state_count;
} OkpalaAutomaton;

// AST node structure
typedef struct OkpalaNode {
    char* value;
    struct OkpalaNode** children;
    size_t child_count;
    struct OkpalaNode* parent;
} OkpalaNode;

// AST structure
typedef struct {
    OkpalaNode* root;
    size_t node_count;
} OkpalaAST;

// Create a new automaton
OkpalaAutomaton* okpala_automaton_create(void);

// Add a state to the automaton
NexusResult okpala_automaton_add_state(OkpalaAutomaton* automaton, 
                                     const char* id, bool is_final);

// Add a transition between states
NexusResult okpala_automaton_add_transition(OkpalaAutomaton* automaton, 
                                         const char* from_id, 
                                         const char* to_id, 
                                         const char* input_symbol);

// Minimize the automaton
OkpalaAutomaton* okpala_minimize_automaton(OkpalaAutomaton* automaton, 
                                        bool use_boolean_reduction);

// Create a new AST
OkpalaAST* okpala_ast_create(void);

// Add a node to the AST
NexusResult okpala_ast_add_node(OkpalaAST* ast, OkpalaNode* parent, 
                              const char* value);

// Optimize the AST
OkpalaAST* okpala_optimize_ast(OkpalaAST* ast, bool use_boolean_reduction);

// Free an automaton
void okpala_automaton_free(OkpalaAutomaton* automaton);

// Free an AST
void okpala_ast_free(OkpalaAST* ast);

#endif // OKPALA_MINIMIZER_H
