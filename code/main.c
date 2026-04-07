#include "utils.h"
#include "mirror.h"
#include "config.h"
#include "process_access.h"
#include "injection.h"
#include "threads.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

static char version_string[] = "v0";

int main(void)
{
	u32 cpu_core_count = 0;
	
	init_translation_matrix();
	
	printf(
		"%s\n"
		"\n"
		"Enter \"r\" to run the generator\n"
		"Enter \"q\" to quit the program\n"
		"\n",
		version_string
	);
	
	while (true) {
		
		printf(">");
		
		char input_str[256];
		
		if (fgets(input_str, sizeof(input_str), stdin) != NULL) {
			
			u32 char_offset = 0;
			while (input_str[char_offset] != '\0') char_offset++;
			
			if (char_offset > 0 && input_str[char_offset - 1] == '\n') {
				input_str[char_offset - 1] = '\0';
			}
			
		}
		
		if (strcmp(input_str, "q") == 0) break;
		
		if (strcmp(input_str, "r") != 0) {
			printf("Unknown command\n");
			continue;
		}
		
		bool run_completed = false;
		
		u32 start_time_win = GetTickCount();
		time_t start_time_unix = time(NULL);
		
		Config *config = NULL;
		
		HANDLE *thread_handles = NULL;
		
		CRITICAL_SECTION run_counter_access;
		CRITICAL_SECTION injection_access;
		CRITICAL_SECTION stdout_access;
		BOOL cs_rca_ret = FALSE;
		BOOL cs_ia_ret = FALSE;
		BOOL cs_sa_ret = FALSE;
		
		if (!(config = init_config())) {
			printf("Error: init_config failed\n");
			goto cleanup;
		}
		
		if (!enable_process_access()) {
			printf("Error: enable_process_access failed - make sure the game is running\n");
			goto cleanup;
		}
		
		if (!init_addresses()) {
			printf("Error: init_addresses failed - use a compatible game version\n");
			goto cleanup;
		}
		
		if (!enable_code_edits()) {
			printf("Error: enable_code_edits failed - restart the game before further use\n");
			goto cleanup;
		}
		
		if (!cpu_core_count && !get_cpu_core_count(&cpu_core_count)) {
			printf("Error: get_cpu_core_count failed\n");
			goto cleanup;
		}
		
		cs_rca_ret = InitializeCriticalSectionEx(&run_counter_access, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
		cs_ia_ret = InitializeCriticalSectionEx(&injection_access, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
		cs_sa_ret = InitializeCriticalSectionEx(&stdout_access, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
		if (!cs_rca_ret || !cs_ia_ret || !cs_sa_ret) {
			printf("Error: InitializeCriticalSectionEx failed\n");
			goto cleanup;
		}
		
		init_mirror(config);
		
		u32 run_counter = 0;
		
		GeneratorThreadArguments generator_thread_arguments = {
			.config = config,
			.run_counter = &run_counter,
			.start_time_win = start_time_win,
			.start_time_unix = start_time_unix,
			.version_string = version_string,
			.run_counter_access = &run_counter_access,
			.injection_access = &injection_access,
			.stdout_access = &stdout_access,
		};
		
		u32 map_count = config->map_count;
		u32 thread_count = map_count < cpu_core_count ? map_count : cpu_core_count;
		
		if (!(thread_handles = (HANDLE *)malloc(sizeof(HANDLE) * thread_count))) {
			printf("Error: malloc failed\n");
			goto cleanup;
		}
		
		u32 threads_run = 0;
		bool thread_error = false;
		
		printf("Generator started\n");
		
		for (u32 i = 0; i < thread_count; i++) {
			thread_handles[i] = CreateThread(NULL, 0, generator_thread, (LPVOID)&generator_thread_arguments, 0, NULL);
			if (thread_handles[i]) continue;
			thread_handles[i] = INVALID_HANDLE_VALUE;
		}
		
		for (u32 i = 0; i < thread_count; i++) {
			if (thread_handles[i] == INVALID_HANDLE_VALUE) continue;
			WaitForSingleObject(thread_handles[i], INFINITE);
		}
		
		for (u32 i = 0; i < thread_count; i++) {
			if (thread_handles[i] == INVALID_HANDLE_VALUE) continue;
			
			threads_run++;
			
			DWORD exit_code;
			GetExitCodeThread(thread_handles[i], &exit_code);
			if (!exit_code) {
				thread_error = true;
			}
			
			CloseHandle(thread_handles[i]);
		}
		
		if (threads_run < thread_count) {
			printf("Error: could not run target number of threads\n");
		}
		
		if (thread_error) {
			printf("Error: thread returned failure - verify and discard any corrupted output\n");
		}
		
		run_completed = true;
		
cleanup:
		
		free(thread_handles);
		
		if (cs_sa_ret) DeleteCriticalSection(&stdout_access);
		if (cs_ia_ret) DeleteCriticalSection(&injection_access);
		if (cs_rca_ret) DeleteCriticalSection(&run_counter_access);
		
		if (!disable_code_edits()) {
			printf("Error: disable_code_edits failed - restart the game before further use\n");
		}
		
		disable_process_access();
		
		free_config(config);
		
		if (!run_completed) continue;
		
		u32 run_time_win = GetTickCount() - start_time_win;
		
		printf("Generator finished, time elapsed: %d%s\n", run_time_win < 1000 ? run_time_win : run_time_win / 1000, run_time_win < 1000 ? "ms" : "s");
		
	}
	
	return 0;
}
