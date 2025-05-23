/**
 * @file nexus_automaton.c
 * @brief Implementation of automaton initialization for NexusLink
 * 
 * This file provides the implementation for the automaton initialization
 * function used by the NexusLink system.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/core/minimizer/okpala_automaton.h"
 #include "nlink/core/common/nexus_core.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 // Global flag to track initialization state
 static bool g_automaton_initialized = false;
 
 /**
  * @brief Initialize the automaton subsystem
  * 
  * This function initializes the automaton subsystem and must be called
  * before using any other automaton functions.
  */
 void nexus_automaton_initialize(void) {
     if (g_automaton_initialized) {
         return;  // Already initialized
     }
     
     // Perform any necessary initialization
     g_automaton_initialized = true;
     
     #ifdef NEXUS_DEBUG
     printf("NexusLink Automaton subsystem initialized\n");
     #endif
 }
 
 /**
  * @brief Create a new Okpala Automaton instance
  * 
  * @return Pointer to the newly created automaton, or NULL if creation failed
  */
 OkpalaAutomaton* okpala_automaton_create(void) {
     if (!g_automaton_initialized) {
         nexus_automaton_initialize();
     }
     
     OkpalaAutomaton* automaton = (OkpalaAutomaton*)malloc(sizeof(OkpalaAutomaton));
     if (!automaton) {
         return NULL;
     }
     
     // Initialize the automaton
     automaton->states = NULL;
     automaton->state_count = 0;
     automaton->initial_state = NULL;
     automaton->final_states = NULL;
     automaton->final_state_count = 0;
     
     return automaton;
 }
 
 /**
  * @brief Find a state by ID in the automaton
  * 
  * @param automaton The automaton to search in
  * @param id The ID to search for
  * @return Pointer to the state if found, NULL otherwise
  */
 static OkpalaState* find_state(OkpalaAutomaton* automaton, const char* id) {
     if (!automaton || !id) {
         return NULL;
     }
     
     for (size_t i = 0; i < automaton->state_count; i++) {
         if (strcmp(automaton->states[i].id, id) == 0) {
             return &automaton->states[i];
         }
     }
     
     return NULL;
 }
 
 /**
  * @brief Add a state to the automaton
  * 
  * @param automaton The automaton to add the state to
  * @param id The unique identifier for the state
  * @param is_final Whether this is a final/accepting state
  * @return NexusResult result code (NEXUS_SUCCESS on success)
  */
 NexusResult okpala_automaton_add_state(OkpalaAutomaton* automaton, 
                                      const char* id, 
                                      bool is_final) {
     if (!automaton || !id) {
         return NEXUS_ERROR_INVALID_ARGUMENT;
     }
     
    // Check if state already exists
    if (find_state(automaton, id)) {
        return NEXUS_ERROR_INVALID_ARGUMENT; // Using existing error code instead
    }
    
     // Allocate or resize states array
     OkpalaState* new_states = (OkpalaState*)realloc(automaton->states, 
                                                (automaton->state_count + 1) * sizeof(OkpalaState));
     if (!new_states) {
         return NEXUS_ERROR_OUT_OF_MEMORY;
     }
     
     automaton->states = new_states;
     
     // Initialize the new state
     OkpalaState* state = &automaton->states[automaton->state_count++];
     state->id = strdup(id);
     if (!state->id) {
         automaton->state_count--;
         return NEXUS_ERROR_OUT_OF_MEMORY;
     }
     
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
         OkpalaState** new_final_states = (OkpalaState**)realloc(automaton->final_states, 
                                                         (automaton->final_state_count + 1) * sizeof(OkpalaState*));
         if (!new_final_states) {
             // Don't roll back state creation, just don't add to final states
             return NEXUS_ERROR_OUT_OF_MEMORY;
         }
         
         automaton->final_states = new_final_states;
         automaton->final_states[automaton->final_state_count++] = state;
     }
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * @brief Add a transition between states in the automaton
  * 
  * @param automaton The automaton to add the transition to
  * @param from_id The ID of the source state
  * @param to_id The ID of the target state
  * @param input_symbol The input symbol that triggers this transition
  * @return NexusResult result code (NEXUS_SUCCESS on success)
  */
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
         return NEXUS_ERROR_INVALID_REFERENCE;
     }
     
     // Allocate or resize transitions arrays
     OkpalaState** new_transitions = (OkpalaState**)realloc(from_state->transitions, 
                                                    (from_state->transition_count + 1) * sizeof(OkpalaState*));
     if (!new_transitions) {
         return NEXUS_ERROR_OUT_OF_MEMORY;
     }
     
     char** new_input_symbols = (char**)realloc(from_state->input_symbols, 
                                         (from_state->transition_count + 1) * sizeof(char*));
     if (!new_input_symbols) {
         free(new_transitions);
         return NEXUS_ERROR_OUT_OF_MEMORY;
     }
     
     from_state->transitions = new_transitions;
     from_state->input_symbols = new_input_symbols;
     
     // Add the transition
     from_state->transitions[from_state->transition_count] = to_state;
     from_state->input_symbols[from_state->transition_count] = strdup(input_symbol);
     if (!from_state->input_symbols[from_state->transition_count]) {
         return NEXUS_ERROR_OUT_OF_MEMORY;
     }
     
     from_state->transition_count++;
     
     return NEXUS_SUCCESS;
 }
 
 /**
  * @brief Free an Okpala Automaton instance and all associated resources
  * 
  * @param automaton The automaton to free
  */
 void okpala_automaton_free(OkpalaAutomaton* automaton) {
     if (!automaton) {
         return;
     }
     
     // Free states and their resources
     for (size_t i = 0; i < automaton->state_count; i++) {
         OkpalaState* state = &automaton->states[i];
         
         // Free state ID
         free(state->id);
         
         // Free transition input symbols
         for (size_t j = 0; j < state->transition_count; j++) {
             free(state->input_symbols[j]);
         }
         
         // Free transition and input symbol arrays
         free(state->transitions);
         free(state->input_symbols);
     }
     
     // Free states array
     free(automaton->states);
     
     // Free final states array (note: these are just pointers to states already freed)
     free(automaton->final_states);
     
     // Free the automaton itself
     free(automaton);
 }