#pragma once
#include "utils.h"
#include "map.h"

extern const Coords four_direction_offsets[4];
extern const Coords eight_direction_offsets[8];

bool insert_blob(CoordsArray *blob_tiles, u32 blob_size, u32 *rng_state);
CoordsArray *get_ai_resource_spot_origins(void);
