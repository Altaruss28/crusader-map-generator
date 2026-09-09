#include <stdlib.h>

#include "common.h"
#include "config.h"
#include "map.h"
#include "mirror.h"

bool generate_keeps(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs)
{
	bool ret = false;

	CoordsArray *potential_origins = nullptr;

	typeof(&map->tile_matrix[0]) tm = map->tile_matrix;

	if (!(potential_origins = coords_array_init()))
		goto err;

	for (int x = 0; x < MAP_SIZE; ++x)
		for (int y = 0; y < MAP_SIZE; ++y) {
			Tile *tile = &tm[x][y];

			if (tile->section != SECTION_VALID
				|| tile->feature != FEATURE_NONE)
				continue;

			if (!coords_array_add(potential_origins, x, y))
				goto err;
		}

	coords_array_shuffle(potential_origins, rng_state);

	int feature_count_multiplier = config->feature_count_multiplier;

	int mirror_point_count = mirror_points_init(0, 0, 1).count;
	int base_owner = 1;

	if (config->generate_players) {
		Range count_range = config->player_count_range;
		int target_count = random(rng_state, count_range.min, count_range.max) * feature_count_multiplier;

		if (target_count > 0) {
			int placed_count = 0;

			int reserved_area_size = 1 + (2 * config->player_reserved_area_radius);
			int door_offset = reserved_area_size / 2;
			int min_mirror_distance = config->player_min_mirror_distance;

			for (usize i = 0; i < potential_origins->usage; ++i) {
				Coords origin = potential_origins->data[i];

				if (mirror_point_count > 1) {
					MirrorPoints mirror_points = mirror_points_init(origin.x + player_door_offset, origin.y + player_door_offset, 1);

					int x1 = mirror_points.points[0].x;
					int y1 = mirror_points.points[0].y;
					int x2 = mirror_points.points[1].x;
					int y2 = mirror_points.points[1].y;

					if (max_int(abs(x1 - x2), abs(y1 - y2)) < player_min_mirror_distance)
						continue;
				}

				if (!map_check_bounds_rectangle(origin.x, origin.y, reserved_area_size, reserved_area_size)
					|| mirror_check_overlap_rectangle(origin.x, origin.y, reserved_area_size, reserved_area_size))
					continue;

				int target_height = tm[origin.x][origin.y].height;

				for (int x = origin.x; x < origin.x + reserved_area_size; ++x)
					for (int y = origin.y; y < origin.y + reserved_area_size; ++y) {
						Tile *tile = &tm[x][y];
						if (tile->section != SECTION_VALID
							|| tile->feature != FEATURE_NONE
							|| tile->height != target_height)
							goto next_player_attempt;
					}

				for (int x = origin.x; x < origin.x + reserved_area_size; ++x)
					for (int y = origin.y; y < origin.y + reserved_area_size; ++y)
						map_set_feature(map, x, y, FEATURE_KEEP);

				if (!map_place_building(map, origin.x + door_offset - 7, origin.y + door_offset - 7, BUILDING_KEEP, 0, base_owner, true))
					goto err;

				base_owner += mirror_point_count;

				if (++placed_count >= target_count)
					break;

				next_player_attempt:
			}

			if (placed_count != target_count
				&& !dynamic_string_add(logs, "generate_keeps: %d/%d players placed\n", placed_count, target_count))
				goto err;
		}
	}

	if (config->generate_spectators) {
		Range count_range = config->spectator_count_range;
		int target_count = random(rng_state, count_range.min, count_range.max) * feature_count_multiplier;

		if (target_count > 0) {
			int placed_count = 0;

			int reserved_area_size = 19;
			int door_offset = reserved_area_size / 2;
			int lion_count = config->spectator_lion_count;

			for (usize i = 0; i < potential_origins->usage; ++i) {
				Coords origin = potential_origins->data[i];

				if (!map_check_bounds_rectangle(origin.x, origin.y, reserved_area_size, reserved_area_size)
					|| mirror_check_overlap_rectangle(origin.x, origin.y, reserved_area_size, reserved_area_size))
					continue;

				int target_height = tm[origin.x][origin.y].height;

				for (int x = origin.x; x < origin.x + reserved_area_size; ++x)
					for (int y = origin.y; y < origin.y + reserved_area_size; ++y) {
						Tile *tile = &tm[x][y];
						if (tile->section != SECTION_VALID
							|| tile->feature != FEATURE_NONE
							|| tile->height != target_height)
							goto next_spectator_attempt;
					}

				for (int x = origin.x; x < origin.x + reserved_area_size; ++x)
					for (int y = origin.y; y < origin.y + reserved_area_size; ++y) {
						map_set_feature(map, x, y, FEATURE_KEEP);
						if ((x == origin.x || x == origin.x + reserved_area_size - 1 || y == origin.y || y == origin.y + reserved_area_size - 1)
							&& !map_place_wall(map, x, y, x, y, WALL_HIGH, base_owner, true))
							goto err;
					}

				Coords door_pos = {origin.x + door_offset, origin.y + door_offset};

				if (!map_place_building(map, door_pos.x - 7, door_pos.y - 7, BUILDING_KEEP, 0, base_owner, true))
					goto err;

				base_owner += mirror_point_count;

				for (int i = 0; i < lion_count; ++i)
					if (!map_place_animal_group(map, door_pos.x, door_pos.y, ANIMAL_GROUP_LION, 1))
						goto err;

				if (++placed_count >= target_count)
					break;

				next_spectator_attempt:
			}

			if (placed_count != target_count
				&& !dynamic_string_add(logs, "generate_keeps: %d/%d spectators placed\n", placed_count, target_count))
				goto err;
		}
	}

	ret = true;

err:
	coords_array_free(potential_origins);
	return ret;
}
