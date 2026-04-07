#include "utils.h"
#include "map.h"
#include "mirror.h"
#include "config.h"
#include "helpers.h"

bool generate_iron_mine_spots(Map *map, Config *config, u32 *rng_state, DynamicString *logs)
{
	bool ret = false;
	
	CoordsArray *available_origins = NULL;
	
	Tile **tm = map->tile_matrix;
	
	if (!(available_origins = get_ai_resource_spot_origins())) goto out;
	
	u32 feature_count_multiplier = config->feature_count_multiplier;
	
	Range spot_count_range = config->iron_mine_spot_count_range;
	u32 spot_count = random(rng_state, spot_count_range.min, spot_count_range.max) * feature_count_multiplier;
	u32 spot_index = 0;
	
	u32 cage_rock_chance = config->iron_mine_spot_cage_rock_chance;
	
	while (spot_index < spot_count && available_origins->usage > 0) {
		
		u32 chosen_tile_index_spot = random(rng_state, 0, available_origins->usage - 1);
		u32 x_origin = available_origins->data[chosen_tile_index_spot].x;
		u32 y_origin = available_origins->data[chosen_tile_index_spot].y;
		remove_coords(available_origins, chosen_tile_index_spot);
		
		u32 origin_height = tm[x_origin][y_origin].height;
		
		for (u32 x = x_origin; x < x_origin + 6; x++) {
			for (u32 y = y_origin; y < y_origin + 6; y++) {
				
				if (tm[x][y].section == SECTION_VALID
				&& (tm[x][y].feature == FEATURE_NONE
				|| ((x == x_origin || x == x_origin + 5 || y == y_origin || y == y_origin + 5)
				&& tm[x][y].feature == FEATURE_IRON_MINE_SPOT))
				&& tm[x][y].height == origin_height) continue;
				
				goto next_attempt;
				
			}
		}
		
		if (has_mirror_overlap_rectangle(x_origin + 1, y_origin + 1, 4, 4)) continue;
		
		for (u32 x = x_origin; x < x_origin + 6; x++) {
			for (u32 y = y_origin; y < y_origin + 6; y++) {
				set_feature(map, x, y, FEATURE_IRON_MINE_SPOT);
			}
		}
		
		Coords middle_iron[4] = {{3, 2}, {3, 3}, {2, 3}, {2, 2}};
		Coords corner_iron[4] = {{4, 1}, {4, 4}, {1, 4}, {1, 1}};
		
		struct {
			Coords rocks_base;
			Coords iron_offset;
		} sides[4] = {
			{{random(rng_state, 2, 3), 0}, {0, 1}}, 
			{{5, random(rng_state, 2, 3)}, {-1, 0}}, 
			{{random(rng_state, 2, 3), 5}, {0, -1}}, 
			{{0, random(rng_state, 2, 3)}, {1, 0}},
		};
		
		for (u32 i = 0; i < 4; i++) {
			set_surface(map, x_origin + middle_iron[i].x, y_origin + middle_iron[i].y, SURFACE_IRON);
			if (random(rng_state, 0, 1)) set_surface(map, x_origin + corner_iron[i].x, y_origin + corner_iron[i].y, SURFACE_IRON);
			
			u32 x_side_rocks = x_origin + sides[i].rocks_base.x;
			u32 y_side_rocks = y_origin + sides[i].rocks_base.y;
			
			u32 x_side_iron = x_side_rocks + sides[i].iron_offset.x;
			u32 y_side_iron = y_side_rocks + sides[i].iron_offset.y;
			
			set_surface(map, x_side_rocks, y_side_rocks, SURFACE_ROCKS);
			
			if (cage_rock_chance >= (u32)random(rng_state, 0, 100)
			&& !has_mirror_overlap_rectangle(x_side_rocks, y_side_rocks, 1, 1)) 
				place_rock(map, x_side_rocks, y_side_rocks, 1);
			
			set_surface(map, x_side_iron, y_side_iron, SURFACE_IRON);
		}
		
		spot_index++;
		
		next_attempt:
		
	}
	
	if (spot_index < spot_count) {
		if (!add_to_string(logs, "generate_iron_mine_spots: %d/%d iron mine spots placed\n", spot_index, spot_count)) goto out;
	}
	
	ret = true;
	
out:
	free_coords_array(available_origins);
	return ret;
}
