#pragma once

#include "common.h"

typedef struct {
	Coords points[8];
	int count;
} MirrorPoints;

typedef MirrorPoints (*MirrorPointsInitFnPtr)(int x, int y, int object_size);
extern MirrorPointsInitFnPtr mirror_points_init;

MirrorPoints mirror_points_init_none(int x, int y, int);
MirrorPoints mirror_points_init_x(int x, int y, int object_size);
MirrorPoints mirror_points_init_y(int x, int y, int object_size);
MirrorPoints mirror_points_init_quad(int x, int y, int object_size);
MirrorPoints mirror_points_init_d_x(int x, int y, int object_size);
MirrorPoints mirror_points_init_d_y(int x, int y, int);
MirrorPoints mirror_points_init_d_quad(int x, int y, int object_size);
MirrorPoints mirror_points_init_octa(int x, int y, int object_size);
MirrorPoints mirror_points_init_2_point(int x, int y, int object_size);
MirrorPoints mirror_points_init_4_point(int x, int y, int object_size);

void mirror_init(const char *mirror_mode);

bool mirror_check_overlap_rectangle(int x_origin, int y_origin, int rectangle_width, int rectangle_length);
bool mirror_check_overlap_array(const CoordsArray *claimed_tiles);
