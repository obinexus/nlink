// include/nlink/core/semverx/nexus_version.h
// OBINexus Nexus Version: Advanced Version Coordination Interface
#ifndef OBINEXUS_NLINK_CORE_SEMVERX_NEXUS_VERSION_H
#define OBINEXUS_NLINK_CORE_SEMVERX_NEXUS_VERSION_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constitutional Nexus Version Information
const char *nexus_get_version(void);
const char *nexus_get_build_date(void);
const char *nexus_get_build_time(void);
const char *nexus_get_commit_hash(void);

// Constitutional Version Validation
int nexus_validate_version_compatibility(const char *required_version);
bool nexus_is_development_build(void);
bool nexus_has_debug_symbols(void);

// Constitutional Build Information
void nexus_print_version_info(void);
void nexus_print_build_configuration(void);

// Advanced Version Coordination
uint32_t nexus_get_api_version(void);
uint32_t nexus_get_abi_version(void);
bool nexus_is_api_compatible(uint32_t required_api_version);
bool nexus_is_abi_compatible(uint32_t required_abi_version);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_SEMVERX_NEXUS_VERSION_H
