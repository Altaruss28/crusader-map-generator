#pragma once

#include "common.h"

extern const Coords four_direction_offsets[4];
extern const Coords eight_direction_offsets[8];

bool genhelp_insert_blob(CoordsArray *blob_tiles, int step_counter, int grow_counter, rng_state_t *rng_state);
bool genhelp_insert_ai_resource_spot_origins(CoordsArray *origins);
