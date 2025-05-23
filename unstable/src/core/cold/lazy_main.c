// main.c – User program
#include <stdio.h>
#include "core/lazy.h"

// Declare lazy-loaded function (implementation in libcold.so)
NEXUS_LAZY(cold_function, void, int x) {
  // Ensure function is loaded before calling
  load_cold_function();
  cold_function_impl(x);
}

int main() {
  printf("Main program started\n");
  
  // First call triggers lazy load
  cold_function(42);
  
  // Subsequent calls use cached implementation
  cold_function(1337);
  
  return 0;
}