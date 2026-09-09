#include "common.h"
#include "config.h"
#include "generator_helpers.h"
#include "map.h"
#include "mirror.h"

bool generate_surfaces(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs)
{
	bool ret = false;

	CoordsArray *available_origins = nullptr;
	CoordsArray *claimed_tiles = nullptr;
	CoordsArray *expandable_tiles = nullptr;

	typeof(&map->tile_matrix[0]) tm = map->tile_matrix;

	if (!(available_origins = coords_array_init()))
		goto err;
	if (!(claimed_tiles = coords_array_init()))
		goto err;
	if (!(expandable_tiles = coords_array_init()))
		goto err;

	map_flag_matrix_t claimed_matrix[MAP_FLAG_MATRIX_WORD_COUNT];
	map_flag_matrix_clear_all(claimed_matrix);

	int feature_count_multiplier = config->feature_count_multiplier;

	Range core_count_range = config->surface_core_count_range;
	int core_count = random(rng_state, core_count_range.min, core_count_range.max) * feature_count_multiplier;
	int cores_placed = 0;

	Range core_size_range = config->surface_core_size_range;
	bool core_allow_mirror_overlap = config->surface_core_allow_mirror_overlap;

	int river_height_offset = config->surface_river_height_offset;
	int river_ford_transform_chance = config->surface_river_ford_transform_chance;

	int river_share = config->surface_river_share;
	int grass_dark_share = config->surface_grass_dark_share;
	int grass_medium_share = config->surface_grass_medium_share;
	int grass_light_share = config->surface_grass_light_share;
	int earth_and_stones_share = config->surface_earth_and_stones_share;
	int earth_share = config->surface_earth_share;
	int surface_share_count = river_share + grass_dark_share + grass_medium_share + grass_light_share + earth_and_stones_share + earth_share;

	for (int x = 0; x < MAP_SIZE; x++) {
		for (int y = 0; y < MAP_SIZE; y++) {
			if (tm[x][y].section != SECTION_VALID)
				continue;
			if (!coords_array_add(available_origins, x, y))
				goto err;
		}
	}

	while (cores_placed < core_count && available_origins->usage > 0) {
		int chosen_tile_index = random(rng_state, 0, available_origins->usage - 1);
		int x_origin = available_origins->data[chosen_tile_index].x;
		int y_origin = available_origins->data[chosen_tile_index].y;
		coords_array_remove(available_origins, chosen_tile_index);

		int core_width = random(rng_state, core_size_range.min, core_size_range.max);
		int core_length = random(rng_state, core_size_range.min, core_size_range.max);

		if (!map_check_bounds_rectangle(x_origin, y_origin, core_width, core_length))
			continue;

		for (int x = x_origin; x < x_origin + core_width; x++) {
			for (int y = y_origin; y < y_origin + core_length; y++) {
				if (tm[x][y].section != SECTION_VALID
				|| map_flag_matrix_test(claimed_matrix, x, y))
					goto next_core;
			}
		}

		if (!core_allow_mirror_overlap && mirror_check_overlap_rectangle(x_origin, y_origin, core_width, core_length))
			goto next_core;

		for (int x = x_origin; x < x_origin + core_width; x++) {
			for (int y = y_origin; y < y_origin + core_length; y++) {
				map_flag_matrix_set(claimed_matrix, x, y, true);
				if (x != x_origin && x != x_origin + core_width - 1 && y != y_origin && y != y_origin + core_length - 1)
					continue;
				if (!coords_array_add(expandable_tiles, x, y))
					goto err;
			}
		}

		cores_placed++;

		next_core:
	}

	if (cores_placed < core_count
	&& !dynamic_string_add(logs, "generate_surfaces: %d/%d surface cores placed\n", cores_placed, core_count))
		goto err;

	while (expandable_tiles->usage > 0) {
		int chosen_tile_index = random(rng_state, 0, expandable_tiles->usage - 1);
		int x_origin = expandable_tiles->data[chosen_tile_index].x;
		int y_origin = expandable_tiles->data[chosen_tile_index].y;
		coords_array_remove(expandable_tiles, chosen_tile_index);

		for (int offset_index = 0; offset_index < 4; offset_index++) {
			int x = x_origin + four_direction_offsets[offset_index].x;
			int y = y_origin + four_direction_offsets[offset_index].y;

			if (tm[x][y].section != SECTION_VALID
			|| map_flag_matrix_test(claimed_matrix, x, y))
				continue;

			map_flag_matrix_set(claimed_matrix, x, y, true);
			if (!coords_array_add(claimed_tiles, x, y))
				goto err;
			if (!coords_array_add(expandable_tiles, x, y))
				goto err;
		}
	}

	int tiles_per_share = claimed_tiles->usage / surface_share_count;
	int river_count = river_share * tiles_per_share;
	int grass_dark_count = grass_dark_share * tiles_per_share;
	int grass_medium_count = grass_medium_share * tiles_per_share;
	int grass_light_count = grass_light_share * tiles_per_share;
	int earth_and_stones_count = earth_and_stones_share * tiles_per_share;
	int earth_count = claimed_tiles->usage - (river_count + grass_dark_count + grass_medium_count + grass_light_count + earth_and_stones_count);

	for (int tile_index = claimed_tiles->usage - 1; tile_index >= 0; tile_index--) {
		int x = claimed_tiles->data[tile_index].x;
		int y = claimed_tiles->data[tile_index].y;

		Surface surface = SURFACE_EARTH;

		if (river_count > 0) {
			surface = SURFACE_RIVER;
			river_count--;
		} else if (grass_dark_count > 0) {
			surface = SURFACE_GRASS_DARK;
			grass_dark_count--;
		} else if (grass_medium_count > 0) {
			surface = SURFACE_GRASS_MEDIUM;
			grass_medium_count--;
		} else if (grass_light_count > 0) {
			surface = SURFACE_GRASS_LIGHT;
			grass_light_count--;
		} else if (earth_and_stones_count > 0) {
			surface = SURFACE_EARTH_AND_STONES;
			earth_and_stones_count--;
		} else if (earth_count > 0) {
			surface = SURFACE_EARTH;
			earth_count--;
		}

		if (tm[x][y].feature != FEATURE_NONE)
			continue;

		if (surface == SURFACE_RIVER) {
			map_set_feature(map, x, y, FEATURE_RIVER);
			map_set_height(map, x, y, tm[x][y].height + river_height_offset);
		} else if (surface == SURFACE_GRASS_DARK || surface == SURFACE_GRASS_MEDIUM || surface == SURFACE_GRASS_LIGHT) {
			map_set_feature(map, x, y, FEATURE_GRASSLAND);
		}

		map_set_surface(map, x, y, surface);
	}

	for (int x_origin = 0; x_origin < MAP_SIZE; x_origin++) {
		for (int y_origin = 0; y_origin < MAP_SIZE; y_origin++) {
			if (tm[x_origin][y_origin].section != SECTION_VALID
			|| tm[x_origin][y_origin].feature != FEATURE_RIVER
			|| tm[x_origin][y_origin].surface != SURFACE_RIVER)
				continue;

			bool next_to_ramp = false;
			bool next_to_non_river = false;

			for (int offset_index = 0; offset_index < 8; offset_index++) {
				int x = x_origin + eight_direction_offsets[offset_index].x;
				int y = y_origin + eight_direction_offsets[offset_index].y;

				if (tm[x][y].section != SECTION_VALID)
					continue;

				Feature feature = tm[x][y].feature;

				if (feature == FEATURE_RIVER)
					continue;

				next_to_non_river = true;

				if (feature != FEATURE_RAMP)
					continue;

				next_to_ramp = true;
				break;
			}

			if (!next_to_ramp
			&& (next_to_non_river || river_ford_transform_chance < random(rng_state, 1, 100)))
				continue;

			map_set_surface(map, x_origin, y_origin, SURFACE_FORD);
		}
	}

	for (int x_origin = 0; x_origin < MAP_SIZE; x_origin++) {
		for (int y_origin = 0; y_origin < MAP_SIZE; y_origin++) {
			if (tm[x_origin][y_origin].section != SECTION_SACRIFICED
			|| tm[x_origin][y_origin].feature != FEATURE_NONE)
				continue;

			bool special_surface_found = false;

			Feature feature;
			int height;
			Surface surface;

			for (int offset_index = 0; offset_index < 8; offset_index++) {
				int x = x_origin + eight_direction_offsets[offset_index].x;
				int y = y_origin + eight_direction_offsets[offset_index].y;

				if (tm[x][y].section != SECTION_VALID
				|| (tm[x][y].feature != FEATURE_RIVER
				&& tm[x][y].feature != FEATURE_GRASSLAND))
					continue;

				special_surface_found = true;

				feature = tm[x][y].feature;
				height = tm[x][y].height;
				surface = tm[x][y].surface;

				break;
			}

			if (!special_surface_found)
				continue;

			tm[x_origin][y_origin].feature = feature;
			tm[x_origin][y_origin].height = height;
			tm[x_origin][y_origin].surface = surface;
		}
	}

	ret = true;

err:
	coords_array_free(expandable_tiles);
	coords_array_free(claimed_tiles);
	coords_array_free(available_origins);
	return ret;
}
