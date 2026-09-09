#pragma once

#include "common.h"
#include "map.h"
#include "process_access.h"

bool addresses_init(void);

bool code_edits_enable(void);
bool code_edits_disable(void);

void coord_conversion_matrix_init(void);

constexpr int LAYER_ARRAY_LENGTH = 80400;
typedef struct {
	u8 base_height_array[LAYER_ARRAY_LENGTH];
	u8 total_height_array[LAYER_ARRAY_LENGTH];
	u8 surface_array[LAYER_ARRAY_LENGTH];
	u32 logical_array[LAYER_ARRAY_LENGTH];
	BatchInjectionData *object_data;
} ConvertedMapData;
ConvertedMapData *converted_map_data_init(const Map *map, rng_state_t *rng_state, DynamicString *logs);
bool converted_map_data_inject(const ConvertedMapData *converted_map_data, const char *map_name, const char *map_description, bool save_map);
void converted_map_data_free(ConvertedMapData *converted_map_data);
