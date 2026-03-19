#pragma once
#include "utils.h"
#include "map.h"
#include "config.h"

typedef struct MirrorPoints {
	Coords points[8];
	u8 count;
} MirrorPoints;

typedef void (*GetMirrorPointsFnPtr)(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size);
extern GetMirrorPointsFnPtr get_mirror_points;

void get_mirror_points_none(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size);
void get_mirror_points_x(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size);
void get_mirror_points_y(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size);
void get_mirror_points_quad(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size);
void get_mirror_points_d_x(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size);
void get_mirror_points_d_y(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size);
void get_mirror_points_d_quad(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size);
void get_mirror_points_octa(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size);
void get_mirror_points_2_point(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size);
void get_mirror_points_4_point(MirrorPoints *current_mirror_points, u32 x, u32 y, u32 object_size);

void init_mirror(Config *config);

bool has_mirror_overlap_rectangle(u32 x_origin, u32 y_origin, u32 rectangle_width, u32 rectangle_length);
bool has_mirror_overlap_array(CoordsArray *claimed_tiles);
