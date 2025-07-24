// include/nlink/core/etps/etps_telemetry.h
// OBINexus ETPS: Entropy-Adaptive Telemetry Polymorphic Coordination
#ifndef OBINEXUS_NLINK_CORE_ETPS_ETPS_TELEMETRY_H
#define OBINEXUS_NLINK_CORE_ETPS_ETPS_TELEMETRY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <nlink/core/etps/etps_types.h>
#include <nlink/core/etps/hotswap_types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constitutional Interface Functions: Polymorphic Coordination Protocol
etps_result_t etps_initialize(void);
etps_result_t etps_shutdown(void);
etps_result_t etps_create_context(etps_context_t **ctx);
etps_result_t etps_destroy_context(etps_context_t *ctx);
etps_result_t etps_configure_telemetry(etps_context_t *ctx, const char *config);
etps_result_t etps_start_telemetry_session(etps_context_t *ctx);
etps_result_t etps_stop_telemetry_session(etps_context_t *ctx);

// Hotswap Coordination Protocol
hotswap_result_t etps_hotswap_prepare(etps_context_t *ctx);
hotswap_result_t etps_hotswap_execute(etps_context_t *ctx);
hotswap_result_t etps_hotswap_validate(etps_context_t *ctx);

// Advanced ETPS Coordination
etps_result_t etps_register_component(etps_context_t *ctx, const char *component_id);
etps_result_t etps_log_event(etps_context_t *ctx, const char *event_type, const char *event_data);
etps_result_t etps_get_statistics(etps_context_t *ctx, void *stats_buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif // OBINEXUS_NLINK_CORE_ETPS_ETPS_TELEMETRY_H
