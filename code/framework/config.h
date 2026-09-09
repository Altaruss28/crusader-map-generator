#pragma once

#include "common.h"

typedef struct {
	int map_count;
	bool save_maps;

	char *mirror_mode;
	int feature_count_multiplier;

	bool generate_heights;
	IntegerArray *plateau_heights;
	Range plateau_core_count_range;
	Range plateau_core_size_range;
	int plateau_core_spacing_min;
	bool plateau_core_allow_mirror_overlap;
	bool generate_ramps;
	Range ramp_count_modifier_range;
	int ramp_base_max_distance_from_center;
	Range ramp_size_range;
	int ramp_step_height_base_difference;
	Range ramp_step_height_variance_range;
	bool generate_cliffs;
	int cliff_step_height_difference;
	Range cliff_growth_factor_range;
	bool generate_cliff_rocks;
	int cliff_rock_count_modifier;

	bool generate_surfaces;
	Range surface_core_count_range;
	Range surface_core_size_range;
	bool surface_core_allow_mirror_overlap;
	int surface_river_height_offset;
	int surface_river_ford_transform_chance;
	int surface_river_share;
	int surface_grass_dark_share;
	int surface_grass_medium_share;
	int surface_grass_light_share;
	int surface_earth_and_stones_share;
	int surface_earth_share;

	bool generate_keeps;
	bool generate_players;
	Range player_count_range;
	int player_reserved_area_radius;
	int player_min_mirror_distance;
	bool generate_spectators;
	Range spectator_count_range;
	int spectator_lion_count;

	bool generate_ruins;
	Range ruin_blob_count_range;
	Range ruin_blob_step_count_range;
	Range ruin_blob_grow_count_range;
	int ruin_count_per_blob_modifier;

	bool generate_quarry_spots;
	Range quarry_spot_count_range;
	Range quarry_spot_size_range;
	int quarry_spot_stone_share;
	int quarry_spot_gravel_share;
	int quarry_spot_grass_light_share;
	int quarry_spot_earth_and_stones_share;

	bool generate_iron_mine_spots;
	Range iron_mine_spot_count_range;
	int iron_mine_spot_cage_rock_chance;

	bool generate_pitch_rig_spots;
	Range pitch_rig_spot_count_range;
	int pitch_rig_spot_cage_rock_chance;

	bool generate_trees;
	Range tree_blob_count_range;
	Range tree_blob_step_count_range;
	Range tree_blob_grow_count_range;

	bool generate_animals;
	bool generate_deer;
	Range deer_group_count_range;
	Range deer_per_group_range;
	int deer_reserved_area_radius;
	bool generate_lions;
	Range lion_group_count_range;
	Range lion_per_group_range;
	int lion_reserved_area_radius;
	bool generate_rabbits;
	Range rabbit_group_count_range;
	Range rabbit_per_group_range;
	int rabbit_reserved_area_radius;
	bool generate_camels;
	Range camel_group_count_range;
	Range camel_per_group_range;
	int camel_reserved_area_radius;

	bool generate_shrubs;
	int shrub_count_modifier;

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

Config *config_init(void);
void config_free(Config *config);
