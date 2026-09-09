#include "common.h"
#include "config.h"
#include "generator_helpers.h"
#include "map.h"
#include "mirror.h"

static const BuildingType size_one_options[] = {
	BUILDING_RUIN_1X1_ONE,
	BUILDING_RUIN_1X1_TWO,
	BUILDING_RUIN_1X1_THREE,
	BUILDING_RUIN_1X1_FOUR,
};
static const BuildingType size_two_options[] = {
	BUILDING_RUIN_2X2_ONE,
	BUILDING_RUIN_2X2_TWO,
	BUILDING_RUIN_2X2_THREE,
	BUILDING_RUIN_2X2_FOUR,
	BUILDING_RUIN_2X2_FIVE,
	BUILDING_RUIN_2X2_SIX,
};
static const BuildingType size_four_options[] = {
	BUILDING_RUIN_4X4_ONE,
	BUILDING_RUIN_4X4_TWO,
	BUILDING_RUIN_4X4_THREE,
};

static const struct {
	const BuildingType *options;
	int option_count;
} ruin_options[] = {
	{size_one_options, sizeof(size_one_options) / sizeof(size_one_options[0])},
	{size_two_options, sizeof(size_two_options) / sizeof(size_two_options[0])},
	{size_four_options, sizeof(size_four_options) / sizeof(size_four_options[0])},
};

bool generate_ruins(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs)
{
	bool ret = false;

	CoordsArray *available_origins = nullptr;
	CoordsArray *blob_tile_offsets = nullptr;
	CoordsArray *blob_tile_coords = nullptr;

	typeof(&map->tile_matrix[0]) tm = map->tile_matrix;

	int feature_count_multiplier = config->feature_count_multiplier;

	Range blob_count_range = config->ruin_blob_count_range;
	int blob_count = random(rng_state, blob_count_range.min, blob_count_range.max) * feature_count_multiplier;
	int blobs_placed = 0;

	Range blob_step_count_range = config->ruin_blob_step_count_range;
	Range blob_grow_count_range = config->ruin_blob_grow_count_range;

	int count_per_blob_modifier = config->ruin_count_per_blob_modifier;

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
		if (!(blob_tile_offsets = coords_array_init()))
			goto err;
		if (!(blob_tile_coords = coords_array_init()))
			goto err;

		coords_array_shuffle(available_origins, rng_state);
		map_flag_matrix_t claimed_matrix[MAP_FLAG_MATRIX_WORD_COUNT];

		for (int blob_index = 0; blob_index < blob_count; blob_index++) {
			int step_count = random(rng_state, blob_step_count_range.min, blob_step_count_range.max);
			int grow_count = random(rng_state, blob_grow_count_range.min, blob_grow_count_range.max);
			if (!genhelp_insert_blob(blob_tile_offsets, step_count, grow_count, rng_state))
				goto err;

			for (usize origin_index = 0; origin_index < available_origins->usage; origin_index++) {
				int x_blob_origin = available_origins->data[origin_index].x;
				int y_blob_origin = available_origins->data[origin_index].y;

				bool pick_is_valid = true;

				blob_tile_coords->usage = 0;
				map_flag_matrix_clear_all(claimed_matrix);

				for (usize tile_index = 0; tile_index < blob_tile_offsets->usage; tile_index++) {
					int x = x_blob_origin + blob_tile_offsets->data[tile_index].x;
					int y = y_blob_origin + blob_tile_offsets->data[tile_index].y;

					if (!map_check_bounds_point(x, y)
					|| tm[x][y].section != SECTION_VALID
					|| tm[x][y].feature != FEATURE_NONE) {
						pick_is_valid = false;
						break;
					}

					map_flag_matrix_set(claimed_matrix, x, y, false);
					if (!coords_array_add(blob_tile_coords, x, y))
						goto err;
				}

				if (!pick_is_valid)
					continue;
				if (mirror_check_overlap_array(blob_tile_coords))
					continue;

				coords_array_shuffle(blob_tile_coords, rng_state);

				int ruin_count = blob_tile_coords->usage / count_per_blob_modifier;

				for (int ruin_index = 0; ruin_index < ruin_count; ruin_index++) {
					for (int ruin_size = random(rng_state, 1, 4); ruin_size > 0; ruin_size--) {
						if (ruin_size == 3)
							continue;

						for (usize tile_index = 0; tile_index < blob_tile_coords->usage; tile_index++) {
							int x_ruin_origin = blob_tile_coords->data[tile_index].x;
							int y_ruin_origin = blob_tile_coords->data[tile_index].y;

							for (int x = x_ruin_origin; x < x_ruin_origin + ruin_size; x++) {
								for (int y = y_ruin_origin; y < y_ruin_origin + ruin_size; y++) {
									if (map_check_bounds_point(x, y)
									&& map_flag_matrix_test(claimed_matrix, x, y)
									&& tm[x][y].object_flags == 0)
										continue;
									goto next_tile;
								}
							}

							for (int x = x_ruin_origin; x < x_ruin_origin + ruin_size; x++)
								for (int y = y_ruin_origin; y < y_ruin_origin + ruin_size; y++)
									map_set_feature(map, x, y, FEATURE_RUIN);

							int option_index = ruin_size / 2;

							int option_count = ruin_options[option_index].option_count;
							BuildingType ruin_type = ruin_options[option_index].options[random(rng_state, 0, option_count - 1)];

							if (!map_place_building(map, x_ruin_origin, y_ruin_origin, ruin_type, 0, 0, false))
								goto err;
							goto next_ruin;

							next_tile:
						}
					}

					next_ruin:
				}

				blobs_placed++;
				break;
			}
		}
	}

	if (blobs_placed < blob_count
	&& !dynamic_string_add(logs, "generate_ruins: %d/%d ruin blobs placed\n", blobs_placed, blob_count))
		goto err;

	ret = true;

err:
	coords_array_free(blob_tile_coords);
	coords_array_free(blob_tile_offsets);
	coords_array_free(available_origins);
	return ret;
}
