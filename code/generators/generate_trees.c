#include "utils.h"
#include "map.h"
#include "mirror.h"
#include "config.h"
#include "helpers.h"

static const PlantType dry_tree_options[] = {
	PLANT_TREE_COCONUT,
	PLANT_TREE_OLIVE,
};
static const PlantType wet_tree_options[] = {
	PLANT_TREE_DATE,
	PLANT_TREE_COCONUT,
	PLANT_TREE_CHERRY,
};

bool generate_trees(Map *map, Config *config, u32 *rng_state, DynamicString *logs)
{
	bool ret = false;
	
	CoordsArray *available_origins = NULL;
	CoordsArray *blob_tile_offsets = NULL;
	CoordsArray *blob_tile_coords = NULL;
	
	Tile **tm = map->tile_matrix;
	
	u32 feature_count_multiplier = config->feature_count_multiplier;
	
	Range blob_count_range = config->tree_blob_count_range;
	u32 blob_count = random(rng_state, blob_count_range.min, blob_count_range.max) * feature_count_multiplier;
	u32 blobs_placed = 0;
	
	Range blob_size_range = config->tree_blob_size_range;
	
	if (!(available_origins = init_coords_array())) goto out;
	
	for (u32 x = 0; x < MAP_SIZE; x++) {
		for (u32 y = 0; y < MAP_SIZE; y++) {
			
			if (tm[x][y].section != SECTION_VALID
			|| (tm[x][y].feature != FEATURE_NONE
			&& tm[x][y].feature != FEATURE_GRASSLAND)) continue;
			
			if (!add_coords(available_origins, x, y)) goto out;
			
		}
	}
	
	if (available_origins->usage > 0) {
		
		if (!(blob_tile_offsets = init_coords_array())) goto out;
		if (!(blob_tile_coords = init_coords_array())) goto out;
		
		shuffle_coords_array(available_origins, rng_state);
		
		for (u32 blob_index = 0; blob_index < blob_count; blob_index++) {
			
			blob_tile_offsets->usage = 0;
			if (!insert_blob(blob_tile_offsets, random(rng_state, blob_size_range.min, blob_size_range.max), rng_state)) goto out;
			
			for (u32 origin_index = 0; origin_index < available_origins->usage; origin_index++) {
				
				u32 x_blob_origin = available_origins->data[origin_index].x;
				u32 y_blob_origin = available_origins->data[origin_index].y;
				
				bool pick_is_valid = true;
				
				blob_tile_coords->usage = 0;
				
				for (u32 tile_index = 0; tile_index < blob_tile_offsets->usage; tile_index++) {
					
					u32 x = x_blob_origin + blob_tile_offsets->data[tile_index].x;
					u32 y = y_blob_origin + blob_tile_offsets->data[tile_index].y;
					
					if (!is_in_bounds(x, y)
					|| tm[x][y].section != SECTION_VALID
					|| (tm[x][y].feature != FEATURE_NONE
					&& tm[x][y].feature != FEATURE_GRASSLAND)) {
						pick_is_valid = false;
						break;
					}
					
					if (!add_coords(blob_tile_coords, x, y)) goto out;
					
				}
				
				if (!pick_is_valid) continue;
				if (has_mirror_overlap_array(blob_tile_coords)) continue;
				
				PlantType dry_option = dry_tree_options[random(rng_state, 0, (sizeof(dry_tree_options) / sizeof(dry_tree_options[0])) - 1)];
				PlantType wet_option = wet_tree_options[random(rng_state, 0, (sizeof(wet_tree_options) / sizeof(wet_tree_options[0])) - 1)];
				
				shuffle_coords_array(blob_tile_coords, rng_state);
				
				for (u32 blob_tile_index = 0; blob_tile_index < blob_tile_coords->usage; blob_tile_index++) {
					
					u32 x_tree_origin = blob_tile_coords->data[blob_tile_index].x;
					u32 y_tree_origin = blob_tile_coords->data[blob_tile_index].y;
					
					for (u32 x = x_tree_origin - 2; x <= x_tree_origin + 2; x++) {
						for (u32 y = y_tree_origin - 2; y <= y_tree_origin + 2; y++) {
							
							if ((tm[x][y].feature != FEATURE_NONE
							&& tm[x][y].feature != FEATURE_GRASSLAND)
							|| tm[x][y].object_flags & OF_PLANT) {
								goto next_tile;
							}
							
						}
					}
					
					if (has_mirror_overlap_rectangle(x_tree_origin - 1, y_tree_origin - 1, 3, 3)) continue;
					
					for (u32 x = x_tree_origin - 2; x <= x_tree_origin + 2; x++) {
						for (u32 y = y_tree_origin - 2; y <= y_tree_origin + 2; y++) {
							set_feature(map, x, y, FEATURE_TREE);
						}
					}
					
					PlantType type;
					
					Surface surface = tm[x_tree_origin][y_tree_origin].surface;
					
					if (surface == SURFACE_GRASS_DARK || surface == SURFACE_GRASS_MEDIUM || surface == SURFACE_GRASS_LIGHT) {
						type = wet_option;
					} else {
						type = dry_option;
					}
					
					u8 variant = random(rng_state, 0, 9);
					u8 stage = random(rng_state, 0, 3);
					
					if (!place_plant(map, x_tree_origin, y_tree_origin, type, variant, stage)) goto out;
					
					next_tile:
					
				}
				
				blobs_placed++;
				break;
				
			}
			
		}
		
	}
	
	if (blobs_placed < blob_count) {
		if (!add_to_string(logs, "generate_trees: %d/%d tree blobs placed\n", blobs_placed, blob_count)) goto out;
	}
	
	ret = true;
	
out:
	free_coords_array(blob_tile_coords);
	free_coords_array(blob_tile_offsets);
	free_coords_array(available_origins);
	return ret;
}
