#include "common.h"
#include "config.h"
#include "generator_helpers.h"
#include "map.h"
#include "mirror.h"

bool generate_iron_mine_spots(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs)
{
	bool ret = false;

	CoordsArray *available_origins = nullptr;

	typeof(&map->tile_matrix[0]) tm = map->tile_matrix;

	Range spot_count_range = config->iron_mine_spot_count_range;
	int target_count = random(rng_state, spot_count_range.min, spot_count_range.max) * config->feature_count_multiplier;
	if (target_count <= 0)
		goto out;

	int placed_count = 0;

	if (!(available_origins = coords_array_init())
		|| !genhelp_insert_ai_resource_spot_origins(available_origins))
		goto err;

	int cage_rock_chance = config->iron_mine_spot_cage_rock_chance;

	while (placed_count < target_count && available_origins->usage > 0) {
		int tile_index = random(rng_state, 0, available_origins->usage - 1);
		Coords origin = available_origins->data[tile_index];
		coords_array_remove(available_origins, tile_index);

		int target_height = tm[origin.x][origin.y].height;

		for (int x = origin.x; x < origin.x + 6; ++x)
			for (int y = origin.y; y < origin.y + 6; ++y) {
				Tile *tile = &tm[x][y];

				if (tile->section != SECTION_VALID)
					goto next_attempt;

				bool is_edge = x == origin.x || x == origin.x + 5 || y == origin.y || y == origin.y + 5;
				bool is_iron_mine_spot = tile->feature == FEATURE_IRON_MINE_SPOT;

				if ((tile->feature != FEATURE_NONE && !(is_edge && is_iron_mine_spot))
					|| tile->height != target_height)
					goto next_attempt;
			}

		if (mirror_check_overlap_rectangle(origin.x + 1, origin.y + 1, 4, 4))
			continue;

		for (int x = origin.x; x < origin.x + 6; ++x)
			for (int y = origin.y; y < origin.y + 6; ++y)
				map_set_feature(map, x, y, FEATURE_IRON_MINE_SPOT);

		Coords middle_iron[4] = {{3, 2}, {3, 3}, {2, 3}, {2, 2}};
		Coords corner_iron[4] = {{4, 1}, {4, 4}, {1, 4}, {1, 1}};

		struct {
			Coords rocks_base;
			Coords iron_offset;
		} sides[4] = {
			{{random(rng_state, 2, 3), 0}, {0, 1}},
			{{5, random(rng_state, 2, 3)}, {-1, 0}},
			{{random(rng_state, 2, 3), 5}, {0, -1}},
			{{0, random(rng_state, 2, 3)}, {1, 0}},
		};

		for (int i = 0; i < 4; ++i) {
			map_set_surface(map, origin.x + middle_iron[i].x, origin.y + middle_iron[i].y, SURFACE_IRON);
			if (random(rng_state, 0, 1))
				map_set_surface(map, origin.x + corner_iron[i].x, origin.y + corner_iron[i].y, SURFACE_IRON);

			Coords rocks_pos = {origin.x + sides[i].rocks_base.x, origin.y + sides[i].rocks_base.y};

			map_set_surface(map, rocks_pos.x, rocks_pos.y, SURFACE_ROCKS);

			if (cage_rock_chance >= random(rng_state, 1, 100)
				&& !mirror_check_overlap_rectangle(rocks_pos.x, rocks_pos.y, 1, 1))
				map_place_rock(map, rocks_pos.x, rocks_pos.y, 1);

			map_set_surface(map, rocks_pos.x + sides[i].iron_offset.x, rocks_pos.y + sides[i].iron_offset.y, SURFACE_IRON);
		}

		++placed_count;

		next_attempt:
	}

	if (placed_count != target_count
		&& !dynamic_string_add(logs, "generate_iron_mine_spots: %d/%d iron mine spots placed\n", placed_count, target_count))
		goto err;

out:
	ret = true;

err:
	coords_array_free(available_origins);
	return ret;
}
