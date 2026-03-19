#include "utils.h"
#include "map.h"
#include "mirror.h"
#include "config.h"
#include "helpers.h"

typedef struct VarTileSpec {
	Range x_offset_range;
	Range y_offset_range;
	Surface surface;
} VarTileSpec;

static const VarTileSpec iron_mine_spot_composition[] = {
	{{3, 3}, {2, 2}, SURFACE_IRON},
	{{3, 3}, {3, 3}, SURFACE_IRON},
	{{2, 2}, {3, 3}, SURFACE_IRON},
	{{2, 2}, {2, 2}, SURFACE_IRON},
	
	{{3, 4}, {1, 1}, SURFACE_IRON},
	{{4, 4}, {1, 2}, SURFACE_IRON},
	{{4, 4}, {3, 4}, SURFACE_IRON},
	{{3, 4}, {4, 4}, SURFACE_IRON},
	{{1, 2}, {4, 4}, SURFACE_IRON},
	{{1, 1}, {3, 4}, SURFACE_IRON},
	{{1, 1}, {1, 2}, SURFACE_IRON},
	{{1, 2}, {1, 1}, SURFACE_IRON},
	
	{{3, 4}, {0, 0}, SURFACE_ROCKS},
	{{5, 5}, {1, 2}, SURFACE_ROCKS},
	{{5, 5}, {3, 4}, SURFACE_ROCKS},
	{{3, 4}, {5, 5}, SURFACE_ROCKS},
	{{1, 2}, {5, 5}, SURFACE_ROCKS},
	{{0, 0}, {3, 4}, SURFACE_ROCKS},
	{{0, 0}, {1, 2}, SURFACE_ROCKS},
	{{1, 2}, {0, 0}, SURFACE_ROCKS},
};

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
		
		for (u32 tile_index = 0; tile_index < sizeof(iron_mine_spot_composition) / sizeof(iron_mine_spot_composition[0]); tile_index++) {
			
			VarTileSpec tile_spec = iron_mine_spot_composition[tile_index];
			
			u32 x = x_origin + random(rng_state, tile_spec.x_offset_range.min, tile_spec.x_offset_range.max);
			u32 y = y_origin + random(rng_state, tile_spec.y_offset_range.min, tile_spec.y_offset_range.max);
			
			set_surface(map, x, y, tile_spec.surface);
			
			if (tile_spec.surface != SURFACE_ROCKS
			|| cage_rock_chance < (u32)random(rng_state, 1, 100)
			|| has_mirror_overlap_rectangle(x, y, 1, 1)) continue;
			
			if (!place_rock(map, x, y, 1)) goto out;
			
		}
		
		for (u32 x = x_origin; x < x_origin + 6; x++) {
			for (u32 y = y_origin; y < y_origin + 6; y++) {
				set_feature(map, x, y, FEATURE_IRON_MINE_SPOT);
			}
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
