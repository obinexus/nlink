#ifndef NEXUS_LAZY_VERSIONED_H
#define NEXUS_LAZY_VERSIONED_H

#include <time.h>
#include <pthread.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include "nlink/core/common/nexus_core.h"

// Version information structure
typedef struct {
	char* resolved_version;
	char* providing_component;
	bool is_exact_match;
} VersionInfo;

// Versioned symbol structure
typedef struct {
	char* name;
	char* version;
	char* component_id;
	void* address;
	int ref_count;
	time_t last_used;
} VersionedSymbol;

// Symbol table structure
typedef struct {
	VersionedSymbol* symbols;
	size_t size;
	size_t capacity;
} SymbolTable;

// Registry for versioned symbols
typedef struct {
	SymbolTable exported;
} VersionedSymbolRegistry;

// Function declarations
void nexus_check_unused_versioned_libraries(VersionedSymbolRegistry* registry);
void nexus_print_symbol_version_info(const char* symbol_name, const VersionInfo* info);
const char* nexus_get_component_version_constraint(
	VersionedSymbolRegistry* registry,
	const char* symbol_name,
	const char* component_id,
	const char* default_constraint
);
void nexus_track_symbol_usage_versioned(
	VersionedSymbolRegistry* registry,
	const char* symbol_name,
	const char* version,
	const char* using_component
);

#endif // NEXUS_LAZY_VERSIONED_H
