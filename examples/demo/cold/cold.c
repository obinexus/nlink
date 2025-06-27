// cold.c – "Cold" code to be lazy-loaded
#include <stdio.h>

void cold_function(int x) {
  printf("Lazy-loaded function called with %d\n", x);
}