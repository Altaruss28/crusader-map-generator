#include "utils.h"
#include "map.h"
#include "mirror.h"
#include "config.h"
#include "helpers.h"

bool generate_border(Map *map, Config *config, u32 *rng_state, DynamicString *logs)
{
	(void)logs;
	
	bool ret = false;
	
	CoordsArray *expandable_tiles = NULL;
	
	Tile **tm = map->tile_matrix;
	
	u32 start_radius = config->border_start_radius;
	Range border_size_range = config->border_size_range;
	u32 border_size = random(rng_state, border_size_range.min, border_size_range.max);
	Range height_variance_range = config->border_height_variance_range;
	
	if (!(expandable_tiles = init_coords_array())) goto out;
	
	u32 claimed_matrix[FLAG_MATRIX_WORD_COUNT];
	clear_all_flags(claimed_matrix);
	
	for (u32 x = 0; x < MAP_SIZE; x++) {
		for (u32 y = 0; y < MAP_SIZE; y++) {
			
			Section section = tm[x][y].section;
			
			if (section != SECTION_VALID && section != SECTION_SACRIFICED) continue;
			
			u32 distance_from_center = tm[x][y].distance_from_center;
			
			if (distance_from_center < start_radius) continue;
			
			set_flag(claimed_matrix, x, y, true);
			
			if (distance_from_center == start_radius) {
				if (!add_coords(expandable_tiles, x, y)) goto out;
			}
			
			u32 height = tm[x][y].height;
			
			if (tm[x][y].feature != FEATURE_NONE
			|| height != 8) continue;
			
			set_feature(map, x, y, FEATURE_BORDER);
			set_height(map, x, y, height + random(rng_state, height_variance_range.min, height_variance_range.max));
			set_surface(map, x, y, SURFACE_ROCKS);
			
		}
	}
	
	u32 tiles_claimed = 0;
	
	while (tiles_claimed < border_size && expandable_tiles->usage > 0) {
		
		u32 chosen_tile_index = random(rng_state, 0, expandable_tiles->usage - 1);
		u32 x_origin = expandable_tiles->data[chosen_tile_index].x;
		u32 y_origin = expandable_tiles->data[chosen_tile_index].y;
		remove_coords(expandable_tiles, chosen_tile_index);
		
		for (u32 offset_index = 0; offset_index < 4; offset_index++) {
			
			if (tiles_claimed >= border_size) break;
			
			u32 x = x_origin + four_direction_offsets[offset_index].x;
			u32 y = y_origin + four_direction_offsets[offset_index].y;
			
			if (tm[x][y].section != SECTION_VALID
			|| test_flag(claimed_matrix, x, y)) continue;
			
			set_flag(claimed_matrix, x, y, true);
			if (!add_coords(expandable_tiles, x, y)) goto out;
			tiles_claimed++;
			
			u32 height = tm[x][y].height;
			
			if (tm[x][y].feature != FEATURE_NONE
			|| tm[x][y].height != 8) continue;
			
			set_feature(map, x, y, FEATURE_BORDER);
			set_height(map, x, y, height + random(rng_state, height_variance_range.min, height_variance_range.max));
			set_surface(map, x, y, SURFACE_ROCKS);
			
		}
		
	}
	
	ret = true;
	
out:
	free_coords_array(expandable_tiles);
	return ret;
}
