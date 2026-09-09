#include "common.h"
#include "config.h"
#include "map.h"
#include "mirror.h"

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

bool generate_shrubs(Map *map, const Config *config, rng_state_t *rng_state, DynamicString *logs)
{
	(void)logs;

	bool ret = false;

	CoordsArray *available_tiles = nullptr;

	typeof(&map->tile_matrix[0]) tm = map->tile_matrix;

	int feature_count_multiplier = config->feature_count_multiplier;
	int shrub_count_modifier = config->shrub_count_modifier;

	if (!(available_tiles = coords_array_init()))
		goto err;

	for (int x = 0; x < MAP_SIZE; x++) {
		for (int y = 0; y < MAP_SIZE; y++) {
			if (tm[x][y].section != SECTION_VALID
			|| (tm[x][y].feature != FEATURE_NONE
			&& tm[x][y].feature != FEATURE_GRASSLAND))
				continue;

			if (!coords_array_add(available_tiles, x, y))
				goto err;
		}
	}

	int shrub_count = (available_tiles->usage / shrub_count_modifier) * feature_count_multiplier;
	int shrubs_placed = 0;

	while (shrubs_placed < shrub_count && available_tiles->usage > 0) {
		int chosen_tile_index = random(rng_state, 0, available_tiles->usage - 1);
		int x = available_tiles->data[chosen_tile_index].x;
		int y = available_tiles->data[chosen_tile_index].y;
		coords_array_remove(available_tiles, chosen_tile_index);

		if (tm[x][y].feature == FEATURE_SHRUB
		|| mirror_check_overlap_rectangle(x, y, 1, 1))
			continue;

		const ShrubOption *shrub_options;
		int option_index;

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

		if (!map_place_plant(map, x, y, type, variant, 0))
			goto err;
		map_set_feature(map, x, y, FEATURE_SHRUB);

		shrubs_placed++;
	}

	ret = true;

err:
	coords_array_free(available_tiles);
	return ret;
}
