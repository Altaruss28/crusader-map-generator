#include "common.h"
#include "config.h"
#include "generator_helpers.h"
#include "map.h"
#include "mirror.h"

static const PlantType dry_tree_options[] = {
	PLANT_TREE_COCONUT,
	PLANT_TREE_OLIVE,
	PLANT_TREE_CHERRY,
};
static const PlantType wet_tree_options[] = {
	PLANT_TREE_DATE,
	PLANT_TREE_COCONUT,
	PLANT_TREE_CHERRY,
};

bool generate_trees(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs)
{
	bool ret = false;

	CoordsArray *available_origins = nullptr;
	CoordsArray *blob_tile_offsets = nullptr;
	CoordsArray *blob_tile_coords = nullptr;

	typeof(&map->tile_matrix[0]) tm = map->tile_matrix;

	int feature_count_multiplier = config->feature_count_multiplier;

	Range blob_count_range = config->tree_blob_count_range;
	int blob_count = random(rng_state, blob_count_range.min, blob_count_range.max) * feature_count_multiplier;
	int blobs_placed = 0;

	Range blob_step_count_range = config->tree_blob_step_count_range;
	Range blob_grow_count_range = config->tree_blob_grow_count_range;

	if (!(available_origins = coords_array_init()))
		goto err;

	for (int x = 0; x < MAP_SIZE; x++) {
		for (int y = 0; y < MAP_SIZE; y++) {
			if (tm[x][y].section != SECTION_VALID
			|| (tm[x][y].feature != FEATURE_NONE
			&& tm[x][y].feature != FEATURE_GRASSLAND))
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

				for (usize tile_index = 0; tile_index < blob_tile_offsets->usage; tile_index++) {
					int x = x_blob_origin + blob_tile_offsets->data[tile_index].x;
					int y = y_blob_origin + blob_tile_offsets->data[tile_index].y;

					if (!map_check_bounds_point(x, y)
					|| tm[x][y].section != SECTION_VALID
					|| (tm[x][y].feature != FEATURE_NONE
					&& tm[x][y].feature != FEATURE_GRASSLAND)) {
						pick_is_valid = false;
						break;
					}

					if (!coords_array_add(blob_tile_coords, x, y))
						goto err;
				}

				if (!pick_is_valid)
					continue;
				if (mirror_check_overlap_array(blob_tile_coords))
					continue;

				PlantType dry_option = dry_tree_options[random(rng_state, 0, (sizeof(dry_tree_options) / sizeof(dry_tree_options[0])) - 1)];
				PlantType wet_option = wet_tree_options[random(rng_state, 0, (sizeof(wet_tree_options) / sizeof(wet_tree_options[0])) - 1)];

				coords_array_shuffle(blob_tile_coords, rng_state);

				for (usize blob_tile_index = 0; blob_tile_index < blob_tile_coords->usage; blob_tile_index++) {
					int x_tree_origin = blob_tile_coords->data[blob_tile_index].x;
					int y_tree_origin = blob_tile_coords->data[blob_tile_index].y;

					for (int x = x_tree_origin - 2; x <= x_tree_origin + 2; x++) {
						for (int y = y_tree_origin - 2; y <= y_tree_origin + 2; y++) {
							if ((tm[x][y].feature != FEATURE_NONE
							&& tm[x][y].feature != FEATURE_GRASSLAND)
							|| tm[x][y].object_flags & OF_PLANT)
								goto next_tile;
						}
					}

					if (mirror_check_overlap_rectangle(x_tree_origin - 1, y_tree_origin - 1, 3, 3))
						continue;

					for (int x = x_tree_origin - 2; x <= x_tree_origin + 2; x++)
						for (int y = y_tree_origin - 2; y <= y_tree_origin + 2; y++)
							map_set_feature(map, x, y, FEATURE_TREE);

					PlantType type;

					Surface surface = tm[x_tree_origin][y_tree_origin].surface;

					if (surface == SURFACE_GRASS_DARK || surface == SURFACE_GRASS_MEDIUM || surface == SURFACE_GRASS_LIGHT) {
						type = wet_option;
					} else {
						type = dry_option;
					}

					int variant = random(rng_state, 0, 9);
					int stage = random(rng_state, 0, 3);

					if (!map_place_plant(map, x_tree_origin, y_tree_origin, type, variant, stage))
						goto err;

					next_tile:
				}

				blobs_placed++;
				break;
			}
		}
	}

	if (blobs_placed < blob_count
	&& !dynamic_string_add(logs, "generate_trees: %d/%d tree blobs placed\n", blobs_placed, blob_count))
		goto err;

	ret = true;

err:
	coords_array_free(blob_tile_coords);
	coords_array_free(blob_tile_offsets);
	coords_array_free(available_origins);
	return ret;
}
