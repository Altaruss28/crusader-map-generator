#include "generator_helpers.h"

#include "common.h"
#include "map.h"

const Coords four_direction_offsets[4] = {
	{0, -1},
	{1, 0},
	{0, 1},
	{-1, 0},
};
const Coords eight_direction_offsets[8] = {
	{0, -1},
	{1, -1},
	{1, 0},
	{1, 1},
	{0, 1},
	{-1, 1},
	{-1, 0},
	{-1, -1},
};

bool genhelp_insert_blob(CoordsArray *blob_tiles, int step_counter, int grow_counter, rng_state_t *rng_state)
{
	bool ret = false;

	blob_tiles->usage = 0;

	CoordsArray *expandable_tiles = nullptr;
	if (!(expandable_tiles = coords_array_init()))
		goto err;

	map_flag_matrix_t check_matrix[MAP_FLAG_MATRIX_WORD_COUNT] = {};

	int mid_coord = MAP_SIZE / 2;

	map_flag_matrix_set(check_matrix, mid_coord, mid_coord, false);
	if (!coords_array_add(blob_tiles, 0, 0)
		|| !coords_array_add(expandable_tiles, mid_coord, mid_coord))
		goto err;

	Coords walker_pos = {mid_coord, mid_coord};
	while (step_counter-- > 0) {
		int offset_index = random(rng_state, 0, ARRAY_LENGTH(four_direction_offsets) - 1);
		walker_pos.x += four_direction_offsets[offset_index].x;
		walker_pos.y += four_direction_offsets[offset_index].y;

		if (!map_check_bounds_point(walker_pos.x, walker_pos.y)
			|| map_flag_matrix_test(check_matrix, walker_pos.x, walker_pos.y))
			continue;

		map_flag_matrix_set(check_matrix, walker_pos.x, walker_pos.y, false);
		if (!coords_array_add(blob_tiles, walker_pos.x - mid_coord, walker_pos.y - mid_coord)
			|| !coords_array_add(expandable_tiles, walker_pos.x, walker_pos.y))
			goto err;
	}

	while (grow_counter > 0 && expandable_tiles->usage > 0) {
		int tile_index = random(rng_state, 0, expandable_tiles->usage - 1);
		Coords origin = expandable_tiles->data[tile_index];

		int grow_direction_indices[ARRAY_LENGTH(four_direction_offsets)];
		int grow_direction_count = 0;

		for (usize i = 0; i < ARRAY_LENGTH(four_direction_offsets); ++i) {
			int x = origin.x + four_direction_offsets[i].x;
			int y = origin.y + four_direction_offsets[i].y;

			if (!map_check_bounds_point(x, y)
				|| map_flag_matrix_test(check_matrix, x, y))
				continue;

			grow_direction_indices[grow_direction_count++] = i;
		}

		if (grow_direction_count > 0) {
			int offset_index = grow_direction_indices[random(rng_state, 0, grow_direction_count - 1)];
			int x = origin.x + four_direction_offsets[offset_index].x;
			int y = origin.y + four_direction_offsets[offset_index].y;

			map_flag_matrix_set(check_matrix, x, y, false);
			if (!coords_array_add(blob_tiles, x - mid_coord, y - mid_coord)
				|| !coords_array_add(expandable_tiles, x, y))
				goto err;

			--grow_counter;

			if (grow_direction_count > 1)
				continue;
		}

		coords_array_remove(expandable_tiles, tile_index);
	}

	ret = true;

err:
	coords_array_free(expandable_tiles);
	return ret;
}

bool genhelp_insert_ai_resource_spot_origins(CoordsArray *origins)
{
	origins->usage = 0;

	for (int x = 4; x < MAP_SIZE - 5; x += 5)
		for (int y = 4; y < MAP_SIZE - 5; y += 5)
			if (!coords_array_add(origins, x, y))
				return false;

	return true;
}
