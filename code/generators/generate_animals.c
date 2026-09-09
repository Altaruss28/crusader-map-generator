#include "common.h"
#include "config.h"
#include "map.h"
#include "mirror.h"

bool generate_animals(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs)
{
	bool ret = false;

	CoordsArray *available_origins = nullptr;

	typeof(&map->tile_matrix[0]) tm = map->tile_matrix;

	int feature_count_multiplier = config->feature_count_multiplier;

	struct {
		char *name_str;
		AnimalGroupType type;
		bool generate;
		Range group_count_range;
		Range per_group_range;
		int reserved_area_radius;
	} settings[] = {
		{"deer", ANIMAL_GROUP_DEER, config->generate_deer, config->deer_group_count_range, config->deer_per_group_range, config->deer_reserved_area_radius},
		{"lion", ANIMAL_GROUP_LION, config->generate_lions, config->lion_group_count_range, config->lion_per_group_range, config->lion_reserved_area_radius},
		{"rabbit", ANIMAL_GROUP_RABBIT, config->generate_rabbits, config->rabbit_group_count_range, config->rabbit_per_group_range, config->rabbit_reserved_area_radius},
		{"camel", ANIMAL_GROUP_CAMEL, config->generate_camels, config->camel_group_count_range, config->camel_per_group_range, config->camel_reserved_area_radius},
	};

	if (!(available_origins = coords_array_init()))
		goto err;

	for (int x = 0; x < MAP_SIZE; ++x)
		for (int y = 0; y < MAP_SIZE; ++y) {
			Tile *tile = &tm[x][y];

			if (tile->section != SECTION_VALID
				|| (tile->feature != FEATURE_NONE && tile->feature != FEATURE_GRASSLAND))
				continue;

			if (!coords_array_add(available_origins, x, y))
				goto err;
		}

	coords_array_shuffle(available_origins, rng_state);

	for (usize setting_index = 0; setting_index < ARRAY_LENGTH(settings); ++setting_index) {
		if (!settings[setting_index].generate)
			continue;

		Range group_count_range = settings[setting_index].group_count_range;

		int target_count = random(rng_state, group_count_range.min, group_count_range.max) * feature_count_multiplier;
		if (target_count <= 0)
			continue;

		int placed_count = 0;

		int reserved_area_radius = settings[setting_index].reserved_area_radius;
		int reserved_area_size = 1 + (reserved_area_radius * 2);

		AnimalGroupType type = settings[setting_index].type;
		Range per_group_range = settings[setting_index].per_group_range;

		for (usize tile_index = 0; tile_index < available_origins->usage; ++tile_index) {
			Coords middle = available_origins->data[tile_index];
			Coords top_left = {middle.x - reserved_area_radius, middle.y - reserved_area_radius}

			for (int x = top_left.x; x < top_left.x + reserved_area_size; ++x)
				for (int y = top_left.y; y < top_left.y + reserved_area_size; ++y) {
					if (!map_check_bounds_point(x, y)
						goto next_tile;

					Tile *tile = &tm[x][y];

					if (tile->section != SECTION_VALID
						|| (tile->feature != FEATURE_NONE && tile->feature != FEATURE_GRASSLAND))
						goto next_tile;
				}

			if (mirror_check_overlap_rectangle(top_left.x, top_left.y, reserved_area_size, reserved_area_size))
				continue;

			for (int x = top_left.x; x < top_left.x + reserved_area_size; ++x)
				for (int y = top_left.y; y < top_left.y + reserved_area_size; ++y)
					map_set_feature(map, x, y, FEATURE_ANIMAL);

			if (!map_place_animal_group(map, middle.x, middle.y, type, random(rng_state, per_group_range.min, per_group_range.max)))
				goto err;

			if (++placed_count >= target_count)
				break;

			next_tile:
		}

		if (placed_count != target_count
			&& !dynamic_string_add(logs, "generate_animals: %d/%d %s groups placed\n", placed_count, target_count, settings[setting_index].name_str))
			goto err;
	}

	ret = true;

err:
	coords_array_free(available_origins);
	return ret;
}
