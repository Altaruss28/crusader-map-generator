#include "common.h"
#include "config.h"
#include "generator_helpers.h"
#include "map.h"
#include "mirror.h"

bool generate_quarry_spots(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs)
{
	bool ret = false;

	CoordsArray *available_origins = nullptr;
	CoordsArray *spot_tile_offsets = nullptr;
	CoordsArray *spot_tile_coords = nullptr;

	typeof(&map->tile_matrix[0]) tm = map->tile_matrix;

	int feature_count_multiplier = config->feature_count_multiplier;

	Range spot_count_range = config->quarry_spot_count_range;
	int spot_count = random(rng_state, spot_count_range.min, spot_count_range.max) * feature_count_multiplier;
	int spots_placed = 0;

	Range spot_size_range = config->quarry_spot_size_range;

	int stone_share = config->quarry_spot_stone_share;
	int gravel_share = config->quarry_spot_gravel_share;
	int grass_light_share = config->quarry_spot_grass_light_share;
	int earth_and_stones_share = config->quarry_spot_earth_and_stones_share;

	int surface_share_count = stone_share + gravel_share + grass_light_share + earth_and_stones_share;

	if (!(available_origins = coords_array_init()))
		goto err;

	for (int x = 0; x < MAP_SIZE; x++) {
		for (int y = 0; y < MAP_SIZE; y++) {
			if (tm[x][y].section != SECTION_VALID
			|| tm[x][y].feature != FEATURE_NONE)
				continue;

			if (!coords_array_add(available_origins, x, y))
				goto err;
		}
	}

	if (available_origins->usage > 0) {
		coords_array_shuffle(available_origins, rng_state);

		if (!(spot_tile_offsets = coords_array_init()))
			goto err;
		if (!(spot_tile_coords = coords_array_init()))
			goto err;

		for (int spot_index = 0; spot_index < spot_count; spot_index++) {
			int spot_size = random(rng_state, spot_size_range.min, spot_size_range.max);
			if (!genhelp_insert_blob(spot_tile_offsets, 0, spot_size, rng_state))
				goto err;

			for (usize origin_index = 0; origin_index < available_origins->usage; origin_index++) {
				int x_origin = available_origins->data[origin_index].x;
				int y_origin = available_origins->data[origin_index].y;

				int origin_height = tm[x_origin][y_origin].height;
				bool origin_is_valid = true;

				spot_tile_coords->usage = 0;

				for (usize tile_index = 0; tile_index < spot_tile_offsets->usage; tile_index++) {
					int x = x_origin + spot_tile_offsets->data[tile_index].x;
					int y = y_origin + spot_tile_offsets->data[tile_index].y;

					if (!map_check_bounds_point(x, y)
					|| tm[x][y].section != SECTION_VALID
					|| tm[x][y].feature != FEATURE_NONE
					|| tm[x][y].height != origin_height) {
						origin_is_valid = false;
						break;
					}

					if (!coords_array_add(spot_tile_coords, x, y))
						goto err;
				}

				if (!origin_is_valid)
					continue;
				if (mirror_check_overlap_array(spot_tile_coords))
					continue;

				int tiles_per_share = spot_size / surface_share_count;

				int stone_count = stone_share * tiles_per_share;
				int gravel_count = gravel_share * tiles_per_share;
				int grass_light_count = grass_light_share * tiles_per_share;
				int earth_and_stones_count = spot_size - (stone_count + gravel_count + grass_light_count);

				for (usize tile_index = 0; tile_index < spot_tile_coords->usage; tile_index++) {
					int x = spot_tile_coords->data[tile_index].x;
					int y = spot_tile_coords->data[tile_index].y;

					Surface current_surface = SURFACE_EARTH;

					if (stone_count > 0) {
						current_surface = SURFACE_STONE;
						stone_count--;
					} else if (gravel_count > 0) {
						current_surface = SURFACE_GRAVEL;
						gravel_count--;
					} else if (grass_light_count > 0) {
						current_surface = SURFACE_GRASS_LIGHT;
						grass_light_count--;
					} else if (earth_and_stones_count > 0) {
						current_surface = SURFACE_EARTH_AND_STONES;
						earth_and_stones_count--;
					}

					if (current_surface != SURFACE_EARTH_AND_STONES)
						map_set_feature(map, x, y, FEATURE_QUARRY_SPOT);

					map_set_surface(map, x, y, current_surface);
				}

				spots_placed++;
				break;
			}
		}
	}

	if (spots_placed < spot_count
	&& !dynamic_string_add(logs, "generate_quarry_spots: %d/%d quarry spots placed\n", spots_placed, spot_count))
		goto err;

	ret = true;

err:
	coords_array_free(spot_tile_coords);
	coords_array_free(spot_tile_offsets);
	coords_array_free(available_origins);
	return ret;
}
