#include "mirror.h"
#include "utils.h"
#include "map.h"
#include "config.h"
#include <string.h>

GetMirrorPointsFnPtr get_mirror_points;

enum {
	MIRRORING_SIZE = MAP_SIZE - 2,
};

void get_mirror_points_none(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size)
{
	(void)object_size;
	current_mirror_points->points[0] = (Coords){ .x = x, .y = y };
	current_mirror_points->count = 1;
}
void get_mirror_points_x(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size)
{
	u32 shift = object_size - 1;
	current_mirror_points->points[0] = (Coords){ .x = x, .y = y };
	current_mirror_points->points[1] = (Coords){ .x = x, .y = MIRRORING_SIZE - y - shift };
	current_mirror_points->count = 2;
}
void get_mirror_points_y(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size)
{
	u32 shift = object_size - 1;
	current_mirror_points->points[0] = (Coords){ .x = x, .y = y };
	current_mirror_points->points[1] = (Coords){ .x = MIRRORING_SIZE - x - shift, .y = y };
	current_mirror_points->count = 2;
}
void get_mirror_points_quad(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size)
{
	u32 shift = object_size - 1;
	current_mirror_points->points[0] = (Coords){ .x = x, .y = y };
	current_mirror_points->points[1] = (Coords){ .x = x, .y = MIRRORING_SIZE - y - shift };
	current_mirror_points->points[2] = (Coords){ .x = MIRRORING_SIZE - x - shift, .y = y };
	current_mirror_points->points[3] = (Coords){ .x = MIRRORING_SIZE - x - shift, .y = MIRRORING_SIZE - y - shift };
	current_mirror_points->count = 4;
}
void get_mirror_points_d_x(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size)
{
	u32 shift = object_size - 1;
	current_mirror_points->points[0] = (Coords){ .x = x, .y = y };
	current_mirror_points->points[1] = (Coords){ .x = MIRRORING_SIZE - y - shift, .y = MIRRORING_SIZE - x - shift };
	current_mirror_points->count = 2;
}
void get_mirror_points_d_y(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size)
{
	(void)object_size;
	current_mirror_points->points[0] = (Coords){ .x = x, .y = y };
	current_mirror_points->points[1] = (Coords){ .x = y, .y = x };
	current_mirror_points->count = 2;
}
void get_mirror_points_d_quad(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size)
{
	u32 shift = object_size - 1;
	current_mirror_points->points[0] = (Coords){ .x = x, .y = y };
	current_mirror_points->points[1] = (Coords){ .x = MIRRORING_SIZE - y - shift, .y = MIRRORING_SIZE - x - shift };
	current_mirror_points->points[2] = (Coords){ .x = y, .y = x };
	current_mirror_points->points[3] = (Coords){ .x = MIRRORING_SIZE - x - shift, .y = MIRRORING_SIZE - y - shift };
	current_mirror_points->count = 4;
}
void get_mirror_points_octa(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size)
{
	u32 shift = object_size - 1;
	current_mirror_points->points[0] = (Coords){ .x = x, .y = y };
	current_mirror_points->points[1] = (Coords){ .x = x, .y = MIRRORING_SIZE - y - shift };
	current_mirror_points->points[2] = (Coords){ .x = MIRRORING_SIZE - x - shift, .y = y };
	current_mirror_points->points[3] = (Coords){ .x = MIRRORING_SIZE - y - shift, .y = MIRRORING_SIZE - x - shift };
	current_mirror_points->points[4] = (Coords){ .x = y, .y = x };
	current_mirror_points->points[5] = (Coords){ .x = y, .y = MIRRORING_SIZE - x - shift };
	current_mirror_points->points[6] = (Coords){ .x = MIRRORING_SIZE - x - shift, .y = MIRRORING_SIZE - y - shift };
	current_mirror_points->points[7] = (Coords){ .x = MIRRORING_SIZE - y - shift, .y = x };
	current_mirror_points->count = 8;
}
void get_mirror_points_2_point(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size)
{
	u32 shift = object_size - 1;
	current_mirror_points->points[0] = (Coords){ .x = x, .y = y };
	current_mirror_points->points[1] = (Coords){ .x = MIRRORING_SIZE - x - shift, .y = MIRRORING_SIZE - y - shift };
	current_mirror_points->count = 2;
}
void get_mirror_points_4_point(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size)
{
	u32 shift = object_size - 1;
	current_mirror_points->points[0] = (Coords){ .x = x, .y = y };
	current_mirror_points->points[1] = (Coords){ .x = y, .y = MIRRORING_SIZE - x - shift };
	current_mirror_points->points[2] = (Coords){ .x = MIRRORING_SIZE - x - shift, .y = MIRRORING_SIZE - y - shift };
	current_mirror_points->points[3] = (Coords){ .x = MIRRORING_SIZE - y - shift, .y = x };
	current_mirror_points->count = 4;
}

void init_mirror(Config *config)
{
	char *mirror_mode = config->mirror_mode;
	
	if (strcmp(mirror_mode, "none") == 0) {
		get_mirror_points = get_mirror_points_none;
		config->mirror_point_count = 1;
	} else if (strcmp(mirror_mode, "x") == 0) {
		get_mirror_points = get_mirror_points_x;
		config->mirror_point_count = 2;
	} else if (strcmp(mirror_mode, "y") == 0) {
		get_mirror_points = get_mirror_points_y;
		config->mirror_point_count = 2;
	} else if (strcmp(mirror_mode, "quad") == 0) {
		get_mirror_points = get_mirror_points_quad;
		config->mirror_point_count = 4;
	} else if (strcmp(mirror_mode, "d_x") == 0) {
		get_mirror_points = get_mirror_points_d_x;
		config->mirror_point_count = 2;
	} else if (strcmp(mirror_mode, "d_y") == 0) {
		get_mirror_points = get_mirror_points_d_y;
		config->mirror_point_count = 2;
	} else if (strcmp(mirror_mode, "d_quad") == 0) {
		get_mirror_points = get_mirror_points_d_quad;
		config->mirror_point_count = 4;
	} else if (strcmp(mirror_mode, "octa") == 0) {
		get_mirror_points = get_mirror_points_octa;
		config->mirror_point_count = 8;
	} else if (strcmp(mirror_mode, "2_point") == 0) {
		get_mirror_points = get_mirror_points_2_point;
		config->mirror_point_count = 2;
	} else if (strcmp(mirror_mode, "4_point") == 0) {
		get_mirror_points = get_mirror_points_4_point;
		config->mirror_point_count = 4;
	}
}

bool has_mirror_overlap_rectangle(u32 x_origin, u32 y_origin, u32 rectangle_width, u32 rectangle_length)
{
	u32 check_matrix[FLAG_MATRIX_WORD_COUNT];
	clear_all_flags(check_matrix);
	
	for (u32 x = x_origin; x < x_origin + rectangle_width; x++) {
		for (u32 y = y_origin; y < y_origin + rectangle_length; y++) {
			
			if (test_flag(check_matrix, x, y)) return true;
			set_flag(check_matrix, x, y, true);
			
		}
	}
	
	return false;
}
bool has_mirror_overlap_array(CoordsArray *claimed_tiles)
{
	u32 check_matrix[FLAG_MATRIX_WORD_COUNT];
	clear_all_flags(check_matrix);
	
	for (u32 tile_index = 0; tile_index < claimed_tiles->usage; tile_index++) {
		
		u32 x = claimed_tiles->data[tile_index].x;
		u32 y = claimed_tiles->data[tile_index].y;
		
		if (test_flag(check_matrix, x, y)) return true;
		set_flag(check_matrix, x, y, true);
		
	}
	
	return false;
}
