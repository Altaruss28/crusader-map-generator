#include "injection.h"
#include "utils.h"
#include "map.h"
#include "process_access.h"
#include <stdlib.h>
#include <string.h>

typedef enum AddressType {
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
	ADDR_TR_BUILDING_AND_PITCH_DITCH_PLACEMENT_FUNCTION,
	ADDR_TR_WALL_PLACEMENT_FUNCTION,
	ADDR_TR_ROCK_PLACEMENT_FUNCTION,
	ADDR_TR_ROCK_ORIENTATION_VALUE,
	ADDR_TR_PLANT_PLACEMENT_FUNCTION,
	ADDR_TR_PLANT_STAGE_VALUE,
	ADDR_TR_APPLE_TREE_PLACEMENT_FUNCTION,
	ADDR_TR_APPLE_TREE_STAGE_VALUE,
	ADDR_TR_PLANT_ARRAY,
	ADDR_TR_UNIT_FUNCTION,
	ADDR_TR_UNIT_STATE_PTR,
	ADDR_TR_ANIMAL_FUNCTION,
	ADDR_TR_TRIBE_STATE_PTR,
	ADDR_TR_ANIMAL_DEER_COUNT_VALUE,
	ADDR_TR_ANIMAL_LION_COUNT_VALUE,
	ADDR_TR_ANIMAL_RABBIT_COUNT_VALUE,
	ADDR_TR_ANIMAL_CAMEL_COUNT_VALUE,
	ADDR_IN_BASE_HEIGHT_ARRAY,
	ADDR_IN_TOTAL_HEIGHT_ARRAY,
	ADDR_IN_SURFACE_ARRAY,
	ADDR_IN_LOGICAL_ARRAY,
	ADDR_IN_CLEAR_MAP_FUNCTION,
	ADDR_IN_MINIMAP_STATE_PTR,
	ADDR_IN_MINIMAP_FIX_FUNCTION_FIRST,
	ADDR_IN_MINIMAP_FIX_FUNCTION_SECOND,
	ADDR_IN_MINIMAP_FIX_FUNCTION_THIRD,
	ADDR_IN_TEXT_BUFFER_BASE,
	ADDR_IN_MAP_DESCRIPTION_BUFFER,
	ADDR_IN_SAVE_MAP_FUNCTION,
	ADDR_IN_MAP_TYPE_VALUE,
	ADDR_IN_MENU_ACTION_VALUE,
	ADDR_IN_MENU_VIEW_VALUE,
} AddressType;
static AddressData address_data[] = {
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
	[ADDR_TR_BUILDING_AND_PITCH_DITCH_PLACEMENT_FUNCTION] = {0, false, "83 EC 08 53 55 8B 6C 24 18 56 8B F1"},
	[ADDR_TR_WALL_PLACEMENT_FUNCTION] = {0, false, "83 EC 28 8B 54"},
	[ADDR_TR_ROCK_PLACEMENT_FUNCTION] = {0, false, "51 8B 44 24 08 53 55"},
	[ADDR_TR_ROCK_ORIENTATION_VALUE] = {0, true, "?? ?? ?? ?? 00 00 00 00 A3 ?? ?? ?? ?? A1 ?? ?? ?? ?? 83"},
	[ADDR_TR_PLANT_PLACEMENT_FUNCTION] = {0, false, "83 EC 0C 53 8B 5C 24 14 81 FB 8F 01 00 00 57"},
	[ADDR_TR_PLANT_STAGE_VALUE] = {0, false, "?? 6A 00 0F B7 C0 6A"},
	[ADDR_TR_APPLE_TREE_PLACEMENT_FUNCTION] = {0, false, "8B 44 24 0C 8B 4C 24 08 6A ?? 6A 00"},
	[ADDR_TR_APPLE_TREE_STAGE_VALUE] = {0, false, "?? 6A 00 6A 00 6A 01 6A 0F"},
	[ADDR_TR_PLANT_ARRAY] = {0, true, "?? ?? ?? ?? 68 9C 00 00 00 51 B9"},
	[ADDR_TR_UNIT_FUNCTION] = {0, false, "53 8B D9 B9 01"},
	[ADDR_TR_UNIT_STATE_PTR] = {0, true, "?? ?? ?? ?? E8 ?? ?? ?? ?? 69 F6 90 04 00 00 0F BF B6"},
	[ADDR_TR_ANIMAL_FUNCTION] = {0, false, "83 EC 08 55 57 33"},
	[ADDR_TR_TRIBE_STATE_PTR] = {0, true, "?? ?? ?? ?? 89 7C 24 20 E8 ?? ?? ?? ?? 8B E8"},
	[ADDR_TR_ANIMAL_DEER_COUNT_VALUE] = {0, false, "?? 89 44 24 10 33 DB 39"},
	[ADDR_TR_ANIMAL_LION_COUNT_VALUE] = {0, false, "?? EB AF 81 FB DE"},
	[ADDR_TR_ANIMAL_RABBIT_COUNT_VALUE] = {0, false, "?? E9 70 FF FF FF 81 FB DF"},
	[ADDR_TR_ANIMAL_CAMEL_COUNT_VALUE] = {0, false, "?? E9 35 FF FF FF 81 FB"},
	[ADDR_IN_BASE_HEIGHT_ARRAY] = {0, true, "?? ?? ?? ?? 3B D1 0F 8D 02"},
	[ADDR_IN_TOTAL_HEIGHT_ARRAY] = {0, true, "?? ?? ?? ?? 0F 8D DC 03"},
	[ADDR_IN_SURFACE_ARRAY] = {0, true, "?? ?? ?? ?? 75 11 83 7C 24 18"},
	[ADDR_IN_LOGICAL_ARRAY] = {0, true, "?? ?? ?? ?? 00 00 00 10 89 74 24 18"},
	[ADDR_IN_CLEAR_MAP_FUNCTION] = {0, false, "53 55 56 8B F1 57 33 FF 89 BE"},
	[ADDR_IN_MINIMAP_STATE_PTR] = {0, true, "?? ?? ?? ?? E8 ?? ?? ?? ?? 83 6C 24 10 01 68"},
	[ADDR_IN_MINIMAP_FIX_FUNCTION_FIRST] = {0, false, "55 8B EC 81 EC 18 01 00 00 53"},
	[ADDR_IN_MINIMAP_FIX_FUNCTION_SECOND] = {0, false, "81 3D ?? ?? ?? ?? C8 00 00 00 8B 44 24 08 8B 54 24 04 50 52 7E 0E 6A 01 6A 02 6A 04 E8 ?? ?? ?? ?? C2 08 00 6A 02 6A 04 6A 04 E8 ?? ?? ?? ?? C2 08 00 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 55"},
	[ADDR_IN_MINIMAP_FIX_FUNCTION_THIRD] = {0, false, "81 3D ?? ?? ?? ?? C8 00 00 00 8B 44 24 08 8B 54 24 04 50 52 7E 0E 6A 01 6A 02 6A 04 E8 ?? ?? ?? ?? C2 08 00 6A 02 6A 04 6A 04 E8 ?? ?? ?? ?? C2 08 00 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 81"},
	[ADDR_IN_TEXT_BUFFER_BASE] = {0, true, "?? ?? ?? ?? E8 ?? ?? ?? ?? 6A 06 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? BE"},
	[ADDR_IN_MAP_DESCRIPTION_BUFFER] = {0, true, "?? ?? ?? ?? EB 25 A1 ?? ?? ?? ?? 3B C6"},
	[ADDR_IN_SAVE_MAP_FUNCTION] = {0, false, "81 EC F4 03 00 00 A1 ?? ?? ?? ?? 33 C4 89 84 24 F0 03 00 00 A1 ?? ?? ?? ?? 53"},
	[ADDR_IN_MAP_TYPE_VALUE] = {0, true, "?? ?? ?? ?? 74 46 57 33 FF 89"},
	[ADDR_IN_MENU_ACTION_VALUE] = {0, true, "?? ?? ?? ?? 09 00 00 00 E8 ?? ?? ?? ?? 6A 00"},
	[ADDR_IN_MENU_VIEW_VALUE] = {0, true, "?? ?? ?? ?? 10 75 37 83 3D"},
};

bool init_addresses(void)
{
	return aob_scan(address_data, sizeof(address_data) / sizeof(address_data[0]));
}

static const struct {
	u32 *address;
	u32 byte_count;
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
static u32 code_edit_entry_count = sizeof(code_edit_entries) / sizeof(code_edit_entries[0]);
static u8 *code_edit_original_byte_buffer = NULL;

bool enable_code_edits(void)
{
	u32 total_byte_count = 0;
	
	for (u32 entry_index = 0; entry_index < code_edit_entry_count; entry_index++) {
		total_byte_count += code_edit_entries[entry_index].byte_count;
	}
	
	code_edit_original_byte_buffer = malloc(total_byte_count);
	if (!code_edit_original_byte_buffer) goto err;
	
	u32 byte_store_offset = 0;
	
	for (u32 entry_index = 0; entry_index < code_edit_entry_count; entry_index++) {
		
		u32 address = *code_edit_entries[entry_index].address;
		u32 byte_count = code_edit_entries[entry_index].byte_count;
		u8 *bytes = code_edit_entries[entry_index].bytes;
		
		for (u32 byte_index = 0; byte_index < byte_count; byte_index++) {
			
			if (!read_u8(address + byte_index, code_edit_original_byte_buffer + byte_store_offset)) goto err;
			byte_store_offset++;
			
			if (!write_u8(address + byte_index, bytes[byte_index])) goto err;
			
		}
		
	}
	
	return true;
	
err:
	free(code_edit_original_byte_buffer);
	code_edit_original_byte_buffer = NULL;
	return false;
}
bool disable_code_edits(void)
{
	if (!code_edit_original_byte_buffer) return true;
	
	bool ret = false;
	
	u32 byte_restore_offset = 0;
	
	for (u32 entry_index = 0; entry_index < code_edit_entry_count; entry_index++) {
		
		u32 address = *code_edit_entries[entry_index].address;
		u32 byte_count = code_edit_entries[entry_index].byte_count;
		u8 *bytes = code_edit_original_byte_buffer + byte_restore_offset;
		
		for (u32 byte_index = 0; byte_index < byte_count; byte_index++) {
			if (!write_u8(address + byte_index, bytes[byte_index])) goto err;
		}
		
		byte_restore_offset += byte_count;
		
	}
	
	ret = true;
	
err:
	free(code_edit_original_byte_buffer);
	code_edit_original_byte_buffer = NULL;
	return ret;
}

enum { 
	OBJECT_LIMIT_BUILDING = 1999,
	OBJECT_LIMIT_PITCH_DITCH = 3999,
	OBJECT_LIMIT_ROCK = 3999,
	OBJECT_LIMIT_PLANT = 1999,
	OBJECT_LIMIT_UNIT = 2499,
};

static const u8 translation_surface_values[] = {
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
static const u32 translation_logical_values[] = {
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
} translation_building_values[] = {
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
static const u8 translation_wall_values[] = {
	[WALL_HIGH] = 25,
	[WALL_LOW] = 46,
	[WALL_CRENEL] = 26,
	[WALL_STAIR] = 27,
};
static const u8 translation_rock_values[][4] = {
	{0, 1, 2, 3},
	{4, 5, 6, 7},
	{8, 9, 10, 11},
	{12, 13, 14, 15},
};
static const u8 translation_plant_values[] = {
	[PLANT_TREE_DATE] = 41,
	[PLANT_TREE_COCONUT] = 42,
	[PLANT_TREE_OLIVE] = 40,
	[PLANT_TREE_CHERRY] = 43,
	[PLANT_SHRUB_OLD] = 130,
	[PLANT_SHRUB_THICK] = 135,
	[PLANT_SHRUB_AGAVE] = 155,
	[PLANT_CACTUS_NORMAL] = 153,
	[PLANT_CACTUS_BLOBBY] = 154,
	[PLANT_CACTUS_WIGGLY] = 156,
};
static const u8 translation_animal_values[] = {
	[ANIMAL_DEER] = 220,
	[ANIMAL_LION] = 221,
	[ANIMAL_RABBIT] = 222,
	[ANIMAL_CAMEL] = 223,
};

static u32 translation_matrix[MAP_SIZE][MAP_SIZE];

void init_translation_matrix(void)
{
	u32 tile_array_index = 0;
	
	for (u32 y = 0; y < MAP_SIZE; y++) {
		for (u32 x = 0; x < MAP_SIZE; x++) {
			if (x + y >= 199 && x <= y + 200 && y <= x + 200 && x + y <= 599) {
				translation_matrix[x][y] = tile_array_index++;
			}
		}
	}
}

InjectionData *translate_map(Map *map, u32 *rng_state, DynamicString *logs)
{
	InjectionData *injection_data = malloc(sizeof(InjectionData));
	if (!injection_data) goto err;
	
	injection_data->object_data = init_injection_loop_array();
	if (!injection_data->object_data) goto err;
	
	for (u32 y = 0; y < MAP_SIZE; y++) {
		for (u32 x = 0; x < MAP_SIZE; x++) {
			
			Tile current_tile = map->tile_matrix[x][y];
			
			if (current_tile.section == SECTION_PADDING) continue;
			
			u32 tile_array_index = translation_matrix[x][y];
			
			if (current_tile.section == SECTION_BORDER) {
				
				injection_data->base_height_array[tile_array_index] = 8;
				injection_data->total_height_array[tile_array_index] = 8;
				injection_data->surface_array[tile_array_index] = 0;
				injection_data->logical_array[tile_array_index] = 16;
				
			} else {
				
				injection_data->base_height_array[tile_array_index] = current_tile.height;
				injection_data->total_height_array[tile_array_index] = current_tile.height;
				
				if (current_tile.height > 8 && current_tile.surface == SURFACE_EARTH) {
					current_tile.surface = SURFACE_PLATEAU_MEDIUM;
				}
				
				injection_data->surface_array[tile_array_index] = translation_surface_values[current_tile.surface];
				injection_data->logical_array[tile_array_index] = translation_logical_values[current_tile.surface];
				
			}
			
		}
	}
	
	u32 buildings_placed = 0;
	u32 buildings_skipped = 0;
	u32 pitch_ditches_placed = 0;
	u32 pitch_ditches_skipped = 0;
	u32 rocks_placed = 0;
	u32 rocks_skipped = 0;
	u32 plants_placed = 0;
	u32 plants_skipped = 0;
	u32 units_placed = 0;
	u32 units_skipped = 0;
	
	for (u32 building_index = 0; building_index < map->building_array.usage; building_index++) {
		
		BuildingObject building_object = map->building_array.data[building_index];
		BuildingType building_type = building_object.type;
		
		u32 sub_building_count = translation_building_values[building_type].sub_count;
		
		if (buildings_placed + sub_building_count > OBJECT_LIMIT_BUILDING
		|| (building_type == BUILDING_APPLE_FARM && plants_placed + 8 > OBJECT_LIMIT_PLANT)) {
			
			buildings_skipped++;
			continue;
			
		}
		
		u32 x = building_object.position.x;
		u32 y = building_object.position.y;
		
		if (building_type == BUILDING_KEEP || building_type == BUILDING_MANOR_HOUSE) {
			x += 4;
		} else if (building_type == BUILDING_STRONGHOLD) {
			x += 4;
		} else if (building_type == BUILDING_ENGINEER_GUILD || building_type == BUILDING_TUNNELER_GUILD) {
			x += x + 4 < (MAP_SIZE / 2) - 1 ? 2 : 3;
		} else if (building_type == BUILDING_OIL_SMELTER) {
			x += 2;
		}
		
		BuildingOrientation building_orientation_value = 0;
		
		if (building_type == BUILDING_GATE_SMALL || building_type == BUILDING_GATE_LARGE) {
			
			if (building_object.orientation == BUILDING_ORIENTATION_HORIZONTAL) {
				building_orientation_value = 80;
			} else if (building_object.orientation == BUILDING_ORIENTATION_VERTICAL) {
				building_orientation_value = 81;
			}
			
		} else if (building_type == BUILDING_DRAWBRIDGE) {
			
			if (building_object.orientation == BUILDING_ORIENTATION_UP) {
				building_orientation_value = 4;
			} else if (building_object.orientation == BUILDING_ORIENTATION_RIGHT) {
				building_orientation_value = 6;
			} else if (building_object.orientation == BUILDING_ORIENTATION_DOWN) {
				building_orientation_value = 0;
			} else if (building_object.orientation == BUILDING_ORIENTATION_LEFT) {
				building_orientation_value = 2;
			}
			
		} else {
			
			building_orientation_value = translation_building_values[building_type].orientation;
			
		}
		
		if (!add_call(
			injection_data->object_data,
			CNV_THISCALL,
			address_data[ADDR_TR_BUILDING_AND_PITCH_DITCH_PLACEMENT_FUNCTION].address,
			address_data[ADDR_TILE_STATE_PTR].address,
			6,
			building_object.owner,
			x,
			y,
			translation_building_values[building_type].id,
			translation_building_values[building_type].size,
			building_orientation_value
		)) goto err;
		
		buildings_placed += sub_building_count;
		if (building_type == BUILDING_APPLE_FARM) plants_placed += 8;
		
	}
	
	for (u32 pitch_ditch_index = 0; pitch_ditch_index < map->pitch_ditch_array.usage; pitch_ditch_index++) {
		
		if (pitch_ditches_placed + 1 > OBJECT_LIMIT_PITCH_DITCH) {
			pitch_ditches_skipped = map->pitch_ditch_array.usage - pitch_ditches_placed;
			break;
		}
		
		PitchDitchObject pitch_ditch_object = map->pitch_ditch_array.data[pitch_ditch_index];
		
		if (!add_call(
			injection_data->object_data,
			CNV_THISCALL,
			address_data[ADDR_TR_BUILDING_AND_PITCH_DITCH_PLACEMENT_FUNCTION].address,
			address_data[ADDR_TILE_STATE_PTR].address,
			6,
			pitch_ditch_object.owner,
			pitch_ditch_object.position.x,
			pitch_ditch_object.position.y,
			99,
			0,
			0
		)) goto err;
		
		pitch_ditches_placed++;
		
	}
	
	for (u32 run_index = 0; run_index < 2; run_index++) {
		for (u32 wall_index = 0; wall_index < map->wall_array.usage; wall_index++) {
			
			WallObject wall_object = map->wall_array.data[wall_index];
			WallType wall_type = wall_object.type;
			
			if (run_index == 0 && (wall_type == WALL_CRENEL || wall_type == WALL_STAIR)) continue;
			if (run_index == 1 && (wall_type == WALL_HIGH || wall_type == WALL_LOW)) continue;
			
			u32 x_start = wall_object.start_coords.x;
			u32 y_start = wall_object.start_coords.y;
			u32 x_end = wall_object.end_coords.x;
			u32 y_end = wall_object.end_coords.y;
			
			u32 x_distance = abs((i32)x_start - (i32)x_end);
			u32 y_distance = abs((i32)y_start - (i32)y_end);
			
			u32 wall_tile_count = 1 + (x_distance > y_distance ? x_distance : y_distance);
			
			if (!add_call(
				injection_data->object_data,
				CNV_THISCALL,
				address_data[ADDR_TR_WALL_PLACEMENT_FUNCTION].address,
				address_data[ADDR_TILE_STATE_PTR].address,
				7,
				wall_object.owner,
				x_start,
				y_start,
				x_end,
				y_end,
				translation_wall_values[wall_type],
				wall_tile_count
			)) goto err;
			
		}
	}
	
	for (u32 rock_index = 0; rock_index < map->rock_array.usage; rock_index++) {
		
		if (rocks_placed + 1 > OBJECT_LIMIT_ROCK) {
			rocks_skipped = map->rock_array.usage - rocks_placed;
			break;
		}
		
		RockObject rock_object = map->rock_array.data[rock_index];
		
		int rock_type_value = translation_rock_values[rock_object.size - 1][random(rng_state, 0, 3)];
		
		if (!add_write_u32(injection_data->object_data, address_data[ADDR_TR_ROCK_ORIENTATION_VALUE].address, 2 * random(rng_state, 0, 3))) goto err;
		
		if (!add_call(
			injection_data->object_data,
			CNV_THISCALL,
			address_data[ADDR_TR_ROCK_PLACEMENT_FUNCTION].address,
			address_data[ADDR_TILE_STATE_PTR].address,
			3,
			rock_object.position.x,
			rock_object.position.y,
			rock_type_value
		)) goto err;
		
		rocks_placed++;
		
	}
	
	for (u32 plant_index = 0; plant_index < map->plant_array.usage; plant_index++) {
		
		if (plants_placed + 1 > OBJECT_LIMIT_PLANT) {
			plants_skipped = map->plant_array.usage - plants_placed;
			break;
		}
		
		PlantObject plant_object = map->plant_array.data[plant_index];
		PlantType plant_type = plant_object.type;
		
		if (plant_type == PLANT_TREE_APPLE) {
			
			if (!add_write_u8(injection_data->object_data, address_data[ADDR_TR_APPLE_TREE_STAGE_VALUE].address, plant_object.stage)) goto err;
			
			if (!add_call(
				injection_data->object_data,
				CNV_STDCALL,
				address_data[ADDR_TR_APPLE_TREE_PLACEMENT_FUNCTION].address,
				0x0,
				3,
				0,
				plant_object.position.x,
				plant_object.position.y
			)) goto err;
			
		} else {
			
			u8 plant_type_value = translation_plant_values[plant_type];
			
			if (!add_write_u8(injection_data->object_data, address_data[ADDR_TR_PLANT_STAGE_VALUE].address, plant_object.stage)) goto err;
			
			if (!add_call(
				injection_data->object_data,
				CNV_THISCALL,
				address_data[ADDR_TR_PLANT_PLACEMENT_FUNCTION].address,
				address_data[ADDR_TILE_STATE_PTR].address,
				3,
				plant_object.position.x,
				plant_object.position.y,
				plant_type_value
			)) goto err;
			
		}
		
		u32 target_offset;
		
		if (plant_type == PLANT_SHRUB_AGAVE
		|| plant_type == PLANT_CACTUS_NORMAL
		|| plant_type == PLANT_CACTUS_BLOBBY
		|| plant_type == PLANT_CACTUS_WIGGLY) {
			target_offset = 0x3C;
		} else {
			target_offset = 0x8;
		}
		
		if (!add_write_u32(injection_data->object_data, address_data[ADDR_TR_PLANT_ARRAY].address + ((plant_index + 1) * 156) + target_offset, plant_object.variant)) goto err;
		
		plants_placed++;
		
	}
	if (!add_write_u8(injection_data->object_data, address_data[ADDR_TR_APPLE_TREE_STAGE_VALUE].address, 0)) goto err;
	if (!add_write_u8(injection_data->object_data, address_data[ADDR_TR_PLANT_STAGE_VALUE].address, 3)) goto err;
	
	for (u32 unit_index = 0; unit_index < map->unit_array.usage; unit_index++) {
		
		if (units_placed + 1 > OBJECT_LIMIT_UNIT) {
			units_skipped = map->unit_array.usage - units_placed;
			break;
		}
		
		UnitObject unit_object = map->unit_array.data[unit_index];
		
		u32 x = unit_object.position.x;
		u32 y = unit_object.position.y;
		
		u32 x_micro = x * 8;
		u32 y_micro = y * 8;
		
		u8 terrain_height = map->tile_matrix[x][y].height;
		
		if (!add_call(
			injection_data->object_data,
			CNV_THISCALL,
			address_data[ADDR_TR_UNIT_FUNCTION].address,
			address_data[ADDR_TR_UNIT_STATE_PTR].address,
			6,
			unit_object.owner,
			unit_object.owner,
			x_micro,
			y_micro,
			terrain_height,
			unit_object.type
		)) goto err;
		
		units_placed++;
		
	}
	
	for (u32 animal_index = 0; animal_index < map->animal_array.usage; animal_index++) {
		
		AnimalObject animal_object = map->animal_array.data[animal_index];
		
		if (units_placed + animal_object.count > OBJECT_LIMIT_UNIT) {
			units_skipped += animal_object.count;
			continue;
		}
		
		AnimalType animal_type = animal_object.type;
		u8 animal_type_value = translation_animal_values[animal_type];
		
		if (animal_type == ANIMAL_DEER) {
			if (!add_write_u8(injection_data->object_data, address_data[ADDR_TR_ANIMAL_DEER_COUNT_VALUE].address, animal_object.count)) goto err;
		} else if (animal_type == ANIMAL_LION) {
			if (!add_write_u8(injection_data->object_data, address_data[ADDR_TR_ANIMAL_LION_COUNT_VALUE].address, animal_object.count)) goto err;
		} else if (animal_type == ANIMAL_RABBIT) {
			if (!add_write_u8(injection_data->object_data, address_data[ADDR_TR_ANIMAL_RABBIT_COUNT_VALUE].address, animal_object.count)) goto err;
		} else if (animal_type == ANIMAL_CAMEL) {
			if (!add_write_u8(injection_data->object_data, address_data[ADDR_TR_ANIMAL_CAMEL_COUNT_VALUE].address, animal_object.count)) goto err;
		}
		
		u32 x = animal_object.position.x;
		u32 y = animal_object.position.y;
		
		u8 terrain_height = map->tile_matrix[x][y].height;
		
		if (!add_call(
			injection_data->object_data,
			CNV_THISCALL,
			address_data[ADDR_TR_ANIMAL_FUNCTION].address,
			address_data[ADDR_TR_TRIBE_STATE_PTR].address,
			4,
			animal_type_value,
			x,
			y,
			terrain_height
		)) goto err;
		
	}
	if (!add_write_u8(injection_data->object_data, address_data[ADDR_TR_ANIMAL_DEER_COUNT_VALUE].address, 9)) goto err;
	if (!add_write_u8(injection_data->object_data, address_data[ADDR_TR_ANIMAL_LION_COUNT_VALUE].address, 3)) goto err;
	if (!add_write_u8(injection_data->object_data, address_data[ADDR_TR_ANIMAL_RABBIT_COUNT_VALUE].address, 14)) goto err;
	if (!add_write_u8(injection_data->object_data, address_data[ADDR_TR_ANIMAL_CAMEL_COUNT_VALUE].address, 2)) goto err;
	
	if (!add_integer(injection_data->object_data->text_address_array, address_data[ADDR_TR_APPLE_TREE_STAGE_VALUE].address)) goto err;
	if (!add_integer(injection_data->object_data->text_address_array, address_data[ADDR_TR_PLANT_STAGE_VALUE].address)) goto err;
	if (!add_integer(injection_data->object_data->text_address_array, address_data[ADDR_TR_ANIMAL_DEER_COUNT_VALUE].address)) goto err;
	if (!add_integer(injection_data->object_data->text_address_array, address_data[ADDR_TR_ANIMAL_LION_COUNT_VALUE].address)) goto err;
	if (!add_integer(injection_data->object_data->text_address_array, address_data[ADDR_TR_ANIMAL_RABBIT_COUNT_VALUE].address)) goto err;
	if (!add_integer(injection_data->object_data->text_address_array, address_data[ADDR_TR_ANIMAL_CAMEL_COUNT_VALUE].address)) goto err;
	
	if (buildings_skipped) if (!add_to_string(logs, "translate_map: %d buildings over limit skipped\n", buildings_skipped)) goto err;
	if (pitch_ditches_skipped) if (!add_to_string(logs, "translate_map: %d pitch ditches over limit skipped\n", pitch_ditches_skipped)) goto err;
	if (rocks_skipped) if (!add_to_string(logs, "translate_map: %d rocks over limit skipped\n", rocks_skipped)) goto err;
	if (plants_skipped) if (!add_to_string(logs, "translate_map: %d plants over limit skipped\n", plants_skipped)) goto err;
	if (units_skipped) if (!add_to_string(logs, "translate_map: %d units over limit skipped\n", units_skipped)) goto err;
	
	return injection_data;
	
err:
	free_injection_data(injection_data);
	return NULL;
}
bool inject_map(InjectionData *injection_data, char *map_name, char *map_description, bool save_map)
{
	if (!execute_code(CNV_THISCALL, address_data[ADDR_IN_CLEAR_MAP_FUNCTION].address, address_data[ADDR_TILE_STATE_PTR].address, 0)) return false;
	
	if (!write_data(address_data[ADDR_IN_BASE_HEIGHT_ARRAY].address, injection_data->base_height_array, sizeof(injection_data->base_height_array))) return false;
	if (!write_data(address_data[ADDR_IN_TOTAL_HEIGHT_ARRAY].address, injection_data->total_height_array, sizeof(injection_data->total_height_array))) return false;
	if (!write_data(address_data[ADDR_IN_SURFACE_ARRAY].address, injection_data->surface_array, sizeof(injection_data->surface_array))) return false;
	if (!write_data(address_data[ADDR_IN_LOGICAL_ARRAY].address, injection_data->logical_array, sizeof(injection_data->logical_array))) return false;
	
	if (!execute_injection_loop(injection_data->object_data)) return false;
	
	if (!execute_code(CNV_THISCALL, address_data[ADDR_IN_MINIMAP_FIX_FUNCTION_FIRST].address, address_data[ADDR_TILE_STATE_PTR].address, 0)) return false;
	if (!execute_code(CNV_THISCALL, address_data[ADDR_IN_MINIMAP_FIX_FUNCTION_SECOND].address, address_data[ADDR_IN_MINIMAP_STATE_PTR].address, 2, 0, 100)) return false;
	if (!execute_code(CNV_THISCALL, address_data[ADDR_IN_MINIMAP_FIX_FUNCTION_THIRD].address, address_data[ADDR_IN_MINIMAP_STATE_PTR].address, 2, 0, 100)) return false;
	
	if (!write_data(address_data[ADDR_IN_MAP_DESCRIPTION_BUFFER].address, map_description, strlen(map_description) + 1)) return false;
	
	if (!save_map) return true;
	
	if (!write_data(address_data[ADDR_IN_TEXT_BUFFER_BASE].address + 0x344, map_name, strlen(map_name) + 1)) return false;
	
	u32 saved_map_type;
	if (!read_u32(address_data[ADDR_IN_MAP_TYPE_VALUE].address, &saved_map_type)) return false;
	if (!write_u32(address_data[ADDR_IN_MAP_TYPE_VALUE].address, 1)) return false;
	
	u32 saved_menu_action;
	if (!read_u32(address_data[ADDR_IN_MENU_ACTION_VALUE].address, &saved_menu_action)) return false;
	if (!write_u32(address_data[ADDR_IN_MENU_ACTION_VALUE].address, 32)) return false;
	u32 saved_menu_view;
	if (!read_u32(address_data[ADDR_IN_MENU_VIEW_VALUE].address, &saved_menu_view)) return false;
	if (!write_u32(address_data[ADDR_IN_MENU_VIEW_VALUE].address, 17)) return false;
	
	if (!execute_code(CNV_CDECL, address_data[ADDR_IN_SAVE_MAP_FUNCTION].address, 0x0, 1, 0)) return false;
	
	if (!write_u32(address_data[ADDR_IN_MAP_TYPE_VALUE].address, saved_map_type)) return false;
	if (!write_u32(address_data[ADDR_IN_MENU_ACTION_VALUE].address, saved_menu_action)) return false;
	if (!write_u32(address_data[ADDR_IN_MENU_VIEW_VALUE].address, saved_menu_view)) return false;
	
	return true;
}
void free_injection_data(InjectionData *injection_data)
{
	if (!injection_data) return;
	
	free_injection_loop_array(injection_data->object_data);
	
	free(injection_data);
}
