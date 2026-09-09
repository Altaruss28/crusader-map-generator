#pragma once

#include <limits.h>

#include "common.h"

typedef enum {
	SECTION_PADDING,
	SECTION_BORDER,
	SECTION_SACRIFICED,
	SECTION_VALID,
} Section;
typedef enum {
	FEATURE_NONE,
	FEATURE_CLIFF,
	FEATURE_RAMP,
	FEATURE_RIVER,
	FEATURE_GRASSLAND,
	FEATURE_KEEP,
	FEATURE_RUIN,
	FEATURE_QUARRY_SPOT,
	FEATURE_IRON_MINE_SPOT,
	FEATURE_PITCH_RIG_SPOT,
	FEATURE_TREE,
	FEATURE_ANIMAL,
	FEATURE_SHRUB,
} Feature;
typedef enum {
	SURFACE_EARTH,
	SURFACE_PLATEAU_MEDIUM,
	SURFACE_PLATEAU_HIGH,
	SURFACE_EARTH_AND_STONES,
	SURFACE_DUNES,
	SURFACE_BEACH,
	SURFACE_GRASS_LIGHT,
	SURFACE_GRASS_MEDIUM,
	SURFACE_GRASS_DARK,
	SURFACE_STONE,
	SURFACE_GRAVEL,
	SURFACE_IRON,
	SURFACE_ROCKS,
	SURFACE_MARSH,
	SURFACE_OIL,
	SURFACE_RIVER,
	SURFACE_FORD,
	SURFACE_SEA,
} Surface;
typedef enum {
	OF_BUILDING     = (1 << 0),
	OF_PITCH_DITCH  = (1 << 1),
	OF_WALL         = (1 << 2),
	OF_ROCK         = (1 << 3),
	OF_PLANT        = (1 << 4),
	OF_UNIT         = (1 << 5),
	OF_ANIMAL_GROUP = (1 << 6),
} ObjectFlag;
typedef struct {
	Section section;
	u16 distance_from_center;
	Feature feature;
	u8 height;
	Surface surface;
	ObjectFlag object_flags;
} Tile;

typedef enum {
	BUILDING_FLETCHER,
	BUILDING_WOODCUTTER,
	BUILDING_STOCKPILE,
	BUILDING_HOVEL,
	BUILDING_OX_TETHER,
	BUILDING_QUARRY,
	BUILDING_TUNNEL,
	BUILDING_SIGNPOST,
	BUILDING_MANOR_HOUSE,
	BUILDING_KEEP,
	BUILDING_STRONGHOLD,
	BUILDING_STABLES,
	BUILDING_WHEAT_FARM,
	BUILDING_HOP_FARM,
	BUILDING_APPLE_FARM,
	BUILDING_DAIRY_FARM,
	BUILDING_MILL,
	BUILDING_BAKERY,
	BUILDING_BREWERY,
	BUILDING_MARKET,
	BUILDING_HUNTER,
	BUILDING_GRANARY,
	BUILDING_ARMORY,
	BUILDING_POLETURNER,
	BUILDING_BLACKSMITH,
	BUILDING_ARMORER,
	BUILDING_TANNER,
	BUILDING_MERCENARY_POST,
	BUILDING_BARRACKS,
	BUILDING_ENGINEER_GUILD,
	BUILDING_TUNNELER_GUILD,
	BUILDING_IRON_MINE,
	BUILDING_PITCH_RIG,
	BUILDING_INN,
	BUILDING_APOTHECARY,
	BUILDING_SIEGE_TOWER_STRUCTURE,
	BUILDING_CHAPEL,
	BUILDING_CHURCH,
	BUILDING_CATHEDRAL,
	BUILDING_KILLING_PIT,
	BUILDING_PITCH_DITCH,
	BUILDING_GATE_LARGE,
	BUILDING_GATE_SMALL,
	BUILDING_DRAWBRIDGE,
	BUILDING_QUARRY_PILE,
	BUILDING_TOWER_ONE,
	BUILDING_TOWER_TWO,
	BUILDING_TOWER_THREE,
	BUILDING_TOWER_FOUR,
	BUILDING_TOWER_FIVE,
	BUILDING_TOWER_ONE_RUIN,
	BUILDING_TOWER_TWO_RUIN,
	BUILDING_TOWER_THREE_RUIN,
	BUILDING_TOWER_FOUR_RUIN,
	BUILDING_TOWER_FIVE_RUIN,
	BUILDING_UNKNOWN_140,
	BUILDING_UNKNOWN_141,
	BUILDING_UNKNOWN_142,
	BUILDING_UNKNOWN_143,
	BUILDING_GARDEN_2X2_ONE,
	BUILDING_GARDEN_2X2_TWO,
	BUILDING_GARDEN_2X2_THREE,
	BUILDING_GARDEN_2X2_FOUR,
	BUILDING_GARDEN_2X2_FIVE,
	BUILDING_GARDEN_2X2_SIX,
	BUILDING_GARDEN_3X3_ONE,
	BUILDING_GARDEN_3X3_TWO,
	BUILDING_GARDEN_3X3_THREE,
	BUILDING_GARDEN_4X4_ONE,
	BUILDING_GARDEN_4X4_TWO,
	BUILDING_GARDEN_4X4_THREE,
	BUILDING_MAYPOLE,
	BUILDING_GALLOWS,
	BUILDING_STOCKS,
	BUILDING_OUTPOST_EUROPEAN,
	BUILDING_OUTPOST_ARABIAN,
	BUILDING_OIL_SMELTER,
	BUILDING_CATAPULT_TENT,
	BUILDING_TREBUCHET_TENT,
	BUILDING_BATTERING_RAM_TENT,
	BUILDING_SIEGE_TOWER_TENT,
	BUILDING_SHIELD_TENT,
	BUILDING_TOWER_ONE_RUIN_SECOND,
	BUILDING_TOWER_TWO_RUIN_SECOND,
	BUILDING_RUIN_1X1_ONE,
	BUILDING_RUIN_1X1_TWO,
	BUILDING_RUIN_1X1_THREE,
	BUILDING_RUIN_1X1_FOUR,
	BUILDING_RUIN_2X2_ONE,
	BUILDING_RUIN_2X2_TWO,
	BUILDING_RUIN_2X2_THREE,
	BUILDING_RUIN_2X2_FOUR,
	BUILDING_RUIN_2X2_FIVE,
	BUILDING_RUIN_2X2_SIX,
	BUILDING_RUIN_4X4_ONE,
	BUILDING_RUIN_4X4_TWO,
	BUILDING_RUIN_4X4_THREE,
	BUILDING_CESSPIT_ONE,
	BUILDING_CESSPIT_TWO,
	BUILDING_CESSPIT_THREE,
	BUILDING_CESSPIT_FOUR,
	BUILDING_BURNING_STAKE,
	BUILDING_GIBBET,
	BUILDING_DUNGEON,
	BUILDING_STRETCHING_RACK,
	BUILDING_FLOGGING_RACK,
	BUILDING_CHOPPING_BLOCK,
	BUILDING_DUNKING_STOOL,
	BUILDING_DOG_CAGE,
	BUILDING_STATUE_ONE,
	BUILDING_STATUE_TWO,
	BUILDING_STATUE_THREE,
	BUILDING_STATUE_FOUR,
	BUILDING_STATUE_FIVE,
	BUILDING_SHRINE_ONE,
	BUILDING_SHRINE_TWO,
	BUILDING_SHRINE_THREE,
	BUILDING_SHRINE_FOUR,
	BUILDING_SHRINE_FIVE,
	BUILDING_BEEHIVE,
	BUILDING_DANCING_BEAR,
	BUILDING_POND_SMALL_ONE,
	BUILDING_POND_SMALL_TWO,
	BUILDING_POND_LARGE_ONE,
	BUILDING_POND_LARGE_TWO,
	BUILDING_BEAR_CAVE,
	BUILDING_WELL,
	BUILDING_WATERPOT,
	BUILDING_FIRE_BALLISTA_TENT,
	BUILDING_FIRE_BALLISTA_TENT_SECOND,
} BuildingType;
typedef enum {
	BUILDING_ORIENTATION_UP,
	BUILDING_ORIENTATION_RIGHT,
	BUILDING_ORIENTATION_DOWN,
	BUILDING_ORIENTATION_LEFT,
	BUILDING_ORIENTATION_HORIZONTAL,
	BUILDING_ORIENTATION_VERTICAL,
} BuildingOrientation;
typedef enum {
	WALL_HIGH,
	WALL_LOW,
	WALL_CRENEL,
	WALL_STAIR,
} WallType;
typedef enum {
	PLANT_TREE_DATE,
	PLANT_TREE_COCONUT,
	PLANT_TREE_OLIVE,
	PLANT_TREE_CHERRY,
	PLANT_TREE_APPLE,
	PLANT_SHRUB_OLD,
	PLANT_SHRUB_THICK,
	PLANT_SHRUB_AGAVE,
	PLANT_CACTUS_NORMAL,
	PLANT_CACTUS_BLOBBY,
	PLANT_CACTUS_WIGGLY,
} PlantType;
typedef enum {
	UNIT_PLACEHOLDER,
	UNIT_PEASANT,
	UNIT_BURNING_MAN,
	UNIT_WOODCUTTER,
	UNIT_FLETCHER,
	UNIT_TUNNELER,
	UNIT_HUNTER,
	UNIT_QUARRY_MASON,
	UNIT_QUARRY_GRUNT,
	UNIT_QUARRY_OX,
	UNIT_PITCH_WORKER,
	UNIT_WHEAT_FARMER,
	UNIT_HOP_FARMER,
	UNIT_APPLE_FARMER,
	UNIT_DAIRY_FARMER,
	UNIT_MILLER,
	UNIT_BAKER,
	UNIT_BREWER,
	UNIT_POLETURNER,
	UNIT_BLACKSMITH,
	UNIT_ARMORER,
	UNIT_TANNER,
	UNIT_EUROPEAN_ARCHER,
	UNIT_CROSSBOWMAN,
	UNIT_SPEARMAN,
	UNIT_PIKEMAN,
	UNIT_MACEMAN,
	UNIT_EUROPEAN_SWORDSMAN,
	UNIT_KNIGHT,
	UNIT_LADERMAN,
	UNIT_ENGINEER,
	UNIT_IRON_MINER_ONE,
	UNIT_IRON_MINER_TWO,
	UNIT_PRIEST,
	UNIT_HEALER,
	UNIT_DRUNKARD,
	UNIT_INNKEEPER,
	UNIT_MONK,
	UNIT_UNKNOWN_ONE,
	UNIT_CATAPULT,
	UNIT_TREBUCHET,
	UNIT_MANGONEL,
	UNIT_TRADER,
	UNIT_TRADER_HORSE,
	UNIT_DEER,
	UNIT_LION,
	UNIT_RABBIT,
	UNIT_CAMEL,
	UNIT_CROW,
	UNIT_SEAGULL,
	UNIT_SIEGE_TENT,
	UNIT_COW,
	UNIT_HUNTER_DOG,
	UNIT_FIREMAN,
	UNIT_GHOST,
	UNIT_LORD,
	UNIT_LADY,
	UNIT_JESTER,
	UNIT_SIEGE_TOWER,
	UNIT_BATTERING_RAM,
	UNIT_SHIELD,
	UNIT_TOWER_BALLISTA,
	UNIT_CHICKEN,
	UNIT_MOTHER,
	UNIT_CHILD,
	UNIT_JUGGLER,
	UNIT_FIRE_EATER,
	UNIT_WAR_DOG,
	UNIT_BURNING_ANIMAL_LARGE,
	UNIT_BURNING_ANIMAL_SMALL,
	UNIT_ARABIAN_ARCHER,
	UNIT_SLAVE,
	UNIT_SLINGER,
	UNIT_ASSASSIN,
	UNIT_HORSE_ARCHER,
	UNIT_ARABIAN_SWORDSMAN,
	UNIT_FIRETHROWER,
	UNIT_FIRE_BALLISTA,
} UnitType;
typedef enum {
	ANIMAL_GROUP_DEER,
	ANIMAL_GROUP_LION,
	ANIMAL_GROUP_RABBIT,
	ANIMAL_GROUP_CAMEL,
} AnimalGroupType;

typedef struct {
	Coords position;
	BuildingType type;
	u8 owner;
	BuildingOrientation orientation;
} BuildingObject;
typedef struct {
	Coords position;
	u8 owner;
} PitchDitchObject;
typedef struct {
	Coords start;
	Coords end;
	WallType type;
	u8 owner;
} WallObject;
typedef struct {
	Coords position;
	u8 size;
} RockObject;
typedef struct {
	Coords position;
	PlantType type;
	u8 variant;
	u8 stage;
} PlantObject;
typedef struct {
	Coords position;
	UnitType type;
	u8 owner;
} UnitObject;
typedef struct {
	Coords position;
	AnimalGroupType type;
	u8 count;
} AnimalGroupObject;

constexpr int MAP_SIZE = 400;
typedef struct {
	Tile tile_matrix[MAP_SIZE][MAP_SIZE];
	struct {
		BuildingObject *data;
		usize usage;
		usize capacity;
	} building_array;
	struct {
		PitchDitchObject *data;
		usize usage;
		usize capacity;
	} pitch_ditch_array;
	struct {
		WallObject *data;
		usize usage;
		usize capacity;
	} wall_array;
	struct {
		RockObject *data;
		usize usage;
		usize capacity;
	} rock_array;
	struct {
		PlantObject *data;
		usize usage;
		usize capacity;
	} plant_array;
	struct {
		UnitObject *data;
		usize usage;
		usize capacity;
	} unit_array;
	struct {
		AnimalGroupObject *data;
		usize usage;
		usize capacity;
	} animal_group_array;
} Map;

Map *map_init(void);
void map_free(Map *map);

bool map_check_bounds_point(int x, int y);
bool map_check_bounds_rectangle(int x_origin, int y_origin, int rectangle_width, int rectangle_length);
bool map_check_bounds_array(const CoordsArray *claimed_tiles);

typedef usize map_flag_matrix_t;
constexpr int MAP_FLAG_MATRIX_WORD_COUNT = ((MAP_SIZE * MAP_SIZE) + ((sizeof(map_flag_matrix_t) * CHAR_BIT) - 1)) / (sizeof(map_flag_matrix_t) * CHAR_BIT);
void map_flag_matrix_set(map_flag_matrix_t *flag_matrix, int x, int y, bool apply_mirror);
void map_flag_matrix_clear(map_flag_matrix_t *flag_matrix, int x, int y, bool apply_mirror);
bool map_flag_matrix_test(const map_flag_matrix_t *flag_matrix, int x, int y);
void map_flag_matrix_set_all(map_flag_matrix_t *flag_matrix);
void map_flag_matrix_clear_all(map_flag_matrix_t *flag_matrix);

void map_set_feature(Map *map, int x, int y, Feature feature);
void map_set_height(Map *map, int x, int y, u8 height);
void map_set_surface(Map *map, int x, int y, Surface surface);

bool map_place_building(Map *map, int x, int y, BuildingType type, BuildingOrientation orientation, int owner, bool mirror_owner);
bool map_place_pitch_ditch(Map *map, int x, int y, int owner, bool mirror_owner);
bool map_place_wall(Map *map, int x_start, int y_start, int x_end, int y_end, WallType type, int owner, bool mirror_owner);
bool map_place_rock(Map *map, int x, int y, int size);
bool map_place_plant(Map *map, int x, int y, PlantType type, int variant, int stage);
bool map_place_unit(Map *map, int x, int y, UnitType type, int owner, bool mirror_owner);
bool map_place_animal_group(Map *map, int x, int y, AnimalGroupType type, int count);
