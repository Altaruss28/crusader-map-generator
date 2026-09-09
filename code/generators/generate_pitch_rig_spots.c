#include "common.h"
#include "config.h"
#include "generator_helpers.h"
#include "map.h"
#include "mirror.h"

bool generate_pitch_rig_spots(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs)
{
	bool ret = false;

	CoordsArray *available_origins = nullptr;

	typeof(&map->tile_matrix[0]) tm = map->tile_matrix;

	Range spot_count_range = config->pitch_rig_spot_count_range;
	int target_count = random(rng_state, spot_count_range.min, spot_count_range.max) * config->feature_count_multiplier;
	if (target_count <= 0)
		goto out;

	int placed_count = 0;

	if (!(available_origins = coords_array_init())
		|| !genhelp_insert_ai_resource_spot_origins(available_origins))
		goto err;

	int cage_rock_chance = config->pitch_rig_spot_cage_rock_chance;

	while (placed_count < target_count && available_origins->usage > 0) {
		int tile_index = random(rng_state, 0, available_origins->usage - 1);
		Coords origin = available_origins->data[tile_index];
		coords_array_remove(available_origins, tile_index);

		for (int x = origin.x; x < origin.x + 6; ++x)
			for (int y = origin.y; y < origin.y + 6; ++y) {
				Tile *tile = &tm[x][y];

				if (tile->section != SECTION_VALID)
					goto next_attempt;

				bool is_edge = x == origin.x || x == origin.x + 5 || y == origin.y || y == origin.y + 5;
				bool is_pitch_rig_spot = tile->feature == FEATURE_PITCH_RIG_SPOT;

				if ((tile->feature != FEATURE_NONE && !(is_edge && is_pitch_rig_spot))
				|| (tile->height != 8 && !is_pitch_rig_spot))
					goto next_attempt;
			}

		if (mirror_check_overlap_rectangle(origin.x + 1, origin.y + 1, 4, 4))
			continue;

		for (int x = origin.x; x < origin.x + 6; ++x)
			for (int y = origin.y; y < origin.y + 6; ++y)
				map_set_feature(map, x, y, FEATURE_PITCH_RIG_SPOT);

		u8 inside_heights[4][4] = {
			{6, 5, 5, 6},
			{5, 4, 4, 5},
			{5, 4, 4, 5},
			{6, 5, 5, 6},
		};

		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j) {
				int x = origin.x + 1 + i;
				int y = origin.y + 1 + j;

				map_set_height(map, x, y, inside_heights[i][j]);
				map_set_surface(map, x, y, SURFACE_MARSH);
			}

		Coords ring_offsets[8] = {
			{random(rng_state, 3, 4), 0},
			{5, random(rng_state, 1, 2)},
			{5, random(rng_state, 3, 4)},
			{random(rng_state, 3, 4), 5},
			{random(rng_state, 1, 2), 5},
			{0, random(rng_state, 3, 4)},
			{0, random(rng_state, 1, 2)},
			{random(rng_state, 1, 2), 0},
		};

		for (int i = 0; i < 8; ++i) {
			int x = origin.x + ring_offsets[i].x;
			int y = origin.y + ring_offsets[i].y;

			map_set_height(map, x, y, 7);
			map_set_surface(map, x, y, SURFACE_MARSH);
		}

		struct {
			Coords rocks_base;
			Coords oil_offset;
		} sides[4] = {
			{{random(rng_state, 2, 3), 0}, {0, 1}},
			{{5, random(rng_state, 2, 3)}, {-1, 0}},
			{{random(rng_state, 2, 3), 5}, {0, -1}},
			{{0, random(rng_state, 2, 3)}, {1, 0}},
		};

		for (int i = 0; i < 4; ++i) {
			Coords rocks_pos = {origin.x + sides[i].rocks_base.x, origin.y + sides[i].rocks_base.y};

			map_set_height(map, rocks_pos.x, rocks_pos.y, 8);
			map_set_surface(map, rocks_pos.x, rocks_pos.y, SURFACE_ROCKS);

			if (cage_rock_chance >= random(rng_state, 1, 100)
				&& !mirror_check_overlap_rectangle(rocks_pos.x, rocks_pos.y, 1, 1))
				map_place_rock(map, rocks_pos.x, rocks_pos.y, 1);

			for (int j = 1; j <= 2; ++j) {
				Coords oil_pos = {rocks_pos.x + (sides[i].oil_offset.x * j), rocks_pos.y + (sides[i].oil_offset.y * j};
				map_set_height(map, oil_pos.x, oil_pos.y, 4);
				map_set_surface(map, oil_pos.x, oil_pos.y, SURFACE_OIL);
			}
		}

		++placed_count;

		next_attempt:
	}

	if (placed_count != target_count
		&& !dynamic_string_add(logs, "generate_pitch_rig_spots: %d/%d pitch rig spots placed\n", placed_count, target_count))
		goto err;

out:
	ret = true;

err:
	coords_array_free(available_origins);
	return ret;
}
