#pragma once
#include "utils.h"

enum {
	MAP_SIZE = 400,
};

enum {
	OF_BUILDING    = (1 << 0),
	OF_PITCH_DITCH = (1 << 1),
	OF_WALL        = (1 << 2),
	OF_ROCK        = (1 << 3),
	OF_PLANT       = (1 << 4),
	OF_UNIT        = (1 << 5),
	OF_ANIMAL      = (1 << 6),
};

typedef enum Section {
	SECTION_PADDING,
	SECTION_BORDER,
	SECTION_SACRIFICED,
	SECTION_VALID,
} Section;
typedef enum Feature {
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
typedef enum Surface {
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
typedef struct Tile {
	Section section;
	u16 distance_from_center;
	Feature feature;
	u8 height;
	Surface surface;
	u8 object_flags;
} Tile;

typedef enum BuildingType {
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
typedef enum WallType {
	WALL_HIGH,
	WALL_LOW,
	WALL_CRENEL,
	WALL_STAIR,
} WallType;
typedef enum PlantType {
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
typedef enum UnitType {
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
typedef enum AnimalType {
	ANIMAL_DEER,
	ANIMAL_LION,
	ANIMAL_RABBIT,
	ANIMAL_CAMEL,
} AnimalType;

typedef enum BuildingOrientation {
	BUILDING_ORIENTATION_UP,
	BUILDING_ORIENTATION_RIGHT,
	BUILDING_ORIENTATION_DOWN,
	BUILDING_ORIENTATION_LEFT,
	BUILDING_ORIENTATION_HORIZONTAL,
	BUILDING_ORIENTATION_VERTICAL,
} BuildingOrientation;

typedef struct BuildingObject {
	Coords position;
	BuildingType type;
	u8 owner;
	BuildingOrientation orientation;
} BuildingObject;
typedef struct PitchDitchObject {
	Coords position;
	u8 owner;
} PitchDitchObject;
typedef struct WallObject {
	Coords start_coords;
	Coords end_coords;
	WallType type;
	u8 owner;
} WallObject;
typedef struct RockObject {
	Coords position;
	u8 size;
} RockObject;
typedef struct PlantObject {
	Coords position;
	PlantType type;
	u8 variant;
	u8 stage;
} PlantObject;
typedef struct UnitObject {
	Coords position;
	UnitType type;
	u8 owner;
} UnitObject;
typedef struct AnimalObject {
	Coords position;
	AnimalType type;
	u8 count;
} AnimalObject;

typedef struct BuildingArray {
	BuildingObject *data;
	u32 usage;
	u32 capacity;
} BuildingArray;
typedef struct PitchDitchArray {
	PitchDitchObject *data;
	u32 usage;
	u32 capacity;
} PitchDitchArray;
typedef struct WallArray {
	WallObject *data;
	u32 usage;
	u32 capacity;
} WallArray;
typedef struct RockArray {
	RockObject *data;
	u32 usage;
	u32 capacity;
} RockArray;
typedef struct PlantArray {
	PlantObject *data;
	u32 usage;
	u32 capacity;
} PlantArray;
typedef struct UnitArray {
	UnitObject *data;
	u32 usage;
	u32 capacity;
} UnitArray;
typedef struct AnimalArray {
	AnimalObject *data;
	u32 usage;
	u32 capacity;
} AnimalArray;

typedef struct Map {
	Tile **tile_matrix;
	BuildingArray building_array;
	PitchDitchArray pitch_ditch_array;
	WallArray wall_array;
	RockArray rock_array;
	PlantArray plant_array;
	UnitArray unit_array;
	AnimalArray animal_array;
} Map;

Map *init_map(void);
void free_map(Map *map);

bool is_in_bounds(i32 x, i32 y);
bool is_in_bounds_rectangle(i32 x_origin, i32 y_origin, u32 rectangle_width, u32 rectangle_length);
bool is_in_bounds_array(CoordsArray *claimed_tiles);

enum {
	FLAG_MATRIX_WORD_COUNT = ((MAP_SIZE * MAP_SIZE) + 31) / 32,
};
void set_flag(u32 *flag_matrix, u32 x, u32 y, bool apply_mirror);
void clear_flag(u32 *flag_matrix, u32 x, u32 y, bool apply_mirror);
bool test_flag(u32 *flag_matrix, u32 x, u32 y);
void set_all_flags(u32 *flag_matrix);
void clear_all_flags(u32 *flag_matrix);

void set_feature(Map *map, u32 x, u32 y, Feature new_feature);
void set_height(Map *map, u32 x, u32 y, u8 new_height);
void set_surface(Map *map, u32 x, u32 y, Surface new_surface);

bool place_building(Map *map, u32 x, u32 y, BuildingType type, u8 owner, BuildingOrientation orientation, bool mirror_owner);
bool place_pitch_ditch(Map *map, u32 x, u32 y, u8 owner, bool mirror_owner);
bool place_wall(Map *map, u32 x_start, u32 y_start, u32 x_end, u32 y_end, WallType type, u8 owner, bool mirror_owner);
bool place_rock(Map *map, u32 x, u32 y, u8 size);
bool place_plant(Map *map, u32 x, u32 y, PlantType type, u8 variant, u8 stage);
bool place_unit(Map *map, u32 x, u32 y, UnitType type, u8 owner, bool mirror_owner);
bool place_animal(Map *map, u32 x, u32 y, AnimalType type, u8 count);
