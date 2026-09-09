#pragma once

#include "common.h"
#include "config.h"
#include "map.h"

bool generate_heights(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs);
bool generate_surfaces(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs);
bool generate_keeps(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs);
bool generate_ruins(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs);
bool generate_quarry_spots(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs);
bool generate_iron_mine_spots(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs);
bool generate_pitch_rig_spots(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs);
bool generate_trees(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs);
bool generate_animals(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs);
bool generate_shrubs(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs);
bool generate_texturing(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs);
