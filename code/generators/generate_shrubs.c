#include "utils.h"
#include "map.h"
#include "mirror.h"
#include "config.h"

typedef struct ShrubOption {
	PlantType type;
	Range variant_range;
} ShrubOption;

static const ShrubOption dry_shrub_options[] = {
	{PLANT_SHRUB_OLD, {0, 1}},
	{PLANT_CACTUS_NORMAL, {10, 17}},
	{PLANT_CACTUS_BLOBBY, {1, 3}},
	{PLANT_CACTUS_WIGGLY, {7, 9}},
};
static const ShrubOption wet_shrub_options[] = {
	{PLANT_SHRUB_OLD, {0, 2}},
	{PLANT_SHRUB_OLD, {7, 9}},
	{PLANT_SHRUB_THICK, {0, 9}},
	{PLANT_SHRUB_AGAVE, {4, 6}},
};

bool generate_shrubs(Map *map, Config *config, u32 *rng_state, DynamicString *logs)
{
	(void)logs;
	
	bool ret = false;
	
	CoordsArray *available_tiles = NULL;
	
	Tile **tm = map->tile_matrix;
	
	u32 feature_count_multiplier = config->feature_count_multiplier;
	u32 shrub_count_modifier = config->shrub_count_modifier;
	
	if (!(available_tiles = init_coords_array())) goto out;
	
	for (u32 x = 0; x < MAP_SIZE; x++) {
		for (u32 y = 0; y < MAP_SIZE; y++) {
			
			if (tm[x][y].section != SECTION_VALID
			|| (tm[x][y].feature != FEATURE_NONE
			&& tm[x][y].feature != FEATURE_GRASSLAND)) continue;
			
			if (!add_coords(available_tiles, x, y)) goto out;
			
		}
	}
	
	u32 shrub_count = (available_tiles->usage / shrub_count_modifier) * feature_count_multiplier;
	u32 shrubs_placed = 0;
	
	while (shrubs_placed < shrub_count && available_tiles->usage > 0) {
		
		u32 chosen_tile_index = random(rng_state, 0, available_tiles->usage - 1);
		u32 x = available_tiles->data[chosen_tile_index].x;
		u32 y = available_tiles->data[chosen_tile_index].y;
		remove_coords(available_tiles, chosen_tile_index);
		
		if (tm[x][y].feature == FEATURE_SHRUB
		|| has_mirror_overlap_rectangle(x, y, 1, 1)) continue;
		
		const ShrubOption *shrub_options;
		u32 option_index;
		
		Surface surface = tm[x][y].surface;
		
		if (surface == SURFACE_GRASS_DARK || surface == SURFACE_GRASS_MEDIUM || surface == SURFACE_GRASS_LIGHT) {
			shrub_options = wet_shrub_options;
			option_index = random(rng_state, 0, (sizeof(wet_shrub_options) / sizeof(wet_shrub_options[0])) - 1);
		} else {
			shrub_options = dry_shrub_options;
			option_index = random(rng_state, 0, (sizeof(dry_shrub_options) / sizeof(dry_shrub_options[0])) - 1);
		}
		
		PlantType type = shrub_options[option_index].type;
		
		Range variant_range = shrub_options[option_index].variant_range;
		u8 variant = random(rng_state, variant_range.min, variant_range.max);
		
		if (!place_plant(map, x, y, type, variant, 0)) goto out;
		set_feature(map, x, y, FEATURE_SHRUB);
		
		shrubs_placed++;
		
	}
	
	ret = true;
	
out:
	free_coords_array(available_tiles);
	return ret;
}
