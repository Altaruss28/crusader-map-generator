#pragma once
#include "utils.h"
#include "map.h"
#include "process_access.h"

bool init_addresses(void);

bool enable_code_edits(void);
bool disable_code_edits(void);

void init_translation_matrix(void);

enum {
	LAYER_ARRAY_TILE_COUNT = 80400,
};
typedef struct InjectionData {
	u8 base_height_array[LAYER_ARRAY_TILE_COUNT];
	u8 total_height_array[LAYER_ARRAY_TILE_COUNT];
	u8 surface_array[LAYER_ARRAY_TILE_COUNT];
	u32 logical_array[LAYER_ARRAY_TILE_COUNT];
	InjectionLoopArray *object_data;
} InjectionData;
InjectionData *translate_map(Map *map, u32 *rng_state, DynamicString *logs);
bool inject_map(InjectionData *injection_data, char *map_name, char *map_description, bool save_map);
void free_injection_data(InjectionData *injection_data);
