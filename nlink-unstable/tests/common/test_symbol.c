/**
 * @file test_symbols.c
 * @brief Unit tests for symbol management
 * 
 * Copyright © 2025 OBINexus Computing
 */

 #include "nexus_symbols.h"
 #include "../common/test_common.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 void test_symbol_table_init() {
     NexusSymbolTable table;
     nexus_symbol_table_init(&table, 16);
     
     test_assert("Symbol table initialization", 
                 table.symbols != NULL && 
                 table.capacity == 16 && 
                 table.size == 0);
     
     // Clean up
     nexus_symbol_table_cleanup(&table);
 }
 
 void test_symbol_table_add() {
     NexusSymbolTable table;
     nexus_symbol_table_init(&table, 4);
     
     // Add a symbol
     NexusResult result = nexus_symbol_table_add(&table, "test_symbol", (void*)0x12345678, 
                                                NEXUS_SYMBOL_FUNCTION, "test_component");
     
     test_assert("Adding symbol to table", 
                 result == NEXUS_SUCCESS && 
                 table.size == 1);
     
     // Verify symbol details
     NexusSymbol* symbol = nexus_symbol_table_find(&table, "test_symbol");
     test_assert("Finding added symbol", 
                 symbol != NULL && 
                 strcmp(symbol->name, "test_symbol") == 0 && 
                 symbol->address == (void*)0x12345678 && 
                 symbol->type == NEXUS_SYMBOL_FUNCTION && 
                 strcmp(symbol->component_id, "test_component") == 0 && 
                 symbol->ref_count == 0);
     
     // Add more symbols to test capacity expansion
     nexus_symbol_table_add(&table, "symbol1", (void*)0x1, NEXUS_SYMBOL_FUNCTION, "comp1");
     nexus_symbol_table_add(&table, "symbol2", (void*)0x2, NEXUS_SYMBOL_VARIABLE, "comp2");
     nexus_symbol_table_add(&table, "symbol3", (void*)0x3, NEXUS_SYMBOL_TYPE, "comp3");
     nexus_symbol_table_add(&table, "symbol4", (void*)0x4, NEXUS_SYMBOL_FUNCTION, "comp4");
     
     test_assert("Table expands capacity", table.capacity > 4 && table.size == 5);
     
     // Clean up
     nexus_symbol_table_cleanup(&table);
 }
 
 void test_symbol_table_find() {
     NexusSymbolTable table;
     nexus_symbol_table_init(&table, 4);
     
     // Add several symbols
     nexus_symbol_table_add(&table, "func1", (void*)0x1, NEXUS_SYMBOL_FUNCTION, "comp1");
     nexus_symbol_table_add(&table, "var1", (void*)0x2, NEXUS_SYMBOL_VARIABLE, "comp1");
     nexus_symbol_table_add(&table, "type1", (void*)0x3, NEXUS_SYMBOL_TYPE, "comp2");
     
     // Find existing symbols
     NexusSymbol* func = nexus_symbol_table_find(&table, "func1");
     test_assert("Find function symbol", 
                 func != NULL && 
                 strcmp(func->name, "func1") == 0);
     
     NexusSymbol* var = nexus_symbol_table_find(&table, "var1");
     test_assert("Find variable symbol", 
                 var != NULL && 
                 strcmp(var->name, "var1") == 0);
     
     NexusSymbol* type = nexus_symbol_table_find(&table, "type1");
     test_assert("Find type symbol", 
                 type != NULL && 
                 strcmp(type->name, "type1") == 0);
     
     // Test non-existent symbol
     NexusSymbol* nonexistent = nexus_symbol_table_find(&table, "nonexistent");
     test_assert("Non-existent symbol returns NULL", nonexistent == NULL);
     
     // Clean up
     nexus_symbol_table_cleanup(&table);
 }
 
 void test_symbol_table_remove() {
     NexusSymbolTable table;
     nexus_symbol_table_init(&table, 4);
     
     // Add several symbols
     nexus_symbol_table_add(&table, "func1", (void*)0x1, NEXUS_SYMBOL_FUNCTION, "comp1");
     nexus_symbol_table_add(&table, "var1", (void*)0x2, NEXUS_SYMBOL_VARIABLE, "comp1");
     nexus_symbol_table_add(&table, "type1", (void*)0x3, NEXUS_SYMBOL_TYPE, "comp2");
     
     // Verify initial size
     test_assert("Initial table size", table.size == 3);
     
     // Remove a symbol
     NexusResult result = nexus_symbol_table_remove(&table, "var1");
     test_assert("Remove symbol result", result == NEXUS_SUCCESS);
     test_assert("Table size after removal", table.size == 2);
     
     // Verify symbol was removed
     NexusSymbol* removed = nexus_symbol_table_find(&table, "var1");
     test_assert("Removed symbol not found", removed == NULL);
     
     // Verify other symbols still exist
     NexusSymbol* func = nexus_symbol_table_find(&table, "func1");
     test_assert("Other symbols remain", 
                 func != NULL && 
                 strcmp(func->name, "func1") == 0);
     
     // Try to remove non-existent symbol
     result = nexus_symbol_table_remove(&table, "nonexistent");
     test_assert("Remove non-existent symbol", result == NEXUS_NOT_FOUND);
     
     // Clean up
     nexus_symbol_table_cleanup(&table);
 }
 
 void test_symbol_registry() {
     // Initialize registry
     NexusSymbolRegistry* registry = nexus_init_symbol_registry();
     test_assert("Registry initialization", registry != NULL);
     
     // Add symbols to different tables
     nexus_symbol_table_add(&registry->global, "global_func", (void*)0x1, 
                           NEXUS_SYMBOL_FUNCTION, "global");
     
     nexus_symbol_table_add(&registry->exported, "exported_func", (void*)0x2, 
                           NEXUS_SYMBOL_FUNCTION, "comp1");
     
     nexus_symbol_table_add(&registry->imported, "imported_func", (void*)0x3, 
                           NEXUS_SYMBOL_FUNCTION, "comp2");
     
     // Test symbol resolution
     void* global_addr = nexus_resolve_symbol(registry, "global_func");
     test_assert("Resolve global symbol", global_addr == (void*)0x1);
     
     void* exported_addr = nexus_resolve_symbol(registry, "exported_func");
     test_assert("Resolve exported symbol", exported_addr == (void*)0x2);
     
     void* imported_addr = nexus_resolve_symbol(registry, "imported_func");
     test_assert("Resolve imported symbol", imported_addr == (void*)0x3);
     
     void* nonexistent_addr = nexus_resolve_symbol(registry, "nonexistent");
     test_assert("Resolve non-existent symbol", nonexistent_addr == NULL);
     
     // Test reference counting
     NexusSymbol* global_sym = nexus_symbol_table_find(&registry->global, "global_func");
     test_assert("Global symbol ref count incremented", global_sym->ref_count == 1);
     
     // Clean up
     nexus_cleanup_symbol_registry(registry);
 }
 
 int main() {
     test_init("Symbol Management");
     
     test_symbol_table_init();
     test_symbol_table_add();
     test_symbol_table_find();
     test_symbol_table_remove();
     test_symbol_registry();
     
     return test_finalize();
 }