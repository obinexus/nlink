// test_symbols.c - Unit tests for symbol management
// Author: Nnamdi Michael Okpala
#include "../../include/nexus_symbols.h"
#include <stdio.h>
#include <assert.h>

// Test function for symbol resolution
void test_function(void) {
    printf("Test function called\n");
}

int main() {
    printf("Running symbol management tests...\n");
    
    // Create a symbol registry
    NexusSymbolRegistry* registry = nexus_symbol_registry_create();
    assert(registry != NULL);
    
    // Add symbols to tables
    NexusResult result;
    
    result = nexus_symbol_add(&registry->global, "global_symbol", "1.0.0", 
                            (void*)&test_function, NEXUS_SYMBOL_FUNCTION, "global");
    assert(result == NEXUS_SUCCESS);
    
    result = nexus_symbol_add(&registry->exported, "exported_symbol", "1.0.0", 
                            (void*)&test_function, NEXUS_SYMBOL_FUNCTION, "component1");
    assert(result == NEXUS_SUCCESS);
    
    result = nexus_symbol_add(&registry->imported, "imported_symbol", "1.0.0", 
                            (void*)&test_function, NEXUS_SYMBOL_FUNCTION, "component2");
    assert(result == NEXUS_SUCCESS);
    
    // Find symbols
    NexusSymbol* global_symbol = nexus_symbol_find(&registry->global, "global_symbol");
    assert(global_symbol != NULL);
    assert(strcmp(global_symbol->name, "global_symbol") == 0);
    
    NexusSymbol* exported_symbol = nexus_symbol_find(&registry->exported, "exported_symbol");
    assert(exported_symbol != NULL);
    assert(strcmp(exported_symbol->name, "exported_symbol") == 0);
    
    NexusSymbol* imported_symbol = nexus_symbol_find(&registry->imported, "imported_symbol");
    assert(imported_symbol != NULL);
    assert(strcmp(imported_symbol->name, "imported_symbol") == 0);
    
    // Resolve symbols
    void* resolved_global = nexus_symbol_resolve(registry, "global_symbol");
    assert(resolved_global != NULL);
    assert(resolved_global == (void*)&test_function);
    
    void* resolved_exported = nexus_symbol_resolve(registry, "exported_symbol");
    assert(resolved_exported != NULL);
    assert(resolved_exported == (void*)&test_function);
    
    void* resolved_imported = nexus_symbol_resolve(registry, "imported_symbol");
    assert(resolved_imported != NULL);
    assert(resolved_imported == (void*)&test_function);
    
    // Check that symbol not found returns NULL
    void* not_found = nexus_symbol_resolve(registry, "nonexistent_symbol");
    assert(not_found == NULL);
    
    // Free the registry
    nexus_symbol_registry_free(registry);
    
    printf("All symbol management tests passed!\n");
    return 0;
}

// Test basic symbol table operations
void test_symbol_table_ops() {
    printf("Testing basic symbol table operations...\n");
    
    SymbolTable table;
    symbol_table_init(&table, 4); // Start with small capacity to test resizing
    
    // Test initial state
    assert(table.capacity == 4);
    assert(table.size == 0);
    assert(table.symbols != NULL);
    
    // Add symbols until we trigger resizing
    symbol_table_add(&table, "symbol1", (void*)test_function, SYMBOL_FUNCTION, "comp1");
    symbol_table_add(&table, "symbol2", (void*)test_function, SYMBOL_FUNCTION, "comp1");
    symbol_table_add(&table, "symbol3", (void*)test_function, SYMBOL_FUNCTION, "comp1");
    symbol_table_add(&table, "symbol4", (void*)test_function, SYMBOL_FUNCTION, "comp1");
    symbol_table_add(&table, "symbol5", (void*)test_function, SYMBOL_FUNCTION, "comp1");
    
    // Verify resizing worked
    assert(table.capacity == 8); // Should have doubled
    assert(table.size == 5);
    
    // Test symbol lookup
    Symbol* found = symbol_table_find(&table, "symbol3");
    assert(found != NULL);
    assert(strcmp(found->name, "symbol3") == 0);
    assert(found->address == (void*)test_function);
    
    // Test symbol removal
    bool removed = symbol_table_remove(&table, "symbol3");
    assert(removed == true);
    assert(table.size == 4);
    
    // Verify symbol was removed
    found = symbol_table_find(&table, "symbol3");
    assert(found == NULL);
    
    // Test reference counting
    Symbol* ref_symbol = symbol_table_find(&table, "symbol1");
    assert(ref_symbol->ref_count == 0);
    ref_symbol->ref_count++;
    assert(ref_symbol->ref_count == 1);
    
    symbol_table_free(&table);
    printf("Basic symbol table operations passed!\n");
}

// Test symbol registry management
void test_symbol_registry() {
    printf("Testing symbol registry management...\n");
    
    NexusSymbolRegistry* registry = nexus_symbol_registry_create();
    assert(registry != NULL);
    
    // Test adding symbols to different tables
    symbol_table_add(&registry->global, "global_func", (void*)test_function, SYMBOL_FUNCTION, "global");
    symbol_table_add(&registry->exported, "exported_func", (void*)test_function, SYMBOL_FUNCTION, "comp1");
    symbol_table_add(&registry->imported, "imported_func", (void*)test_function, SYMBOL_FUNCTION, "comp2");
    
    // Test symbol resolution priority (exported > imported > global)
    symbol_table_add(&registry->exported, "priority_test", (void*)1, SYMBOL_FUNCTION, "comp1");
    symbol_table_add(&registry->imported, "priority_test", (void*)2, SYMBOL_FUNCTION, "comp2");
    symbol_table_add(&registry->global, "priority_test", (void*)3, SYMBOL_FUNCTION, "global");
    
    void* resolved = nexus_resolve_symbol(registry, "priority_test");
    assert(resolved == (void*)1); // Should resolve to exported version
    
    // Test reference counting during resolution
    Symbol* symbol = symbol_table_find(&registry->exported, "priority_test");
    assert(symbol->ref_count == 1); // Should have been incremented by resolve
    
    nexus_symbol_registry_free(registry);
    printf("Symbol registry management passed!\n");
}

// Add these tests to main()
int main() {
    // ... (existing main code) ...
    
    // Add new test cases
    test_symbol_table_ops();
    test_symbol_registry();
    
    printf("All symbol management tests passed!\n");
    return 0;
}
