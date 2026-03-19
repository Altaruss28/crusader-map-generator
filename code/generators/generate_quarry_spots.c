#include "utils.h"
#include "map.h"
#include "mirror.h"
#include "config.h"
#include "helpers.h"

bool generate_quarry_spots(Map *map, Config *config, u32 *rng_state, DynamicString *logs)
{
	bool ret = false;
	
	CoordsArray *available_origins = NULL;
	CoordsArray *spot_tile_offsets = NULL;
	CoordsArray *spot_tile_coords = NULL;
	
	Tile **tm = map->tile_matrix;
	
	u32 feature_count_multiplier = config->feature_count_multiplier;
	
	Range spot_count_range = config->quarry_spot_count_range;
	u32 spot_count = random(rng_state, spot_count_range.min, spot_count_range.max) * feature_count_multiplier;
	u32 spots_placed = 0;
	
	Range spot_size_range = config->quarry_spot_size_range;
	
	u32 stone_share = config->quarry_spot_stone_share;
	u32 gravel_share = config->quarry_spot_gravel_share;
	u32 grass_light_share = config->quarry_spot_grass_light_share;
	u32 earth_and_stones_share = config->quarry_spot_earth_and_stones_share;
	
	u32 surface_share_count = stone_share + gravel_share + grass_light_share + earth_and_stones_share;
	
	if (!(available_origins = init_coords_array())) goto out;
	
	for (u32 x = 0; x < MAP_SIZE; x++) {
		for (u32 y = 0; y < MAP_SIZE; y++) {
			
			if (tm[x][y].section != SECTION_VALID
			|| tm[x][y].feature != FEATURE_NONE) continue;
			
			if (!add_coords(available_origins, x, y)) goto out;
			
		}
	}
	
	if (available_origins->usage > 0) {
		
		shuffle_coords_array(available_origins, rng_state);
		
		if (!(spot_tile_offsets = init_coords_array())) goto out;
		if (!(spot_tile_coords = init_coords_array())) goto out;
		
		for (u32 spot_index = 0; spot_index < spot_count; spot_index++) {
			
			spot_tile_offsets->usage = 0;
			u32 spot_size = random(rng_state, spot_size_range.min, spot_size_range.max);
			if (!insert_blob(spot_tile_offsets, spot_size, rng_state)) goto out;
			
			for (u32 origin_index = 0; origin_index < available_origins->usage; origin_index++) {
				
				u32 x_origin = available_origins->data[origin_index].x;
				u32 y_origin = available_origins->data[origin_index].y;
				
				u32 origin_height = tm[x_origin][y_origin].height;
				bool origin_is_valid = true;
				
				spot_tile_coords->usage = 0;
				
				for (u32 tile_index = 0; tile_index < spot_tile_offsets->usage; tile_index++) {
					
					u32 x = x_origin + spot_tile_offsets->data[tile_index].x;
					u32 y = y_origin + spot_tile_offsets->data[tile_index].y;
					
					if (!is_in_bounds(x, y)
					|| tm[x][y].section != SECTION_VALID
					|| tm[x][y].feature != FEATURE_NONE
					|| tm[x][y].height != origin_height) {
						origin_is_valid = false;
						break;
					}
					
					if (!add_coords(spot_tile_coords, x, y)) goto out;
					
				}
				
				if (!origin_is_valid) continue;
				if (has_mirror_overlap_array(spot_tile_coords)) continue;
				
				u32 tiles_per_share = spot_size / surface_share_count;
				
				u32 stone_count = stone_share * tiles_per_share;
				u32 gravel_count = gravel_share * tiles_per_share;
				u32 grass_light_count = grass_light_share * tiles_per_share;
				u32 earth_and_stones_count = spot_size - (stone_count + gravel_count + grass_light_count);
				
				for (u32 tile_index = 0; tile_index < spot_tile_coords->usage; tile_index++) {
					
					u32 x = spot_tile_coords->data[tile_index].x;
					u32 y = spot_tile_coords->data[tile_index].y;
					
					Surface current_surface = SURFACE_EARTH;
					
					if (stone_count > 0) {
						current_surface = SURFACE_STONE;
						stone_count--;
					} else if (gravel_count > 0) {
						current_surface = SURFACE_GRAVEL;
						gravel_count--;
					} else if (grass_light_count > 0) {
						current_surface = SURFACE_GRASS_LIGHT;
						grass_light_count--;
					} else if (earth_and_stones_count > 0) {
						current_surface = SURFACE_EARTH_AND_STONES;
						earth_and_stones_count--;
					}
					
					if (current_surface != SURFACE_EARTH_AND_STONES) set_feature(map, x, y, FEATURE_QUARRY_SPOT);
					set_surface(map, x, y, current_surface);
					
				}
				
				spots_placed++;
				break;
				
			}
			
		}
		
	}
	
	if (spots_placed < spot_count) {
		if (!add_to_string(logs, "generate_quarry_spots: %d/%d quarry spots placed\n", spots_placed, spot_count)) goto out;
	}
	
	ret = true;
	
out:
	free_coords_array(spot_tile_coords);
	free_coords_array(spot_tile_offsets);
	free_coords_array(available_origins);
	return ret;
}
