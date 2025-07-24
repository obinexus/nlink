// include/nlink/core/semverx/lazy_versioned.h
// OBINexus Lazy Versioned: Deferred Version Resolution Interface
#ifndef OBINEXUS_NLINK_CORE_SEMVERX_LAZY_VERSIONED_H
#define OBINEXUS_NLINK_CORE_SEMVERX_LAZY_VERSIONED_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constitutional Lazy Resource Management
int lazy_versioned_register(const char *resource_id, const char *version);
void *lazy_versioned_load(const char *resource_id);
void lazy_versioned_unload(const char *resource_id);
void lazy_versioned_cleanup(void);

// Advanced Lazy Coordination
bool lazy_versioned_is_loaded(const char *resource_id);
const char *lazy_versioned_get_version(const char *resource_id);
size_t lazy_versioned_get_resource_count(void);

// Constitutional Resource Statistics
uint64_t lazy_versioned_get_access_count(const char *resource_id);
void lazy_versioned_reset_statistics(void);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_SEMVERX_LAZY_VERSIONED_H
