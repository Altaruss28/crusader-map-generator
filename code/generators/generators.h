#pragma once
#include "utils.h"
#include "map.h"
#include "config.h"

bool generate_heights(Map *map, Config *config, u32 *rng_state, DynamicString *logs);
bool generate_border(Map *map, Config *config, u32 *rng_state, DynamicString *logs);
bool generate_grasslands(Map *map, Config *config, u32 *rng_state, DynamicString *logs);
bool generate_keeps(Map *map, Config *config, u32 *rng_state, DynamicString *logs);
bool generate_quarry_spots(Map *map, Config *config, u32 *rng_state, DynamicString *logs);
bool generate_iron_mine_spots(Map *map, Config *config, u32 *rng_state, DynamicString *logs);
bool generate_pitch_rig_spots(Map *map, Config *config, u32 *rng_state, DynamicString *logs);
bool generate_trees(Map *map, Config *config, u32 *rng_state, DynamicString *logs);
bool generate_animals(Map *map, Config *config, u32 *rng_state, DynamicString *logs);
bool generate_shrubs(Map *map, Config *config, u32 *rng_state, DynamicString *logs);
bool generate_texturing(Map *map, Config *config, u32 *rng_state, DynamicString *logs);
