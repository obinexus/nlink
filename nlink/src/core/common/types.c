/**
 * @file types.c
 * @brief Implementation of common types for NexusLink
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nlink/core/common/types.h"

 // This file is currently empty as all types are defined in the header file.
 // It exists as a placeholder for future common type-related implementations.
 
 // Additional utility functions for type handling may be added here in the future.
 // For example:
 
 const char* nexus_symbol_type_to_string(NexusSymbolType type) {
     switch (type) {
         case NEXUS_SYMBOL_UNKNOWN:  return "unknown";
         case NEXUS_SYMBOL_FUNCTION: return "function";
         case NEXUS_SYMBOL_DATA:     return "data";
         case NEXUS_SYMBOL_CONST:    return "constant";
         default:                    return "invalid";
     }
 }
 
