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

bool insert_blob(CoordsArray *blob_tiles, u32 blob_size, u32 *rng_state)
{
	CoordsArray *expandable_tiles = NULL;
	
	u32 check_matrix[FLAG_MATRIX_WORD_COUNT];
	clear_all_flags(check_matrix);
	
	u32 mid_coord = MAP_SIZE / 2;
	
	set_flag(check_matrix, mid_coord, mid_coord, false);
	if (!add_coords(blob_tiles, mid_coord, mid_coord)) goto err;
	
	if (!(expandable_tiles = init_coords_array())) goto err;
	if (!add_coords(expandable_tiles, mid_coord, mid_coord)) goto err;
	
	u32 tile_count = 1;
	
	while (tile_count < blob_size && expandable_tiles->usage > 0) {
		
		u32 chosen_tile_index = random(rng_state, 0, expandable_tiles->usage - 1);
		u32 x_base = expandable_tiles->data[chosen_tile_index].x;
		u32 y_base = expandable_tiles->data[chosen_tile_index].y;
		
		u32 growth_directions[4];
		u32 growth_direction_count = 0;
		
		for (u32 offset_index = 0; offset_index < 4; offset_index++) {
			
			u32 x = x_base + four_direction_offsets[offset_index].x;
			u32 y = y_base + four_direction_offsets[offset_index].y;
			
			if (!is_in_bounds(x, y)
			|| test_flag(check_matrix, x, y)) continue;
			
			growth_directions[growth_direction_count++] = offset_index;
			
		}
		
		if (growth_direction_count > 0) {
			
			u32 chosen_offset_index = growth_directions[random(rng_state, 0, growth_direction_count - 1)];
			u32 x = x_base + four_direction_offsets[chosen_offset_index].x;
			u32 y = y_base + four_direction_offsets[chosen_offset_index].y;
			
			if (!add_coords(blob_tiles, x, y)) goto err;
			if (!add_coords(expandable_tiles, x, y)) goto err;
			set_flag(check_matrix, x, y, false);
			
			tile_count++;
			
			if (growth_direction_count > 1) continue;
			
		}
		
		remove_coords(expandable_tiles, chosen_tile_index);
		
	}
	
	free_coords_array(expandable_tiles);
	
	for (u32 tile_index = 0; tile_index < blob_tiles->usage; tile_index++) {
		blob_tiles->data[tile_index].x -= mid_coord;
		blob_tiles->data[tile_index].y -= mid_coord;
	}
	
	return true;
	
err:
	free_coords_array(expandable_tiles);
	return false;
}

CoordsArray *get_ai_resource_spot_origins(void)
{
	CoordsArray *origins = init_coords_array();
	if (!origins) return NULL;
	
	for (u32 x = 4; x < MAP_SIZE - 5; x += 5) {
		for (u32 y = 4; y < MAP_SIZE - 5; y += 5) {
			add_coords(origins, x, y);
		}
	}
	
	return origins;
}
