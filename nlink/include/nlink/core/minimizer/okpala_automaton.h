/**
 * @file okpala_automaton.h
 * @brief Definitions for the Okpala Automaton system for state machine minimization
 * 
 * This header defines the data structures and functions for the Okpala Automaton,
 * a specialized automaton implementation designed for efficient state machine
 * minimization in the NexusLink system.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #ifndef NLINK_CORE_MINIMIZER_OKPALA_AUTOMATON_H
 #define NLINK_CORE_MINIMIZER_OKPALA_AUTOMATON_H
 
 #include "nlink/core/common/types.h"
 #include "nlink/core/common/result.h"
 #include <stdbool.h>
 #include <stddef.h>
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Structure representing a state in the Okpala Automaton
  */
 typedef struct OkpalaState {
	 char* id;                      /**< Unique identifier for the state */
	 bool is_final;                 /**< Whether this is a final/accepting state */
	 struct OkpalaState** transitions; /**< Array of pointers to transition target states */
	 char** input_symbols;          /**< Array of input symbols corresponding to transitions */
	 size_t transition_count;       /**< Number of transitions from this state */
 } OkpalaState;
 
 /**
  * @brief Structure representing the complete Okpala Automaton
  */
 typedef struct OkpalaAutomaton {
	 OkpalaState* states;           /**< Array of all states in the automaton */
	 size_t state_count;            /**< Number of states in the automaton */
	 OkpalaState* initial_state;    /**< Pointer to the initial state */
	 OkpalaState** final_states;    /**< Array of pointers to final states */
	 size_t final_state_count;      /**< Number of final states */
 } OkpalaAutomaton;
 
 /**
  * @brief Initialize the automaton subsystem
  * 
  * This function initializes the automaton subsystem and must be called
  * before using any other automaton functions.
  */
 void nexus_automaton_initialize(void);
 
 /**
  * @brief Create a new Okpala Automaton instance
  * 
  * @return Pointer to the newly created automaton, or NULL if creation failed
  */
 OkpalaAutomaton* okpala_automaton_create(void);
 
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
									  bool is_final);
 
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
										  const char* input_symbol);
 
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
										 bool use_boolean_reduction);
 
 /**
  * @brief Free an Okpala Automaton instance and all associated resources
  * 
  * @param automaton The automaton to free
  */
 void okpala_automaton_free(OkpalaAutomaton* automaton);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* NLINK_CORE_MINIMIZER_OKPALA_AUTOMATON_H */