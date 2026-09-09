#include "common.h"
#include "config.h"
#include "generator_helpers.h"
#include "map.h"

bool generate_texturing(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs)
{
	(void)logs;

	bool ret = false;

	CoordsArray *valid_tiles = nullptr;
	CoordsArray *blob_tiles = nullptr;

	typeof(&map->tile_matrix[0]) tm = map->tile_matrix;

	struct {
		Surface surface;
		Range blob_count_range;
		Range blob_size_range;
	} settings[] = {
		{SURFACE_BEACH, config->texturing_beach_blob_count_range, config->texturing_beach_blob_size_range},
		{SURFACE_DUNES, config->texturing_dunes_blob_count_range, config->texturing_dunes_blob_size_range},
		{SURFACE_EARTH_AND_STONES, config->texturing_earth_and_stones_blob_count_range, config->texturing_earth_and_stones_blob_size_range},
		{SURFACE_GRASS_LIGHT, config->texturing_grass_light_blob_count_range, config->texturing_grass_light_blob_size_range},
		{SURFACE_GRASS_MEDIUM, config->texturing_grass_medium_blob_count_range, config->texturing_grass_medium_blob_size_range},
	};

	if (!(valid_tiles = coords_array_init()))
		goto err;

	for (int x = 0; x < MAP_SIZE; x++) {
		for (int y = 0; y < MAP_SIZE; y++) {
			if (tm[x][y].section != SECTION_VALID)
				continue;
			if (!coords_array_add(valid_tiles, x, y))
				goto err;
		}
	}

	if (!(blob_tiles = coords_array_init()))
		goto err;

	for (usize setting_index = 0; setting_index < sizeof(settings) / sizeof(settings[0]); setting_index++) {
		Surface current_surface = settings[setting_index].surface;

		Range blob_count_range = settings[setting_index].blob_count_range;
		int blob_count = random(rng_state, blob_count_range.min, blob_count_range.max);

		Range blob_size_range = settings[setting_index].blob_size_range;

		for (int blob_index = 0; blob_index < blob_count; blob_index++) {
			if (!genhelp_insert_blob(blob_tiles, 0, random(rng_state, blob_size_range.min, blob_size_range.max), rng_state))
				goto err;

			int chosen_tile_index = random(rng_state, 0, valid_tiles->usage - 1);
			int x_origin = valid_tiles->data[chosen_tile_index].x;
			int y_origin = valid_tiles->data[chosen_tile_index].y;

			for (usize tile_index = 0; tile_index < blob_tiles->usage; tile_index++) {
				int x = x_origin + blob_tiles->data[tile_index].x;
				int y = y_origin + blob_tiles->data[tile_index].y;

				if (!map_check_bounds_point(x, y)
				|| tm[x][y].section != SECTION_VALID
				|| tm[x][y].feature == FEATURE_RAMP
				|| tm[x][y].surface != SURFACE_EARTH)
					continue;

				tm[x][y].surface = current_surface;
			}
		}
	}

	ret = true;

err:
	coords_array_free(blob_tiles);
	coords_array_free(valid_tiles);
	return ret;
}
