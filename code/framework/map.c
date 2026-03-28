#include "map.h"
#include "utils.h"
#include "mirror.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Map *init_map(void)
{
	Map *map = malloc(sizeof(Map));
	if (!map) goto err;
	
	map->tile_matrix = NULL;
	
	u32 initial_object_array_capacity = 8;
	
	map->building_array.data = NULL;
	map->pitch_ditch_array.data = NULL;
	map->wall_array.data = NULL;
	map->rock_array.data = NULL;
	map->plant_array.data = NULL;
	map->unit_array.data = NULL;
	map->animal_array.data = NULL;
	
	map->building_array.usage = 0;
	map->pitch_ditch_array.usage = 0;
	map->wall_array.usage = 0;
	map->rock_array.usage = 0;
	map->plant_array.usage = 0;
	map->unit_array.usage = 0;
	map->animal_array.usage = 0;
	
	map->building_array.capacity = initial_object_array_capacity;
	map->pitch_ditch_array.capacity = initial_object_array_capacity;
	map->wall_array.capacity = initial_object_array_capacity;
	map->rock_array.capacity = initial_object_array_capacity;
	map->plant_array.capacity = initial_object_array_capacity;
	map->unit_array.capacity = initial_object_array_capacity;
	map->animal_array.capacity = initial_object_array_capacity;
	
	map->tile_matrix = malloc((MAP_SIZE * sizeof(Tile *)) + (MAP_SIZE * MAP_SIZE * sizeof(Tile)));
	if (!map->tile_matrix) goto err;
	
	map->building_array.data = malloc(sizeof(BuildingObject) * initial_object_array_capacity);
	if (!map->building_array.data) goto err;
	map->pitch_ditch_array.data = malloc(sizeof(PitchDitchObject) * initial_object_array_capacity);
	if (!map->pitch_ditch_array.data) goto err;
	map->wall_array.data = malloc(sizeof(WallObject) * initial_object_array_capacity);
	if (!map->wall_array.data) goto err;
	map->rock_array.data = malloc(sizeof(RockObject) * initial_object_array_capacity);
	if (!map->rock_array.data) goto err;
	map->plant_array.data = malloc(sizeof(PlantObject) * initial_object_array_capacity);
	if (!map->plant_array.data) goto err;
	map->unit_array.data = malloc(sizeof(UnitObject) * initial_object_array_capacity);
	if (!map->unit_array.data) goto err;
	map->animal_array.data = malloc(sizeof(AnimalObject) * initial_object_array_capacity);
	if (!map->animal_array.data) goto err;
	
	Tile *data_start = (Tile *)(map->tile_matrix + MAP_SIZE);
	for (u32 i = 0; i < MAP_SIZE; i++) {
		map->tile_matrix[i] = data_start + (i * MAP_SIZE);
	}
	
	for (i32 x = 0; x < MAP_SIZE; x++) {
		for (i32 y = 0; y < MAP_SIZE; y++) {
			
			u32 distance_from_center = abs(199 - x) + abs(199 - y);
			
			if (distance_from_center <= 197) {
				map->tile_matrix[x][y].section = SECTION_VALID;
			} else if (x + y >= 201 && x <= y + 198 && y <= x + 198 && x + y <= 597) {
				map->tile_matrix[x][y].section = SECTION_SACRIFICED;
			} else if (x + y >= 199 && x <= y + 200 && y <= x + 200 && x + y <= 599) {
				map->tile_matrix[x][y].section = SECTION_BORDER;
			} else {
				map->tile_matrix[x][y].section = SECTION_PADDING;
			}
			
			map->tile_matrix[x][y].distance_from_center = distance_from_center;
			map->tile_matrix[x][y].feature = FEATURE_NONE;
			map->tile_matrix[x][y].height = 8;
			map->tile_matrix[x][y].surface = SURFACE_EARTH;
			map->tile_matrix[x][y].object_flags = 0;
			
		}
	}
	
	return map;
	
err:
	free_map(map);
	return NULL;
}
void free_map(Map *map)
{
	if (!map) return;
	
	free(map->tile_matrix);
	
	free(map->building_array.data);
	free(map->pitch_ditch_array.data);
	free(map->wall_array.data);
	free(map->rock_array.data);
	free(map->plant_array.data);
	free(map->unit_array.data);
	free(map->animal_array.data);
	
	free(map);
}

bool is_in_bounds(i32 x, i32 y)
{
	return x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE;
}
bool is_in_bounds_rectangle(i32 x_origin, i32 y_origin, u32 rectangle_width, u32 rectangle_length)
{
	if (!is_in_bounds(x_origin, y_origin)
	|| !is_in_bounds(x_origin + rectangle_width - 1, y_origin + rectangle_length - 1)) return false;

	return true;
}
bool is_in_bounds_array(CoordsArray *claimed_tiles)
{
	for (u32 i = 0; i < claimed_tiles->usage; i++) {
		if (!is_in_bounds(claimed_tiles->data[i].x, claimed_tiles->data[i].y)) return false;
	}
	
	return true;
}

void set_flag(u32 *flag_matrix, u32 x, u32 y, bool apply_mirror)
{
	MirrorPoints mirror_points;
	
	if (apply_mirror) {
		get_mirror_points(&mirror_points, x, y, 1);
	} else {
		mirror_points.points[0] = (Coords){x, y};
		mirror_points.count = 1;
	}
	
	for (u32 i = 0; i < mirror_points.count; i++) {
		
		u32 bit_index = mirror_points.points[i].x * MAP_SIZE + mirror_points.points[i].y;
		u32 word_index = bit_index / 32;
		u32 bit_mask = 1U << (bit_index % 32);
		
		flag_matrix[word_index] |= bit_mask;
		
	}
}
void clear_flag(u32 *flag_matrix, u32 x, u32 y, bool apply_mirror)
{
	MirrorPoints mirror_points;
	
	if (apply_mirror) {
		get_mirror_points(&mirror_points, x, y, 1);
	} else {
		mirror_points.points[0] = (Coords){x, y};
		mirror_points.count = 1;
	}
	
	for (u32 i = 0; i < mirror_points.count; i++) {
		
		u32 bit_index = mirror_points.points[i].x * MAP_SIZE + mirror_points.points[i].y;
		u32 word_index = bit_index / 32;
		u32 bit_mask = ~(1U << (bit_index % 32));
		
		flag_matrix[word_index] &= bit_mask;
	}
}
bool test_flag(u32 *flag_matrix, u32 x, u32 y)
{
	u32 bit_index = x * MAP_SIZE + y;
	u32 word_index = bit_index / 32;
	u32 bit_mask = 1U << (bit_index % 32);
	
	return (flag_matrix[word_index] & bit_mask) != 0;
}
void set_all_flags(u32 *flag_matrix)
{
	memset(flag_matrix, -1, FLAG_MATRIX_WORD_COUNT * sizeof(u32));
}
void clear_all_flags(u32 *flag_matrix)
{
	memset(flag_matrix, 0, FLAG_MATRIX_WORD_COUNT * sizeof(u32));
}

void set_feature(Map *map, u32 x, u32 y, Feature new_feature)
{
	MirrorPoints mirror_points;
	get_mirror_points(&mirror_points, x, y, 1);
	
	for (u32 i = 0; i < mirror_points.count; i++) {
		map->tile_matrix[mirror_points.points[i].x][mirror_points.points[i].y].feature = new_feature;
	}
}
void set_height(Map *map, u32 x, u32 y, u8 new_height)
{
	MirrorPoints mirror_points;
	get_mirror_points(&mirror_points, x, y, 1);
	
	for (u32 i = 0; i < mirror_points.count; i++) {
		map->tile_matrix[mirror_points.points[i].x][mirror_points.points[i].y].height = new_height;
	}
}
void set_surface(Map *map, u32 x, u32 y, Surface new_surface)
{
	MirrorPoints mirror_points;
	get_mirror_points(&mirror_points, x, y, 1);
	
	for (u32 i = 0; i < mirror_points.count; i++) {
		map->tile_matrix[mirror_points.points[i].x][mirror_points.points[i].y].surface = new_surface;
	}
}

static const u8 building_sizes[] = {
	[BUILDING_FLETCHER] = 4,
	[BUILDING_WOODCUTTER] = 3,
	[BUILDING_STOCKPILE] = 5,
	[BUILDING_HOVEL] = 4,
	[BUILDING_OX_TETHER] = 2,
	[BUILDING_QUARRY] = 6,
	[BUILDING_TUNNEL] = 3,
	[BUILDING_SIGNPOST] = 2,
	[BUILDING_MANOR_HOUSE] = 15,
	[BUILDING_KEEP] = 15,
	[BUILDING_STRONGHOLD] = 19,
	[BUILDING_STABLES] = 6,
	[BUILDING_WHEAT_FARM] = 9,
	[BUILDING_HOP_FARM] = 9,
	[BUILDING_APPLE_FARM] = 11,
	[BUILDING_DAIRY_FARM] = 10,
	[BUILDING_MILL] = 3,
	[BUILDING_BAKERY] = 4,
	[BUILDING_BREWERY] = 4,
	[BUILDING_MARKET] = 5,
	[BUILDING_HUNTER] = 3,
	[BUILDING_GRANARY] = 4,
	[BUILDING_ARMORY] = 4,
	[BUILDING_POLETURNER] = 4,
	[BUILDING_BLACKSMITH] = 4,
	[BUILDING_ARMORER] = 4,
	[BUILDING_TANNER] = 4,
	[BUILDING_MERCENARY_POST] = 10,
	[BUILDING_BARRACKS] = 10,
	[BUILDING_ENGINEER_GUILD] = 10,
	[BUILDING_TUNNELER_GUILD] = 10,
	[BUILDING_IRON_MINE] = 4,
	[BUILDING_PITCH_RIG] = 4,
	[BUILDING_INN] = 5,
	[BUILDING_APOTHECARY] = 6,
	[BUILDING_SIEGE_TOWER_STRUCTURE] = 3,
	[BUILDING_CHAPEL] = 6,
	[BUILDING_CHURCH] = 9,
	[BUILDING_CATHEDRAL] = 13,
	[BUILDING_KILLING_PIT] = 1,
	[BUILDING_PITCH_DITCH] = 1,
	[BUILDING_GATE_LARGE] = 7,
	[BUILDING_GATE_SMALL] = 5,
	[BUILDING_DRAWBRIDGE] = 5,
	[BUILDING_QUARRY_PILE] = 2,
	[BUILDING_TOWER_ONE] = 3,
	[BUILDING_TOWER_TWO] = 4,
	[BUILDING_TOWER_THREE] = 5,
	[BUILDING_TOWER_FOUR] = 6,
	[BUILDING_TOWER_FIVE] = 6,
	[BUILDING_TOWER_ONE_RUIN] = 3,
	[BUILDING_TOWER_TWO_RUIN] = 4,
	[BUILDING_TOWER_THREE_RUIN] = 5,
	[BUILDING_TOWER_FOUR_RUIN] = 6,
	[BUILDING_TOWER_FIVE_RUIN] = 6,
	[BUILDING_GARDEN_2X2_ONE] = 2,
	[BUILDING_GARDEN_2X2_TWO] = 2,
	[BUILDING_GARDEN_2X2_THREE] = 2,
	[BUILDING_GARDEN_2X2_FOUR] = 2,
	[BUILDING_GARDEN_2X2_FIVE] = 2,
	[BUILDING_GARDEN_2X2_SIX] = 2,
	[BUILDING_GARDEN_3X3_ONE] = 3,
	[BUILDING_GARDEN_3X3_TWO] = 3,
	[BUILDING_GARDEN_3X3_THREE] = 3,
	[BUILDING_GARDEN_4X4_ONE] = 4,
	[BUILDING_GARDEN_4X4_TWO] = 4,
	[BUILDING_GARDEN_4X4_THREE] = 4,
	[BUILDING_MAYPOLE] = 3,
	[BUILDING_GALLOWS] = 2,
	[BUILDING_STOCKS] = 3,
	[BUILDING_OUTPOST_EUROPEAN] = 5,
	[BUILDING_OUTPOST_ARABIAN] = 5,
	[BUILDING_OIL_SMELTER] = 8,
	[BUILDING_CATAPULT_TENT] = 3,
	[BUILDING_TREBUCHET_TENT] = 3,
	[BUILDING_BATTERING_RAM_TENT] = 3,
	[BUILDING_SIEGE_TOWER_TENT] = 3,
	[BUILDING_SHIELD_TENT] = 3,
	[BUILDING_TOWER_ONE_RUIN_SECOND] = 3,
	[BUILDING_TOWER_TWO_RUIN_SECOND] = 4,
	[BUILDING_RUIN_1X1_ONE] = 1,
	[BUILDING_RUIN_1X1_TWO] = 1,
	[BUILDING_RUIN_1X1_THREE] = 1,
	[BUILDING_RUIN_1X1_FOUR] = 1,
	[BUILDING_RUIN_2X2_ONE] = 2,
	[BUILDING_RUIN_2X2_TWO] = 2,
	[BUILDING_RUIN_2X2_THREE] = 2,
	[BUILDING_RUIN_2X2_FOUR] = 2,
	[BUILDING_RUIN_2X2_FIVE] = 2,
	[BUILDING_RUIN_2X2_SIX] = 2,
	[BUILDING_RUIN_4X4_ONE] = 4,
	[BUILDING_RUIN_4X4_TWO] = 4,
	[BUILDING_RUIN_4X4_THREE] = 4,
	[BUILDING_CESSPIT_ONE] = 5,
	[BUILDING_CESSPIT_TWO] = 5,
	[BUILDING_CESSPIT_THREE] = 5,
	[BUILDING_CESSPIT_FOUR] = 5,
	[BUILDING_BURNING_STAKE] = 3,
	[BUILDING_GIBBET] = 2,
	[BUILDING_DUNGEON] = 5,
	[BUILDING_STRETCHING_RACK] = 3,
	[BUILDING_FLOGGING_RACK] = 3,
	[BUILDING_CHOPPING_BLOCK] = 3,
	[BUILDING_DUNKING_STOOL] = 5,
	[BUILDING_DOG_CAGE] = 5,
	[BUILDING_STATUE_ONE] = 2,
	[BUILDING_STATUE_TWO] = 2,
	[BUILDING_STATUE_THREE] = 2,
	[BUILDING_STATUE_FOUR] = 2,
	[BUILDING_STATUE_FIVE] = 2,
	[BUILDING_SHRINE_ONE] = 2,
	[BUILDING_SHRINE_TWO] = 2,
	[BUILDING_SHRINE_THREE] = 2,
	[BUILDING_SHRINE_FOUR] = 2,
	[BUILDING_SHRINE_FIVE] = 1,
	[BUILDING_BEEHIVE] = 3,
	[BUILDING_DANCING_BEAR] = 5,
	[BUILDING_POND_SMALL_ONE] = 5,
	[BUILDING_POND_SMALL_TWO] = 5,
	[BUILDING_POND_LARGE_ONE] = 6,
	[BUILDING_POND_LARGE_TWO] = 6,
	[BUILDING_BEAR_CAVE] = 3,
	[BUILDING_WELL] = 3,
	[BUILDING_WATERPOT] = 4,
	[BUILDING_FIRE_BALLISTA_TENT] = 3,
	[BUILDING_FIRE_BALLISTA_TENT_SECOND] = 3,
};

bool place_building(Map *map, u32 x, u32 y, BuildingType type, u8 owner, BuildingOrientation orientation, bool mirror_owner)
{
	MirrorPoints mirror_points;
	u8 building_size = building_sizes[type];
	get_mirror_points(&mirror_points, x, y, building_size);
	
	for (u32 i = 0; i < mirror_points.count; i++) {
		
		u8 current_owner = mirror_owner ? owner + i : owner;
		
		if (map->building_array.usage == map->building_array.capacity) {
			u32 new_capacity = map->building_array.capacity * 2;
			BuildingObject *new_data = realloc(map->building_array.data, new_capacity * sizeof(BuildingObject));
			if (!new_data) return false;
			map->building_array.data = new_data;
			map->building_array.capacity = new_capacity;
		}
		map->building_array.data[map->building_array.usage++] = (BuildingObject){mirror_points.points[i], type, current_owner, orientation};
		
		u32 x_origin = mirror_points.points[i].x;
		u32 y_origin = mirror_points.points[i].y;
		
		for (u32 x_current = x_origin; x_current < x_origin + building_size; x_current++) {
			for (u32 y_current = y_origin; y_current < y_origin + building_size; y_current++) {
				map->tile_matrix[x_current][y_current].object_flags |= OF_BUILDING;
			}
		}
		
	}
	
	return true;
}
bool place_pitch_ditch(Map *map, u32 x, u32 y, u8 owner, bool mirror_owner)
{
	MirrorPoints mirror_points;
	get_mirror_points(&mirror_points, x, y, 1);
	
	for (u32 i = 0; i < mirror_points.count; i++) {
		
		u8 current_owner = mirror_owner ? owner + i : owner;
		
		if (map->pitch_ditch_array.usage == map->pitch_ditch_array.capacity) {
			u32 new_capacity = map->pitch_ditch_array.capacity * 2;
			PitchDitchObject *new_data = realloc(map->pitch_ditch_array.data, new_capacity * sizeof(PitchDitchObject));
			if (!new_data) return false;
			map->pitch_ditch_array.data = new_data;
			map->pitch_ditch_array.capacity = new_capacity;
		}
		map->pitch_ditch_array.data[map->pitch_ditch_array.usage++] = (PitchDitchObject){mirror_points.points[i], current_owner};
		
		map->tile_matrix[mirror_points.points[i].x][mirror_points.points[i].y].object_flags |= OF_PITCH_DITCH;
		
	}
	
	return true;
}
bool place_wall(Map *map, u32 x_start, u32 y_start, u32 x_end, u32 y_end, WallType type, u8 owner, bool mirror_owner)
{
	MirrorPoints mirror_points_start;
	MirrorPoints mirror_points_end;
	get_mirror_points(&mirror_points_start, x_start, y_start, 1);
	get_mirror_points(&mirror_points_end, x_end, y_end, 1);
	
	for (u32 i = 0; i < mirror_points_start.count; i++) {
		
		u32 x_base_start = mirror_points_start.points[i].x;
		u32 y_base_start = mirror_points_start.points[i].y;
		u32 x_base_end = mirror_points_end.points[i].x;
		u32 y_base_end = mirror_points_end.points[i].y;
		
		u8 current_owner = mirror_owner ? owner + i : owner;
		
		if (map->wall_array.usage == map->wall_array.capacity) {
			u32 new_capacity = map->wall_array.capacity * 2;
			WallObject *new_data = realloc(map->wall_array.data, new_capacity * sizeof(WallObject));
			if (!new_data) return false;
			map->wall_array.data = new_data;
			map->wall_array.capacity = new_capacity;
		}
		map->wall_array.data[map->wall_array.usage++] = (WallObject){(Coords){x_base_start, y_base_start}, (Coords){x_base_end, y_base_end}, type, current_owner};
		
		u32 x_current = x_base_start;
		u32 y_current = y_base_start;
		map->tile_matrix[x_base_start][y_base_start].object_flags |= OF_WALL;
		u32 extra_wall_length = abs((i32)x_base_start - (i32)x_base_end) + abs((i32)y_base_start - (i32)y_base_end);
		
		for (u32 j = 0; j < extra_wall_length; j++) {
			
			if (x_current < x_base_end) {
				x_current++;
			} else if (x_current > x_base_end) {
				x_current--;
			}
			if (y_current < y_base_end) {
				y_current++;
			} else if (y_current > y_base_end) {
				y_current--;
			}
			
			map->tile_matrix[x_current][y_current].object_flags |= OF_WALL;
			
		}
		
	}
	
	return true;
}
bool place_rock(Map *map, u32 x, u32 y, u8 size)
{
	MirrorPoints mirror_points;
	get_mirror_points(&mirror_points, x, y, size);
	
	for (u32 i = 0; i < mirror_points.count; i++) {
		
		if (map->rock_array.usage == map->rock_array.capacity) {
			u32 new_capacity = map->rock_array.capacity * 2;
			RockObject *new_data = realloc(map->rock_array.data, new_capacity * sizeof(RockObject));
			if (!new_data) return false;
			map->rock_array.data = new_data;
			map->rock_array.capacity = new_capacity;
		}
		map->rock_array.data[map->rock_array.usage++] = (RockObject){mirror_points.points[i], size};
		
		u32 x_origin = mirror_points.points[i].x;
		u32 y_origin = mirror_points.points[i].y;
		
		for (u32 x_current = x_origin; x_current < x_origin + size; x_current++) {
			for (u32 y_current = y_origin; y_current < y_origin + size; y_current++) {
				map->tile_matrix[x_current][y_current].object_flags |= OF_ROCK;
			}
		}
		
	}
	
	return true;
}
bool place_plant(Map *map, u32 x, u32 y, PlantType type, u8 variant, u8 stage)
{
	MirrorPoints mirror_points;
	get_mirror_points(&mirror_points, x, y, 1);
	
	for (u32 i = 0; i < mirror_points.count; i++) {
		
		if (map->plant_array.usage == map->plant_array.capacity) {
			u32 new_capacity = map->plant_array.capacity * 2;
			PlantObject *new_data = realloc(map->plant_array.data, new_capacity * sizeof(PlantObject));
			if (!new_data) return false;
			map->plant_array.data = new_data;
			map->plant_array.capacity = new_capacity;
		}
		map->plant_array.data[map->plant_array.usage++] = (PlantObject){mirror_points.points[i], type, variant, stage};
		
		map->tile_matrix[mirror_points.points[i].x][mirror_points.points[i].y].object_flags |= OF_PLANT;
		
	}
	
	return true;
}
bool place_unit(Map *map, u32 x, u32 y, UnitType type, u8 owner, bool mirror_owner)
{
	MirrorPoints mirror_points;
	get_mirror_points(&mirror_points, x, y, 1);
	
	for (u32 i = 0; i < mirror_points.count; i++) {
		
		u8 current_owner = mirror_owner ? owner + i : owner;
		
		if (map->unit_array.usage == map->unit_array.capacity) {
			u32 new_capacity = map->unit_array.capacity * 2;
			UnitObject *new_data = realloc(map->unit_array.data, new_capacity * sizeof(UnitObject));
			if (!new_data) return false;
			map->unit_array.data = new_data;
			map->unit_array.capacity = new_capacity;
		}
		map->unit_array.data[map->unit_array.usage++] = (UnitObject){mirror_points.points[i], type, current_owner};
		
		map->tile_matrix[mirror_points.points[i].x][mirror_points.points[i].y].object_flags |= OF_UNIT;
		
	}
	
	return true;
}
bool place_animal(Map *map, u32 x, u32 y, AnimalType type, u8 count)
{
	MirrorPoints mirror_points;
	get_mirror_points(&mirror_points, x, y, 1);
	
	for (u32 i = 0; i < mirror_points.count; i++) {
		
		if (map->animal_array.usage == map->animal_array.capacity) {
			u32 new_capacity = map->animal_array.capacity * 2;
			AnimalObject *new_data = realloc(map->animal_array.data, new_capacity * sizeof(AnimalObject));
			if (!new_data) return false;
			map->animal_array.data = new_data;
			map->animal_array.capacity = new_capacity;
		}
		map->animal_array.data[map->animal_array.usage++] = (AnimalObject){mirror_points.points[i], type, count};
		
		map->tile_matrix[mirror_points.points[i].x][mirror_points.points[i].y].object_flags |= OF_ANIMAL;
		
	}
	
	return true;
}
