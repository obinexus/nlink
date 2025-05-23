// cold.c – "Cold" code to be lazy-loaded
#include <stdio.h>

__attribute__((visibility("hidden"))) void cold_function_impl(int x) {
  printf("Lazy-loaded function called with %d\n", x);
}

// Function to load the cold function implementation
void load_cold_function(void) {
  // In a real implementation, this would dynamically load the code from a shared library
  // For this demonstration, we'll just simulate the loading process
  static int loaded = 0;
  
  if (!loaded) {
    printf("Loading cold function implementation...\n");
    // In a real implementation, this would involve dlopen/dlsym calls
    loaded = 1;
  }
}
