#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <windows.h>

#include "common.h"
#include "config.h"
#include "injection.h"
#include "mirror.h"
#include "process_access.h"
#include "thread_related.h"
#include "version.h"

int main(void)
{
	int cpu_core_count = 0;

	coord_conversion_matrix_init();

	printf(
		VERSION_STR "\n"
		"\n"
		"Enter \"r\" to run the generator\n"
		"Enter \"q\" to quit the program\n"
		"\n"
	);

	while (true) {
		printf(">");

		char input_str[256] = {};
		fgets(input_str, sizeof(input_str), stdin);

		int input_char_offset = 0;
		while (input_str[input_char_offset] != '\0')
			++input_char_offset;
		if (input_char_offset > 0 && input_str[input_char_offset - 1] == '\n')
			input_str[input_char_offset - 1] = '\0';

		if (strcmp(input_str, "q") == 0) {
			break;
		} else if (strcmp(input_str, "r") != 0) {
			printf("Unknown command\n");
			continue;
		}

		bool run_completed = false;

		u32 start_time_win = GetTickCount();
		time_t start_time_unix = time(nullptr);

		Config *config = nullptr;

		CRITICAL_SECTION map_attempt_counter_cs;
		CRITICAL_SECTION injection_cs;
		CRITICAL_SECTION stdout_cs;
		bool map_attempt_counter_cs_is_init = false;
		bool injection_cs_is_init = false;
		bool stdout_cs_is_init = false;

		HANDLE *thread_handles = nullptr;

		if (!(config = config_init())) {
			printf("Error: config_init failed\n");
			goto err;
		}
		mirror_init(config->mirror_mode);

		if (!process_access_enable()) {
			printf("Error: process_access_enable failed - make sure the game is running\n");
			goto err;
		}

		if (!addresses_init()) {
			printf("Error: addresses_init failed - use a compatible game version\n");
			goto err;
		}
		if (!code_edits_enable()) {
			printf("Error: code_edits_enable failed - restart the game before further use\n");
			goto err;
		}

		if (!cpu_core_count && !(cpu_core_count = cpu_core_count_get())) {
			printf("Error: cpu_core_count_get failed\n");
			goto err;
		}

		map_attempt_counter_cs_is_init = InitializeCriticalSectionEx(&map_attempt_counter_cs, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
		injection_cs_is_init = InitializeCriticalSectionEx(&injection_cs, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
		stdout_cs_is_init = InitializeCriticalSectionEx(&stdout_cs, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
		if (!map_attempt_counter_cs_is_init || !injection_cs_is_init || !stdout_cs_is_init) {
			printf("Error: InitializeCriticalSectionEx failed\n");
			goto err;
		}

		int map_count = config->map_count;
		int thread_count = map_count < cpu_core_count ? map_count : cpu_core_count;

		if (!(thread_handles = malloc(sizeof(HANDLE) * thread_count))) {
			printf("Error: malloc failed\n");
			goto err;
		}

		printf("Generator started\n");

		int map_attempt_counter = 0;
		GeneratorThreadArguments thread_args = {
			.config = config,
			.map_attempt_counter = &map_attempt_counter,
			.map_attempt_counter_cs = &map_attempt_counter_cs,
			.start_time_win = start_time_win,
			.start_time_unix = start_time_unix,
			.injection_cs = &injection_cs,
			.stdout_cs = &stdout_cs,
		};

		for (int i = 0; i < thread_count; ++i)
			if (!(thread_handles[i] = CreateThread(nullptr, 0, generator_thread, (LPVOID)&thread_args, 0, nullptr)))
				thread_handles[i] = INVALID_HANDLE_VALUE;

		for (int i = 0; i < thread_count; ++i)
			if (thread_handles[i] != INVALID_HANDLE_VALUE)
				WaitForSingleObject(thread_handles[i], INFINITE);

		int threads_run = 0;
		bool thread_error = false;

		for (int i = 0; i < thread_count; ++i) {
			if (thread_handles[i] == INVALID_HANDLE_VALUE)
				continue;
			++threads_run;

			DWORD exit_code;
			GetExitCodeThread(thread_handles[i], &exit_code);
			if (!exit_code)
				thread_error = true;

			CloseHandle(thread_handles[i]);
		}

		if (threads_run == 0)
			printf("Error: could not run any generator threads\n");
		else if (threads_run < thread_count)
			printf("Warning: could not run target number of generator threads\n");

		if (thread_error)
			printf("Error: generator thread returned failure - verify and discard any corrupted output\n");

		run_completed = true;

err:
		free(thread_handles);

		if (stdout_cs_is_init)
			DeleteCriticalSection(&stdout_cs);
		if (injection_cs_is_init)
			DeleteCriticalSection(&injection_cs);
		if (map_attempt_counter_cs_is_init)
			DeleteCriticalSection(&map_attempt_counter_cs);

		if (!code_edits_disable())
			printf("Error: code_edits_disable failed - restart the game before further use\n");

		process_access_disable();

		config_free(config);

		if (!run_completed)
			continue;

		u32 run_time_win = GetTickCount() - start_time_win;
		bool is_below_second = run_time_win < 1000;
		printf("Generator finished, time elapsed: %d%s\n", is_below_second ? run_time_win : run_time_win / 1000, is_below_second ? "ms" : "s");
	}

	return 0;
}
