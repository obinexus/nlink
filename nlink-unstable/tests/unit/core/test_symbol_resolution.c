/**
 * @file test_symbol_resolution.c
 * @brief Unit tests for symbol resolution functionality
 * 
 * Tests the three-tier symbol table and context-aware resolution.
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nexus_symbols.h"
 #include "../common/test_common.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 void test_basic_symbol_resolution() {
     // Initialize registry
     NexusSymbolRegistry* registry = nexus_init_symbol_registry();
     
     // Add symbols to different tables
     // Global table
     nexus_symbol_table_add(&registry->global, "global_function", (void*)0x100, 
                           NEXUS_SYMBOL_FUNCTION, "global");
     
     // Exported table
     nexus_symbol_table_add(&registry->exported, "exported_function", (void*)0x200, 
                           NEXUS_SYMBOL_FUNCTION, "component1");
     
     // Imported table
     nexus_symbol_table_add(&registry->imported, "imported_function", (void*)0x300, 
                           NEXUS_SYMBOL_FUNCTION, "component2");
     
     // Add a symbol that exists in multiple tables (with different addresses)
     nexus_symbol_table_add(&registry->global, "common_function", (void*)0x400, 
                           NEXUS_SYMBOL_FUNCTION, "global");
     nexus_symbol_table_add(&registry->exported, "common_function", (void*)0x500, 
                           NEXUS_SYMBOL_FUNCTION, "component1");
     nexus_symbol_table_add(&registry->imported, "common_function", (void*)0x600, 
                           NEXUS_SYMBOL_FUNCTION, "component2");
     
     // Resolve symbols
     void* global_addr = nexus_resolve_symbol(registry, "global_function");
     test_assert("Resolve global symbol", global_addr == (void*)0x100);
     
     void* exported_addr = nexus_resolve_symbol(registry, "exported_function");
     test_assert("Resolve exported symbol", exported_addr == (void*)0x200);
     
     void* imported_addr = nexus_resolve_symbol(registry, "imported_function");
     test_assert("Resolve imported symbol", imported_addr == (void*)0x300);
     
     // Resolve common symbol - should prioritize exported
     void* common_addr = nexus_resolve_symbol(registry, "common_function");
     test_assert("Resolve common symbol prioritizes exported", common_addr == (void*)0x500);
     
     // Clean up
     nexus_cleanup_symbol_registry(registry);
 }
 
 void test_reference_counting() {
     // Initialize registry
     NexusSymbolRegistry* registry = nexus_init_symbol_registry();
     
     // Add symbols
     nexus_symbol_table_add(&registry->global, "ref_counted_function", (void*)0x100, 
                           NEXUS_SYMBOL_FUNCTION, "global");
     
     // Resolve symbol to increment ref count
     void* addr = nexus_resolve_symbol(registry, "ref_counted_function");
     test_assert("Symbol resolution returns address", addr == (void*)0x100);
     
     // Check ref count
     NexusSymbol* symbol = nexus_symbol_table_find(&registry->global, "ref_counted_function");
     test_assert("Reference count incremented on resolution", symbol->ref_count == 1);
     
     // Resolve again
     addr = nexus_resolve_symbol(registry, "ref_counted_function");
     test_assert("Symbol resolution returns same address", addr == (void*)0x100);
     
     // Check ref count again
     test_assert("Reference count incremented again", symbol->ref_count == 2);
     
     // Count used symbols
     size_t used_count = nexus_symbol_table_count_used(&registry->global);
     test_assert("Count used symbols", used_count == 1);
     
     // Clean up
     nexus_cleanup_symbol_registry(registry);
 }
 
 void test_resolution_priority() {
     // Initialize registry
     NexusSymbolRegistry* registry = nexus_init_symbol_registry();
     
     // Add the same symbol to all tables with different addresses
     // The resolution order should be: exported > imported > global
     
     // Global table (lowest priority)
     nexus_symbol_table_add(&registry->global, "priority_function", (void*)0x100, 
                           NEXUS_SYMBOL_FUNCTION, "global");
     
     // Imported table (medium priority)
     nexus_symbol_table_add(&registry->imported, "priority_function", (void*)0x200, 
                           NEXUS_SYMBOL_FUNCTION, "component2");
     
     // Exported table (highest priority)
     nexus_symbol_table_add(&registry->exported, "priority_function", (void*)0x300, 
                           NEXUS_SYMBOL_FUNCTION, "component1");
     
     // Resolve symbol - should find the one in exported table
     void* addr = nexus_resolve_symbol(registry, "priority_function");
     test_assert("Resolution prioritizes exported table", addr == (void*)0x300);
     
     // Remove from exported table
     nexus_symbol_table_remove(&registry->exported, "priority_function");
     
     // Resolve again - should find the one in imported table
     addr = nexus_resolve_symbol(registry, "priority_function");
     test_assert("Resolution falls back to imported table", addr == (void*)0x200);
     
     // Remove from imported table
     nexus_symbol_table_remove(&registry->imported, "priority_function");
     
     // Resolve again - should find the one in global table
     addr = nexus_resolve_symbol(registry, "priority_function");
     test_assert("Resolution falls back to global table", addr == (void*)0x100);
     
     // Clean up
     nexus_cleanup_symbol_registry(registry);
 }
 
 void test_type_safe_resolution() {
     // Initialize registry
     NexusSymbolRegistry* registry = nexus_init_symbol_registry();
     
     // Add symbols of different types
     nexus_symbol_table_add(&registry->global, "func_symbol", (void*)0x100, 
                           NEXUS_SYMBOL_FUNCTION, "global");
     nexus_symbol_table_add(&registry->global, "var_symbol", (void*)0x200, 
                           NEXUS_SYMBOL_VARIABLE, "global");
     nexus_symbol_table_add(&registry->global, "type_symbol", (void*)0x300, 
                           NEXUS_SYMBOL_TYPE, "global");
     
     // Resolve with correct types
     void* func_addr = nexus_lookup_symbol_with_type(registry, "func_symbol", 
                                                   NEXUS_SYMBOL_FUNCTION, "test");
     test_assert("Resolve function symbol with correct type", func_addr == (void*)0x100);
     
     void* var_addr = nexus_lookup_symbol_with_type(registry, "var_symbol", 
                                                  NEXUS_SYMBOL_VARIABLE, "test");
     test_assert("Resolve variable symbol with correct type", var_addr == (void*)0x200);
     
     void* type_addr = nexus_lookup_symbol_with_type(registry, "type_symbol", 
                                                   NEXUS_SYMBOL_TYPE, "test");
     test_assert("Resolve type symbol with correct type", type_addr == (void*)0x300);
     
     // Resolve with incorrect types
     void* wrong_func = nexus_lookup_symbol_with_type(registry, "func_symbol", 
                                                    NEXUS_SYMBOL_VARIABLE, "test");
     test_assert("Resolve function symbol with wrong type fails", wrong_func == NULL);
     
     void* wrong_var = nexus_lookup_symbol_with_type(registry, "var_symbol", 
                                                   NEXUS_SYMBOL_TYPE, "test");
     test_assert("Resolve variable symbol with wrong type fails", wrong_var == NULL);
     
     void* wrong_type = nexus_lookup_symbol_with_type(registry, "type_symbol", 
                                                    NEXUS_SYMBOL_FUNCTION, "test");
     test_assert("Resolve type symbol with wrong type fails", wrong_type == NULL);
     
     // Clean up
     nexus_cleanup_symbol_registry(registry);
 }
 
 void test_context_aware_resolution() {
     // Initialize registry
     NexusSymbolRegistry* registry = nexus_init_symbol_registry();
     
     // Add context-specific symbols
     nexus_symbol_table_add(&registry->exported, "context_function", (void*)0x100, 
                           NEXUS_SYMBOL_FUNCTION, "component1");
     nexus_symbol_table_add(&registry->exported, "context_function", (void*)0x200, 
                           NEXUS_SYMBOL_FUNCTION, "component2");
     
     // Standard resolution should prioritize in some deterministic way
     void* std_addr = nexus_resolve_symbol(registry, "context_function");
     test_assert("Standard resolution finds one of the symbols", 
                 std_addr == (void*)0x100 || std_addr == (void*)0x200);
     
     // Context-aware resolution
     void* ctx1_addr = nexus_context_aware_resolve(registry, "context_function", 
                                                 "component1", "requester");
     test_assert("Context-aware resolution for component1", ctx1_addr == (void*)0x100);
     
     void* ctx2_addr = nexus_context_aware_resolve(registry, "context_function", 
                                                 "component2", "requester");
     test_assert("Context-aware resolution for component2", ctx2_addr == (void*)0x200);
     
     // Clean up
     nexus_cleanup_symbol_registry(registry);
 }
 
 int main() {
     test_init("Symbol Resolution");
     
     test_basic_symbol_resolution();
     test_reference_counting();
     test_resolution_priority();
     test_type_safe_resolution();
     test_context_aware_resolution();
     
     return test_finalize();
 }