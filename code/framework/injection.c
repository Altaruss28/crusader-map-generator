#include "injection.h"

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "map.h"
#include "process_access.h"

typedef enum {
	ADDR_CE_BUILDING_PLACEMENT_VALIDATION,
	ADDR_CE_WHEAT_FARM_PLACEMENT_CHANGING_SURFACE,
	ADDR_CE_HOP_FARM_PLACEMENT_CHANGING_SURFACE,
	ADDR_CE_APPLE_FARM_PLACEMENT_CHANGING_SURFACE,
	ADDR_CE_DAIRY_FARM_PLACEMENT_CHANGING_SURFACE,
	ADDR_CE_KEEP_MAIN_BUILDING_PLACEMENT_CHANGING_SURFACE,
	ADDR_CE_KEEP_CAMPFIRE_PLACEMENT_CHANGING_SURFACE,
	ADDR_CE_KEEP_PLACEMENT_STARTING_SCENARIO_GAME,
	ADDR_CE_PITCH_DITCH_CHANGING_HEIGHT,
	ADDR_CE_WALL_PLACEMENT_VALIDATION,
	ADDR_CE_WALL_COST_CHECK,
	ADDR_CE_TREE_PLACEMENT_NEXT_TO_MOST_THINGS,
	ADDR_CE_DEER_COUNT_RANDOMIZATION,
	ADDR_CE_LION_COUNT_RANDOMIZATION,
	ADDR_CE_RABBIT_COUNT_RANDOMIZATION,
	ADDR_CE_CAMEL_COUNT_RANDOMIZATION,
	ADDR_TILE_STATE_PTR,
	ADDR_CNV_BUILDING_AND_PITCH_DITCH_PLACEMENT_FUNCTION,
	ADDR_CNV_WALL_PLACEMENT_FUNCTION,
	ADDR_CNV_ROCK_PLACEMENT_FUNCTION,
	ADDR_CNV_ROCK_ORIENTATION_VALUE,
	ADDR_CNV_PLANT_PLACEMENT_FUNCTION,
	ADDR_CNV_PLANT_STAGE_VALUE,
	ADDR_CNV_APPLE_TREE_PLACEMENT_FUNCTION,
	ADDR_CNV_APPLE_TREE_STAGE_VALUE,
	ADDR_CNV_PLANT_ARRAY,
	ADDR_CNV_UNIT_FUNCTION,
	ADDR_CNV_UNIT_STATE_PTR,
	ADDR_CNV_ANIMAL_GROUP_FUNCTION,
	ADDR_CNV_TRIBE_STATE_PTR,
	ADDR_CNV_ANIMAL_GROUP_DEER_COUNT_VALUE,
	ADDR_CNV_ANIMAL_GROUP_LION_COUNT_VALUE,
	ADDR_CNV_ANIMAL_GROUP_RABBIT_COUNT_VALUE,
	ADDR_CNV_ANIMAL_GROUP_CAMEL_COUNT_VALUE,
	ADDR_INJ_BASE_HEIGHT_ARRAY,
	ADDR_INJ_TOTAL_HEIGHT_ARRAY,
	ADDR_INJ_SURFACE_ARRAY,
	ADDR_INJ_LOGICAL_ARRAY,
	ADDR_INJ_CLEAR_MAP_FUNCTION,
	ADDR_INJ_MINIMAP_STATE_PTR,
	ADDR_INJ_MINIMAP_FIX_FUNCTION_FIRST,
	ADDR_INJ_MINIMAP_FIX_FUNCTION_SECOND,
	ADDR_INJ_MINIMAP_FIX_FUNCTION_THIRD,
	ADDR_INJ_TEXT_BUFFER_BASE,
	ADDR_INJ_MAP_DESCRIPTION_BUFFER,
	ADDR_INJ_SAVE_MAP_FUNCTION,
	ADDR_INJ_MAP_TYPE_VALUE,
	ADDR_INJ_MENU_ACTION_VALUE,
	ADDR_INJ_MENU_VIEW_VALUE,
} AddressType;
static AobScanEntry address_data[] = {
	[ADDR_CE_BUILDING_PLACEMENT_VALIDATION] = {0, false, "?? ?? ?? ?? 50 51 8B 4C 24 24 57"},
	[ADDR_CE_WHEAT_FARM_PLACEMENT_CHANGING_SURFACE] = {0, false, "?? 03 CF 51 50 52 8B CE E8 ?? ?? ?? ?? 83 C3 01 3B 9E ?? ?? ?? ?? 0F 8C ?? ?? ?? ?? 8B 5C 24 10"},
	[ADDR_CE_HOP_FARM_PLACEMENT_CHANGING_SURFACE] = {0, false, "?? 03 D5 52 50 8B"},
	[ADDR_CE_APPLE_FARM_PLACEMENT_CHANGING_SURFACE] = {0, false, "?? 03 D7 52 55 50"},
	[ADDR_CE_DAIRY_FARM_PLACEMENT_CHANGING_SURFACE] = {0, false, "?? 6A 00 6A 06 03 CB"},
	[ADDR_CE_KEEP_MAIN_BUILDING_PLACEMENT_CHANGING_SURFACE] = {0, false, "?? ?? 8B 8E ?? ?? ?? ?? 68 80"},
	[ADDR_CE_KEEP_CAMPFIRE_PLACEMENT_CHANGING_SURFACE] = {0, false, "?? ?? 68 80 00 00 00 6A 00 52"},
	[ADDR_CE_KEEP_PLACEMENT_STARTING_SCENARIO_GAME] = {0, false, "?? ?? ?? ?? ?? ?? 83 F8 06 0F 84 ?? ?? ?? ?? 6A 00"},
	[ADDR_CE_PITCH_DITCH_CHANGING_HEIGHT] = {0, false, "?? ?? 80 00 FC EB"},
	[ADDR_CE_WALL_PLACEMENT_VALIDATION] = {0, false, "?? ?? ?? ?? ?? ?? 8B 44 24 3C 6A 04 50 B9"},
	[ADDR_CE_WALL_COST_CHECK] = {0, false, "?? 7F 8B 15 ?? ?? ?? ?? 83"},
	[ADDR_CE_TREE_PLACEMENT_NEXT_TO_MOST_THINGS] = {0, false, "?? 50 56 55 8D"},
	[ADDR_CE_DEER_COUNT_RANDOMIZATION] = {0, false, "?? BF 2C 00 00 00 83"},
	[ADDR_CE_LION_COUNT_RANDOMIZATION] = {0, false, "?? BF 2D 00 00 00 83 C0"},
	[ADDR_CE_RABBIT_COUNT_RANDOMIZATION] = {0, false, "?? BF 2E 00 00 00 83"},
	[ADDR_CE_CAMEL_COUNT_RANDOMIZATION] = {0, false, "?? BF 2F 00 00 00 83"},
	[ADDR_TILE_STATE_PTR] = {0, true, "?? ?? ?? ?? E8 ?? ?? ?? ?? 3B 44 24 6C 89 44 24 24 7E 08"},
	[ADDR_CNV_BUILDING_AND_PITCH_DITCH_PLACEMENT_FUNCTION] = {0, false, "83 EC 08 53 55 8B 6C 24 18 56 8B F1"},
	[ADDR_CNV_WALL_PLACEMENT_FUNCTION] = {0, false, "83 EC 28 8B 54"},
	[ADDR_CNV_ROCK_PLACEMENT_FUNCTION] = {0, false, "51 8B 44 24 08 53 55"},
	[ADDR_CNV_ROCK_ORIENTATION_VALUE] = {0, true, "?? ?? ?? ?? 00 00 00 00 A3 ?? ?? ?? ?? A1 ?? ?? ?? ?? 83"},
	[ADDR_CNV_PLANT_PLACEMENT_FUNCTION] = {0, false, "83 EC 0C 53 8B 5C 24 14 81 FB 8F 01 00 00 57"},
	[ADDR_CNV_PLANT_STAGE_VALUE] = {0, false, "?? 6A 00 0F B7 C0 6A"},
	[ADDR_CNV_APPLE_TREE_PLACEMENT_FUNCTION] = {0, false, "8B 44 24 0C 8B 4C 24 08 6A ?? 6A 00"},
	[ADDR_CNV_APPLE_TREE_STAGE_VALUE] = {0, false, "?? 6A 00 6A 00 6A 01 6A 0F"},
	[ADDR_CNV_PLANT_ARRAY] = {0, true, "?? ?? ?? ?? 68 9C 00 00 00 51 B9"},
	[ADDR_CNV_UNIT_FUNCTION] = {0, false, "53 8B D9 B9 01"},
	[ADDR_CNV_UNIT_STATE_PTR] = {0, true, "?? ?? ?? ?? E8 ?? ?? ?? ?? 69 F6 90 04 00 00 0F BF B6"},
	[ADDR_CNV_ANIMAL_GROUP_FUNCTION] = {0, false, "83 EC 08 55 57 33"},
	[ADDR_CNV_TRIBE_STATE_PTR] = {0, true, "?? ?? ?? ?? 89 7C 24 20 E8 ?? ?? ?? ?? 8B E8"},
	[ADDR_CNV_ANIMAL_GROUP_DEER_COUNT_VALUE] = {0, false, "?? 89 44 24 10 33 DB 39"},
	[ADDR_CNV_ANIMAL_GROUP_LION_COUNT_VALUE] = {0, false, "?? EB AF 81 FB DE"},
	[ADDR_CNV_ANIMAL_GROUP_RABBIT_COUNT_VALUE] = {0, false, "?? E9 70 FF FF FF 81 FB DF"},
	[ADDR_CNV_ANIMAL_GROUP_CAMEL_COUNT_VALUE] = {0, false, "?? E9 35 FF FF FF 81 FB"},
	[ADDR_INJ_BASE_HEIGHT_ARRAY] = {0, true, "?? ?? ?? ?? 3B D1 0F 8D 02"},
	[ADDR_INJ_TOTAL_HEIGHT_ARRAY] = {0, true, "?? ?? ?? ?? 0F 8D DC 03"},
	[ADDR_INJ_SURFACE_ARRAY] = {0, true, "?? ?? ?? ?? 75 11 83 7C 24 18"},
	[ADDR_INJ_LOGICAL_ARRAY] = {0, true, "?? ?? ?? ?? 00 00 00 10 89 74 24 18"},
	[ADDR_INJ_CLEAR_MAP_FUNCTION] = {0, false, "53 55 56 8B F1 57 33 FF 89 BE"},
	[ADDR_INJ_MINIMAP_STATE_PTR] = {0, true, "?? ?? ?? ?? E8 ?? ?? ?? ?? 83 6C 24 10 01 68"},
	[ADDR_INJ_MINIMAP_FIX_FUNCTION_FIRST] = {0, false, "55 8B EC 81 EC 18 01 00 00 53"},
	[ADDR_INJ_MINIMAP_FIX_FUNCTION_SECOND] = {0, false, "81 3D ?? ?? ?? ?? C8 00 00 00 8B 44 24 08 8B 54 24 04 50 52 7E 0E 6A 01 6A 02 6A 04 E8 ?? ?? ?? ?? C2 08 00 6A 02 6A 04 6A 04 E8 ?? ?? ?? ?? C2 08 00 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 55"},
	[ADDR_INJ_MINIMAP_FIX_FUNCTION_THIRD] = {0, false, "81 3D ?? ?? ?? ?? C8 00 00 00 8B 44 24 08 8B 54 24 04 50 52 7E 0E 6A 01 6A 02 6A 04 E8 ?? ?? ?? ?? C2 08 00 6A 02 6A 04 6A 04 E8 ?? ?? ?? ?? C2 08 00 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 81"},
	[ADDR_INJ_TEXT_BUFFER_BASE] = {0, true, "?? ?? ?? ?? E8 ?? ?? ?? ?? 6A 06 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? BE"},
	[ADDR_INJ_MAP_DESCRIPTION_BUFFER] = {0, true, "?? ?? ?? ?? EB 25 A1 ?? ?? ?? ?? 3B C6"},
	[ADDR_INJ_SAVE_MAP_FUNCTION] = {0, false, "81 EC F4 03 00 00 A1 ?? ?? ?? ?? 33 C4 89 84 24 F0 03 00 00 A1 ?? ?? ?? ?? 53"},
	[ADDR_INJ_MAP_TYPE_VALUE] = {0, true, "?? ?? ?? ?? 74 46 57 33 FF 89"},
	[ADDR_INJ_MENU_ACTION_VALUE] = {0, true, "?? ?? ?? ?? 09 00 00 00 E8 ?? ?? ?? ?? 6A 00"},
	[ADDR_INJ_MENU_VIEW_VALUE] = {0, true, "?? ?? ?? ?? 10 75 37 83 3D"},
};
bool addresses_init(void)
{
	return aob_scan(address_data, ARRAY_LENGTH(address_data));
}

static const struct {
	u32 *address;
	int byte_count;
	u8 *bytes;
} code_edit_entries[] = {
	{&address_data[ADDR_CE_BUILDING_PLACEMENT_VALIDATION].address, 4, (u8[]){0xEB, 0x1F, 0x90, 0x90}},
	{&address_data[ADDR_CE_WHEAT_FARM_PLACEMENT_CHANGING_SURFACE].address, 1, (u8[]){0x00}},
	{&address_data[ADDR_CE_HOP_FARM_PLACEMENT_CHANGING_SURFACE].address, 1, (u8[]){0x00}},
	{&address_data[ADDR_CE_APPLE_FARM_PLACEMENT_CHANGING_SURFACE].address, 1, (u8[]){0x00}},
	{&address_data[ADDR_CE_DAIRY_FARM_PLACEMENT_CHANGING_SURFACE].address, 1, (u8[]){0x00}},
	{&address_data[ADDR_CE_KEEP_MAIN_BUILDING_PLACEMENT_CHANGING_SURFACE].address, 2, (u8[]){0xEB, 0x2C}},
	{&address_data[ADDR_CE_KEEP_CAMPFIRE_PLACEMENT_CHANGING_SURFACE].address, 2, (u8[]){0xEB, 0x2B}},
	{&address_data[ADDR_CE_KEEP_PLACEMENT_STARTING_SCENARIO_GAME].address, 6, (u8[]){0xE9, 0x8B, 0x05, 0x00, 0x00, 0x90}},
	{&address_data[ADDR_CE_PITCH_DITCH_CHANGING_HEIGHT].address, 2, (u8[]){0xEB, 0x08}},
	{&address_data[ADDR_CE_WALL_PLACEMENT_VALIDATION].address, 6, (u8[]){0x90, 0x90, 0x90, 0x90, 0x90, 0x90}},
	{&address_data[ADDR_CE_WALL_COST_CHECK].address, 1, (u8[]){0xEB}},
	{&address_data[ADDR_CE_TREE_PLACEMENT_NEXT_TO_MOST_THINGS].address, 1, (u8[]){0xEB}},
	{&address_data[ADDR_CE_DEER_COUNT_RANDOMIZATION].address, 1, (u8[]){0x00}},
	{&address_data[ADDR_CE_LION_COUNT_RANDOMIZATION].address, 1, (u8[]){0x00}},
	{&address_data[ADDR_CE_RABBIT_COUNT_RANDOMIZATION].address, 1, (u8[]){0x00}},
	{&address_data[ADDR_CE_CAMEL_COUNT_RANDOMIZATION].address, 1, (u8[]){0x00}},
};
static u8 *code_edit_original_byte_buffer = nullptr;

bool code_edits_enable(void)
{
	int buffer_size = 0;
	for (usize i = 0; i < ARRAY_LENGTH(code_edit_entries); ++i)
		buffer_size += code_edit_entries[i].byte_count;
	if (!(code_edit_original_byte_buffer = malloc(buffer_size)))
		goto err;

	int store_offset = 0;

	for (usize i = 0; i < ARRAY_LENGTH(code_edit_entries); ++i) {
		u32 address = *code_edit_entries[i].address;
		int byte_count = code_edit_entries[i].byte_count;
		u8 *bytes = code_edit_entries[i].bytes;

		if (!read_data(address, code_edit_original_byte_buffer + store_offset, byte_count))
			goto err;
		store_offset += byte_count;

		if (!write_data(address, bytes, byte_count))
			goto err;
	}

	return true;

err:
	free(code_edit_original_byte_buffer);
	code_edit_original_byte_buffer = nullptr;
	return false;
}
bool code_edits_disable(void)
{
	if (!code_edit_original_byte_buffer)
		return true;

	bool ret = false;

	int restore_offset = 0;

	for (usize i = 0; i < ARRAY_LENGTH(code_edit_entries); ++i) {
		u32 address = *code_edit_entries[i].address;
		int byte_count = code_edit_entries[i].byte_count;
		u8 *bytes = code_edit_original_byte_buffer + restore_offset;

		if (!write_data(address, bytes, byte_count))
			goto err;

		restore_offset += byte_count;
	}

	ret = true;

err:
	free(code_edit_original_byte_buffer);
	code_edit_original_byte_buffer = nullptr;
	return ret;
}

static int coord_conversion_matrix[MAP_SIZE][MAP_SIZE];
void coord_conversion_matrix_init(void)
{
	int tile_array_index = 0;

	static_assert(MAP_SIZE == 400);
	for (int y = 0; y < MAP_SIZE; ++y)
		for (int x = 0; x < MAP_SIZE; ++x)
			if (x + y >= 199 && x <= y + 200 && y <= x + 200 && x + y <= 599)
				coord_conversion_matrix[x][y] = tile_array_index++;
}

static const u8 conversion_surface_values[] = {
	[SURFACE_EARTH] = 0,
	[SURFACE_PLATEAU_MEDIUM] = 4,
	[SURFACE_PLATEAU_HIGH] = 8,
	[SURFACE_EARTH_AND_STONES] = 2,
	[SURFACE_DUNES] = 64,
	[SURFACE_BEACH] = 32,
	[SURFACE_GRASS_LIGHT] = 1,
	[SURFACE_GRASS_MEDIUM] = 128,
	[SURFACE_GRASS_DARK] = 16,
	[SURFACE_STONE] = 0,
	[SURFACE_GRAVEL] = 0,
	[SURFACE_IRON] = 0,
	[SURFACE_ROCKS] = 0,
	[SURFACE_MARSH] = 0,
	[SURFACE_OIL] = 0,
	[SURFACE_RIVER] = 0,
	[SURFACE_FORD] = 0,
	[SURFACE_SEA] = 0,
};
static const u32 conversion_logical_values[] = {
	[SURFACE_EARTH] = 32768,
	[SURFACE_PLATEAU_MEDIUM] = 32768,
	[SURFACE_PLATEAU_HIGH] = 32768,
	[SURFACE_EARTH_AND_STONES] = 32768,
	[SURFACE_DUNES] = 32768,
	[SURFACE_BEACH] = 32768,
	[SURFACE_GRASS_LIGHT] = 32768,
	[SURFACE_GRASS_MEDIUM] = 32768,
	[SURFACE_GRASS_DARK] = 32768,
	[SURFACE_STONE] = 163840,
	[SURFACE_GRAVEL] = 294912,
	[SURFACE_IRON] = 557056,
	[SURFACE_ROCKS] = 32896,
	[SURFACE_MARSH] = 536903680,
	[SURFACE_OIL] = 2684387328,
	[SURFACE_RIVER] = 1048576,
	[SURFACE_FORD] = 2097152,
	[SURFACE_SEA] = 1,
};

static const struct {
	u16 id;
	u8 size;
	u8 orientation;
	u8 sub_count;
} conversion_building_values[] = {
	[BUILDING_FLETCHER] = {50, 4, 8, 1},
	[BUILDING_WOODCUTTER] = {51, 3, 15, 1},
	[BUILDING_STOCKPILE] = {52, 5, 15, 4},
	[BUILDING_HOVEL] = {54, 4, 15, 1},
	[BUILDING_OX_TETHER] = {55, 2, 15, 1},
	[BUILDING_QUARRY] = {56, 6, 15, 2},
	[BUILDING_TUNNEL] = {57, 3, 15, 1},
	[BUILDING_SIGNPOST] = {59, 2, 15, 1},
	[BUILDING_MANOR_HOUSE] = {60, 7, 15, 9},
	[BUILDING_KEEP] = {61, 7, 15, 9},
	[BUILDING_STRONGHOLD] = {62, 11, 15, 9},
	[BUILDING_STABLES] = {65, 6, 15, 1},
	[BUILDING_WHEAT_FARM] = {70, 9, 15, 1},
	[BUILDING_HOP_FARM] = {71, 9, 15, 1},
	[BUILDING_APPLE_FARM] = {72, 10, 15, 1},
	[BUILDING_DAIRY_FARM] = {73, 10, 15, 1},
	[BUILDING_MILL] = {74, 3, 15, 1},
	[BUILDING_BAKERY] = {75, 4, 8, 1},
	[BUILDING_BREWERY] = {76, 4, 8, 1},
	[BUILDING_MARKET] = {77, 5, 15, 1},
	[BUILDING_HUNTER] = {78, 3, 15, 1},
	[BUILDING_GRANARY] = {80, 4, 15, 1},
	[BUILDING_ARMORY] = {81, 4, 15, 1},
	[BUILDING_POLETURNER] = {82, 4, 8, 1},
	[BUILDING_BLACKSMITH] = {83, 4, 8, 1},
	[BUILDING_ARMORER] = {84, 4, 8, 1},
	[BUILDING_TANNER] = {85, 4, 8, 1},
	[BUILDING_MERCENARY_POST] = {86, 5, 15, 4},
	[BUILDING_BARRACKS] = {87, 5, 15, 4},
	[BUILDING_ENGINEER_GUILD] = {88, 5, 15, 2},
	[BUILDING_TUNNELER_GUILD] = {89, 5, 15, 2},
	[BUILDING_IRON_MINE] = {90, 4, 15, 1},
	[BUILDING_PITCH_RIG] = {91, 4, 15, 1},
	[BUILDING_INN] = {92, 5, 15, 1},
	[BUILDING_APOTHECARY] = {93, 6, 15, 1},
	[BUILDING_SIEGE_TOWER_STRUCTURE] = {94, 3, 15, 1},
	[BUILDING_CHAPEL] = {95, 6, 15, 1},
	[BUILDING_CHURCH] = {96, 9, 15, 1},
	[BUILDING_CATHEDRAL] = {97, 13, 15, 1},
	[BUILDING_KILLING_PIT] = {98, 1, 15, 1},
	[BUILDING_PITCH_DITCH] = {99, 1, 15, 0},
	[BUILDING_GATE_LARGE] = {101, 7, 0, 1},
	[BUILDING_GATE_SMALL] = {102, 5, 0, 1},
	[BUILDING_DRAWBRIDGE] = {105, 5, 0, 1},
	[BUILDING_QUARRY_PILE] = {109, 2, 15, 1},
	[BUILDING_TOWER_ONE] = {110, 3, 15, 1},
	[BUILDING_TOWER_TWO] = {111, 4, 15, 1},
	[BUILDING_TOWER_THREE] = {112, 5, 15, 1},
	[BUILDING_TOWER_FOUR] = {113, 6, 15, 1},
	[BUILDING_TOWER_FIVE] = {114, 6, 15, 1},
	[BUILDING_TOWER_ONE_RUIN] = {115, 3, 15, 1},
	[BUILDING_TOWER_TWO_RUIN] = {116, 4, 15, 1},
	[BUILDING_TOWER_THREE_RUIN] = {117, 5, 15, 1},
	[BUILDING_TOWER_FOUR_RUIN] = {118, 6, 15, 1},
	[BUILDING_TOWER_FIVE_RUIN] = {119, 6, 15, 1},
	[BUILDING_GARDEN_2X2_ONE] = {160, 2, 15, 1},
	[BUILDING_GARDEN_2X2_TWO] = {161, 2, 15, 1},
	[BUILDING_GARDEN_2X2_THREE] = {162, 2, 15, 1},
	[BUILDING_GARDEN_2X2_FOUR] = {163, 2, 15, 1},
	[BUILDING_GARDEN_2X2_FIVE] = {164, 2, 15, 1},
	[BUILDING_GARDEN_2X2_SIX] = {165, 2, 15, 1},
	[BUILDING_GARDEN_3X3_ONE] = {166, 3, 15, 1},
	[BUILDING_GARDEN_3X3_TWO] = {167, 3, 15, 1},
	[BUILDING_GARDEN_3X3_THREE] = {168, 3, 15, 1},
	[BUILDING_GARDEN_4X4_ONE] = {169, 4, 15, 1},
	[BUILDING_GARDEN_4X4_TWO] = {170, 4, 15, 1},
	[BUILDING_GARDEN_4X4_THREE] = {171, 4, 15, 1},
	[BUILDING_MAYPOLE] = {175, 3, 15, 1},
	[BUILDING_GALLOWS] = {176, 2, 15, 1},
	[BUILDING_STOCKS] = {177, 3, 15, 1},
	[BUILDING_OUTPOST_EUROPEAN] = {178, 5, 15, 1},
	[BUILDING_OUTPOST_ARABIAN] = {179, 5, 15, 1},
	[BUILDING_OIL_SMELTER] = {180, 4, 15, 2},
	[BUILDING_CATAPULT_TENT] = {190, 3, 15, 1},
	[BUILDING_TREBUCHET_TENT] = {191, 3, 15, 1},
	[BUILDING_BATTERING_RAM_TENT] = {192, 3, 15, 1},
	[BUILDING_SIEGE_TOWER_TENT] = {193, 3, 15, 1},
	[BUILDING_SHIELD_TENT] = {194, 3, 15, 1},
	[BUILDING_TOWER_ONE_RUIN_SECOND] = {210, 3, 15, 1},
	[BUILDING_TOWER_TWO_RUIN_SECOND] = {211, 4, 15, 1},
	[BUILDING_RUIN_1X1_ONE] = {248, 1, 15, 1},
	[BUILDING_RUIN_1X1_TWO] = {249, 1, 15, 1},
	[BUILDING_RUIN_1X1_THREE] = {250, 1, 15, 1},
	[BUILDING_RUIN_1X1_FOUR] = {251, 1, 15, 1},
	[BUILDING_RUIN_2X2_ONE] = {252, 2, 15, 1},
	[BUILDING_RUIN_2X2_TWO] = {253, 2, 15, 1},
	[BUILDING_RUIN_2X2_THREE] = {254, 2, 15, 1},
	[BUILDING_RUIN_2X2_FOUR] = {255, 2, 15, 1},
	[BUILDING_RUIN_2X2_FIVE] = {256, 2, 15, 1},
	[BUILDING_RUIN_2X2_SIX] = {257, 2, 15, 1},
	[BUILDING_RUIN_4X4_ONE] = {258, 4, 15, 1},
	[BUILDING_RUIN_4X4_TWO] = {259, 4, 15, 1},
	[BUILDING_RUIN_4X4_THREE] = {260, 4, 15, 1},
	[BUILDING_CESSPIT_ONE] = {301, 5, 15, 1},
	[BUILDING_CESSPIT_TWO] = {302, 5, 15, 1},
	[BUILDING_CESSPIT_THREE] = {303, 5, 15, 1},
	[BUILDING_CESSPIT_FOUR] = {304, 5, 15, 1},
	[BUILDING_BURNING_STAKE] = {305, 3, 15, 1},
	[BUILDING_GIBBET] = {306, 2, 15, 1},
	[BUILDING_DUNGEON] = {307, 5, 15, 1},
	[BUILDING_STRETCHING_RACK] = {308, 3, 15, 1},
	[BUILDING_FLOGGING_RACK] = {309, 3, 15, 1},
	[BUILDING_CHOPPING_BLOCK] = {310, 3, 15, 1},
	[BUILDING_DUNKING_STOOL] = {311, 5, 15, 1},
	[BUILDING_DOG_CAGE] = {312, 3, 15, 1},
	[BUILDING_STATUE_ONE] = {313, 2, 15, 1},
	[BUILDING_STATUE_TWO] = {314, 2, 15, 1},
	[BUILDING_STATUE_THREE] = {315, 2, 15, 1},
	[BUILDING_STATUE_FOUR] = {316, 2, 15, 1},
	[BUILDING_STATUE_FIVE] = {317, 2, 15, 1},
	[BUILDING_SHRINE_ONE] = {318, 2, 15, 1},
	[BUILDING_SHRINE_TWO] = {319, 2, 15, 1},
	[BUILDING_SHRINE_THREE] = {320, 2, 15, 1},
	[BUILDING_SHRINE_FOUR] = {321, 2, 15, 1},
	[BUILDING_SHRINE_FIVE] = {322, 1, 15, 1},
	[BUILDING_BEEHIVE] = {323, 3, 15, 1},
	[BUILDING_DANCING_BEAR] = {324, 5, 15, 1},
	[BUILDING_POND_SMALL_ONE] = {325, 5, 15, 1},
	[BUILDING_POND_SMALL_TWO] = {326, 5, 15, 1},
	[BUILDING_POND_LARGE_ONE] = {327, 6, 15, 1},
	[BUILDING_POND_LARGE_TWO] = {328, 6, 15, 1},
	[BUILDING_BEAR_CAVE] = {329, 3, 15, 1},
	[BUILDING_WELL] = {330, 3, 15, 1},
	[BUILDING_WATERPOT] = {342, 4, 15, 1},
	[BUILDING_FIRE_BALLISTA_TENT] = {357, 3, 15, 1},
	[BUILDING_FIRE_BALLISTA_TENT_SECOND] = {358, 3, 15, 1},
};
static const u8 conversion_building_special_orientation_values[] = {
	[BUILDING_ORIENTATION_UP] = 4,
	[BUILDING_ORIENTATION_RIGHT] = 6,
	[BUILDING_ORIENTATION_DOWN] = 0,
	[BUILDING_ORIENTATION_LEFT] = 2,
	[BUILDING_ORIENTATION_HORIZONTAL] = 80,
	[BUILDING_ORIENTATION_VERTICAL] = 81,
};
static const u8 conversion_wall_values[] = {
	[WALL_HIGH] = 25,
	[WALL_LOW] = 46,
	[WALL_CRENEL] = 26,
	[WALL_STAIR] = 27,
};
static const u8 conversion_rock_values[][4] = {
	{0, 1, 2, 3},
	{4, 5, 6, 7},
	{8, 9, 10, 11},
	{12, 13, 14, 15},
};
static const struct {
	u8 id;
	u8 variant_field_offset;
} conversion_plant_values[] = {
	[PLANT_TREE_DATE] = {41, 8},
	[PLANT_TREE_COCONUT] = {42, 8},
	[PLANT_TREE_OLIVE] = {40, 8},
	[PLANT_TREE_CHERRY] = {43, 8},
	[PLANT_SHRUB_OLD] = {130, 8},
	[PLANT_SHRUB_THICK] = {135, 8},
	[PLANT_SHRUB_AGAVE] = {155, 60},
	[PLANT_CACTUS_NORMAL] = {153, 60},
	[PLANT_CACTUS_BLOBBY] = {154, 60},
	[PLANT_CACTUS_WIGGLY] = {156, 60},
};
static const u8 conversion_animal_group_values[] = {
	[ANIMAL_GROUP_DEER] = 220,
	[ANIMAL_GROUP_LION] = 221,
	[ANIMAL_GROUP_RABBIT] = 222,
	[ANIMAL_GROUP_CAMEL] = 223,
};

enum {
	OBJECT_LIMIT_BUILDING = 1999,
	OBJECT_LIMIT_PITCH_DITCH = 3999,
	OBJECT_LIMIT_ROCK = 3999,
	OBJECT_LIMIT_PLANT = 1999,
	OBJECT_LIMIT_UNIT = 2499,
};

ConvertedMapData *converted_map_data_init(const Map *map, rng_state_t *rng_state, DynamicString *logs)
{
	ConvertedMapData *converted_map_data = malloc(sizeof(ConvertedMapData));
	if (!converted_map_data)
		goto err;

	if (!(converted_map_data->object_data = batch_injection_data_init()))
		goto err;

	for (int y = 0; y < MAP_SIZE; ++y)
		for (int x = 0; x < MAP_SIZE; ++x) {
			Tile tile = map->tile_matrix[x][y];
			if (tile.section == SECTION_PADDING)
				continue;

			int tile_array_index = coord_conversion_matrix[x][y];

			if (tile.section == SECTION_BORDER) {
				converted_map_data->base_height_array[tile_array_index] = 8;
				converted_map_data->total_height_array[tile_array_index] = 8;
				converted_map_data->surface_array[tile_array_index] = 0;
				converted_map_data->logical_array[tile_array_index] = 16;
			} else {
				converted_map_data->base_height_array[tile_array_index] = tile.height;
				converted_map_data->total_height_array[tile_array_index] = tile.height;
				if (tile.height > 8 && tile.surface == SURFACE_EARTH)
					tile.surface = SURFACE_PLATEAU_MEDIUM;
				converted_map_data->surface_array[tile_array_index] = conversion_surface_values[tile.surface];
				converted_map_data->logical_array[tile_array_index] = conversion_logical_values[tile.surface];
			}
		}

	int building_array_usage = map->building_array.usage;
	int building_skip_counter = 0;
	int building_sub_placement_counter = 0;
	int building_plant_placement_counter = 0;

	for (int i = 0; i < building_array_usage; ++i) {
		BuildingObject object = map->building_array.data[i];

		BuildingType type = object.type;
		int sub_count = conversion_building_values[type].sub_count;

		if (building_sub_placement_counter + sub_count > OBJECT_LIMIT_BUILDING
			|| (type == BUILDING_APPLE_FARM && building_plant_placement_counter + 8 > OBJECT_LIMIT_PLANT)) {
			if (building_sub_placement_counter >= OBJECT_LIMIT_BUILDING) {
				building_skip_counter += building_array_usage - i;
				break;
			}

			++building_skip_counter;
			continue;
		}

		Coords position = object.position;

		if (type == BUILDING_KEEP || type == BUILDING_MANOR_HOUSE || type == BUILDING_STRONGHOLD)
			position.x += 4;
		else if (type == BUILDING_ENGINEER_GUILD || type == BUILDING_TUNNELER_GUILD)
			position.x += position.x + 4 < (MAP_SIZE / 2) - 1 ? 2 : 3;
		else if (type == BUILDING_OIL_SMELTER)
			position.x += 2;

		BuildingOrientation orientation_value;

		if (type == BUILDING_GATE_SMALL || type == BUILDING_GATE_LARGE || type == BUILDING_DRAWBRIDGE) {
			orientation_value = conversion_building_special_orientation_values[object.orientation];
		} else {
			orientation_value = conversion_building_values[type].orientation;
		}

		if (!batch_injection_data_add_call(
			converted_map_data->object_data,
			CNV_THISCALL,
			address_data[ADDR_CNV_BUILDING_AND_PITCH_DITCH_PLACEMENT_FUNCTION].address,
			address_data[ADDR_TILE_STATE_PTR].address,
			6,
			object.owner,
			position.x,
			position.y,
			conversion_building_values[type].id,
			conversion_building_values[type].size,
			orientation_value))
			goto err;

		building_sub_placement_counter += sub_count;
		if (type == BUILDING_APPLE_FARM)
			building_plant_placement_counter += 8;
	}

	int pitch_ditch_array_usage = map->pitch_ditch_array.usage;
	int pitch_ditch_skip_counter = pitch_ditch_array_usage > OBJECT_LIMIT_PITCH_DITCH ? pitch_ditch_array_usage - OBJECT_LIMIT_PITCH_DITCH : 0;

	for (int i = 0; i < min_int(pitch_ditch_array_usage, OBJECT_LIMIT_PITCH_DITCH); ++i) {
		PitchDitchObject object = map->pitch_ditch_array.data[i];

		if (!batch_injection_data_add_call(
			converted_map_data->object_data,
			CNV_THISCALL,
			address_data[ADDR_CNV_BUILDING_AND_PITCH_DITCH_PLACEMENT_FUNCTION].address,
			address_data[ADDR_TILE_STATE_PTR].address,
			6,
			object.owner,
			object.position.x,
			object.position.y,
			99,
			0,
			0))
			goto err;
	}

	for (int i = 0; i < 2; ++i)
		for (usize j = 0; j < map->wall_array.usage; ++j) {
			WallObject object = map->wall_array.data[j];
			WallType type = object.type;

			if (i == 0 && (type == WALL_CRENEL || type == WALL_STAIR))
				continue;
			if (i == 1 && (type == WALL_HIGH || type == WALL_LOW))
				continue;

			Coords start_coords = object.start;
			Coords end_coords = object.end;

			int x_distance = abs(start_coords.x - end_coords.x);
			int y_distance = abs(start_coords.y - end_coords.y);

			int wall_tile_count = 1 + (x_distance > y_distance ? x_distance : y_distance);

			if (!batch_injection_data_add_call(
				converted_map_data->object_data,
				CNV_THISCALL,
				address_data[ADDR_CNV_WALL_PLACEMENT_FUNCTION].address,
				address_data[ADDR_TILE_STATE_PTR].address,
				7,
				object.owner,
				start_coords.x,
				start_coords.y,
				end_coords.x,
				end_coords.y,
				conversion_wall_values[type],
				wall_tile_count))
				goto err;
		}

	int rock_array_usage = map->rock_array.usage;
	int rock_skip_counter = rock_array_usage > OBJECT_LIMIT_ROCK ? rock_array_usage - OBJECT_LIMIT_ROCK : 0;

	for (int i = 0; i < min_int(rock_array_usage, OBJECT_LIMIT_ROCK); ++i) {
		RockObject object = map->rock_array.data[i];

		if (!batch_injection_data_add_write_u32(
			converted_map_data->object_data,
			address_data[ADDR_CNV_ROCK_ORIENTATION_VALUE].address,
			2 * random(rng_state, 0, 3)))
			goto err;

		if (!batch_injection_data_add_call(
			converted_map_data->object_data,
			CNV_THISCALL,
			address_data[ADDR_CNV_ROCK_PLACEMENT_FUNCTION].address,
			address_data[ADDR_TILE_STATE_PTR].address,
			3,
			object.position.x,
			object.position.y,
			conversion_rock_values[object.size - 1][random(rng_state, 0, 3)]))
			goto err;
	}

	int plant_array_usage = map->plant_array.usage;
	int plant_total_count = plant_array_usage + building_plant_placement_counter;
	int plant_skip_counter = plant_total_count > OBJECT_LIMIT_PLANT ? plant_total_count - OBJECT_LIMIT_PLANT : 0;

	for (int i = 0; i < min_int(plant_total_count, OBJECT_LIMIT_PLANT) - building_plant_placement_counter; ++i) {
		PlantObject object = map->plant_array.data[i];
		PlantType type = object.type;

		if (type == PLANT_TREE_APPLE) {
			if (!batch_injection_data_add_write_u8(
				converted_map_data->object_data,
				address_data[ADDR_CNV_APPLE_TREE_STAGE_VALUE].address,
				object.stage))
				goto err;

			if (!batch_injection_data_add_call(
				converted_map_data->object_data,
				CNV_STDCALL,
				address_data[ADDR_CNV_APPLE_TREE_PLACEMENT_FUNCTION].address,
				0,
				3,
				0,
				object.position.x,
				object.position.y))
				goto err;
		} else {
			if (!batch_injection_data_add_write_u8(
				converted_map_data->object_data,
				address_data[ADDR_CNV_PLANT_STAGE_VALUE].address,
				object.stage))
				goto err;

			if (!batch_injection_data_add_call(
				converted_map_data->object_data,
				CNV_THISCALL,
				address_data[ADDR_CNV_PLANT_PLACEMENT_FUNCTION].address,
				address_data[ADDR_TILE_STATE_PTR].address,
				3,
				object.position.x,
				object.position.y,
				conversion_plant_values[type].id))
				goto err;
		}

		if (!batch_injection_data_add_write_u32(
			converted_map_data->object_data,
			address_data[ADDR_CNV_PLANT_ARRAY].address + ((i + 1) * 156) + conversion_plant_values[type].variant_field_offset,
			object.variant))
			goto err;
	}
	if (!batch_injection_data_add_write_u8(converted_map_data->object_data, address_data[ADDR_CNV_APPLE_TREE_STAGE_VALUE].address, 0)
		|| !batch_injection_data_add_write_u8(converted_map_data->object_data, address_data[ADDR_CNV_PLANT_STAGE_VALUE].address, 3))
		goto err;

	int unit_placement_counter = min_int(map->unit_array.usage, OBJECT_LIMIT_UNIT);
	int unit_skip_counter = map->unit_array.usage > OBJECT_LIMIT_UNIT ? map->unit_array.usage - OBJECT_LIMIT_UNIT : 0;

	for (int i = 0; i < unit_placement_counter; ++i) {
		UnitObject object = map->unit_array.data[i];
		Coords position = object.position;

		if (!batch_injection_data_add_call(
			converted_map_data->object_data,
			CNV_THISCALL,
			address_data[ADDR_CNV_UNIT_FUNCTION].address,
			address_data[ADDR_CNV_UNIT_STATE_PTR].address,
			6,
			object.owner,
			object.owner,
			position.x * 8,
			position.y * 8,
			map->tile_matrix[position.x][position.y].height,
			object.type))
			goto err;
	}

	for (usize i = 0; i < map->animal_group_array.usage; ++i) {
		AnimalGroupObject object = map->animal_group_array.data[i];
		int count = object.count;

		if (unit_placement_counter + count > OBJECT_LIMIT_UNIT) {
			unit_skip_counter += count;
			continue;
		}

		AnimalGroupType type = object.type;

		if (type == ANIMAL_GROUP_DEER) {
			if (!batch_injection_data_add_write_u8(converted_map_data->object_data, address_data[ADDR_CNV_ANIMAL_GROUP_DEER_COUNT_VALUE].address, count))
				goto err;
		} else if (type == ANIMAL_GROUP_LION) {
			if (!batch_injection_data_add_write_u8(converted_map_data->object_data, address_data[ADDR_CNV_ANIMAL_GROUP_LION_COUNT_VALUE].address, count))
				goto err;
		} else if (type == ANIMAL_GROUP_RABBIT) {
			if (!batch_injection_data_add_write_u8(converted_map_data->object_data, address_data[ADDR_CNV_ANIMAL_GROUP_RABBIT_COUNT_VALUE].address, count))
				goto err;
		} else if (type == ANIMAL_GROUP_CAMEL) {
			if (!batch_injection_data_add_write_u8(converted_map_data->object_data, address_data[ADDR_CNV_ANIMAL_GROUP_CAMEL_COUNT_VALUE].address, count))
				goto err;
		}

		Coords position = object.position;

		if (!batch_injection_data_add_call(
			converted_map_data->object_data,
			CNV_THISCALL,
			address_data[ADDR_CNV_ANIMAL_GROUP_FUNCTION].address,
			address_data[ADDR_CNV_TRIBE_STATE_PTR].address,
			4,
			conversion_animal_group_values[type],
			position.x,
			position.y,
			map->tile_matrix[position.x][position.y].height))
			goto err;

		unit_placement_counter += count;
	}
	if (!batch_injection_data_add_write_u8(converted_map_data->object_data, address_data[ADDR_CNV_ANIMAL_GROUP_DEER_COUNT_VALUE].address, 9)
		|| !batch_injection_data_add_write_u8(converted_map_data->object_data, address_data[ADDR_CNV_ANIMAL_GROUP_LION_COUNT_VALUE].address, 3)
		|| !batch_injection_data_add_write_u8(converted_map_data->object_data, address_data[ADDR_CNV_ANIMAL_GROUP_RABBIT_COUNT_VALUE].address, 14)
		|| !batch_injection_data_add_write_u8(converted_map_data->object_data, address_data[ADDR_CNV_ANIMAL_GROUP_CAMEL_COUNT_VALUE].address, 2))
		goto err;

	if (!integer_array_add(converted_map_data->object_data->text_addresses, address_data[ADDR_CNV_APPLE_TREE_STAGE_VALUE].address)
		|| !integer_array_add(converted_map_data->object_data->text_addresses, address_data[ADDR_CNV_PLANT_STAGE_VALUE].address)
		|| !integer_array_add(converted_map_data->object_data->text_addresses, address_data[ADDR_CNV_ANIMAL_GROUP_DEER_COUNT_VALUE].address)
		|| !integer_array_add(converted_map_data->object_data->text_addresses, address_data[ADDR_CNV_ANIMAL_GROUP_LION_COUNT_VALUE].address)
		|| !integer_array_add(converted_map_data->object_data->text_addresses, address_data[ADDR_CNV_ANIMAL_GROUP_RABBIT_COUNT_VALUE].address)
		|| !integer_array_add(converted_map_data->object_data->text_addresses, address_data[ADDR_CNV_ANIMAL_GROUP_CAMEL_COUNT_VALUE].address))
		goto err;

	if ((building_skip_counter && !dynamic_string_add(logs, "converted_map_data_init: %d buildings skipped\n", building_skip_counter))
		|| (pitch_ditch_skip_counter && !dynamic_string_add(logs, "converted_map_data_init: %d pitch ditches skipped\n", pitch_ditch_skip_counter))
		|| (rock_skip_counter && !dynamic_string_add(logs, "converted_map_data_init: %d rocks skipped\n", rock_skip_counter))
		|| (plant_skip_counter && !dynamic_string_add(logs, "converted_map_data_init: %d plants skipped\n", plant_skip_counter))
		|| (unit_skip_counter && !dynamic_string_add(logs, "converted_map_data_init: %d units skipped\n", unit_skip_counter)))
		goto err;

	return converted_map_data;

err:
	converted_map_data_free(converted_map_data);
	return nullptr;
}
bool converted_map_data_inject(const ConvertedMapData *converted_map_data, const char *map_name, const char *map_description, bool save_map)
{
	if (!execute_code(CNV_THISCALL, address_data[ADDR_INJ_CLEAR_MAP_FUNCTION].address, address_data[ADDR_TILE_STATE_PTR].address, 0))
		return false;

	if (!write_data(address_data[ADDR_INJ_BASE_HEIGHT_ARRAY].address, converted_map_data->base_height_array, sizeof(converted_map_data->base_height_array))
		|| !write_data(address_data[ADDR_INJ_TOTAL_HEIGHT_ARRAY].address, converted_map_data->total_height_array, sizeof(converted_map_data->total_height_array))
		|| !write_data(address_data[ADDR_INJ_SURFACE_ARRAY].address, converted_map_data->surface_array, sizeof(converted_map_data->surface_array))
		|| !write_data(address_data[ADDR_INJ_LOGICAL_ARRAY].address, converted_map_data->logical_array, sizeof(converted_map_data->logical_array)))
		return false;

	if (!batch_injection_data_inject(converted_map_data->object_data))
		return false;

	if (!execute_code(CNV_THISCALL, address_data[ADDR_INJ_MINIMAP_FIX_FUNCTION_FIRST].address, address_data[ADDR_TILE_STATE_PTR].address, 0)
		|| !execute_code(CNV_THISCALL, address_data[ADDR_INJ_MINIMAP_FIX_FUNCTION_SECOND].address, address_data[ADDR_INJ_MINIMAP_STATE_PTR].address, 2, 0, 100)
		|| !execute_code(CNV_THISCALL, address_data[ADDR_INJ_MINIMAP_FIX_FUNCTION_THIRD].address, address_data[ADDR_INJ_MINIMAP_STATE_PTR].address, 2, 0, 100))
		return false;

	if (!write_data(address_data[ADDR_INJ_MAP_DESCRIPTION_BUFFER].address, map_description, strlen(map_description) + 1))
		return false;

	if (!save_map)
		return true;

	if (!write_data(address_data[ADDR_INJ_TEXT_BUFFER_BASE].address + 836, map_name, strlen(map_name) + 1))
		return false;

	u32 saved_map_type;
	if (!read_u32(address_data[ADDR_INJ_MAP_TYPE_VALUE].address, &saved_map_type)
		|| !write_u32(address_data[ADDR_INJ_MAP_TYPE_VALUE].address, 1))
		return false;
	u32 saved_menu_action;
	if (!read_u32(address_data[ADDR_INJ_MENU_ACTION_VALUE].address, &saved_menu_action)
		|| !write_u32(address_data[ADDR_INJ_MENU_ACTION_VALUE].address, 32))
		return false;
	u32 saved_menu_view;
	if (!read_u32(address_data[ADDR_INJ_MENU_VIEW_VALUE].address, &saved_menu_view)
		|| !write_u32(address_data[ADDR_INJ_MENU_VIEW_VALUE].address, 17))
		return false;

	if (!execute_code(CNV_CDECL, address_data[ADDR_INJ_SAVE_MAP_FUNCTION].address, 0, 1, 0))
		return false;

	if (!write_u32(address_data[ADDR_INJ_MAP_TYPE_VALUE].address, saved_map_type)
		|| !write_u32(address_data[ADDR_INJ_MENU_ACTION_VALUE].address, saved_menu_action)
		|| !write_u32(address_data[ADDR_INJ_MENU_VIEW_VALUE].address, saved_menu_view))
		return false;

	return true;
}
void converted_map_data_free(ConvertedMapData *converted_map_data)
{
	if (!converted_map_data)
		return;

	batch_injection_data_free(converted_map_data->object_data);
	free(converted_map_data);
}
