#include "mirror.h"

#include <string.h>

#include "common.h"
#include "map.h"

MirrorPointsInitFnPtr mirror_points_init;

constexpr int MIRRORING_SIZE = MAP_SIZE - 2;

MirrorPoints mirror_points_init_none(int x, int y, int)
{
	MirrorPoints mirror_points;

	mirror_points.points[0] = (Coords){.x = x, .y = y};
	mirror_points.count = 1;

	return mirror_points;
}
MirrorPoints mirror_points_init_x(int x, int y, int object_size)
{
	MirrorPoints mirror_points;
	int shift = object_size - 1;

	mirror_points.points[0] = (Coords){.x = x, .y = y};
	mirror_points.points[1] = (Coords){.x = x, .y = MIRRORING_SIZE - y - shift};
	mirror_points.count = 2;

	return mirror_points;
}
MirrorPoints mirror_points_init_y(int x, int y, int object_size)
{
	MirrorPoints mirror_points;
	int shift = object_size - 1;

	mirror_points.points[0] = (Coords){.x = x, .y = y};
	mirror_points.points[1] = (Coords){.x = MIRRORING_SIZE - x - shift, .y = y};
	mirror_points.count = 2;

	return mirror_points;
}
MirrorPoints mirror_points_init_quad(int x, int y, int object_size)
{
	MirrorPoints mirror_points;
	int shift = object_size - 1;

	mirror_points.points[0] = (Coords){.x = x, .y = y};
	mirror_points.points[1] = (Coords){.x = x, .y = MIRRORING_SIZE - y - shift};
	mirror_points.points[2] = (Coords){.x = MIRRORING_SIZE - x - shift, .y = y};
	mirror_points.points[3] = (Coords){.x = MIRRORING_SIZE - x - shift, .y = MIRRORING_SIZE - y - shift};
	mirror_points.count = 4;

	return mirror_points;
}
MirrorPoints mirror_points_init_d_x(int x, int y, int object_size)
{
	MirrorPoints mirror_points;
	int shift = object_size - 1;

	mirror_points.points[0] = (Coords){.x = x, .y = y};
	mirror_points.points[1] = (Coords){.x = MIRRORING_SIZE - y - shift, .y = MIRRORING_SIZE - x - shift};
	mirror_points.count = 2;

	return mirror_points;
}
MirrorPoints mirror_points_init_d_y(int x, int y, int)
{
	MirrorPoints mirror_points;

	mirror_points.points[0] = (Coords){.x = x, .y = y};
	mirror_points.points[1] = (Coords){.x = y, .y = x};
	mirror_points.count = 2;

	return mirror_points;
}
MirrorPoints mirror_points_init_d_quad(int x, int y, int object_size)
{
	MirrorPoints mirror_points;
	int shift = object_size - 1;

	mirror_points.points[0] = (Coords){.x = x, .y = y};
	mirror_points.points[1] = (Coords){.x = MIRRORING_SIZE - y - shift, .y = MIRRORING_SIZE - x - shift};
	mirror_points.points[2] = (Coords){.x = y, .y = x};
	mirror_points.points[3] = (Coords){.x = MIRRORING_SIZE - x - shift, .y = MIRRORING_SIZE - y - shift};
	mirror_points.count = 4;

	return mirror_points;
}
MirrorPoints mirror_points_init_octa(int x, int y, int object_size)
{
	MirrorPoints mirror_points;
	int shift = object_size - 1;

	mirror_points.points[0] = (Coords){.x = x, .y = y};
	mirror_points.points[1] = (Coords){.x = x, .y = MIRRORING_SIZE - y - shift};
	mirror_points.points[2] = (Coords){.x = MIRRORING_SIZE - x - shift, .y = y};
	mirror_points.points[3] = (Coords){.x = MIRRORING_SIZE - y - shift, .y = MIRRORING_SIZE - x - shift};
	mirror_points.points[4] = (Coords){.x = y, .y = x};
	mirror_points.points[5] = (Coords){.x = y, .y = MIRRORING_SIZE - x - shift};
	mirror_points.points[6] = (Coords){.x = MIRRORING_SIZE - x - shift, .y = MIRRORING_SIZE - y - shift};
	mirror_points.points[7] = (Coords){.x = MIRRORING_SIZE - y - shift, .y = x};
	mirror_points.count = 8;

	return mirror_points;
}
MirrorPoints mirror_points_init_2_point(int x, int y, int object_size)
{
	MirrorPoints mirror_points;
	int shift = object_size - 1;

	mirror_points.points[0] = (Coords){.x = x, .y = y};
	mirror_points.points[1] = (Coords){.x = MIRRORING_SIZE - x - shift, .y = MIRRORING_SIZE - y - shift};
	mirror_points.count = 2;

	return mirror_points;
}
MirrorPoints mirror_points_init_4_point(int x, int y, int object_size)
{
	MirrorPoints mirror_points;
	int shift = object_size - 1;

	mirror_points.points[0] = (Coords){.x = x, .y = y};
	mirror_points.points[1] = (Coords){.x = y, .y = MIRRORING_SIZE - x - shift};
	mirror_points.points[2] = (Coords){.x = MIRRORING_SIZE - x - shift, .y = MIRRORING_SIZE - y - shift};
	mirror_points.points[3] = (Coords){.x = MIRRORING_SIZE - y - shift, .y = x};
	mirror_points.count = 4;

	return mirror_points;
}

void mirror_init(const char *mirror_mode)
{
	if (strcmp(mirror_mode, "none") == 0)
		mirror_points_init = mirror_points_init_none;
	else if (strcmp(mirror_mode, "x") == 0)
		mirror_points_init = mirror_points_init_x;
	else if (strcmp(mirror_mode, "y") == 0)
		mirror_points_init = mirror_points_init_y;
	else if (strcmp(mirror_mode, "quad") == 0)
		mirror_points_init = mirror_points_init_quad;
	else if (strcmp(mirror_mode, "d_x") == 0)
		mirror_points_init = mirror_points_init_d_x;
	else if (strcmp(mirror_mode, "d_y") == 0)
		mirror_points_init = mirror_points_init_d_y;
	else if (strcmp(mirror_mode, "d_quad") == 0)
		mirror_points_init = mirror_points_init_d_quad;
	else if (strcmp(mirror_mode, "octa") == 0)
		mirror_points_init = mirror_points_init_octa;
	else if (strcmp(mirror_mode, "2_point") == 0)
		mirror_points_init = mirror_points_init_2_point;
	else if (strcmp(mirror_mode, "4_point") == 0)
		mirror_points_init = mirror_points_init_4_point;
}

bool mirror_check_overlap_rectangle(int x_origin, int y_origin, int rectangle_width, int rectangle_length)
{
	int mp_count = mirror_points_init(0, 0, 1).count;
	if (mp_count == 1)
		return false;

	MirrorPoints mp_tl = mirror_points_init(x_origin, y_origin, 1);
	MirrorPoints mp_br = mirror_points_init(x_origin + rectangle_width - 1, y_origin + rectangle_length - 1, 1);

	typedef struct {
		int l, r, t, b;
	} Rectangle;

	Rectangle rects[mp_count];

	for (int i = 0; i < mp_count; ++i) {
		int x1 = mp_tl.points[i].x;
		int x2 = mp_br.points[i].x;
		int y1 = mp_tl.points[i].y;
		int y2 = mp_br.points[i].y;

		rects[i].l = min_int(x1, x2);
		rects[i].r = max_int(x1, x2);
		rects[i].t = min_int(y1, y2);
		rects[i].b = max_int(y1, y2);
	}

	Rectangle zrect = rects[0];

	for (int i = 1; i < mp_count; ++i)
		if (zrect.l <= rects[i].r
			&& zrect.r >= rects[i].l
			&& zrect.t <= rects[i].b
			&& zrect.b >= rects[i].t)
			return true;

	return false;
}
bool mirror_check_overlap_array(const CoordsArray *claimed_tiles)
{
	if (mirror_points_init(0, 0, 1).count == 1)
		return false;

	map_flag_matrix_t check_matrix[MAP_FLAG_MATRIX_WORD_COUNT] = {};

	for (usize i = 0; i < claimed_tiles->usage; ++i) {
		int x = claimed_tiles->data[i].x;
		int y = claimed_tiles->data[i].y;
		if (map_flag_matrix_test(check_matrix, x, y))
			return true;
		map_flag_matrix_set(check_matrix, x, y, true);
	}

	return false;
}
