#include "utils.h"
#include "map.h"
#include "mirror.h"
#include "config.h"

bool generate_animals(Map *map, Config *config, u32 *rng_state, DynamicString *logs)
{
	bool ret = false;
	
	CoordsArray *available_origins = NULL;
	
	Tile **tm = map->tile_matrix;
	
	u32 feature_count_multiplier = config->feature_count_multiplier;
	
	struct {
		char *name_str;
		AnimalType type;
		bool generate_animal;
		Range group_count_range;
		Range per_group_range;
		u32 reserved_area_radius;
	} animal_settings[] = {
		{"deer", ANIMAL_DEER, config->generate_deer, config->deer_group_count_range, config->deer_per_group_range, config->deer_reserved_area_radius},
		{"lion", ANIMAL_LION, config->generate_lions, config->lion_group_count_range, config->lion_per_group_range, config->lion_reserved_area_radius},
		{"rabbit", ANIMAL_RABBIT, config->generate_rabbits, config->rabbit_group_count_range, config->rabbit_per_group_range, config->rabbit_reserved_area_radius},
		{"camel", ANIMAL_CAMEL, config->generate_camels, config->camel_group_count_range, config->camel_per_group_range, config->camel_reserved_area_radius},
	};
	
	if (!(available_origins = init_coords_array())) goto out;
	
	for (u32 x = 0; x < MAP_SIZE; x++) {
		for (u32 y = 0; y < MAP_SIZE; y++) {
			
			if (tm[x][y].section != SECTION_VALID
			|| (tm[x][y].feature != FEATURE_NONE
			&& tm[x][y].feature != FEATURE_GRASSLAND)) continue;
			
			if (!add_coords(available_origins, x, y)) goto out;
			
		}
	}
	
	shuffle_coords_array(available_origins, rng_state);
	
	for (u32 setting_index = 0; setting_index < sizeof(animal_settings) / sizeof(animal_settings[0]); setting_index++) {
		
		if (!animal_settings[setting_index].generate_animal) continue;
		
		AnimalType type = animal_settings[setting_index].type;
		Range group_count_range = animal_settings[setting_index].group_count_range;
		Range per_group_range = animal_settings[setting_index].per_group_range;
		
		u32 reserved_area_radius = animal_settings[setting_index].reserved_area_radius;
		u32 reserved_area_size = 1 + (reserved_area_radius * 2);
		
		u32 group_count = random(rng_state, group_count_range.min, group_count_range.max) * feature_count_multiplier;
		u32 group_index = 0;
		
		for (u32 tile_index = 0; tile_index < available_origins->usage; tile_index++) {
			
			u32 x_origin = available_origins->data[tile_index].x;
			u32 y_origin = available_origins->data[tile_index].y;
			
			u32 x_base = x_origin - reserved_area_radius;
			u32 y_base = y_origin - reserved_area_radius;
			
			for (u32 x = x_base; x < x_base + reserved_area_size; x++) {
				for (u32 y = y_base; y < y_base + reserved_area_size; y++) {
					
					if (!is_in_bounds(x, y)
					|| tm[x][y].section != SECTION_VALID
					|| (tm[x][y].feature != FEATURE_NONE
					&& tm[x][y].feature != FEATURE_GRASSLAND)) {
						goto next_tile;
					}
					
				}
			}
			
			if (has_mirror_overlap_rectangle(x_base, y_base, reserved_area_size, reserved_area_size)) continue;
			
			for (u32 x = x_base; x < x_base + reserved_area_size; x++) {
				for (u32 y = y_base; y < y_base + reserved_area_size; y++) {
					set_feature(map, x, y, FEATURE_ANIMAL);
				}
			}
			
			if (!place_animal(map, x_origin, y_origin, type, random(rng_state, per_group_range.min, per_group_range.max))) goto out;
			
			if (++group_index >= group_count) break;
			
			next_tile:
			
		}
		
		if (group_index < group_count) {
			if (!add_to_string(logs, "generate_animals: %d/%d %s groups placed\n", group_index, group_count, animal_settings[setting_index].name_str)) goto out;
		}
		
	}
	
	ret = true;
	
out:
	free_coords_array(available_origins);
	return ret;
}
