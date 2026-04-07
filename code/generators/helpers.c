#include "helpers.h"
#include "utils.h"
#include "map.h"
#include "mirror.h"

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

bool insert_blob(CoordsArray *blob_tiles, u32 step_count, u32 grow_count, u32 *rng_state)
{
	bool ret = false;
	
	CoordsArray *expandable_tiles = NULL;
	
	if (!(expandable_tiles = init_coords_array())) goto out;
	
	u32 check_matrix[FLAG_MATRIX_WORD_COUNT];
	clear_all_flags(check_matrix);
	
	u32 mid_coord = MAP_SIZE / 2;
	
	set_flag(check_matrix, mid_coord, mid_coord, false);
	if (!add_coords(blob_tiles, 0, 0)) goto out;
	if (!add_coords(expandable_tiles, mid_coord, mid_coord)) goto out;
	u32 tiles_added = 1;
	
	u32 x_step = mid_coord;
	u32 y_step = mid_coord;
	
	for (u32 step = 0; step < step_count; step++) {
		
		u32 offset_index = random(rng_state, 0, 3);
		x_step = x_step + four_direction_offsets[offset_index].x;
		y_step = y_step + four_direction_offsets[offset_index].y;
		
		if (!is_in_bounds(x_step, y_step)
		|| test_flag(check_matrix, x_step, y_step)) continue;
		
		set_flag(check_matrix, x_step, y_step, false);
		if (!add_coords(blob_tiles, x_step - mid_coord, y_step - mid_coord)) goto out;
		if (!add_coords(expandable_tiles, x_step, y_step)) goto out;
		
	}
	
	while (tiles_added < grow_count && expandable_tiles->usage > 0) {
		
		u32 chosen_tile_index = random(rng_state, 0, expandable_tiles->usage - 1);
		u32 x_base = expandable_tiles->data[chosen_tile_index].x;
		u32 y_base = expandable_tiles->data[chosen_tile_index].y;
		
		u32 grow_directions[4];
		u32 grow_direction_count = 0;
		
		for (u32 offset_index = 0; offset_index < 4; offset_index++) {
			
			u32 x = x_base + four_direction_offsets[offset_index].x;
			u32 y = y_base + four_direction_offsets[offset_index].y;
			
			if (!is_in_bounds(x, y)
			|| test_flag(check_matrix, x, y)) continue;
			
			grow_directions[grow_direction_count++] = offset_index;
			
		}
		
		if (grow_direction_count > 0) {
			
			u32 chosen_offset_index = grow_directions[random(rng_state, 0, grow_direction_count - 1)];
			u32 x = x_base + four_direction_offsets[chosen_offset_index].x;
			u32 y = y_base + four_direction_offsets[chosen_offset_index].y;
			
			set_flag(check_matrix, x, y, false);
			if (!add_coords(blob_tiles, x - mid_coord, y - mid_coord)) goto out;
			if (!add_coords(expandable_tiles, x, y)) goto out;
			
			tiles_added++;
			
			if (grow_direction_count > 1) continue;
			
		}
		
		remove_coords(expandable_tiles, chosen_tile_index);
		
	}
	
	ret = true;
	
out:
	free_coords_array(expandable_tiles);
	return ret;
}

CoordsArray *get_ai_resource_spot_origins(void)
{
	CoordsArray *origins = init_coords_array();
	if (!origins) goto err;
	
	for (u32 x = 4; x < MAP_SIZE - 5; x += 5) {
		for (u32 y = 4; y < MAP_SIZE - 5; y += 5) {
			if (!add_coords(origins, x, y)) goto err;
		}
	}
	
	return origins;
	
err:
	free_coords_array(origins);
	return NULL;
}
