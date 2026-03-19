#include "utils.h"
#include "map.h"
#include "mirror.h"
#include "config.h"
#include "helpers.h"

bool generate_heights(Map *map, Config *config, u32 *rng_state, DynamicString *logs)
{
	bool ret = false;
	
	CoordsArray *plateau_expandable_tiles = NULL;
	CoordsArray *plateau_available_tiles = NULL;
	CoordsArray *plateau_core_origins = NULL;
	CoordsArray *ramp_origins = NULL;
	CoordsArray *ramp_check_tiles = NULL;
	CoordsArray *ramp_current_tiles = NULL;
	CoordsArray *ramp_next_tiles = NULL;
	CoordsArray *cliff_current_tiles = NULL;
	CoordsArray *cliff_next_tiles = NULL;
	CoordsArray *cliff_grown_tiles = NULL;
	CoordsArray *sacrificed_tiles = NULL;
	
	Tile **tm = map->tile_matrix;
	
	if (!(plateau_expandable_tiles = init_coords_array())) goto out;
	if (!(plateau_available_tiles = init_coords_array())) goto out;
	
	u32 claimed_matrix[FLAG_MATRIX_WORD_COUNT];
	clear_all_flags(claimed_matrix);
	
	u32 border_matrix[FLAG_MATRIX_WORD_COUNT];
	clear_all_flags(border_matrix);
	
	if (config->generate_plateau_border) {
		
		u32 plateau_border_start_radius = config->plateau_border_start_radius;
		
		for (u32 x = 0; x < MAP_SIZE; x++) {
			for (u32 y = 0; y < MAP_SIZE; y++) {
				
				if (tm[x][y].section != SECTION_VALID) continue;
				
				u32 distance_from_center = tm[x][y].distance_from_center;
				
				if (distance_from_center >= plateau_border_start_radius) {
					
					set_flag(claimed_matrix, x, y, true);
					set_flag(border_matrix, x, y, true);
					
					if (distance_from_center == plateau_border_start_radius) {
						if (!add_coords(plateau_expandable_tiles, x, y)) goto out;
					}
					
				} else {
					if (!add_coords(plateau_available_tiles, x, y)) goto out;
				}
				
			}
		}
		
	} else {
		
		for (u32 x = 0; x < MAP_SIZE; x++) {
			for (u32 y = 0; y < MAP_SIZE; y++) {
				if (tm[x][y].section != SECTION_VALID) continue;
				if (!add_coords(plateau_available_tiles, x, y)) goto out;
			}
		}
		
	}
	
	u32 feature_count_multiplier = config->feature_count_multiplier;
	
	u32 *plateau_heights = (u32 *)config->plateau_heights->data;
	u32 plateau_height_count = config->plateau_heights->usage;
	
	u32 target_core_count = random(rng_state, config->plateau_core_count_range.min, config->plateau_core_count_range.max) * feature_count_multiplier;
	u32 core_index = 0;
	
	Range core_size_range = config->plateau_core_size_range;
	u32 core_spacing_min = config->plateau_core_spacing_min;
	bool core_allow_mirror_overlap = config->plateau_core_allow_mirror_overlap;
	
	if (!(plateau_core_origins = init_coords_array())) goto out;
	
	while (core_index < target_core_count && plateau_available_tiles->usage > 0) {
		
		u32 chosen_tile_index = random(rng_state, 0, plateau_available_tiles->usage - 1);
		u32 x_origin = plateau_available_tiles->data[chosen_tile_index].x;
		u32 y_origin = plateau_available_tiles->data[chosen_tile_index].y;
		remove_coords(plateau_available_tiles, chosen_tile_index);
		
		if (test_flag(claimed_matrix, x_origin, y_origin)) continue;
		
		u32 core_width = random(rng_state, core_size_range.min, core_size_range.max);
		u32 core_length = random(rng_state, core_size_range.min, core_size_range.max);
		
		u32 x_base = x_origin - core_spacing_min;
		u32 y_base = y_origin - core_spacing_min;
		u32 check_width = core_width + (core_spacing_min * 2);
		u32 check_length = core_length + (core_spacing_min * 2);
		
		if (!is_in_bounds_rectangle(x_base, y_base, check_width, check_length)) continue;
		
		for (u32 x = x_base; x < x_base + check_width; x++) {
			for (u32 y = y_base; y < y_base + check_length; y++) {
				
				if (tm[x][y].section != SECTION_VALID
				|| test_flag(claimed_matrix, x, y)) {
					goto next_core;
				}
				
			}
		}
		
		if (!core_allow_mirror_overlap) {
			
			u32 half_spacing = core_spacing_min % 2 ? (core_spacing_min / 2) + 1 : core_spacing_min / 2;
			u32 full_spacing = half_spacing * 2;
			
			if (has_mirror_overlap_rectangle(x_origin - half_spacing, y_origin - half_spacing, core_width + full_spacing, core_length + full_spacing)) continue;
			
		}
		
		u32 core_height = core_index < plateau_height_count ? plateau_heights[core_index] : plateau_heights[random(rng_state, 0, plateau_height_count - 1)];
		
		for (u32 x = x_origin; x < x_origin + core_width; x++) {
			for (u32 y = y_origin; y < y_origin + core_length; y++) {
				
				set_flag(claimed_matrix, x, y, true);
				set_height(map, x, y, core_height);
				
				if (x != x_origin && x != x_origin + core_width - 1 && y != y_origin && y != y_origin + core_length - 1) continue;
				
				if (!add_coords(plateau_expandable_tiles, x, y)) goto out;
				
			}
		}
		
		if (!add_coords(plateau_core_origins, x_origin + (core_width / 2), y_origin + (core_length / 2))) goto out;
		core_index++;
		
		next_core:
		
	}
	
	if (core_index < target_core_count) {
		if (!add_to_string(logs, "generate_heights: %d/%d plateau cores placed\n", core_index, target_core_count)) goto out;
	}
	
	while (plateau_expandable_tiles->usage > 0) {
		
		u32 chosen_tile_index = random(rng_state, 0, plateau_expandable_tiles->usage - 1);
		u32 x_origin = plateau_expandable_tiles->data[chosen_tile_index].x;
		u32 y_origin = plateau_expandable_tiles->data[chosen_tile_index].y;
		remove_coords(plateau_expandable_tiles, chosen_tile_index);
		
		u32 origin_height = tm[x_origin][y_origin].height;
		
		bool is_border = test_flag(border_matrix, x_origin, y_origin);
		
		for (u32 offset_index = 0; offset_index < 4; offset_index++) {
			
			u32 x = x_origin + four_direction_offsets[offset_index].x;
			u32 y = y_origin + four_direction_offsets[offset_index].y;
			
			if (tm[x][y].section != SECTION_VALID
			|| test_flag(claimed_matrix, x, y)) continue;
			
			set_flag(claimed_matrix, x, y, true);
			if (is_border) set_flag(border_matrix, x, y, true);
			set_height(map, x, y, origin_height);
			if (!add_coords(plateau_expandable_tiles, x, y)) goto out;
			
		}
		
	}
	
	for (u32 x_origin = 0; x_origin < MAP_SIZE; x_origin++) {
		for (u32 y_origin = 0; y_origin < MAP_SIZE; y_origin++) {
			
			if (tm[x_origin][y_origin].section != SECTION_VALID) continue;
			
			u32 origin_height = tm[x_origin][y_origin].height;
			
			for (u32 offset_index = 0; offset_index < 8; offset_index++) {
				
				u32 x = x_origin + eight_direction_offsets[offset_index].x;
				u32 y = y_origin + eight_direction_offsets[offset_index].y;
				
				if (tm[x][y].height >= (i32)origin_height - 16) continue;
				
				set_feature(map, x_origin, y_origin, FEATURE_CLIFF);
				set_surface(map, x_origin, y_origin, SURFACE_ROCKS);
				break;
				
			}
			
		}
	}
	
	if (config->generate_ramps) {
		
		Range ramp_count_modifier_range = config->ramp_count_modifier_range;
		bool ramp_restrict_growth_towards_edge = config->ramp_restrict_growth_towards_edge;
		u32 ramp_base_max_distance_from_center = config->ramp_base_max_distance_from_center;
		Range ramp_size_range = config->ramp_size_range;
		u32 ramp_step_height_base_difference = config->ramp_step_height_base_difference;
		Range ramp_step_height_variance_range = config->ramp_step_height_variance_range;
		
		if (!(ramp_origins = init_coords_array())) goto out;
		if (!(ramp_check_tiles = init_coords_array())) goto out;
		
		if (!(ramp_current_tiles = init_coords_array())) goto out;
		if (!(ramp_next_tiles = init_coords_array())) goto out;
		
		if (ramp_restrict_growth_towards_edge) {
			
			for (u32 x_origin = 0; x_origin < MAP_SIZE; x_origin++) {
				for (u32 y_origin = 0; y_origin < MAP_SIZE; y_origin++) {
					
					if (tm[x_origin][y_origin].section != SECTION_VALID
					|| tm[x_origin][y_origin].feature != FEATURE_CLIFF) continue;
					
					for (u32 offset_index = 0; offset_index < 8; offset_index++) {
						
						u32 x = x_origin + eight_direction_offsets[offset_index].x;
						u32 y = y_origin + eight_direction_offsets[offset_index].y;
						
						if (!test_flag(border_matrix, x, y) || tm[x][y].feature == FEATURE_CLIFF) continue;
						
						set_flag(border_matrix, x_origin, y_origin, true);
						break;
						
					}
					
				}
			}
			
		}
		
		for (u32 core_origin_index = 0; core_origin_index < plateau_core_origins->usage; core_origin_index++) {
			
			u32 x_core_origin = plateau_core_origins->data[core_origin_index].x;
			u32 y_core_origin = plateau_core_origins->data[core_origin_index].y;
			
			ramp_origins->usage = 0;
			
			for (u32 offset_index = 0; offset_index < 8; offset_index++) {
				
				u32 x_step_offset = eight_direction_offsets[offset_index].x;
				u32 y_step_offset = eight_direction_offsets[offset_index].y;
				
				for (u32 step = 1; step < MAP_SIZE; step++) {
					
					u32 x = x_core_origin + (x_step_offset * step);
					u32 y = y_core_origin + (y_step_offset * step);
					
					if (tm[x][y].section != SECTION_VALID
					|| tm[x][y].distance_from_center > ramp_base_max_distance_from_center
					|| tm[x][y].feature == FEATURE_RAMP
					|| test_flag(border_matrix, x, y)) break;
					
					if (tm[x][y].feature != FEATURE_CLIFF) continue;
					
					if (!add_coords(ramp_origins, x, y)) goto out;
					break;
					
				}
				
			}
			
			u32 ramp_count = random(rng_state, ramp_count_modifier_range.min, ramp_count_modifier_range.max);
			u32 ramps_placed = 0;
			
			while (ramps_placed < ramp_count && ramp_origins->usage > 0) {
				
				u32 chosen_origin_index = random(rng_state, 0, ramp_origins->usage - 1);
				u32 x_ramp_origin = ramp_origins->data[chosen_origin_index].x;
				u32 y_ramp_origin = ramp_origins->data[chosen_origin_index].y;
				remove_coords(ramp_origins, chosen_origin_index);
				
				if (tm[x_ramp_origin][y_ramp_origin].feature == FEATURE_RAMP) continue;
				
				ramp_check_tiles->usage = 0;
				if (!add_coords(ramp_check_tiles, x_ramp_origin, y_ramp_origin)) goto out;
				
				u32 target_size = random(rng_state, ramp_size_range.min, ramp_size_range.max);
				u32 current_size = 1;
				u32 target_height = tm[x_ramp_origin][y_ramp_origin].height;
				
				while (current_size < target_size && ramp_check_tiles->usage > 0) {
					
					u32 chosen_tile_index = random(rng_state, 0, ramp_check_tiles->usage - 1);
					u32 x_origin = ramp_check_tiles->data[chosen_tile_index].x;
					u32 y_origin = ramp_check_tiles->data[chosen_tile_index].y;
					remove_coords(ramp_check_tiles, chosen_tile_index);
					
					for (u32 offset_index = 0; offset_index < 4; offset_index++) {
						
						u32 x = x_origin + four_direction_offsets[offset_index].x;
						u32 y = y_origin + four_direction_offsets[offset_index].y;
						
						if (tm[x][y].distance_from_center > ramp_base_max_distance_from_center
						|| tm[x][y].feature != FEATURE_CLIFF
						|| tm[x][y].height != target_height
						|| test_flag(border_matrix, x, y)) continue;
						
						set_feature(map, x, y, FEATURE_RAMP);
						set_surface(map, x, y, SURFACE_EARTH);
						if (!add_coords(ramp_check_tiles, x, y)) goto out;
						if (!add_coords(ramp_current_tiles, x, y)) goto out;
						
						if (++current_size >= target_size) break;
						
					}
					
				}
				
				ramps_placed++;
				
			}
			
		}
		
		while (ramp_current_tiles->usage > 0) {
			
			for (u32 tile_index = 0; tile_index < ramp_current_tiles->usage; tile_index++) {
				
				u32 x_origin = ramp_current_tiles->data[tile_index].x;
				u32 y_origin = ramp_current_tiles->data[tile_index].y;
				
				i32 target_height = tm[x_origin][y_origin].height - ramp_step_height_base_difference + random(rng_state, ramp_step_height_variance_range.min, ramp_step_height_variance_range.max);
				
				for (u32 offset_index = 0; offset_index < 4; offset_index++) {
					
					u32 x = x_origin + four_direction_offsets[offset_index].x;
					u32 y = y_origin + four_direction_offsets[offset_index].y;
					
					if (tm[x][y].section != SECTION_VALID
					|| tm[x][y].height > target_height) continue;
					
					set_feature(map, x, y, FEATURE_RAMP);
					set_height(map, x, y, target_height);
					set_surface(map, x, y, SURFACE_EARTH);
					if (!add_coords(ramp_next_tiles, x, y)) goto out;
					
				}
				
			}
			
			CoordsArray *swap_ptr = ramp_current_tiles;
			ramp_current_tiles = ramp_next_tiles;
			ramp_next_tiles = swap_ptr;
			ramp_next_tiles->usage = 0;
			
		}
		
	}
	
	if (config->generate_cliffs) {
		
		u32 cliff_step_height_difference = config->cliff_step_height_difference;
		Range cliff_growth_factor_range = config->cliff_growth_factor_range;
		
		if (!(cliff_current_tiles = init_coords_array())) goto out;
		if (!(cliff_next_tiles = init_coords_array())) goto out;
		
		if (!(cliff_grown_tiles = init_coords_array())) goto out;
		
		for (u32 x = 0; x < MAP_SIZE; x++) {
			for (u32 y = 0; y < MAP_SIZE; y++) {
				
				if (tm[x][y].section != SECTION_VALID
				|| tm[x][y].feature != FEATURE_CLIFF) continue;
				
				if (!add_coords(cliff_current_tiles, x, y)) goto out;
				
			}
		}
		
		while (cliff_current_tiles->usage > 0) {
			
			for (u32 tile_index = 0; tile_index < cliff_current_tiles->usage; tile_index++) {
				
				u32 x_origin = cliff_current_tiles->data[tile_index].x;
				u32 y_origin = cliff_current_tiles->data[tile_index].y;
				
				i32 target_height = tm[x_origin][y_origin].height - cliff_step_height_difference;
				
				u32 growth_amount = random(rng_state, cliff_growth_factor_range.min, cliff_growth_factor_range.max);
				if (growth_amount == 0) continue;
				
				for (u32 offset_index = 0; offset_index < 4; offset_index++) {
					for (u32 step = 1; step <= growth_amount; step++) {
						
						u32 x = x_origin + (step * four_direction_offsets[offset_index].x);
						u32 y = y_origin + (step * four_direction_offsets[offset_index].y);
						
						if (!is_in_bounds(x, y) || tm[x][y].section != SECTION_VALID) break;
						
						Feature current_feature = tm[x][y].feature;
						u32 current_height = tm[x][y].height;
						
						if ((current_feature != FEATURE_CLIFF && (i32)current_height > target_height)
						|| (current_feature == FEATURE_CLIFF && (i32)current_height >= target_height)) break;
						
						set_feature(map, x, y, FEATURE_CLIFF);
						set_height(map, x, y, target_height);
						set_surface(map, x, y, SURFACE_ROCKS);
						if (!add_coords(cliff_next_tiles, x, y)) goto out;
						if (!add_coords(cliff_grown_tiles, x, y)) goto out;
						
					}
				}
				
			}
			
			CoordsArray *swap_ptr = cliff_current_tiles;
			cliff_current_tiles = cliff_next_tiles;
			cliff_next_tiles = swap_ptr;
			cliff_next_tiles->usage = 0;
			
		}
		
		u32 check_stop_index = cliff_grown_tiles->usage;
		
		for (u32 tile_index = 0; tile_index < check_stop_index; tile_index++) {
			
			u32 x_origin = cliff_grown_tiles->data[tile_index].x;
			u32 y_origin = cliff_grown_tiles->data[tile_index].y;
			
			u32 origin_height = tm[x_origin][y_origin].height;
			
			for (u32 offset_index = 0; offset_index < 8; offset_index++) {
				
				u32 x = x_origin + eight_direction_offsets[offset_index].x;
				u32 y = y_origin + eight_direction_offsets[offset_index].y;
				
				if (tm[x][y].feature == FEATURE_CLIFF
				|| tm[x][y].height >= origin_height) continue;
				
				set_feature(map, x, y, FEATURE_CLIFF);
				set_surface(map, x, y, SURFACE_ROCKS);
				if (!add_coords(cliff_grown_tiles, x, y)) goto out;
				
			}
			
		}
		
		if (config->generate_cliff_rocks) {
			
			shuffle_coords_array(cliff_grown_tiles, rng_state);
			u32 target_rock_count = (cliff_grown_tiles->usage / config->cliff_rock_count_modifier) * feature_count_multiplier;
			u32 rocks_placed = 0;
			
			for (u32 rock_size = 4; rock_size >= 1; rock_size--) {
				
				if (rocks_placed >= target_rock_count) break;
				
				for (u32 tile_index = 0; tile_index < cliff_grown_tiles->usage; tile_index++) {
					
					u32 x_origin = cliff_grown_tiles->data[tile_index].x;
					u32 y_origin = cliff_grown_tiles->data[tile_index].y;
					
					if (!is_in_bounds_rectangle(x_origin, y_origin, rock_size, rock_size)) continue;
					
					u32 target_height = tm[x_origin][y_origin].height;
					
					for (u32 x = x_origin; x < x_origin + rock_size; x++) {
						for (u32 y = y_origin; y < y_origin + rock_size; y++) {
							
							if (tm[x][y].feature != FEATURE_CLIFF
							|| tm[x][y].height != target_height
							|| tm[x][y].object_flags & OF_ROCK) {
								goto next_rock;
							}
							
						}
					}
					
					if (has_mirror_overlap_rectangle(x_origin, y_origin, rock_size, rock_size)) continue;
					
					if (!place_rock(map, x_origin, y_origin, rock_size)) goto out;
					if (++rocks_placed >= target_rock_count) break;
					
					next_rock:
					
				}
				
			}
			
		}
		
	}
	
	if (!(sacrificed_tiles = init_coords_array())) goto out;
	
	for (u32 x_origin = 0; x_origin < MAP_SIZE; x_origin++) {
		for (u32 y_origin = 0; y_origin < MAP_SIZE; y_origin++) {
			
			if (tm[x_origin][y_origin].section != SECTION_SACRIFICED) continue;
			
			bool valid_found = false;
			
			for (u32 offset_index = 0; offset_index < 8; offset_index++) {
				
				u32 x = x_origin + eight_direction_offsets[offset_index].x;
				u32 y = y_origin + eight_direction_offsets[offset_index].y;
				
				if (tm[x][y].section != SECTION_VALID) continue;
				
				Feature feature = tm[x][y].feature;
				if (feature != FEATURE_CLIFF && feature != FEATURE_RAMP) continue;
				
				valid_found = true;
				break;
				
			}
			
			if (!valid_found) continue;
			
			if (!add_coords(sacrificed_tiles, x_origin, y_origin)) goto out;
			
		}
	}
	
	for (u32 cycle = 0; cycle < 2; cycle++) {
		for (i32 tile_index = sacrificed_tiles->usage - 1; tile_index >= 0; tile_index--) {
			
			u32 x_origin = sacrificed_tiles->data[tile_index].x;
			u32 y_origin = sacrificed_tiles->data[tile_index].y;
			
			bool target_found = false;
			u32 target_height = 0;
			
			for (u32 offset_index = 0; offset_index < 4; offset_index++) {
				
				u32 x = x_origin + four_direction_offsets[offset_index].x;
				u32 y = y_origin + four_direction_offsets[offset_index].y;
				
				Section section = tm[x][y].section;
				if (section != SECTION_VALID && section != SECTION_SACRIFICED) continue;
				
				Feature feature = tm[x][y].feature;
				if (feature != FEATURE_CLIFF && feature != FEATURE_RAMP) continue;
				
				target_found = true;
				
				u32 current_height = tm[x][y].height;
				if (current_height <= target_height) continue;
				
				target_height = current_height;
				
			}
			
			if (!target_found) continue;
			
			tm[x_origin][y_origin].feature = FEATURE_CLIFF;
			tm[x_origin][y_origin].height = target_height;
			tm[x_origin][y_origin].surface = SURFACE_ROCKS;
			
			remove_coords(sacrificed_tiles, tile_index);
			
		}
	}
	
	ret = true;
	
out:
	free_coords_array(sacrificed_tiles);
	free_coords_array(cliff_grown_tiles);
	free_coords_array(cliff_next_tiles);
	free_coords_array(cliff_current_tiles);
	free_coords_array(ramp_next_tiles);
	free_coords_array(ramp_current_tiles);
	free_coords_array(ramp_check_tiles);
	free_coords_array(ramp_origins);
	free_coords_array(plateau_core_origins);
	free_coords_array(plateau_available_tiles);
	free_coords_array(plateau_expandable_tiles);
	return ret;
}
