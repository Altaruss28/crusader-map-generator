#pragma once

#include <time.h>

#include <windows.h>

#include "common.h"
#include "config.h"

int cpu_core_count_get(void);

typedef struct {
	Config *config;
	int *map_attempt_counter;
	CRITICAL_SECTION *map_attempt_counter_cs;
	u32 start_time_win;
	time_t start_time_unix;
	CRITICAL_SECTION *injection_cs;
	CRITICAL_SECTION *stdout_cs;
} GeneratorThreadArguments;
DWORD WINAPI generator_thread(LPVOID params);
