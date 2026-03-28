#include "utils.h"
#include "map.h"
#include "mirror.h"
#include "config.h"
#include "helpers.h"

BuildingType size_one_options[] = {
	BUILDING_RUIN_1X1_ONE,
	BUILDING_RUIN_1X1_TWO,
	BUILDING_RUIN_1X1_THREE,
	BUILDING_RUIN_1X1_FOUR,
};
BuildingType size_two_options[] = {
	BUILDING_RUIN_2X2_ONE,
	BUILDING_RUIN_2X2_TWO,
	BUILDING_RUIN_2X2_THREE,
	BUILDING_RUIN_2X2_FOUR,
	BUILDING_RUIN_2X2_FIVE,
	BUILDING_RUIN_2X2_SIX,
};
BuildingType size_four_options[] = {
	BUILDING_RUIN_4X4_ONE,
	BUILDING_RUIN_4X4_TWO,
	BUILDING_RUIN_4X4_THREE,
};

struct {
	BuildingType *options;
	u32 option_count;
} ruin_options[] = {
	{size_one_options, sizeof(size_one_options) / sizeof(size_one_options[0])},
	{size_two_options, sizeof(size_two_options) / sizeof(size_two_options[0])},
	{size_four_options, sizeof(size_four_options) / sizeof(size_four_options[0])},
};

bool generate_ruins(Map *map, Config *config, u32 *rng_state, DynamicString *logs)
{
	bool ret = false;
	
	CoordsArray *available_origins = NULL;
	CoordsArray *blob_tile_offsets = NULL;
	CoordsArray *blob_tile_coords = NULL;
	
	Tile **tm = map->tile_matrix;
	
	u32 feature_count_multiplier = config->feature_count_multiplier;
	
	Range blob_count_range = config->ruin_blob_count_range;
	u32 blob_count = random(rng_state, blob_count_range.min, blob_count_range.max) * feature_count_multiplier;
	u32 blobs_placed = 0;
	
	Range blob_size_range = config->ruin_blob_size_range;
	i32 count_per_blob_modifier = config->ruin_count_per_blob_modifier;
	
	if (!(available_origins = init_coords_array())) goto out;
	
	for (u32 x = 0; x < MAP_SIZE; x++) {
		for (u32 y = 0; y < MAP_SIZE; y++) {
			if (tm[x][y].section != SECTION_VALID
			|| tm[x][y].feature != FEATURE_NONE) continue;
			if (!add_coords(available_origins, x, y)) goto out;
		}
	}
	
	if (available_origins->usage > 0) {
		
		if (!(blob_tile_offsets = init_coords_array())) goto out;
		if (!(blob_tile_coords = init_coords_array())) goto out;
		
		shuffle_coords_array(available_origins, rng_state);
		u32 claimed_matrix[FLAG_MATRIX_WORD_COUNT];
		
		for (u32 blob_index = 0; blob_index < blob_count; blob_index++) {
			
			blob_tile_offsets->usage = 0;
			if (!insert_blob(blob_tile_offsets, random(rng_state, blob_size_range.min, blob_size_range.max), rng_state)) goto out;
			
			for (u32 origin_index = 0; origin_index < available_origins->usage; origin_index++) {
				
				u32 x_blob_origin = available_origins->data[origin_index].x;
				u32 y_blob_origin = available_origins->data[origin_index].y;
				
				bool pick_is_valid = true;
				
				blob_tile_coords->usage = 0;
				clear_all_flags(claimed_matrix);
				
				for (u32 tile_index = 0; tile_index < blob_tile_offsets->usage; tile_index++) {
					
					u32 x = x_blob_origin + blob_tile_offsets->data[tile_index].x;
					u32 y = y_blob_origin + blob_tile_offsets->data[tile_index].y;
					
					if (!is_in_bounds(x, y)
					|| tm[x][y].section != SECTION_VALID
					|| tm[x][y].feature != FEATURE_NONE) {
						pick_is_valid = false;
						break;
					}
					
					set_flag(claimed_matrix, x, y, false);
					if (!add_coords(blob_tile_coords, x, y)) goto out;
					
				}
				
				if (!pick_is_valid) continue;
				if (has_mirror_overlap_array(blob_tile_coords)) continue;
				
				shuffle_coords_array(blob_tile_coords, rng_state);
				
				u32 ruin_count = blob_tile_coords->usage / count_per_blob_modifier;
				
				for (u32 ruin_index = 0; ruin_index < ruin_count; ruin_index++) {
					
					for (u32 ruin_size = random(rng_state, 1, 4); ruin_size > 0; ruin_size--) {
						
						if (ruin_size == 3) continue;
						
						for (u32 tile_index = 0; tile_index < blob_tile_coords->usage; tile_index++) {
							
							u32 x_ruin_origin = blob_tile_coords->data[tile_index].x;
							u32 y_ruin_origin = blob_tile_coords->data[tile_index].y;
							
							for (u32 x = x_ruin_origin; x < x_ruin_origin + ruin_size; x++) {
								for (u32 y = y_ruin_origin; y < y_ruin_origin + ruin_size; y++) {
									if (is_in_bounds(x, y)
									&& test_flag(claimed_matrix, x, y)
									&& tm[x][y].object_flags == 0) continue;
									goto next_tile;
								}
							}
							
							for (u32 x = x_ruin_origin; x < x_ruin_origin + ruin_size; x++) {
								for (u32 y = y_ruin_origin; y < y_ruin_origin + ruin_size; y++) {
									set_feature(map, x, y, FEATURE_RUIN);
								}
							}
							
							u32 option_index = ruin_size / 2;
							
							u32 option_count = ruin_options[option_index].option_count;
							BuildingType ruin_type = ruin_options[option_index].options[random(rng_state, 0, option_count - 1)];
							
							if (!place_building(map, x_ruin_origin, y_ruin_origin, ruin_type, 0, 0, false)) goto out;
							goto next_ruin;
							
							next_tile:
							
						}
						
					}
					
					next_ruin:
					
				}
				
				blobs_placed++;
				break;
				
			}
			
		}
		
	}
	
	if (blobs_placed < blob_count) {
		if (!add_to_string(logs, "generate_ruins: %d/%d ruin blobs placed\n", blobs_placed, blob_count)) goto out;
	}
	
	ret = true;
	
out:
	free_coords_array(blob_tile_coords);
	free_coords_array(blob_tile_offsets);
	free_coords_array(available_origins);
	return ret;
}