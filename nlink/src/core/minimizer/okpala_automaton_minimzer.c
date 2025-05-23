/**
 * @file okpala_automaton_minimizer.c
 * @brief Implementation of the Okpala Automaton minimization algorithm
 * 
 * This file implements the automaton minimization algorithm based on
 * Nnamdi Michael Okpala's state machine minimization technique.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/core/minimizer/okpala_automaton.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 
 /**
  * @brief Check if two states are equivalent
  * 
  * Two states are equivalent if:
  * 1. They are both final states or both non-final states
  * 2. For each input symbol, they transition to equivalent states
  * 
  * @param automaton The automaton containing the states
  * @param state1_idx Index of first state
  * @param state2_idx Index of second state
  * @param equivalence_matrix Matrix of current equivalence relations
  * @return true if states are equivalent, false otherwise
  */
 static bool are_states_equivalent(OkpalaAutomaton* automaton, 
                                  size_t state1_idx, 
                                  size_t state2_idx,
                                  bool** equivalence_matrix) {
     if (state1_idx == state2_idx) {
         return true;  // A state is always equivalent to itself
     }
     
     OkpalaState* state1 = &automaton->states[state1_idx];
     OkpalaState* state2 = &automaton->states[state2_idx];
     
     // States with different acceptance status cannot be equivalent
     if (state1->is_final != state2->is_final) {
         return false;
     }
     
     // Check transitions
     for (size_t i = 0; i < state1->transition_count; i++) {
         char* symbol1 = state1->input_symbols[i];
         OkpalaState* target1 = state1->transitions[i];
         size_t target1_idx = target1 - automaton->states;
         
         // Find matching transition in state2
         bool found_matching_transition = false;
         
         for (size_t j = 0; j < state2->transition_count; j++) {
             char* symbol2 = state2->input_symbols[j];
             
             // If symbols match, check if target states are equivalent
             if (strcmp(symbol1, symbol2) == 0) {
                 OkpalaState* target2 = state2->transitions[j];
                 size_t target2_idx = target2 - automaton->states;
                 
                 if (!equivalence_matrix[target1_idx][target2_idx]) {
                     return false;  // Target states are not equivalent
                 }
                 
                 found_matching_transition = true;
                 break;
             }
         }
         
         if (!found_matching_transition) {
             return false;  // No matching transition found
         }
     }
     
     // Check state2's transitions that are not in state1
     for (size_t j = 0; j < state2->transition_count; j++) {
         char* symbol2 = state2->input_symbols[j];
         
         // Find matching transition in state1
         bool found_matching_transition = false;
         
         for (size_t i = 0; i < state1->transition_count; i++) {
             char* symbol1 = state1->input_symbols[i];
             
             if (strcmp(symbol2, symbol1) == 0) {
                 found_matching_transition = true;
                 break;
             }
         }
         
         if (!found_matching_transition) {
             return false;  // No matching transition found
         }
     }
     
     return true;  // All checks passed, states are equivalent
 }
 
 /**
  * @brief Apply boolean reduction to further minimize the automaton
  * 
  * This function implements advanced reduction techniques based on boolean algebra
  * to further minimize the automaton.
  * 
  * @param automaton The automaton to reduce
  * @return NexusResult result code (NEXUS_SUCCESS on success)
  */
 static NexusResult apply_boolean_reduction(OkpalaAutomaton* automaton) {
     if (!automaton) {
         return NEXUS_ERROR_INVALID_ARGUMENT;
     }
     
     // Implementation of Okpala's boolean reduction algorithm
     // This is a simplified version that focuses on redundant path elimination
     
     // Look for redundant paths (multiple paths from A to B with same result)
     for (size_t i = 0; i < automaton->state_count; i++) {
         OkpalaState* state = &automaton->states[i];
         
         // Create a map of destination states and their input symbols
         struct {
             OkpalaState* target;
             char** symbols;
             size_t symbol_count;
         } *target_map = NULL;
         size_t target_count = 0;
         
         // Identify transitions to the same target state
         for (size_t j = 0; j < state->transition_count; j++) {
             OkpalaState* target = state->transitions[j];
             char* symbol = state->input_symbols[j];
             
             // Check if target is already in the map
             size_t target_idx = SIZE_MAX;
             for (size_t k = 0; k < target_count; k++) {
                 if (target_map[k].target == target) {
                     target_idx = k;
                     break;
                 }
             }
             
             if (target_idx == SIZE_MAX) {
                 // Add new target to map
                 target_map = realloc(target_map, (target_count + 1) * sizeof(*target_map));
                 if (!target_map) {
                     return NEXUS_ERROR_OUT_OF_MEMORY;
                 }
                 
                 target_idx = target_count++;
                 target_map[target_idx].target = target;
                 target_map[target_idx].symbols = NULL;
                 target_map[target_idx].symbol_count = 0;
             }
             
             // Add symbol to target's list
             target_map[target_idx].symbols = realloc(target_map[target_idx].symbols, 
                                                     (target_map[target_idx].symbol_count + 1) * sizeof(char*));
             if (!target_map[target_idx].symbols) {
                 // Clean up
                 for (size_t k = 0; k < target_count; k++) {
                     free(target_map[k].symbols);
                 }
                 free(target_map);
                 return NEXUS_ERROR_OUT_OF_MEMORY;
             }
             
             target_map[target_idx].symbols[target_map[target_idx].symbol_count++] = symbol;
         }
         
         // If there are targets with multiple symbols, we can optimize
         // by creating a combined transition pattern
         // This would require modifying the automaton structure to support
         // regular expressions as transition symbols
         
         // For now, we just log the potential optimizations
         #ifdef NEXUS_DEBUG
         for (size_t j = 0; j < target_count; j++) {
             if (target_map[j].symbol_count > 1) {
                 printf("Boolean reduction opportunity: State %s has %zu transitions to the same target\n",
                        state->id, target_map[j].symbol_count);
             }
         }
         #endif
         
         // Clean up
         for (size_t j = 0; j < target_count; j++) {
             free(target_map[j].symbols);
         }
         free(target_map);
     }
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * @brief Create a new state in the minimized automaton
  * 
  * @param minimized The minimized automaton
  * @param state_id ID for the new state
  * @param is_final Whether the state is a final state
  * @return NexusResult result code (NEXUS_SUCCESS on success)
  */
 static NexusResult create_minimized_state(OkpalaAutomaton* minimized, 
                                         const char* state_id, 
                                         bool is_final) {
     return okpala_automaton_add_state(minimized, state_id, is_final);
 }
 
 /**
  * @brief Minimize an automaton using Okpala's state machine minimization algorithm
  * 
  * This function creates a new minimized automaton based on the input automaton.
  * The original automaton is not modified.
  * 
  * @param automaton The automaton to minimize
  * @param use_boolean_reduction Whether to use boolean reduction for further optimization
  * @return A new minimized automaton, or NULL if minimization failed
  */
 OkpalaAutomaton* okpala_minimize_automaton(OkpalaAutomaton* automaton, 
                                         bool use_boolean_reduction) {
     if (!automaton || automaton->state_count == 0) {
         return NULL;
     }
     
     // Create equivalence matrix
     // equivalence_matrix[i][j] is true if states i and j are equivalent
     bool** equivalence_matrix = (bool**)malloc(automaton->state_count * sizeof(bool*));
     if (!equivalence_matrix) {
         return NULL;
     }
     
     for (size_t i = 0; i < automaton->state_count; i++) {
         equivalence_matrix[i] = (bool*)malloc(automaton->state_count * sizeof(bool));
         if (!equivalence_matrix[i]) {
             // Clean up previously allocated rows
             for (size_t j = 0; j < i; j++) {
                 free(equivalence_matrix[j]);
             }
             free(equivalence_matrix);
             return NULL;
         }
         
         // Initialize: states are equivalent if they have the same final status
         for (size_t j = 0; j < automaton->state_count; j++) {
             equivalence_matrix[i][j] = (automaton->states[i].is_final == automaton->states[j].is_final);
         }
     }
     
     // Refine equivalence classes until no more refinements are possible
     bool changed;
     do {
         changed = false;
         
         for (size_t i = 0; i < automaton->state_count; i++) {
             for (size_t j = i + 1; j < automaton->state_count; j++) {
                 if (equivalence_matrix[i][j]) {
                     if (!are_states_equivalent(automaton, i, j, equivalence_matrix)) {
                         equivalence_matrix[i][j] = false;
                         equivalence_matrix[j][i] = false;
                         changed = true;
                     }
                 }
             }
         }
     } while (changed);
     
     // Create equivalence classes
     // Each class contains states that are equivalent to each other
     size_t* class_of_state = (size_t*)malloc(automaton->state_count * sizeof(size_t));
     if (!class_of_state) {
         // Clean up
         for (size_t i = 0; i < automaton->state_count; i++) {
             free(equivalence_matrix[i]);
         }
         free(equivalence_matrix);
         return NULL;
     }
     
     // Initialize each state to its own class
     for (size_t i = 0; i < automaton->state_count; i++) {
         class_of_state[i] = i;
     }
     
     // Group equivalent states into classes
     for (size_t i = 0; i < automaton->state_count; i++) {
         for (size_t j = i + 1; j < automaton->state_count; j++) {
             if (equivalence_matrix[i][j]) {
                 // States i and j are equivalent, put them in the same class
                 size_t j_class = class_of_state[j];
                 
                 // Update all states in j's class to be in i's class
                 for (size_t k = 0; k < automaton->state_count; k++) {
                     if (class_of_state[k] == j_class) {
                         class_of_state[k] = class_of_state[i];
                     }
                 }
             }
         }
     }
     
     // Count unique classes
     size_t class_count = 0;
     size_t* class_ids = (size_t*)malloc(automaton->state_count * sizeof(size_t));
     if (!class_ids) {
         // Clean up
         free(class_of_state);
         for (size_t i = 0; i < automaton->state_count; i++) {
             free(equivalence_matrix[i]);
         }
         free(equivalence_matrix);
         return NULL;
     }
     
     for (size_t i = 0; i < automaton->state_count; i++) {
         bool found = false;
         for (size_t j = 0; j < class_count; j++) {
             if (class_ids[j] == class_of_state[i]) {
                 found = true;
                 break;
             }
         }
         
         if (!found) {
             class_ids[class_count++] = class_of_state[i];
         }
     }
     
     // Create minimized automaton
     OkpalaAutomaton* minimized = okpala_automaton_create();
     if (!minimized) {
         // Clean up
         free(class_ids);
         free(class_of_state);
         for (size_t i = 0; i < automaton->state_count; i++) {
             free(equivalence_matrix[i]);
         }
         free(equivalence_matrix);
         return NULL;
     }
     
     // Create states in minimized automaton
     char** class_state_ids = (char**)malloc(class_count * sizeof(char*));
     if (!class_state_ids) {
         // Clean up
         okpala_automaton_free(minimized);
         free(class_ids);
         free(class_of_state);
         for (size_t i = 0; i < automaton->state_count; i++) {
             free(equivalence_matrix[i]);
         }
         free(equivalence_matrix);
         return NULL;
     }
     
     for (size_t i = 0; i < class_count; i++) {
         // Find a representative state for this class
         size_t rep_state_idx = 0;
         while (class_of_state[rep_state_idx] != class_ids[i]) {
             rep_state_idx++;
         }
         
         // Create new state ID
         char new_state_id[32];
         snprintf(new_state_id, sizeof(new_state_id), "q%zu", i);
         
         // Create state in minimized automaton
         create_minimized_state(minimized, new_state_id, automaton->states[rep_state_idx].is_final);
         
         // Store the new state ID for this class
         class_state_ids[i] = strdup(new_state_id);
         if (!class_state_ids[i]) {
             // Clean up
             for (size_t j = 0; j < i; j++) {
                 free(class_state_ids[j]);
             }
             free(class_state_ids);
             okpala_automaton_free(minimized);
             free(class_ids);
             free(class_of_state);
             for (size_t j = 0; j < automaton->state_count; j++) {
                 free(equivalence_matrix[j]);
             }
             free(equivalence_matrix);
             return NULL;
         }
     }
     
     // Add transitions to minimized automaton
     // We only need to add one transition for each class
     for (size_t i = 0; i < class_count; i++) {
         // Find a representative state for this class
         size_t rep_state_idx = 0;
         while (class_of_state[rep_state_idx] != class_ids[i]) {
             rep_state_idx++;
         }
         
         OkpalaState* rep_state = &automaton->states[rep_state_idx];
         
         // Process transitions
         for (size_t j = 0; j < rep_state->transition_count; j++) {
             OkpalaState* target_state = rep_state->transitions[j];
             size_t target_state_idx = target_state - automaton->states;
             size_t target_class = class_of_state[target_state_idx];
             
             // Find class ID for target class
             size_t target_class_idx = 0;
             while (class_ids[target_class_idx] != target_class) {
                 target_class_idx++;
             }
             
             // Add transition to minimized automaton
             okpala_automaton_add_transition(minimized, 
                                           class_state_ids[i], 
                                           class_state_ids[target_class_idx], 
                                           rep_state->input_symbols[j]);
         }
     }
     
     // Apply boolean reduction if requested
     if (use_boolean_reduction) {
         apply_boolean_reduction(minimized);
     }
     
     // Clean up
     for (size_t i = 0; i < class_count; i++) {
         free(class_state_ids[i]);
     }
     free(class_state_ids);
     free(class_ids);
     free(class_of_state);
     for (size_t i = 0; i < automaton->state_count; i++) {
         free(equivalence_matrix[i]);
     }
     free(equivalence_matrix);
     
     return minimized;
 }