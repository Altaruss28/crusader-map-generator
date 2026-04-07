#include "utils.h"
#include "map.h"
#include "mirror.h"
#include "config.h"
#include "helpers.h"

bool generate_pitch_rig_spots(Map *map, Config *config, u32 *rng_state, DynamicString *logs)
{
	bool ret = false;
	
	CoordsArray *available_origins = NULL;
	
	Tile **tm = map->tile_matrix;
	
	if (!(available_origins = get_ai_resource_spot_origins())) goto out;
	
	u32 feature_count_multiplier = config->feature_count_multiplier;
	
	Range spot_count_range = config->pitch_rig_spot_count_range;
	u32 spot_count = random(rng_state, spot_count_range.min, spot_count_range.max) * feature_count_multiplier;
	u32 spot_index = 0;
	
	u32 cage_rock_chance = config->pitch_rig_spot_cage_rock_chance;
	
	while (spot_index < spot_count && available_origins->usage > 0) {
		
		u32 chosen_tile_index_spot = random(rng_state, 0, available_origins->usage - 1);
		u32 x_origin = available_origins->data[chosen_tile_index_spot].x;
		u32 y_origin = available_origins->data[chosen_tile_index_spot].y;
		remove_coords(available_origins, chosen_tile_index_spot);
		
		for (u32 x = x_origin; x < x_origin + 6; x++) {
			for (u32 y = y_origin; y < y_origin + 6; y++) {
				
				if (tm[x][y].section == SECTION_VALID
				&& (tm[x][y].feature == FEATURE_NONE
				|| ((x == x_origin || x == x_origin + 5 || y == y_origin || y == y_origin + 5)
				&& tm[x][y].feature == FEATURE_PITCH_RIG_SPOT))
				&& (tm[x][y].height == 8
				|| tm[x][y].feature == FEATURE_PITCH_RIG_SPOT)) continue;
				
				goto next_attempt;
				
			}
		}
		
		if (has_mirror_overlap_rectangle(x_origin + 1, y_origin + 1, 4, 4)) continue;
		
		for (u32 x = x_origin; x < x_origin + 6; x++) {
			for (u32 y = y_origin; y < y_origin + 6; y++) {
				set_feature(map, x, y, FEATURE_PITCH_RIG_SPOT);
			}
		}
		
		u8 inside_heights[4][4] = {
			{6, 5, 5, 6},
			{5, 4, 4, 5},
			{5, 4, 4, 5},
			{6, 5, 5, 6},
		};
		
		for (u32 x_height = 0; x_height < 4; x_height++) {
			for (u32 y_height = 0; y_height < 4; y_height++) {
				u32 x = x_origin + 1 + x_height;
				u32 y = y_origin + 1 + y_height;
				
				set_height(map, x, y, inside_heights[x_height][y_height]);
				set_surface(map, x, y, SURFACE_MARSH);
			}
		}
		
		Coords ring_offsets[8] = {
			{random(rng_state, 3, 4), 0},
			{5, random(rng_state, 1, 2)},
			{5, random(rng_state, 3, 4)},
			{random(rng_state, 3, 4), 5},
			{random(rng_state, 1, 2), 5},
			{0, random(rng_state, 3, 4)},
			{0, random(rng_state, 1, 2)},
			{random(rng_state, 1, 2), 0},
		};
		
		for (u32 i = 0; i < 8; i++) {
			u32 x = x_origin + ring_offsets[i].x;
			u32 y = y_origin + ring_offsets[i].y;
			
			set_height(map, x, y, 7);
			set_surface(map, x, y, SURFACE_MARSH);
		}
		
		struct {
			Coords rocks_base;
			Coords oil_offset;
		} sides[4] = {
			{{random(rng_state, 2, 3), 0}, {0, 1}}, 
			{{5, random(rng_state, 2, 3)}, {-1, 0}}, 
			{{random(rng_state, 2, 3), 5}, {0, -1}}, 
			{{0, random(rng_state, 2, 3)}, {1, 0}},
		};
		
		for (u32 i = 0; i < 4; i++) {
			u32 x_rocks = x_origin + sides[i].rocks_base.x;
			u32 y_rocks = y_origin + sides[i].rocks_base.y;
			
			set_height(map, x_rocks, y_rocks, 8);
			set_surface(map, x_rocks, y_rocks, SURFACE_ROCKS);
			
			if (cage_rock_chance >= (u32)random(rng_state, 0, 100)
			&& !has_mirror_overlap_rectangle(x_rocks, y_rocks, 1, 1))
				place_rock(map, x_rocks, y_rocks, 1);
			
			for (u32 j = 1; j <= 2; j++) {
				u32 x_oil = x_rocks + (sides[i].oil_offset.x * j);
				u32 y_oil = y_rocks + (sides[i].oil_offset.y * j);
				
				set_height(map, x_oil, y_oil, 4);
				set_surface(map, x_oil, y_oil, SURFACE_OIL);
			}
		}
		
		spot_index++;
		
		next_attempt:
		
	}
	
	if (spot_index < spot_count) {
		if (!add_to_string(logs, "generate_pitch_rig_spots: %d/%d pitch rig spots placed\n", spot_index, spot_count)) goto out;
	}
	
	ret = true;
	
out:
	free_coords_array(available_origins);
	return ret;
}
