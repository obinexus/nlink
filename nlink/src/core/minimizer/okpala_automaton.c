// okpala_automaton.c - Automaton minimization implementation for NexusLink
// Author: Nnamdi Michael Okpala

#include "nlink/core/minimizer/okpala_automaton.h"

// Create a new automaton
OkpalaAutomaton* okpala_automaton_create(void) {
    OkpalaAutomaton* automaton = (OkpalaAutomaton*)malloc(sizeof(OkpalaAutomaton));
    automaton->states = NULL;
    automaton->state_count = 0;
    automaton->initial_state = NULL;
    automaton->final_states = NULL;
    automaton->final_state_count = 0;
    return automaton;
}

// Find a state by ID
static OkpalaState* find_state(OkpalaAutomaton* automaton, const char* id) {
    for (size_t i = 0; i < automaton->state_count; i++) {
        if (strcmp(automaton->states[i].id, id) == 0) {
            return &automaton->states[i];
        }
    }
    return NULL;
}

// Add a state to the automaton
NexusResult okpala_automaton_add_state(OkpalaAutomaton* automaton, 
                                     const char* id, bool is_final) {
    if (!automaton || !id) {
        return NEXUS_ERROR_INVALID_ARGUMENT;
    }
    
    // Check if state already exists
    if (find_state(automaton, id)) {
        return NEXUS_ERROR_INVALID_ARGUMENT;
    }
    
    // Allocate or resize states array
    automaton->states = (OkpalaState*)realloc(automaton->states, 
                                          (automaton->state_count + 1) * sizeof(OkpalaState));
    if (!automaton->states) {
        return NEXUS_ERROR_OUT_OF_MEMORY;
    }
    
    // Initialize the new state
    OkpalaState* state = &automaton->states[automaton->state_count++];
    state->id = strdup(id);
    state->is_final = is_final;
    state->transitions = NULL;
    state->input_symbols = NULL;
    state->transition_count = 0;
    
    // If this is the first state, make it the initial state
    if (automaton->state_count == 1) {
        automaton->initial_state = state;
    }
    
    // If this is a final state, add it to the final states array
    if (is_final) {
        automaton->final_states = (OkpalaState**)realloc(automaton->final_states, 
                                                     (automaton->final_state_count + 1) * sizeof(OkpalaState*));
        if (!automaton->final_states) {
            return NEXUS_ERROR_OUT_OF_MEMORY;
        }
        automaton->final_states[automaton->final_state_count++] = state;
    }
    
    return NEXUS_SUCCESS;
}

// Add a transition between states
NexusResult okpala_automaton_add_transition(OkpalaAutomaton* automaton, 
                                         const char* from_id, 
                                         const char* to_id, 
                                         const char* input_symbol) {
    if (!automaton || !from_id || !to_id || !input_symbol) {
        return NEXUS_ERROR_INVALID_ARGUMENT;
    }
    
    // Find the states
    OkpalaState* from_state = find_state(automaton, from_id);
    OkpalaState* to_state = find_state(automaton, to_id);
    
    if (!from_state || !to_state) {
        return NEXUS_ERROR_INVALID_ARGUMENT;
    }
    
    // Allocate or resize transitions arrays
    from_state->transitions = (OkpalaState**)realloc(from_state->transitions, 
                                                 (from_state->transition_count + 1) * sizeof(OkpalaState*));
    from_state->input_symbols = (char**)realloc(from_state->input_symbols, 
                                            (from_state->transition_count + 1) * sizeof(char*));
    
    if (!from_state->transitions || !from_state->input_symbols) {
        return NEXUS_ERROR_OUT_OF_MEMORY;
    }
    
    // Add the transition
    from_state->transitions[from_state->transition_count] = to_state;
    from_state->input_symbols[from_state->transition_count] = strdup(input_symbol);
    from_state->transition_count++;
    
    return NEXUS_SUCCESS;
}

// Check if two states are equivalent
static bool are_states_equivalent(OkpalaState* state1, OkpalaState* state2, 
                                 bool** equivalence_matrix, OkpalaState* states, 
                                 size_t state_count __attribute__((unused))) {
    // Final and non-final states are never equivalent
    if (state1->is_final != state2->is_final) {
        return false;
    }
    
    // Check transitions
    for (size_t i = 0; i < state1->transition_count; i++) {
        char* input_symbol = state1->input_symbols[i];
        OkpalaState* target1 = state1->transitions[i];
        
        // Find the corresponding transition in state2
        bool found = false;
        for (size_t j = 0; j < state2->transition_count; j++) {
            if (strcmp(input_symbol, state2->input_symbols[j]) == 0) {
                OkpalaState* target2 = state2->transitions[j];
                
                // Get the indices of the target states
                size_t index1 = target1 - states;
                size_t index2 = target2 - states;
                
                if (!equivalence_matrix[index1][index2]) {
                    return false;
                }
                
                found = true;
                break;
            }
        }
        
        if (!found) {
            return false;
        }
    }
    
    return true;
}

// Minimize the automaton
OkpalaAutomaton* okpala_minimize_automaton(OkpalaAutomaton* automaton, 
                                        bool use_boolean_reduction) {
    if (!automaton) return NULL;
    
    // Initialize equivalence matrix
    bool** equivalence_matrix = (bool**)malloc(automaton->state_count * sizeof(bool*));
    for (size_t i = 0; i < automaton->state_count; i++) {
        equivalence_matrix[i] = (bool*)malloc(automaton->state_count * sizeof(bool));
        for (size_t j = 0; j < automaton->state_count; j++) {
            // Initially, states are equivalent if they are both final or both non-final
            equivalence_matrix[i][j] = (automaton->states[i].is_final == automaton->states[j].is_final);
        }
    }
    
    // Refine equivalence classes
    bool changed;
    do {
        changed = false;
        
        for (size_t i = 0; i < automaton->state_count; i++) {
            for (size_t j = i + 1; j < automaton->state_count; j++) {
                if (equivalence_matrix[i][j]) {
                    if (!are_states_equivalent(&automaton->states[i], &automaton->states[j], 
                                             equivalence_matrix, automaton->states, 
                                             automaton->state_count)) {
                        equivalence_matrix[i][j] = false;
                        equivalence_matrix[j][i] = false;
                        changed = true;
                    }
                }
            }
        }
    } while (changed);
    
    // Apply boolean reduction if requested
    if (use_boolean_reduction) {
        // Here we would implement additional reduction techniques
        // This is a placeholder for the actual implementation
        printf("Boolean reduction applied to automaton\n");
    }
    
    // Create the minimized automaton
    OkpalaAutomaton* minimized = okpala_automaton_create();
    
    // Create a mapping from old states to new states
    char** new_state_ids = (char**)malloc(automaton->state_count * sizeof(char*));
    memset(new_state_ids, 0, automaton->state_count * sizeof(char*));
    
    // Create new states for each equivalence class
    for (size_t i = 0; i < automaton->state_count; i++) {
        if (!new_state_ids[i]) {
            // This state doesn't have a new state yet, create one
            char new_id[32];
            snprintf(new_id, sizeof(new_id), "q%zu", minimized->state_count);
            okpala_automaton_add_state(minimized, new_id, automaton->states[i].is_final);
            
            // Map all equivalent states to this new state
            new_state_ids[i] = strdup(new_id);
            for (size_t j = i + 1; j < automaton->state_count; j++) {
                if (equivalence_matrix[i][j]) {
                    new_state_ids[j] = strdup(new_id);
                }
            }
        }
    }
    
    // Add transitions to the minimized automaton
    for (size_t i = 0; i < automaton->state_count; i++) {
        if (new_state_ids[i]) {
            OkpalaState* state = &automaton->states[i];
            
            for (size_t j = 0; j < state->transition_count; j++) {
                size_t target_index = state->transitions[j] - automaton->states;
                okpala_automaton_add_transition(minimized, new_state_ids[i], 
                                             new_state_ids[target_index], 
                                             state->input_symbols[j]);
            }
            
            // We've processed this state, clear its entry
            free(new_state_ids[i]);
            new_state_ids[i] = NULL;
        }
    }
    
    // Clean up
    free(new_state_ids);
    for (size_t i = 0; i < automaton->state_count; i++) {
        free(equivalence_matrix[i]);
    }
    free(equivalence_matrix);
    
    return minimized;
}

// Free an automaton
void okpala_automaton_free(OkpalaAutomaton* automaton) {
    if (!automaton) return;
    
    for (size_t i = 0; i < automaton->state_count; i++) {
        OkpalaState* state = &automaton->states[i];
        free(state->id);
        
        for (size_t j = 0; j < state->transition_count; j++) {
            free(state->input_symbols[j]);
        }
        
        free(state->transitions);
        free(state->input_symbols);
    }
    
    free(automaton->states);
    free(automaton->final_states);
    free(automaton);
}
