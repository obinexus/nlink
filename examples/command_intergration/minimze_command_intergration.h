#ifndef MINIMIZE_COMMAND_INTEGRATION_H
#define MINIMIZE_COMMAND_INTEGRATION_H

#include "nlink/core/minimizer/nexus_minimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "commands/minimize.h"
#include "nlink/core/minimizer/nexus_minimizer.h"
#include "nlink/core/minimizer/okpala_automaton.h"
/**
 * @brief Structure for minimize command data
 */
typedef struct {
	char* component_path;      /**< Path to the component */
	NexusMinimizerConfig config;  /**< Minimization configuration */
	bool output_metrics;       /**< Whether to output metrics */
	char* output_file;         /**< Output file path (optional) */
} MinimizeCommandData;

int load_with_minimization(NexusContext* ctx, const char* component_path, 
						  bool minimize, int minimize_level);
void demonstrate_automaton_minimization(void);

#endif /* MINIMIZE_COMMAND_INTEGRATION_H */
