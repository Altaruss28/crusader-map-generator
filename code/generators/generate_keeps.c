#include "utils.h"
#include "map.h"
#include "mirror.h"
#include "config.h"
#include <stdlib.h>

bool generate_keeps(Map *map, Config *config, u32 *rng_state, DynamicString *logs)
{
	bool ret = false;
	
	CoordsArray *potential_origins = NULL;
	
	Tile **tm = map->tile_matrix;
	
	if (!(potential_origins = init_coords_array())) goto out;
	
	for (u32 x = 0; x < MAP_SIZE; x++) {
		for (u32 y = 0; y < MAP_SIZE; y++) {
			
			if (tm[x][y].section != SECTION_VALID
			|| tm[x][y].feature != FEATURE_NONE) continue;
			
			if (!add_coords(potential_origins, x, y)) goto out;
			
		}
	}
	
	shuffle_coords_array(potential_origins, rng_state);
	
	u32 feature_count_multiplier = config->feature_count_multiplier;
	
	u32 mirror_point_count = config->mirror_point_count;
	u32 base_owner = 1;
	
	if (config->generate_players) {
		
		Range player_count_range = config->player_count_range;
		u32 player_count = random(rng_state, player_count_range.min, player_count_range.max) * feature_count_multiplier;
		u32 players_placed = 0;
		u32 player_reserved_area_size = 1 + (2 * config->player_reserved_area_radius);
		u32 player_door_offset = player_reserved_area_size / 2;
		u32 player_min_mirror_distance = config->player_min_mirror_distance;
		
		for (u32 tile_index = 0; tile_index < potential_origins->usage; tile_index++) {
			
			u32 x_origin = potential_origins->data[tile_index].x;
			u32 y_origin = potential_origins->data[tile_index].y;
			
			if (mirror_point_count > 1) {
				
				MirrorPoints mirror_points;
				get_mirror_points(&mirror_points, x_origin + player_door_offset, y_origin + player_door_offset, 1);
				
				u32 x1 = mirror_points.points[0].x;
				u32 y1 = mirror_points.points[0].y;
				u32 x2 = mirror_points.points[1].x;
				u32 y2 = mirror_points.points[1].y;
				
				if ((u32)max_int(abs((i32)x1 - (i32)x2), abs((i32)y1 - (i32)y2)) < player_min_mirror_distance) continue;
				
			}
			
			u32 origin_height = tm[x_origin][y_origin].height;
			
			for (u32 x = x_origin; x < x_origin + player_reserved_area_size; x++) {
				for (u32 y = y_origin; y < y_origin + player_reserved_area_size; y++) {
					
					if (is_in_bounds(x, y)
					&& tm[x][y].section == SECTION_VALID
					&& tm[x][y].feature == FEATURE_NONE
					&& tm[x][y].height == origin_height) continue;
					
					goto next_player_attempt;
					
				}
			}
			
			if (has_mirror_overlap_rectangle(x_origin, y_origin, player_reserved_area_size, player_reserved_area_size)) continue;
			
			for (u32 x = x_origin; x < x_origin + player_reserved_area_size; x++) {
				for (u32 y = y_origin; y < y_origin + player_reserved_area_size; y++) {
					set_feature(map, x, y, FEATURE_KEEP);
				}
			}
			
			if (!place_building(map, x_origin + player_door_offset - 7, y_origin + player_door_offset - 7, BUILDING_KEEP, base_owner, 0, true)) goto out;
			base_owner += mirror_point_count;
			
			if (++players_placed >= player_count) break;
			
			next_player_attempt:
			
		}
		
		if (players_placed < player_count) {
			if (!add_to_string(logs, "generate_keeps: %d/%d players placed\n", players_placed, player_count)) goto out;
		}
		
	}
	
	if (config->generate_spectators) {
		
		Range spectator_count_range = config->spectator_count_range;
		u32 spectator_count = random(rng_state, spectator_count_range.min, spectator_count_range.max) * feature_count_multiplier;
		u32 spectators_placed = 0;
		u32 spectator_reserved_area_size = 19;
		u32 spectator_door_offset = spectator_reserved_area_size / 2;
		u32 spectator_lion_count = config->spectator_lion_count;
		
		for (u32 tile_index = 0; tile_index < potential_origins->usage; tile_index++) {
			
			u32 x_origin = potential_origins->data[tile_index].x;
			u32 y_origin = potential_origins->data[tile_index].y;
			
			u32 origin_height = tm[x_origin][y_origin].height;
			
			for (u32 x = x_origin; x < x_origin + spectator_reserved_area_size; x++) {
				for (u32 y = y_origin; y < y_origin + spectator_reserved_area_size; y++) {
					
					if (is_in_bounds(x, y)
					&& tm[x][y].section == SECTION_VALID
					&& tm[x][y].feature == FEATURE_NONE
					&& tm[x][y].height == origin_height) continue;
					
					goto next_spectator_attempt;
					
				}
			}
			
			if (has_mirror_overlap_rectangle(x_origin, y_origin, spectator_reserved_area_size, spectator_reserved_area_size)) continue;
			
			for (u32 x = x_origin; x < x_origin + spectator_reserved_area_size; x++) {
				for (u32 y = y_origin; y < y_origin + spectator_reserved_area_size; y++) {
					
					set_feature(map, x, y, FEATURE_KEEP);
					
					if (x == x_origin || x == x_origin + spectator_reserved_area_size - 1 || y == y_origin || y == y_origin + spectator_reserved_area_size - 1) {
						if (!place_wall(map, x, y, x, y, WALL_HIGH, base_owner, true)) goto out;
					}
					
				}
			}
			
			u32 x_door = x_origin + spectator_door_offset;
			u32 y_door = y_origin + spectator_door_offset;
			
			if (!place_building(map, x_door - 7, y_door - 7, BUILDING_KEEP, base_owner, 0, true)) goto out;
			base_owner += mirror_point_count;
			
			for (u32 lion_index = 0; lion_index < spectator_lion_count; lion_index++) {
				if (!place_animal(map, x_door, y_door, ANIMAL_LION, 1)) goto out;
			}
			
			if (++spectators_placed >= spectator_count) break;
			
			next_spectator_attempt:
			
		}
		
		if (spectators_placed < spectator_count) {
			if (!add_to_string(logs, "generate_keeps: %d/%d spectators placed\n", spectators_placed, spectator_count)) goto out;
		}
		
	}
	
	ret = true;
	
out:
	free_coords_array(potential_origins);
	return ret;
}
