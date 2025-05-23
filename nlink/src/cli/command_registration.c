/**
 * @file command_registration.c
 * @brief Implementation of command registration functions for NexusLink CLI
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/cli/command_registration.h"
 #include "nlink/cli/command_router.h"
 #include "nlink/cli/command.h"
 #include "nlink/core/common/nexus_core.h"
 #include "nlink/core/pattern/matcher.h"
 #include <string.h>
 #include <stdlib.h>
 #include <ctype.h>
 #include <stdio.h>

 
 /* External command declarations */
 extern NexusCommand load_command;
 extern NexusCommand minimal_command;
 extern NexusCommand minimize_command;
 extern NexusCommand pipeline_command;
 extern NexusCommand version_command;
 extern NexusCommand list_command;
 extern NexusCommand help_command;
 extern NexusCommand parse_command;
 extern NexusCommand stats_command;
 
 /**
  * @brief Register commands for the load functionality
  * 
  * @param router Command router to register with
  * @return NexusResult Result code
  */
 NexusResult load_command_register_with_params(NlinkCommandRouter* router) {
     if (!router) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     NexusResult result;
     
     /* Basic load command */
     const char* basic_pattern = "^load ([a-zA-Z0-9_.-]+)$";
     const char* basic_params[] = {"component"};
     
     result = nlink_command_router_register_with_params(
         router,
         basic_pattern,
         &load_command,
         NLINK_PATTERN_FLAG_REGEX,
         basic_params,
         1
     );
     
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     /* Load with version */
     const char* version_pattern = "^load ([a-zA-Z0-9_.-]+) version ([a-zA-Z0-9_.-]+)$";
     const char* version_params[] = {"component", "version"};
     
     result = nlink_command_router_register_with_params(
         router,
         version_pattern,
         &load_command,
         NLINK_PATTERN_FLAG_REGEX,
         version_params,
         2
     );
     
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     /* Load with version and function */
     const char* func_pattern = "^load ([a-zA-Z0-9_.-]+) version ([a-zA-Z0-9_.-]+) function ([a-zA-Z0-9_.-]+)$";
     const char* func_params[] = {"component", "version", "function"};
     
     result = nlink_command_router_register_with_params(
         router,
         func_pattern,
         &load_command,
         NLINK_PATTERN_FLAG_REGEX,
         func_params,
         3
     );
     
     return result;
 }
 
/**
 * @brief Register commands for the minimal functionality
 * 
 * @param router Command router to register with
 * @return NexusResult Result code
 */
NexusResult minimal_command_register_with_params(NlinkCommandRouter* router) {
    if (!router) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    /* Enhanced minimal command pattern supporting component@version:function syntax */
    const char* minimal_pattern = "^([a-zA-Z0-9_-]+)(@([0-9.]+))?(:([a-zA-Z0-9_-]+))?$";
    const char* minimal_params[] = {"component", NULL, "version", NULL, "function"};
    
    return nlink_command_router_register_with_params(
        router,
        minimal_pattern,
        &minimal_command,
        NLINK_PATTERN_FLAG_REGEX,
        minimal_params,
        5  // Include all capturing groups
    );
}
 
 /**
  * @brief Register commands for the minimize functionality
  * 
  * @param router Command router to register with
  * @return NexusResult Result code
  */
 NexusResult minimize_command_register_with_params(NlinkCommandRouter* router) {
     if (!router) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     NexusResult result;
     
     /* Basic minimize command */
     const char* basic_pattern = "^minimize ([a-zA-Z0-9_.-/]+)$";
     const char* basic_params[] = {"component"};
     
     result = nlink_command_router_register_with_params(
         router,
         basic_pattern,
         &minimize_command,
         NLINK_PATTERN_FLAG_REGEX,
         basic_params,
         1
     );
     
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     /* Minimize with level */
     const char* level_pattern = "^minimize ([a-zA-Z0-9_.-/]+) level ([0-9])$";
     const char* level_params[] = {"component", "level"};
     
     result = nlink_command_router_register_with_params(
         router,
         level_pattern,
         &minimize_command,
         NLINK_PATTERN_FLAG_REGEX,
         level_params,
         2
     );
     
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     /* Minimize with boolean reduction flag */
     const char* bool_pattern = "^minimize ([a-zA-Z0-9_.-/]+) (with|without) boolean$";
     const char* bool_params[] = {"component", "boolean_option"};
     
     result = nlink_command_router_register_with_params(
         router,
         bool_pattern,
         &minimize_command,
         NLINK_PATTERN_FLAG_REGEX,
         bool_params,
         2
     );
     
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     /* Minimize with output */
     const char* output_pattern = "^minimize ([a-zA-Z0-9_.-/]+) output ([a-zA-Z0-9_.-/]+)$";
     const char* output_params[] = {"component", "output"};
     
     result = nlink_command_router_register_with_params(
         router,
         output_pattern,
         &minimize_command,
         NLINK_PATTERN_FLAG_REGEX,
         output_params,
         2
     );
     
     return result;
 }
 
 /**
  * @brief Register commands for the pipeline functionality
  * 
  * @param router Command router to register with
  * @return NexusResult Result code
  */
 NexusResult pipeline_command_register_with_params(NlinkCommandRouter* router) {
     if (!router) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     NexusResult result;
     
     /* Basic pipeline command */
     const char* basic_pattern = "^pipeline ([a-zA-Z0-9_.-/]+)$";
     const char* basic_params[] = {"component"};
     
     result = nlink_command_router_register_with_params(
         router,
         basic_pattern,
         &pipeline_command,
         NLINK_PATTERN_FLAG_REGEX,
         basic_params,
         1
     );
     
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     /* Pipeline with subcommand */
     const char* subcmd_pattern = "^pipeline ([a-zA-Z0-9_.-/]+) ([a-zA-Z0-9_.-]+)$";
     const char* subcmd_params[] = {"component", "subcommand"};
     
     result = nlink_command_router_register_with_params(
         router,
         subcmd_pattern,
         &pipeline_command,
         NLINK_PATTERN_FLAG_REGEX,
         subcmd_params,
         2
     );
     
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     /* Pipeline with subcommand and argument */
     const char* arg_pattern = "^pipeline ([a-zA-Z0-9_.-/]+) ([a-zA-Z0-9_.-]+) ([a-zA-Z0-9_.-/]+)$";
     const char* arg_params[] = {"component", "subcommand", "argument"};
     
     result = nlink_command_router_register_with_params(
         router,
         arg_pattern,
         &pipeline_command,
         NLINK_PATTERN_FLAG_REGEX,
         arg_params,
         3
     );
     
     return result;
 }
 
 /**
  * @brief Register commands for the version functionality
  * 
  * @param router Command router to register with
  * @return NexusResult Result code
  */
 NexusResult version_command_register_with_params(NlinkCommandRouter* router) {
     if (!router) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     /* Basic version command */
     const char* pattern = "^version$";
     
     return nlink_command_router_register(
         router,
         pattern,
         &version_command,
         NLINK_PATTERN_FLAG_REGEX
     );
 }
/**
  * @brief Register built-in utility commands
  * 
  * @param router Command router to register with
  * @return NexusResult Result code
  */
 static NexusResult register_utility_commands(NlinkCommandRouter* router) {
     if (!router) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     NexusResult result;
     
     /* Help command */
     result = nlink_command_router_register(
         router,
         "^help$",
         &help_command,
         NLINK_PATTERN_FLAG_REGEX
     );
     
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     /* Help with command */
     const char* help_pattern = "^help ([a-zA-Z0-9_.-]+)$";
     const char* help_params[] = {"command"};
     
     result = nlink_command_router_register_with_params(
         router,
         help_pattern,
         &help_command,
         NLINK_PATTERN_FLAG_REGEX,
         help_params,
         1
     );
     
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     /* List command */
     result = nlink_command_router_register(
         router,
         "^list$",
         &list_command,
         NLINK_PATTERN_FLAG_REGEX
     );
     
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     /* List with category */
     const char* list_pattern = "^list ([a-zA-Z0-9_.-]+)$";
     const char* list_params[] = {"category"};
     
     result = nlink_command_router_register_with_params(
         router,
         list_pattern,
         &list_command,
         NLINK_PATTERN_FLAG_REGEX,
         list_params,
         1
     );
     
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     /* Stats command */
     result = nlink_command_router_register(
         router,
         "^stats$",
         &stats_command,
         NLINK_PATTERN_FLAG_REGEX
     );
     
     return result;
}

/**
 * @brief Register commands for the parse functionality
 * 
 * @param router Command router to register with
 * @return NexusResult Result code
 */
NexusResult parse_command_register_with_params(NlinkCommandRouter* router) {
    if (!router) {
        return NEXUS_INVALID_PARAMETER;
    }
    
    /* Parse command */
    const char* parse_pattern = "^parse ([a-zA-Z0-9_.-/]+)( to ([a-zA-Z0-9_.-/]+))?$";
    const char* parse_params[] = {"input", NULL, "output"};
    
    return nlink_command_router_register_with_params(
        router,
        parse_pattern,
        &parse_command,
        NLINK_PATTERN_FLAG_REGEX,
        parse_params,
        3
    );
}
 /**
  * @brief Register all built-in commands
  * 
  * @param router Command router to register with
  * @return NexusResult Result code
  */
 NexusResult register_all_commands(NlinkCommandRouter* router) {
     if (!router) {
         return NEXUS_INVALID_PARAMETER;
     }
     
     NexusResult result;
     
     /* Register utility commands */
     result = register_utility_commands(router);
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     /* Register functional commands */
     result = load_command_register_with_params(router);
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     result = minimal_command_register_with_params(router);
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     result = minimize_command_register_with_params(router);
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     result = parse_command_register_with_params(router);
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     result = pipeline_command_register_with_params(router);
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     result = version_command_register_with_params(router);
     if (result != NEXUS_SUCCESS) {
         return result;
     }
     
     return NEXUS_SUCCESS;
 }

