#pragma once
#include "utils.h"
#include "config.h"
#include <time.h>
#include <windows.h>

bool get_cpu_core_count(u32 *cpu_core_count);

typedef struct GeneratorThreadArguments {
	Config *config;
	u32 *run_counter;
	u32 start_time_win;
	time_t start_time_unix;
	char *version_string;
	CRITICAL_SECTION *run_counter_access;
	CRITICAL_SECTION *injection_access;
	CRITICAL_SECTION *stdout_access;
} GeneratorThreadArguments;
DWORD WINAPI generator_thread(LPVOID param);
