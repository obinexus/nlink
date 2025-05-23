#ifndef NLINK_CORE_VERSIONING_LAZY_VERSIONED_H
#define NLINK_CORE_VERSIONING_LAZY_VERSIONED_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include  "nexus_lazy_versioned.h"
#include "nlink/core/common/nexus_core.h"

typedef struct {
	void** handles;
	char** paths;
	char** components;
	size_t count;
	pthread_mutex_t mutex;
} NexusHandleRegistry;

typedef struct {
	bool auto_unload;
	time_t unload_timeout_sec;
} NexusVersionedLazyConfig;

extern NexusVersionedLazyConfig nexus_versioned_lazy_config;
extern NexusHandleRegistry* nexus_handle_registry;

#endif // NLINK_CORE_VERSIONING_LAZY_VERSIONED_H
