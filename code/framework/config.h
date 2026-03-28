#pragma once
#include "utils.h"

typedef struct Config {
	i32 map_count;
	bool save_maps;
	
	char *mirror_mode;
	i32 mirror_point_count;
	i32 feature_count_multiplier;
	
	bool generate_heights;
	IntegerArray *plateau_heights;
	Range plateau_core_count_range;
	Range plateau_core_size_range;
	i32 plateau_core_spacing_min;
	bool plateau_core_allow_mirror_overlap;
	bool generate_ramps;
	Range ramp_count_modifier_range;
	i32 ramp_base_max_distance_from_center;
	Range ramp_size_range;
	i32 ramp_step_height_base_difference;
	Range ramp_step_height_variance_range;
	bool generate_cliffs;
	i32 cliff_step_height_difference;
	Range cliff_growth_factor_range;
	bool generate_cliff_rocks;
	i32 cliff_rock_count_modifier;
	
	bool generate_surfaces;
	Range surface_core_count_range;
	Range surface_core_size_range;
	bool surface_core_allow_mirror_overlap;
	i32 surface_river_height_offset;
	i32 surface_river_ford_transform_chance;
	i32 surface_river_share;
	i32 surface_grass_dark_share;
	i32 surface_grass_medium_share;
	i32 surface_grass_light_share;
	i32 surface_earth_and_stones_share;
	i32 surface_earth_share;
	
	bool generate_keeps;
	bool generate_players;
	Range player_count_range;
	i32 player_reserved_area_radius;
	i32 player_min_mirror_distance;
	bool generate_spectators;
	Range spectator_count_range;
	i32 spectator_lion_count;
	
	bool generate_ruins;
	Range ruin_blob_count_range;
	Range ruin_blob_size_range;
	i32 ruin_count_per_blob_modifier;
	
	bool generate_quarry_spots;
	Range quarry_spot_count_range;
	Range quarry_spot_size_range;
	i32 quarry_spot_stone_share;
	i32 quarry_spot_gravel_share;
	i32 quarry_spot_grass_light_share;
	i32 quarry_spot_earth_and_stones_share;
	
	bool generate_iron_mine_spots;
	Range iron_mine_spot_count_range;
	i32 iron_mine_spot_cage_rock_chance;
	
	bool generate_pitch_rig_spots;
	Range pitch_rig_spot_count_range;
	i32 pitch_rig_spot_cage_rock_chance;
	
	bool generate_trees;
	Range tree_blob_count_range;
	Range tree_blob_size_range;
	
	bool generate_animals;
	bool generate_deer;
	Range deer_group_count_range;
	Range deer_per_group_range;
	i32 deer_reserved_area_radius;
	bool generate_lions;
	Range lion_group_count_range;
	Range lion_per_group_range;
	i32 lion_reserved_area_radius;
	bool generate_rabbits;
	Range rabbit_group_count_range;
	Range rabbit_per_group_range;
	i32 rabbit_reserved_area_radius;
	bool generate_camels;
	Range camel_group_count_range;
	Range camel_per_group_range;
	i32 camel_reserved_area_radius;
	
	bool generate_shrubs;
	i32 shrub_count_modifier;
	
	bool generate_texturing;
	Range texturing_beach_blob_count_range;
	Range texturing_beach_blob_size_range;
	Range texturing_dunes_blob_count_range;
	Range texturing_dunes_blob_size_range;
	Range texturing_earth_and_stones_blob_count_range;
	Range texturing_earth_and_stones_blob_size_range;
	Range texturing_grass_light_blob_count_range;
	Range texturing_grass_light_blob_size_range;
	Range texturing_grass_medium_blob_count_range;
	Range texturing_grass_medium_blob_size_range;
} Config;

Config *init_config(void);
void free_config(Config *config);
