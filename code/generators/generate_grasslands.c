#include "utils.h"
#include "map.h"
#include "mirror.h"
#include "config.h"
#include "helpers.h"

bool generate_grasslands(Map *map, Config *config, u32 *rng_state, DynamicString *logs)
{
	bool ret = false;
	
	CoordsArray *available_core_origins = NULL;
	CoordsArray *claimed_tiles = NULL;
	CoordsArray *expandable_tiles = NULL;
	
	Tile **tm = map->tile_matrix;
	
	if (!(available_core_origins = init_coords_array())) goto out;
	if (!(claimed_tiles = init_coords_array())) goto out;
	if (!(expandable_tiles = init_coords_array())) goto out;
	
	u32 claimed_matrix[FLAG_MATRIX_WORD_COUNT];
	clear_all_flags(claimed_matrix);
	
	u32 feature_count_multiplier = config->feature_count_multiplier;
	
	Range core_count_range = config->grassland_core_count_range;
	u32 core_count = random(rng_state, core_count_range.min, core_count_range.max) * feature_count_multiplier;
	u32 cores_placed = 0;
	
	Range core_size_range = config->grassland_core_size_range;
	bool core_allow_mirror_overlap = config->grassland_core_allow_mirror_overlap;
	
	u32 grass_dark_share = config->grassland_grass_dark_share;
	u32 grass_medium_share = config->grassland_grass_medium_share;
	u32 grass_light_share = config->grassland_grass_light_share;
	u32 earth_and_stones_share = config->grassland_earth_and_stones_share;
	u32 earth_share = config->grassland_earth_share;
	u32 surface_share_count = grass_dark_share + grass_medium_share + grass_light_share + earth_and_stones_share + earth_share;
	
	for (u32 x = 0; x < MAP_SIZE; x++) {
		for (u32 y = 0; y < MAP_SIZE; y++) {
			if (tm[x][y].section != SECTION_VALID) continue;
			if (!add_coords(available_core_origins, x, y)) goto out;
		}
	}
	
	while (cores_placed < core_count && available_core_origins->usage > 0) {
		
		u32 chosen_tile_index = random(rng_state, 0, available_core_origins->usage - 1);
		u32 x_origin = available_core_origins->data[chosen_tile_index].x;
		u32 y_origin = available_core_origins->data[chosen_tile_index].y;
		remove_coords(available_core_origins, chosen_tile_index);
		
		u32 core_width = random(rng_state, core_size_range.min, core_size_range.max);
		u32 core_length = random(rng_state, core_size_range.min, core_size_range.max);
		
		if (!is_in_bounds_rectangle(x_origin, y_origin, core_width, core_length)) continue;
		
		for (u32 x = x_origin; x < x_origin + core_width; x++) {
			for (u32 y = y_origin; y < y_origin + core_length; y++) {
				if (tm[x][y].section != SECTION_VALID
				|| test_flag(claimed_matrix, x, y)) goto next_core;
			}
		}
		
		if (!core_allow_mirror_overlap && has_mirror_overlap_rectangle(x_origin, y_origin, core_width, core_length)) goto next_core;
		
		for (u32 x = x_origin; x < x_origin + core_width; x++) {
			for (u32 y = y_origin; y < y_origin + core_length; y++) {
				set_flag(claimed_matrix, x, y, true);
				if (x != x_origin && x != x_origin + core_width - 1 && y != y_origin && y != y_origin + core_length - 1) continue;
				if (!add_coords(expandable_tiles, x, y)) goto out;
			}
		}
		
		cores_placed++;
		
		next_core:
		
	}
	
	if (cores_placed < core_count) {
		if (!add_to_string(logs, "generate_grasslands: %d/%d grassland cores placed\n", cores_placed, core_count)) goto out;
	}
	
	while (expandable_tiles->usage > 0) {
		
		u32 chosen_tile_index = random(rng_state, 0, expandable_tiles->usage - 1);
		u32 x_origin = expandable_tiles->data[chosen_tile_index].x;
		u32 y_origin = expandable_tiles->data[chosen_tile_index].y;
		remove_coords(expandable_tiles, chosen_tile_index);
		
		for (u32 offset_index = 0; offset_index < 4; offset_index++) {
			
			u32 x = x_origin + four_direction_offsets[offset_index].x;
			u32 y = y_origin + four_direction_offsets[offset_index].y;
			
			if (tm[x][y].section != SECTION_VALID
			|| test_flag(claimed_matrix, x, y)) continue;
			
			set_flag(claimed_matrix, x, y, true);
			if (!add_coords(claimed_tiles, x, y)) goto out;
			if (!add_coords(expandable_tiles, x, y)) goto out;
			
		}
		
	}
	
	u32 tiles_per_share = claimed_tiles->usage / surface_share_count;
	u32 grass_dark_count = grass_dark_share * tiles_per_share;
	u32 grass_medium_count = grass_medium_share * tiles_per_share;
	u32 grass_light_count = grass_light_share * tiles_per_share;
	u32 earth_and_stones_count = earth_and_stones_share * tiles_per_share;
	u32 earth_count = claimed_tiles->usage - (grass_dark_count + grass_medium_count + grass_light_count + earth_and_stones_count);
	
	for (i32 tile_index = claimed_tiles->usage - 1; tile_index >= 0; tile_index--) {
		
		u32 x = claimed_tiles->data[tile_index].x;
		u32 y = claimed_tiles->data[tile_index].y;
		
		Surface surface = SURFACE_EARTH;
		
		if (grass_dark_count > 0) {
			surface = SURFACE_GRASS_DARK;
			grass_dark_count--;
		} else if (grass_medium_count > 0) {
			surface = SURFACE_GRASS_MEDIUM;
			grass_medium_count--;
		} else if (grass_light_count > 0) {
			surface = SURFACE_GRASS_LIGHT;
			grass_light_count--;
		} else if (earth_and_stones_count > 0) {
			surface = SURFACE_EARTH_AND_STONES;
			earth_and_stones_count--;
		} else if (earth_count > 0) {
			surface = SURFACE_EARTH;
			earth_count--;
		}
		
		if (tm[x][y].feature != FEATURE_NONE) continue;
		
		if (surface != SURFACE_EARTH_AND_STONES && surface != SURFACE_EARTH) set_feature(map, x, y, FEATURE_GRASSLAND);
		set_surface(map, x, y, surface);
		
	}
	
	for (u32 x_origin = 0; x_origin < MAP_SIZE; x_origin++) {
		for (u32 y_origin = 0; y_origin < MAP_SIZE; y_origin++) {
			
			if (tm[x_origin][y_origin].section != SECTION_SACRIFICED
			|| tm[x_origin][y_origin].feature != FEATURE_NONE) continue;
			
			bool grassland_found = false;
			Surface surface;
			
			for (u32 offset_index = 0; offset_index < 8; offset_index++) {
				
				u32 x = x_origin + eight_direction_offsets[offset_index].x;
				u32 y = y_origin + eight_direction_offsets[offset_index].y;
				
				if (tm[x][y].section != SECTION_VALID
				|| tm[x][y].feature != FEATURE_GRASSLAND) continue;
				
				grassland_found = true;
				surface = tm[x][y].surface;
				break;
				
			}
			
			if (!grassland_found) continue;
			
			tm[x_origin][y_origin].feature = FEATURE_GRASSLAND;
			tm[x_origin][y_origin].surface = surface;
			
		}
	}
	
	ret = true;
	
out:
	free_coords_array(expandable_tiles);
	free_coords_array(claimed_tiles);
	free_coords_array(available_core_origins);
	return ret;
}
