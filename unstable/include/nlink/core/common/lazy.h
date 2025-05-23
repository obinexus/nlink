// nexus_lazy.h – Header for lazy loading system
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>  // Added for exit() function

// Macro to declare lazy-loading functions
#define NEXUS_LAZY(func_name, ret_type, ...) \
  typedef ret_type (*func_name##_t)(__VA_ARGS__); \
  static func_name##_t func_name##_impl = NULL; \
  static void load_##func_name() { \
    if (!func_name##_impl) { \
      void* handle = dlopen("./libcold.so", RTLD_LAZY); \
      if (!handle) { \
        fprintf(stderr, "NexusLink error: %s\n", dlerror()); \
        exit(1); \
      } \
      func_name##_impl = (func_name##_t)dlsym(handle, #func_name); \
      if (!func_name##_impl) { \
        fprintf(stderr, "NexusLink symbol error: %s\n", dlerror()); \
        exit(1); \
      } \
    } \
  } \
  static ret_type func_name(__VA_ARGS__)