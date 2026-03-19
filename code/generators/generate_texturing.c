#include "utils.h"
#include "map.h"
#include "config.h"
#include "helpers.h"

bool generate_texturing(Map *map, Config *config, u32 *rng_state, DynamicString *logs)
{
	(void)logs;
	
	bool ret = false;
	
	CoordsArray *valid_tiles = NULL;
	CoordsArray *blob_tiles = NULL;
	
	Tile **tm = map->tile_matrix;
	
	struct {
		Surface surface;
		Range blob_count_range;
		Range blob_size_range;
	} texture_settings[] = {
		{SURFACE_BEACH, config->texturing_beach_blob_count_range, config->texturing_beach_blob_size_range},
		{SURFACE_DUNES, config->texturing_dunes_blob_count_range, config->texturing_dunes_blob_size_range},
		{SURFACE_EARTH_AND_STONES, config->texturing_earth_and_stones_blob_count_range, config->texturing_earth_and_stones_blob_size_range},
		{SURFACE_GRASS_LIGHT, config->texturing_grass_light_blob_count_range, config->texturing_grass_light_blob_size_range},
		{SURFACE_GRASS_MEDIUM, config->texturing_grass_medium_blob_count_range, config->texturing_grass_medium_blob_size_range},
	};
	
	if (!(valid_tiles = init_coords_array())) goto out;
	
	for (u32 x = 0; x < MAP_SIZE; x++) {
		for (u32 y = 0; y < MAP_SIZE; y++) {
			if (tm[x][y].section != SECTION_VALID) continue;
			if (!add_coords(valid_tiles, x, y)) goto out;
		}
	}
	
	if (!(blob_tiles = init_coords_array())) goto out;
	
	for (u32 setting_index = 0; setting_index < sizeof(texture_settings) / sizeof(texture_settings[0]); setting_index++) {
		
		Surface current_surface = texture_settings[setting_index].surface;
		
		Range blob_count_range = texture_settings[setting_index].blob_count_range;
		u32 blob_count = random(rng_state, blob_count_range.min, blob_count_range.max);
		
		Range blob_size_range = texture_settings[setting_index].blob_size_range;
		
		for (u32 blob_index = 0; blob_index < blob_count; blob_index++) {
			
			if (!insert_blob(blob_tiles, random(rng_state, blob_size_range.min, blob_size_range.max), rng_state)) goto out;
			
			u32 chosen_tile_index = random(rng_state, 0, valid_tiles->usage - 1);
			u32 x_origin = valid_tiles->data[chosen_tile_index].x;
			u32 y_origin = valid_tiles->data[chosen_tile_index].y;
			
			for (u32 tile_index = 0; tile_index < blob_tiles->usage; tile_index++) {
				
				u32 x = x_origin + blob_tiles->data[tile_index].x;
				u32 y = y_origin + blob_tiles->data[tile_index].y;
				
				if (!is_in_bounds(x, y)
				|| tm[x][y].section != SECTION_VALID
				|| tm[x][y].feature == FEATURE_RAMP
				|| tm[x][y].surface != SURFACE_EARTH) continue;
				
				tm[x][y].surface = current_surface;
				
			}
			
			blob_tiles->usage = 0;
			
		}
		
	}
	
	ret = true;
	
out:
	free_coords_array(blob_tiles);
	free_coords_array(valid_tiles);
	return ret;
}
