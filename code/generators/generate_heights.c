#include "common.h"
#include "config.h"
#include "generator_helpers.h"
#include "map.h"
#include "mirror.h"

bool generate_heights(Map *map, Config *config, rng_state_t *rng_state, DynamicString *logs)
{
	bool ret = false;

	CoordsArray *plateau_expandable_tiles = nullptr;
	CoordsArray *plateau_available_tiles = nullptr;
	CoordsArray *plateau_core_middles = nullptr;
	CoordsArray *ramp_origins = nullptr;
	CoordsArray *ramp_check_tiles = nullptr;
	CoordsArray *ramp_current_tiles = nullptr;
	CoordsArray *ramp_next_tiles = nullptr;
	CoordsArray *cliff_current_tiles = nullptr;
	CoordsArray *cliff_next_tiles = nullptr;
	CoordsArray *cliff_grown_tiles = nullptr;
	CoordsArray *sacrificed_tiles = nullptr;

	typeof(&map->tile_matrix[0]) tm = map->tile_matrix;

	int feature_count_multiplier = config->feature_count_multiplier;

	Range core_count_range = config->plateau_core_count_range;
	int target_core_count = random(rng_state, core_count_range.min, core_count_range.max) * feature_count_multiplier;
	if (target_core_count <= 0)
		goto out;

	int placed_core_count = 0;

	Range core_size_range = config->plateau_core_size_range;
	int core_spacing_min = config->plateau_core_spacing_min;
	bool core_allow_mirror_overlap = config->plateau_core_allow_mirror_overlap;

	int *plateau_heights = config->plateau_heights->data;
	int plateau_height_count = config->plateau_heights->usage;

	map_flag_matrix_t claimed_matrix[MAP_FLAG_MATRIX_WORD_COUNT] = {};

	if (!(plateau_available_tiles = coords_array_init())
		|| !(plateau_expandable_tiles = coords_array_init())
		|| !(plateau_core_middles = coords_array_init()))
		goto err;

	for (int x = 0; x < MAP_SIZE; ++x)
		for (int y = 0; y < MAP_SIZE; ++y) {
			if (tm[x][y].section != SECTION_VALID)
				continue;
			if (!coords_array_add(plateau_available_tiles, x, y))
				goto err;
		}

	while (placed_core_count < target_core_count && plateau_available_tiles->usage > 0) {
		int tile_index = random(rng_state, 0, plateau_available_tiles->usage - 1);
		Coords core_origin = plateau_available_tiles->data[tile_index];
		coords_array_remove(plateau_available_tiles, tile_index);

		int core_width = random(rng_state, core_size_range.min, core_size_range.max);
		int core_length = random(rng_state, core_size_range.min, core_size_range.max);

		Coords spaced_origin = {core_origin.x - core_spacing_min, core_origin.y - core_spacing_min};
		int spaced_width = core_width + (core_spacing_min * 2);
		int spaced_length = core_length + (core_spacing_min * 2);

		if (!map_check_bounds_rectangle(x_base, y_base, spaced_width, spaced_length))
			continue;

		if (!core_allow_mirror_overlap) {
			int half_spacing = core_spacing_min % 2 ? (core_spacing_min / 2) + 1 : core_spacing_min / 2;
			int full_spacing = half_spacing * 2;
			if (mirror_check_overlap_rectangle(core_origin.x - half_spacing, core_origin.y - half_spacing, core_width + full_spacing, core_length + full_spacing))
				continue;
		}

		for (int x = spaced_origin.x; x < spaced_origin.x + spaced_width; ++x)
			for (int y = spaced_origin.y; y < spaced_origin.y + spaced_length; ++y) {
				if (tm[x][y].section != SECTION_VALID
					|| map_flag_matrix_test(claimed_matrix, x, y))
					goto next_core;
			}

		int height = placed_core_count < plateau_height_count ? plateau_heights[placed_core_count] : plateau_heights[random(rng_state, 0, plateau_height_count - 1)];

		for (int x = core_origin.x; x < core_origin.x + core_width; ++x)
			for (int y = core_origin.y; y < core_origin.y + core_length; ++y) {
				map_set_height(map, x, y, height);
				map_flag_matrix_set(claimed_matrix, x, y, true);

				if ((x == core_origin.x || x == core_origin.x + core_width - 1 || y == core_origin.y || y == core_origin.y + core_length - 1)
					|| !coords_array_add(plateau_expandable_tiles, x, y))
					goto err;
			}

		if (!coords_array_add(plateau_core_middles, core_origin.x + (core_width / 2), core_origin.y + (core_length / 2)))
			goto err;

		++placed_core_count;

		next_core:
	}

	if (placed_core_count != target_core_count
		&& !dynamic_string_add(logs, "generate_heights: %d/%d plateau cores placed\n", placed_core_count, target_core_count))
		goto err;

	while (plateau_expandable_tiles->usage > 0) {
		int tile_index = random(rng_state, 0, plateau_expandable_tiles->usage - 1);
		Coords origin = plateau_expandable_tiles->data[tile_index];
		coords_array_remove(plateau_expandable_tiles, tile_index);

		int height = tm[origin.x][origin.y].height;

		for (usize i = 0; i < ARRAY_LENGTH(four_direction_offsets); ++i) {
			int x = origin.x + four_direction_offsets[i].x;
			int y = origin.y + four_direction_offsets[i].y;

			if (tm[x][y].section != SECTION_VALID
				|| map_flag_matrix_test(claimed_matrix, x, y))
				continue;

			map_set_height(map, x, y, height);
			map_flag_matrix_set(claimed_matrix, x, y, true);
			if (!coords_array_add(plateau_expandable_tiles, x, y))
				goto err;
		}
	}

	for (int origin_x = 0; origin_x < MAP_SIZE; ++origin_x)
		for (int origin_y = 0; origin_y < MAP_SIZE; ++origin_y) {
			if (tm[origin_x][origin_y].section != SECTION_VALID)
				continue;

			int height = tm[origin_x][origin_y].height;

			for (usize i = 0; i < ARRAY_LENGTH(eight_direction_offsets); ++i) {
				int x = origin_x + eight_direction_offsets[i].x;
				int y = origin_y + eight_direction_offsets[i].y;

				if (tm[x][y].height + 16 >= height)
					continue;

				map_set_feature(map, origin_x, origin_y, FEATURE_CLIFF);
				map_set_surface(map, origin_x, origin_y, SURFACE_ROCKS);
				break;
			}
		}

	if (config->generate_ramps) {
		Range count_modifier_range = config->ramp_count_modifier_range;
		int base_max_distance_from_center = config->ramp_base_max_distance_from_center;
		Range size_range = config->ramp_size_range;

		if (!(ramp_origins = coords_array_init())
			|| !(ramp_check_tiles = coords_array_init())
			|| !(ramp_current_tiles = coords_array_init()))
			goto err;

		for (usize i = 0; i < plateau_core_middles->usage; ++i) {
			Coords core_middle = plateau_core_middles->data[i];

			ramp_origins->usage = 0;

			for (usize j = 0; j < ARRAY_LENGTH(eight_direction_offsets); ++j) {
				Coords step_offset = eight_direction_offsets[j];

				for (int step = 1; step < MAP_SIZE; ++step) {
					int x = core_middle.x + (step_offset.x * step);
					int y = core_middle.y + (step_offset.y * step);

					Tile *tile = &tm[x][y];

					if (tile->section != SECTION_VALID
						|| tile->distance_from_center > base_max_distance_from_center
						|| tile->feature == FEATURE_RAMP)
						break;

					if (tile->feature != FEATURE_CLIFF)
						continue;

					if (!coords_array_add(ramp_origins, x, y))
						goto err;

					break;
				}
			}

			int ramp_counter = random(rng_state, count_modifier_range.min, count_modifier_range.max);

			while (ramp_counter-- > 0 && ramp_origins->usage > 0) {
				int origin_index = random(rng_state, 0, ramp_origins->usage - 1);
				Coords ramp_origin = ramp_origins->data[origin_index];
				coords_array_remove(ramp_origins, origin_index);

				if (tm[ramp_origin.x][ramp_origin.y].feature == FEATURE_RAMP)
					continue;

				ramp_check_tiles->usage = 0;
				if (!coords_array_add(ramp_check_tiles, ramp_origin.x, ramp_origin.y))
					goto err;

				int target_size = random(rng_state, size_range.min, size_range.max);
				int current_size = 1;
				int target_height = tm[ramp_origin.x][ramp_origin.y].height;

				while (current_size < target_size && ramp_check_tiles->usage > 0) {
					int tile_index = random(rng_state, 0, ramp_check_tiles->usage - 1);
					Coords tile_origin = ramp_check_tiles->data[tile_index];
					coords_array_remove(ramp_check_tiles, tile_index);

					for (usize j = 0; j < ARRAY_LENGTH(four_direction_offsets), current_size < target_size; ++j) {
						int x = tile_origin.x + four_direction_offsets[j].x;
						int y = tile_origin.y + four_direction_offsets[j].y;

						Tile *tile = &tm[x][y];

						if (tile->distance_from_center > base_max_distance_from_center
							|| tile->feature != FEATURE_CLIFF
							|| tile->height != target_height)
							continue;

						map_set_feature(map, x, y, FEATURE_RAMP);
						map_set_surface(map, x, y, SURFACE_EARTH);

						if (!coords_array_add(ramp_check_tiles, x, y)
							|| !coords_array_add(ramp_current_tiles, x, y))
							goto err;

						++current_size;
					}
				}
			}
		}

		int step_height_base_difference = config->ramp_step_height_base_difference;
		Range step_height_variance_range = config->ramp_step_height_variance_range;

		if (!(ramp_next_tiles = coords_array_init()))
			goto err;

		while (ramp_current_tiles->usage > 0) {
			for (usize i = 0; i < ramp_current_tiles->usage; ++i) {
				Coords origin = ramp_current_tiles->data[i];

				int target_height = tm[origin.x][origin.y].height - step_height_base_difference + random(rng_state, step_height_variance_range.min, step_height_variance_range.max);

				for (usize j = 0; j < ARRAY_LENGTH(four_direction_offsets); ++j) {
					int x = origin.x + four_direction_offsets[j].x;
					int y = origin.y + four_direction_offsets[j].y;

					Tile *tile = &tm[x][y];

					if (tile->section != SECTION_VALID
						|| tile->height > target_height)
						continue;

					map_set_feature(map, x, y, FEATURE_RAMP);
					map_set_height(map, x, y, target_height);
					map_set_surface(map, x, y, SURFACE_EARTH);

					if (!coords_array_add(ramp_next_tiles, x, y))
						goto err;
				}
			}

			CoordsArray *swap_ptr = ramp_current_tiles;
			ramp_current_tiles = ramp_next_tiles;
			ramp_next_tiles = swap_ptr;
			ramp_next_tiles->usage = 0;
		}
	}

	if (config->generate_cliffs) {
		int step_height_difference = config->cliff_step_height_difference;
		Range growth_factor_range = config->cliff_growth_factor_range;

		if (!(cliff_current_tiles = coords_array_init())
			|| !(cliff_next_tiles = coords_array_init())
			|| !(cliff_grown_tiles = coords_array_init()))
			goto err;

		for (int x = 0; x < MAP_SIZE; ++x)
			for (int y = 0; y < MAP_SIZE; ++y) {
				Tile *tile = &tm[x][y];

				if (tile->section != SECTION_VALID
					|| tile->feature != FEATURE_CLIFF)
					continue;

				if (!coords_array_add(cliff_current_tiles, x, y))
					goto err;
			}

		while (cliff_current_tiles->usage > 0) {
			for (usize i = 0; i < cliff_current_tiles->usage; ++i) {
				Coords origin = cliff_current_tiles->data[i];

				int target_height = tm[origin.x][origin.y].height - step_height_difference;

				int growth_amount = random(rng_state, growth_factor_range.min, growth_factor_range.max);
				if (growth_amount == 0)
					continue;

				for (usize j = 0; j < ARRAY_LENGTH(four_direction_offsets); ++j) {
					for (int step = 1; step <= growth_amount; ++step) {
						int x = origin.x + (step * four_direction_offsets[j].x);
						int y = origin.y + (step * four_direction_offsets[j].y);

						if (!map_check_bounds_point(x, y))
							break;

						Tile *tile = &tm[x][y];

						if (tile->section != SECTION_VALID)
							break;

						Feature current_feature = tile->feature;
						int current_height = tile->height;

						if ((current_feature != FEATURE_CLIFF && current_height > target_height)
							|| (current_feature == FEATURE_CLIFF && current_height >= target_height))
							break;

						map_set_feature(map, x, y, FEATURE_CLIFF);
						map_set_height(map, x, y, target_height);
						map_set_surface(map, x, y, SURFACE_ROCKS);

						if (!coords_array_add(cliff_next_tiles, x, y)
							|| !coords_array_add(cliff_grown_tiles, x, y))
							goto err;
					}
				}
			}

			CoordsArray *swap_ptr = cliff_current_tiles;
			cliff_current_tiles = cliff_next_tiles;
			cliff_next_tiles = swap_ptr;
			cliff_next_tiles->usage = 0;
		}

		int check_stop_index = cliff_grown_tiles->usage;

		for (int i = 0; i < check_stop_index; ++i) {
			Coords origin = cliff_grown_tiles->data[i];

			int height = tm[origin.x][origin.y].height;

			for (usize j = 0; j < ARRAY_LENGTH(eight_direction_offsets); ++j) {
				int x = origin.x + eight_direction_offsets[j].x;
				int y = origin.y + eight_direction_offsets[j].y;

				if (tm[x][y].feature == FEATURE_CLIFF
					|| tm[x][y].height >= height)
					continue;

				map_set_feature(map, x, y, FEATURE_CLIFF);
				map_set_surface(map, x, y, SURFACE_ROCKS);

				if (!coords_array_add(cliff_grown_tiles, x, y))
					goto err;
			}
		}

		if (config->generate_cliff_rocks) {
			coords_array_shuffle(cliff_grown_tiles, rng_state);

			int target_count = (cliff_grown_tiles->usage / config->cliff_rock_count_modifier) * feature_count_multiplier;
			int placed_count = 0;

			for (int rock_size = 4; rock_size >= 1, placed_count < target_count; --rock_size)
				for (usize i = 0; i < cliff_grown_tiles->usage, placed_count < target_count; ++i) {
					Coords origin = cliff_grown_tiles->data[i];

					if (!map_check_bounds_rectangle(origin.x, origin.y, rock_size, rock_size))
						continue;

					int target_height = tm[origin.x][origin.y].height;

					for (int x = origin.x; x < origin.x + rock_size; ++x)
						for (int y = origin.y; y < origin.y + rock_size; ++y) {
							Tile *tile = &tm[x][y];

							if (tile->feature != FEATURE_CLIFF
								|| tile->height != target_height
								|| tile->object_flags & OF_ROCK)
								goto next_rock;
						}

					if (mirror_check_overlap_rectangle(origin.x, origin.y, rock_size, rock_size))
						continue;

					if (!map_place_rock(map, origin.x, origin.y, rock_size))
						goto err;

					++placed_count;

					next_rock:
				}
		}
	}

	if (!(sacrificed_tiles = coords_array_init()))
		goto err;

	for (int origin_x = 0; origin_x < MAP_SIZE; ++origin_x)
		for (int origin_y = 0; origin_y < MAP_SIZE; ++origin_y) {
			if (tm[origin_x][origin_y].section != SECTION_SACRIFICED)
				continue;

			bool valid_found = false;

			for (usize i = 0; i < ARRAY_LENGTH(eight_direction_offsets); ++i) {
				int x = origin_x + eight_direction_offsets[i].x;
				int y = origin_y + eight_direction_offsets[i].y;

				Tile *tile = &tm[x][y];

				if (tile->section != SECTION_VALID)
					|| (tile->feature != FEATURE_CLIFF && tile->feature != FEATURE_RAMP))
					continue;

				valid_found = true;
				break;
			}

			if (!valid_found)
				continue;

			if (!coords_array_add(sacrificed_tiles, origin_x, origin_y))
				goto err;
		}

	for (int cycle = 0; cycle < 2; ++cycle) {
		for (int tile_index = sacrificed_tiles->usage - 1; tile_index >= 0; --tile_index) {
			Coords origin = sacrificed_tiles->data[tile_index];

			bool target_found = false;
			int target_height = 0;

			for (usize i = 0; i < 4; ++i) {
				int x = origin.x + four_direction_offsets[i].x;
				int y = origin.y + four_direction_offsets[i].y;

				Tile *tile = &tm[x][y];

				if ((tile->section != SECTION_VALID && tile->section != SECTION_SACRIFICED))
					|| (tile->feature != FEATURE_CLIFF && tile->feature != FEATURE_RAMP))
					continue;

				target_found = true;

				if (tile->height <= target_height)
					continue;

				target_height = tile->height;
			}

			if (!target_found)
				continue;

			Tile *tile = &tm[origin.x][origin.y];

			tile->feature = FEATURE_CLIFF;
			tile->height = target_height;
			tile->surface = SURFACE_ROCKS;

			coords_array_remove(sacrificed_tiles, tile_index);
		}
	}

out:
	ret = true;

err:
	coords_array_free(sacrificed_tiles);
	coords_array_free(cliff_grown_tiles);
	coords_array_free(cliff_next_tiles);
	coords_array_free(cliff_current_tiles);
	coords_array_free(ramp_next_tiles);
	coords_array_free(ramp_current_tiles);
	coords_array_free(ramp_check_tiles);
	coords_array_free(ramp_origins);
	coords_array_free(plateau_core_middles);
	coords_array_free(plateau_available_tiles);
	coords_array_free(plateau_expandable_tiles);
	return ret;
}
