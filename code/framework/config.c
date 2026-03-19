#include "config.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

static char *valid_mirror_modes[] = {"none", "x", "y", "quad", "d_x", "d_y", "d_quad", "octa", "2_point", "4_point", NULL};

typedef enum ConfigFieldType {
	CFT_INT,
	CFT_RANGE,
	CFT_INT_ARRAY,
	CFT_BOOL,
	CFT_STR,
} ConfigFieldType;

static struct {
	char *key;
	u32 offset;
	ConfigFieldType type;
	Range valid_integer_range;
	Range valid_element_count_range;
	char **valid_strings;
	bool loaded;
} field_map[] = {
	{"map_count", offsetof(Config, map_count), CFT_INT, {1, 1000}, {0, 0}, NULL, false},
	{"save_maps", offsetof(Config, save_maps), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	
	{"mirror_mode", offsetof(Config, mirror_mode), CFT_STR, {0, 0}, {0, 0}, valid_mirror_modes, false},
	{"feature_count_multiplier", offsetof(Config, feature_count_multiplier), CFT_INT, {1, 8}, {0, 0}, NULL, false},
	
	{"generate_heights", offsetof(Config, generate_heights), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"plateau_heights", offsetof(Config, plateau_heights), CFT_INT_ARRAY, {8, 255}, {1, 1000}, NULL, false},
	{"plateau_core_count_range", offsetof(Config, plateau_core_count_range), CFT_RANGE, {1, 1000}, {0, 0}, NULL, false},
	{"plateau_core_size_range", offsetof(Config, plateau_core_size_range), CFT_RANGE, {1, 400}, {0, 0}, NULL, false},
	{"plateau_core_spacing_min", offsetof(Config, plateau_core_spacing_min), CFT_INT, {0, 400}, {0, 0}, NULL, false},
	{"plateau_core_allow_mirror_overlap", offsetof(Config, plateau_core_allow_mirror_overlap), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"generate_plateau_border", offsetof(Config, generate_plateau_border), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"plateau_border_start_radius", offsetof(Config, plateau_border_start_radius), CFT_INT, {0, 197}, {0, 0}, NULL, false},
	{"generate_ramps", offsetof(Config, generate_ramps), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"ramp_count_modifier_range", offsetof(Config, ramp_count_modifier_range), CFT_RANGE, {0, 8}, {0, 0}, NULL, false},
	{"ramp_restrict_growth_towards_edge", offsetof(Config, ramp_restrict_growth_towards_edge), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"ramp_base_max_distance_from_center", offsetof(Config, ramp_base_max_distance_from_center), CFT_INT, {0, 197}, {0, 0}, NULL, false},
	{"ramp_size_range", offsetof(Config, ramp_size_range), CFT_RANGE, {1, 1000}, {0, 0}, NULL, false},
	{"ramp_step_height_base_difference", offsetof(Config, ramp_step_height_base_difference), CFT_INT, {0, 16}, {0, 0}, NULL, false},
	{"ramp_step_height_variance_range", offsetof(Config, ramp_step_height_variance_range), CFT_RANGE, {-16, 16}, {0, 0}, NULL, false},
	{"generate_cliffs", offsetof(Config, generate_cliffs), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"cliff_step_height_difference", offsetof(Config, cliff_step_height_difference), CFT_INT, {1, 255}, {0, 0}, NULL, false},
	{"cliff_growth_factor_range", offsetof(Config, cliff_growth_factor_range), CFT_RANGE, {0, 100}, {0, 0}, NULL, false},
	{"generate_cliff_rocks", offsetof(Config, generate_cliff_rocks), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"cliff_rock_count_modifier", offsetof(Config, cliff_rock_count_modifier), CFT_INT, {1, 10000}, {0, 0}, NULL, false},
	
	{"generate_border", offsetof(Config, generate_border), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"border_start_radius", offsetof(Config, border_start_radius), CFT_INT, {0, 197}, {0, 0}, NULL, false},
	{"border_size_range", offsetof(Config, border_size_range), CFT_RANGE, {1, 100000}, {0, 0}, NULL, false},
	{"border_height_variance_range", offsetof(Config, border_height_variance_range), CFT_RANGE, {-8, 8}, {0, 0}, NULL, false},
	
	{"generate_grasslands", offsetof(Config, generate_grasslands), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"grassland_core_count_range", offsetof(Config, grassland_core_count_range), CFT_RANGE, {1, 1000}, {0, 0}, NULL, false},
	{"grassland_core_size_range", offsetof(Config, grassland_core_size_range), CFT_RANGE, {1, 400}, {0, 0}, NULL, false},
	{"grassland_core_allow_mirror_overlap", offsetof(Config, grassland_core_allow_mirror_overlap), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"grassland_grass_dark_share", offsetof(Config, grassland_grass_dark_share), CFT_INT, {1, 100}, {0, 0}, NULL, false},
	{"grassland_grass_medium_share", offsetof(Config, grassland_grass_medium_share), CFT_INT, {1, 100}, {0, 0}, NULL, false},
	{"grassland_grass_light_share", offsetof(Config, grassland_grass_light_share), CFT_INT, {1, 100}, {0, 0}, NULL, false},
	{"grassland_earth_and_stones_share", offsetof(Config, grassland_earth_and_stones_share), CFT_INT, {1, 100}, {0, 0}, NULL, false},
	{"grassland_earth_share", offsetof(Config, grassland_earth_share), CFT_INT, {1, 100}, {0, 0}, NULL, false},
	
	{"generate_keeps", offsetof(Config, generate_keeps), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"generate_players", offsetof(Config, generate_players), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"player_count_range", offsetof(Config, player_count_range), CFT_RANGE, {1, 8}, {0, 0}, NULL, false},
	{"player_reserved_area_radius", offsetof(Config, player_reserved_area_radius), CFT_INT, {7, 100}, {0, 0}, NULL, false},
	{"player_min_mirror_distance", offsetof(Config, player_min_mirror_distance), CFT_INT, {15, 400}, {0, 0}, NULL, false},
	{"generate_spectators", offsetof(Config, generate_spectators), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"spectator_count_range", offsetof(Config, spectator_count_range), CFT_RANGE, {1, 8}, {0, 0}, NULL, false},
	{"spectator_lion_count", offsetof(Config, spectator_lion_count), CFT_INT, {1, 100}, {0, 0}, NULL, false},
	
	{"generate_quarry_spots", offsetof(Config, generate_quarry_spots), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"quarry_spot_count_range", offsetof(Config, quarry_spot_count_range), CFT_RANGE, {1, 10000}, {0, 0}, NULL, false},
	{"quarry_spot_size_range", offsetof(Config, quarry_spot_size_range), CFT_RANGE, {1, 10000}, {0, 0}, NULL, false},
	{"quarry_spot_stone_share", offsetof(Config, quarry_spot_stone_share), CFT_INT, {1, 100}, {0, 0}, NULL, false},
	{"quarry_spot_gravel_share", offsetof(Config, quarry_spot_gravel_share), CFT_INT, {1, 100}, {0, 0}, NULL, false},
	{"quarry_spot_grass_light_share", offsetof(Config, quarry_spot_grass_light_share), CFT_INT, {1, 100}, {0, 0}, NULL, false},
	{"quarry_spot_earth_and_stones_share", offsetof(Config, quarry_spot_earth_and_stones_share), CFT_INT, {1, 100}, {0, 0}, NULL, false},
	
	{"generate_iron_mine_spots", offsetof(Config, generate_iron_mine_spots), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"iron_mine_spot_count_range", offsetof(Config, iron_mine_spot_count_range), CFT_RANGE, {1, 10000}, {0, 0}, NULL, false},
	{"iron_mine_spot_cage_rock_chance", offsetof(Config, iron_mine_spot_cage_rock_chance), CFT_INT, {0, 100}, {0, 0}, NULL, false},
	
	{"generate_pitch_rig_spots", offsetof(Config, generate_pitch_rig_spots), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"pitch_rig_spot_count_range", offsetof(Config, pitch_rig_spot_count_range), CFT_RANGE, {1, 10000}, {0, 0}, NULL, false},
	{"pitch_rig_spot_cage_rock_chance", offsetof(Config, pitch_rig_spot_cage_rock_chance), CFT_INT, {0, 100}, {0, 0}, NULL, false},
	
	{"generate_trees", offsetof(Config, generate_trees), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"tree_blob_count_range", offsetof(Config, tree_blob_count_range), CFT_RANGE, {1, 10000}, {0, 0}, NULL, false},
	{"tree_blob_size_range", offsetof(Config, tree_blob_size_range), CFT_RANGE, {1, 10000}, {0, 0}, NULL, false},
	
	{"generate_animals", offsetof(Config, generate_animals), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"generate_deer", offsetof(Config, generate_deer), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"deer_group_count_range", offsetof(Config, deer_group_count_range), CFT_RANGE, {1, 1000}, {0, 0}, NULL, false},
	{"deer_per_group_range", offsetof(Config, deer_per_group_range), CFT_RANGE, {1, 127}, {0, 0}, NULL, false},
	{"deer_reserved_area_radius", offsetof(Config, deer_reserved_area_radius), CFT_INT, {0, 10}, {0, 0}, NULL, false},
	{"generate_lions", offsetof(Config, generate_lions), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"lion_group_count_range", offsetof(Config, lion_group_count_range), CFT_RANGE, {1, 1000}, {0, 0}, NULL, false},
	{"lion_per_group_range", offsetof(Config, lion_per_group_range), CFT_RANGE, {1, 127}, {0, 0}, NULL, false},
	{"lion_reserved_area_radius", offsetof(Config, lion_reserved_area_radius), CFT_INT, {0, 10}, {0, 0}, NULL, false},
	{"generate_rabbits", offsetof(Config, generate_rabbits), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"rabbit_group_count_range", offsetof(Config, rabbit_group_count_range), CFT_RANGE, {1, 1000}, {0, 0}, NULL, false},
	{"rabbit_per_group_range", offsetof(Config, rabbit_per_group_range), CFT_RANGE, {1, 127}, {0, 0}, NULL, false},
	{"rabbit_reserved_area_radius", offsetof(Config, rabbit_reserved_area_radius), CFT_INT, {0, 10}, {0, 0}, NULL, false},
	{"generate_camels", offsetof(Config, generate_camels), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"camel_group_count_range", offsetof(Config, camel_group_count_range), CFT_RANGE, {1, 1000}, {0, 0}, NULL, false},
	{"camel_per_group_range", offsetof(Config, camel_per_group_range), CFT_RANGE, {1, 127}, {0, 0}, NULL, false},
	{"camel_reserved_area_radius", offsetof(Config, camel_reserved_area_radius), CFT_INT, {0, 10}, {0, 0}, NULL, false},
	
	{"generate_shrubs", offsetof(Config, generate_shrubs), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"shrub_count_modifier", offsetof(Config, shrub_count_modifier), CFT_INT, {1, 10000}, {0, 0}, NULL, false},
	
	{"generate_texturing", offsetof(Config, generate_texturing), CFT_BOOL, {0, 0}, {0, 0}, NULL, false},
	{"texturing_beach_blob_count_range", offsetof(Config, texturing_beach_blob_count_range), CFT_RANGE, {0, 100000}, {0, 0}, NULL, false},
	{"texturing_beach_blob_size_range", offsetof(Config, texturing_beach_blob_size_range), CFT_RANGE, {1, 10000}, {0, 0}, NULL, false},
	{"texturing_dunes_blob_count_range", offsetof(Config, texturing_dunes_blob_count_range), CFT_RANGE, {0, 100000}, {0, 0}, NULL, false},
	{"texturing_dunes_blob_size_range", offsetof(Config, texturing_dunes_blob_size_range), CFT_RANGE, {1, 10000}, {0, 0}, NULL, false},
	{"texturing_earth_and_stones_blob_count_range", offsetof(Config, texturing_earth_and_stones_blob_count_range), CFT_RANGE, {0, 100000}, {0, 0}, NULL, false},
	{"texturing_earth_and_stones_blob_size_range", offsetof(Config, texturing_earth_and_stones_blob_size_range), CFT_RANGE, {1, 10000}, {0, 0}, NULL, false},
	{"texturing_grass_light_blob_count_range", offsetof(Config, texturing_grass_light_blob_count_range), CFT_RANGE, {0, 100000}, {0, 0}, NULL, false},
	{"texturing_grass_light_blob_size_range", offsetof(Config, texturing_grass_light_blob_size_range), CFT_RANGE, {1, 10000}, {0, 0}, NULL, false},
	{"texturing_grass_medium_blob_count_range", offsetof(Config, texturing_grass_medium_blob_count_range), CFT_RANGE, {0, 100000}, {0, 0}, NULL, false},
	{"texturing_grass_medium_blob_size_range", offsetof(Config, texturing_grass_medium_blob_size_range), CFT_RANGE, {1, 10000}, {0, 0}, NULL, false},
};
static u32 field_count = sizeof(field_map) / sizeof(field_map[0]);

static void trim_spaces(char *str)
{
	char *start = str;
	while (isspace((unsigned char)*start)) start++;
	if (*start == '\0') {
		*str = '\0';
		return;
	}
	
	char *end = start + strlen(start) - 1;
	while (end > start && isspace((unsigned char)*end)) end--;
	*(end + 1) = '\0';
	
	if (start != str) {
		memmove(str, start, end - start + 2);
	}
}

Config *init_config(void)
{
	Config *config = NULL;
	FILE *file = NULL;
	
	config = calloc(1, sizeof(Config));
	if (!config) goto err;
	
	file = fopen("config.txt", "r");
	if (!file) {
		printf("Config: failed to open \"config.txt\"\n");
		goto err;
	}
	
	bool config_is_valid = true;
	
	for (u32 field_index = 0; field_index < field_count; field_index++) {
		field_map[field_index].loaded = false;
	}
	
	char line_str[256];
	u32 line_num = 0;
	
	while (fgets(line_str, sizeof(line_str), file)) {
		
		line_num++;
		
		line_str[strcspn(line_str, "\n")] = '\0';
		
		char *hash_check = strchr(line_str, '#');
		if (hash_check) *hash_check = '\0';
		
		char *empty_check = line_str;
		while (*empty_check && isspace((unsigned char)*empty_check)) empty_check++;
		if (*empty_check == '\0') continue;
		
		char key_str[128], value_str[128];
		
		if (sscanf(line_str, "%127[^=]=%127[^\n]", key_str, value_str) != 2) {
			printf("Config: line %d: invalid entry\n", line_num);
			config_is_valid = false;
			continue;
		}
		
		trim_spaces(key_str);
		trim_spaces(value_str);
		
		bool entry_found = false;
		
		for (u32 field_index = 0; field_index < field_count; field_index++) {
			
			if (strcmp(field_map[field_index].key, key_str) != 0) continue;
			
			entry_found = true;
			
			if (field_map[field_index].loaded == true) {
				printf("Config: line %d: duplicate entry\n", line_num);
				config_is_valid = false;
				goto next_line;
			}
			field_map[field_index].loaded = true;
			
			void *target = (u8 *)config + field_map[field_index].offset;
			
			switch (field_map[field_index].type) {
				case CFT_INT: {
					char *end_ptr;
					errno = 0;
					
					i32 num = (i32)strtol(value_str, &end_ptr, 10);
					
					if (end_ptr == value_str || *end_ptr != '\0') {
						printf("Config: line %d: invalid integer value\n", line_num);
						config_is_valid = false;
						goto next_line;
					}
					if (errno == ERANGE) {
						printf("Config: line %d: integer value out of integer range\n", line_num);
						config_is_valid = false;
						goto next_line;
					}
					
					i32 min = field_map[field_index].valid_integer_range.min;
					i32 max = field_map[field_index].valid_integer_range.max;
					
					if (num < min || num > max) {
						printf("Config: line %d: integer value out of allowed range (min: %d, max: %d)\n", line_num, min, max);
						config_is_valid = false;
						goto next_line;
					}
					
					*(i32 *)target = num;
					
					break;
				}
				case CFT_RANGE: {
					char min_str[64], max_str[64];
					
					if (sscanf(value_str, "%63[^,],%63s", min_str, max_str) != 2) {
						printf("Config: line %d: invalid range value\n", line_num);
						config_is_valid = false;
						goto next_line;
					}
					
					trim_spaces(min_str);
					trim_spaces(max_str);
					
					char *min_end_ptr;
					char *max_end_ptr;
					
					errno = 0;
					i32 min_num = (i32)strtol(min_str, &min_end_ptr, 10);
					i32 min_err = errno;
					
					errno = 0;
					i32 max_num = (i32)strtol(max_str, &max_end_ptr, 10);
					i32 max_err = errno;
					
					if (min_end_ptr == min_str || *min_end_ptr != '\0' || max_end_ptr == max_str || *max_end_ptr != '\0') {
						printf("Config: line %d: invalid range value\n", line_num);
						config_is_valid = false;
						goto next_line;
					}
					if (min_err == ERANGE || max_err == ERANGE) {
						printf("Config: line %d: range value out of integer range\n", line_num);
						config_is_valid = false;
						goto next_line;
					}
					
					i32 min_val = field_map[field_index].valid_integer_range.min;
					i32 max_val = field_map[field_index].valid_integer_range.max;
					
					if (min_num < min_val || max_num > max_val) {
						printf("Config: line %d: range value out of allowed range (min: %d, max: %d)\n", line_num, min_val, max_val);
						config_is_valid = false;
						goto next_line;
					}
					if (min_num > max_num) {
						printf("Config: line %d: range value with min > max\n", line_num);
						config_is_valid = false;
						goto next_line;
					}
					
					*(Range *)target = (Range){min_num, max_num};
					
					break;
				}
				case CFT_INT_ARRAY: {
					IntegerArray *integer_array = init_integer_array();
					if (!integer_array) goto err;
					
					i32 value_min = field_map[field_index].valid_integer_range.min;
					i32 value_max = field_map[field_index].valid_integer_range.max;
					
					u32 element_count = 0;
					u32 element_count_min = field_map[field_index].valid_element_count_range.min;
					u32 element_count_max = field_map[field_index].valid_element_count_range.max;
					
					bool found_invalid = false;
					bool found_out_of_int_range = false;
					bool found_out_of_allowed_range = false;
					
					char *next_token = strtok(value_str, ",");
					
					while (next_token != NULL) {
						
						element_count++;
						
						char *current_token = next_token;
						char *end_ptr;
						errno = 0;
						
						i32 num = (i32)strtol(current_token, &end_ptr, 10);
						
						next_token = strtok(NULL, ",");
						
						if (current_token == end_ptr || *end_ptr != '\0') {
							found_invalid = true;
							continue;
						} else if (errno == ERANGE) {
							found_out_of_int_range = true;
							continue;
						} else if (num < value_min || num > value_max) {
							found_out_of_allowed_range = true;
							continue;
						}
						
						add_integer(integer_array, num);
						
					}
					
					if (found_invalid) {
						printf("Config: line %d: invalid integer array value\n", line_num);
						config_is_valid = false;
					}
					if (found_out_of_int_range) {
						printf("Config: line %d: integer array value out of integer range\n", line_num);
						config_is_valid = false;
					}
					if (found_out_of_allowed_range) {
						printf("Config: line %d: integer array value out of allowed range (min: %d, max: %d)\n", line_num, value_min, value_max);
						config_is_valid = false;
					}
					if (element_count < element_count_min || element_count > element_count_max) {
						printf("Config: line %d: integer array element count out of allowed range at (min: %d, max: %d)\n", line_num, element_count_min, element_count_max);
						config_is_valid = false;
					}
					
					*(IntegerArray **)target = integer_array;
					
					break;
				}
				case CFT_BOOL: {
					if (strcmp(value_str, "true") == 0) {
						*(bool *)target = true;
					} else if (strcmp(value_str, "false") == 0) {
						*(bool *)target = false;
					} else {
						printf("Config: line %d: invalid boolean value (options: true, false)\n", line_num);
						config_is_valid = false;
						goto next_line;
					}
					
					break;
				}
				case CFT_STR: {
					bool validated = false;
					char **valid_strings = field_map[field_index].valid_strings;
					
					for (u32 i = 0; valid_strings[i]; i++) {
						if (strcmp(value_str, valid_strings[i]) == 0) {
							*(char **)target = valid_strings[i];
							validated = true;
							break;
						}
					}
					
					if (!validated) {
						
						DynamicString *string_options = init_dynamic_string();
						if (!string_options) goto err;
						
						for (u32 i = 0; valid_strings[i]; i++) {
							add_to_string(string_options, valid_strings[i]);
							if (!valid_strings[i + 1]) break;
							add_to_string(string_options, ", ");
						}
						
						printf("Config: line %d: invalid string value (options: %s)\n", line_num, string_options->data);
						
						free_dynamic_string(string_options);
						
						config_is_valid = false;
						goto next_line;
						
					}
					
					break;
				}
			}
			
			break;
			
		}
		
		if (!entry_found) {
			printf("Config: line %d: unknown entry\n", line_num);
			config_is_valid = false;
		}
		
		next_line:
		
	}
	
	fclose(file);
	
	for (u32 field_index = 0; field_index < field_count; field_index++) {
		if (!field_map[field_index].loaded) {
			printf("Config: %s entry missing\n", field_map[field_index].key);
			config_is_valid = false;
		}
	}
	
	if (!config_is_valid) goto err;
	
	return config;
	
err:
	if (file) fclose(file);
	free_config(config);
	return NULL;
}

void free_config(Config *config)
{
	if (!config) return;
	
	for (u32 field_index = 0; field_index < field_count; field_index++) {
		if (field_map[field_index].type == CFT_INT_ARRAY) {
			IntegerArray **integer_array = (IntegerArray **)((u8 *)config + field_map[field_index].offset);
			free_integer_array(*integer_array);
		}
	}
	
	free(config);
}
