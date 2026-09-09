#include "map.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "mirror.h"

Map *map_init(void)
{
	Map *map = malloc(sizeof(*map));
	if (!map)
		goto err;

	int initial_capacity = 8;

	map->building_array.data = nullptr;
	map->pitch_ditch_array.data = nullptr;
	map->wall_array.data = nullptr;
	map->rock_array.data = nullptr;
	map->plant_array.data = nullptr;
	map->unit_array.data = nullptr;
	map->animal_group_array.data = nullptr;

	map->building_array.usage = 0;
	map->pitch_ditch_array.usage = 0;
	map->wall_array.usage = 0;
	map->rock_array.usage = 0;
	map->plant_array.usage = 0;
	map->unit_array.usage = 0;
	map->animal_group_array.usage = 0;

	map->building_array.capacity = initial_capacity;
	map->pitch_ditch_array.capacity = initial_capacity;
	map->wall_array.capacity = initial_capacity;
	map->rock_array.capacity = initial_capacity;
	map->plant_array.capacity = initial_capacity;
	map->unit_array.capacity = initial_capacity;
	map->animal_group_array.capacity = initial_capacity;

	if (!(map->building_array.data = malloc(map->building_array.capacity * sizeof(*map->building_array.data)))
		|| !(map->pitch_ditch_array.data = malloc(map->pitch_ditch_array.capacity * sizeof(*map->pitch_ditch_array.data)))
		|| !(map->wall_array.data = malloc(map->wall_array.capacity * sizeof(*map->wall_array.data)))
		|| !(map->rock_array.data = malloc(map->rock_array.capacity * sizeof(*map->rock_array.data)))
		|| !(map->plant_array.data = malloc(map->plant_array.capacity * sizeof(*map->plant_array.data)))
		|| !(map->unit_array.data = malloc(map->unit_array.capacity * sizeof(*map->unit_array.data)))
		|| !(map->animal_group_array.data = malloc(map->animal_group_array.capacity * sizeof(*map->animal_group_array.data))))
		goto err;

	static_assert(MAP_SIZE == 400);
	for (int x = 0; x < MAP_SIZE; ++x)
		for (int y = 0; y < MAP_SIZE; ++y) {
			int distance_from_center = abs(199 - x) + abs(199 - y);

			if (distance_from_center <= 197)
				map->tile_matrix[x][y].section = SECTION_VALID;
			else if (x + y >= 201 && x <= y + 198 && y <= x + 198 && x + y <= 597)
				map->tile_matrix[x][y].section = SECTION_SACRIFICED;
			else if (x + y >= 199 && x <= y + 200 && y <= x + 200 && x + y <= 599)
				map->tile_matrix[x][y].section = SECTION_BORDER;
			else
				map->tile_matrix[x][y].section = SECTION_PADDING;

			map->tile_matrix[x][y].distance_from_center = distance_from_center;
			map->tile_matrix[x][y].feature = FEATURE_NONE;
			map->tile_matrix[x][y].height = 8;
			map->tile_matrix[x][y].surface = SURFACE_EARTH;
			map->tile_matrix[x][y].object_flags = 0;
		}

	return map;

err:
	map_free(map);
	return nullptr;
}
void map_free(Map *map)
{
	if (!map)
		return;

	free(map->building_array.data);
	free(map->pitch_ditch_array.data);
	free(map->wall_array.data);
	free(map->rock_array.data);
	free(map->plant_array.data);
	free(map->unit_array.data);
	free(map->animal_group_array.data);

	free(map);
}

bool map_check_bounds_point(int x, int y)
{
	return x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE;
}
bool map_check_bounds_rectangle(int x_origin, int y_origin, int rectangle_width, int rectangle_length)
{
	if (!map_check_bounds_point(x_origin, y_origin)
		|| !map_check_bounds_point(x_origin + rectangle_width - 1, y_origin + rectangle_length - 1))
		return false;

	return true;
}
bool map_check_bounds_array(const CoordsArray *claimed_tiles)
{
	for (usize i = 0; i < claimed_tiles->usage; ++i)
		if (!map_check_bounds_point(claimed_tiles->data[i].x, claimed_tiles->data[i].y))
			return false;

	return true;
}

void map_flag_matrix_set(map_flag_matrix_t *flag_matrix, int x, int y, bool apply_mirror)
{
	MirrorPoints mirror_points = apply_mirror ? mirror_points_init(x, y, 1) : mirror_points_init_none(x, y, 1);

	for (int i = 0; i < mirror_points.count; ++i) {
		int bit_index = mirror_points.points[i].x * MAP_SIZE + mirror_points.points[i].y;
		int word_index = bit_index / (sizeof(map_flag_matrix_t) * CHAR_BIT);
		map_flag_matrix_t bit_mask = 1U << (bit_index % (sizeof(map_flag_matrix_t) * CHAR_BIT));
		flag_matrix[word_index] |= bit_mask;
	}
}
void map_flag_matrix_clear(map_flag_matrix_t *flag_matrix, int x, int y, bool apply_mirror)
{
	MirrorPoints mirror_points = apply_mirror ? mirror_points_init(x, y, 1) : mirror_points_init_none(x, y, 1);

	for (int i = 0; i < mirror_points.count; ++i) {
		int bit_index = mirror_points.points[i].x * MAP_SIZE + mirror_points.points[i].y;
		int word_index = bit_index / (sizeof(map_flag_matrix_t) * CHAR_BIT);
		map_flag_matrix_t bit_mask = ~(1U << (bit_index % (sizeof(map_flag_matrix_t) * CHAR_BIT)));
		flag_matrix[word_index] &= bit_mask;
	}
}
bool map_flag_matrix_test(const map_flag_matrix_t *flag_matrix, int x, int y)
{
	int bit_index = x * MAP_SIZE + y;
	int word_index = bit_index / (sizeof(map_flag_matrix_t) * CHAR_BIT);
	map_flag_matrix_t bit_mask = 1U << (bit_index % (sizeof(map_flag_matrix_t) * CHAR_BIT));
	return (flag_matrix[word_index] & bit_mask) != 0;
}
void map_flag_matrix_set_all(map_flag_matrix_t *flag_matrix)
{
	memset(flag_matrix, -1, MAP_FLAG_MATRIX_WORD_COUNT * sizeof(map_flag_matrix_t));
}
void map_flag_matrix_clear_all(map_flag_matrix_t *flag_matrix)
{
	memset(flag_matrix, 0, MAP_FLAG_MATRIX_WORD_COUNT * sizeof(map_flag_matrix_t));
}

void map_set_feature(Map *map, int x, int y, Feature feature)
{
	MirrorPoints mirror_points = mirror_points_init(x, y, 1);
	for (int i = 0; i < mirror_points.count; ++i)
		map->tile_matrix[mirror_points.points[i].x][mirror_points.points[i].y].feature = feature;
}
void map_set_height(Map *map, int x, int y, u8 height)
{
	MirrorPoints mirror_points = mirror_points_init(x, y, 1);
	for (int i = 0; i < mirror_points.count; ++i)
		map->tile_matrix[mirror_points.points[i].x][mirror_points.points[i].y].height = height;
}
void map_set_surface(Map *map, int x, int y, Surface surface)
{
	MirrorPoints mirror_points = mirror_points_init(x, y, 1);
	for (int i = 0; i < mirror_points.count; ++i)
		map->tile_matrix[mirror_points.points[i].x][mirror_points.points[i].y].surface = surface;
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

bool map_place_building(Map *map, int x, int y, BuildingType type, BuildingOrientation orientation, int owner, bool mirror_owner)
{
	int size = building_sizes[type];
	MirrorPoints mirror_points = mirror_points_init(x, y, size);

	for (int i = 0; i < mirror_points.count; ++i) {
		Coords origin = mirror_points.points[i];
		int copy_owner = mirror_owner ? owner + i : owner;

		if (map->building_array.usage == map->building_array.capacity) {
			usize new_capacity = map->building_array.capacity * 2;
			BuildingObject *new_data = realloc(map->building_array.data, new_capacity * sizeof(*map->building_array.data));
			if (!new_data)
				return false;
			map->building_array.data = new_data;
			map->building_array.capacity = new_capacity;
		}
		map->building_array.data[map->building_array.usage++] = (BuildingObject){origin, type, copy_owner, orientation};

		for (int j = origin.x; j < origin.x + size; ++j)
			for (int k = origin.y; k < origin.y + size; ++k)
				map->tile_matrix[j][k].object_flags |= OF_BUILDING;
	}

	return true;
}
bool map_place_pitch_ditch(Map *map, int x, int y, int owner, bool mirror_owner)
{
	MirrorPoints mirror_points = mirror_points_init(x, y, 1);

	for (int i = 0; i < mirror_points.count; ++i) {
		Coords coords = mirror_points.points[i];
		int copy_owner = mirror_owner ? owner + i : owner;

		if (map->pitch_ditch_array.usage == map->pitch_ditch_array.capacity) {
			usize new_capacity = map->pitch_ditch_array.capacity * 2;
			PitchDitchObject *new_data = realloc(map->pitch_ditch_array.data, new_capacity * sizeof(*map->pitch_ditch_array.data));
			if (!new_data)
				return false;
			map->pitch_ditch_array.data = new_data;
			map->pitch_ditch_array.capacity = new_capacity;
		}
		map->pitch_ditch_array.data[map->pitch_ditch_array.usage++] = (PitchDitchObject){coords, copy_owner};

		map->tile_matrix[coords.x][coords.y].object_flags |= OF_PITCH_DITCH;
	}

	return true;
}
bool map_place_wall(Map *map, int x_start, int y_start, int x_end, int y_end, WallType type, int owner, bool mirror_owner)
{
	MirrorPoints mirror_points_start = mirror_points_init(x_start, y_start, 1);
	MirrorPoints mirror_points_end = mirror_points_init(x_end, y_end, 1);

	for (int i = 0; i < mirror_points_start.count; ++i) {
		Coords start = mirror_points_start.points[i];
		Coords end = mirror_points_end.points[i];
		int copy_owner = mirror_owner ? owner + i : owner;

		if (map->wall_array.usage == map->wall_array.capacity) {
			usize new_capacity = map->wall_array.capacity * 2;
			WallObject *new_data = realloc(map->wall_array.data, new_capacity * sizeof(*map->wall_array.data));
			if (!new_data)
				return false;
			map->wall_array.data = new_data;
			map->wall_array.capacity = new_capacity;
		}
		map->wall_array.data[map->wall_array.usage++] = (WallObject){start, end, type, copy_owner};

		map->tile_matrix[start.x][start.y].object_flags |= OF_WALL;

		Coords walker_pos = start;
		while (walker_pos.x != end.x || walker_pos.y != end.y) {
			walker_pos.x += (walker_pos.x < end.x) - (walker_pos.x > end.x);
			walker_pos.y += (walker_pos.y < end.y) - (walker_pos.y > end.y);
			map->tile_matrix[walker_pos.x][walker_pos.y].object_flags |= OF_WALL;
		}
	}

	return true;
}
bool map_place_rock(Map *map, int x, int y, int size)
{
	MirrorPoints mirror_points = mirror_points_init(x, y, size);

	for (int i = 0; i < mirror_points.count; ++i) {
		Coords origin = mirror_points.points[i];

		if (map->rock_array.usage == map->rock_array.capacity) {
			usize new_capacity = map->rock_array.capacity * 2;
			RockObject *new_data = realloc(map->rock_array.data, new_capacity * sizeof(*map->rock_array.data));
			if (!new_data)
				return false;
			map->rock_array.data = new_data;
			map->rock_array.capacity = new_capacity;
		}
		map->rock_array.data[map->rock_array.usage++] = (RockObject){origin, size};

		for (int j = origin.x; j < origin.x + size; ++j)
			for (int k = origin.y; k < origin.y + size; ++k)
				map->tile_matrix[j][k].object_flags |= OF_ROCK;
	}

	return true;
}
bool map_place_plant(Map *map, int x, int y, PlantType type, int variant, int stage)
{
	MirrorPoints mirror_points = mirror_points_init(x, y, 1);

	for (int i = 0; i < mirror_points.count; ++i) {
		Coords coords = mirror_points.points[i];

		if (map->plant_array.usage == map->plant_array.capacity) {
			usize new_capacity = map->plant_array.capacity * 2;
			PlantObject *new_data = realloc(map->plant_array.data, new_capacity * sizeof(*map->plant_array.data));
			if (!new_data)
				return false;
			map->plant_array.data = new_data;
			map->plant_array.capacity = new_capacity;
		}
		map->plant_array.data[map->plant_array.usage++] = (PlantObject){coords, type, variant, stage};

		map->tile_matrix[coords.x][coords.y].object_flags |= OF_PLANT;
	}

	return true;
}
bool map_place_unit(Map *map, int x, int y, UnitType type, int owner, bool mirror_owner)
{
	MirrorPoints mirror_points = mirror_points_init(x, y, 1);

	for (int i = 0; i < mirror_points.count; ++i) {
		Coords coords = mirror_points.points[i];
		int copy_owner = mirror_owner ? owner + i : owner;

		if (map->unit_array.usage == map->unit_array.capacity) {
			usize new_capacity = map->unit_array.capacity * 2;
			UnitObject *new_data = realloc(map->unit_array.data, new_capacity * sizeof(*map->unit_array.data));
			if (!new_data)
				return false;
			map->unit_array.data = new_data;
			map->unit_array.capacity = new_capacity;
		}
		map->unit_array.data[map->unit_array.usage++] = (UnitObject){coords, type, copy_owner};

		map->tile_matrix[coords.x][coords.y].object_flags |= OF_UNIT;
	}

	return true;
}
bool map_place_animal_group(Map *map, int x, int y, AnimalGroupType type, int count)
{
	MirrorPoints mirror_points = mirror_points_init(x, y, 1);

	for (int i = 0; i < mirror_points.count; ++i) {
		Coords coords = mirror_points.points[i];

		if (map->animal_group_array.usage == map->animal_group_array.capacity) {
			usize new_capacity = map->animal_group_array.capacity * 2;
			AnimalGroupObject *new_data = realloc(map->animal_group_array.data, new_capacity * sizeof(*map->animal_group_array.data));
			if (!new_data)
				return false;
			map->animal_group_array.data = new_data;
			map->animal_group_array.capacity = new_capacity;
		}
		map->animal_group_array.data[map->animal_group_array.usage++] = (AnimalGroupObject){coords, type, count};

		map->tile_matrix[coords.x][coords.y].object_flags |= OF_ANIMAL_GROUP;
	}

	return true;
}
