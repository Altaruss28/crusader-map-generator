#pragma once
#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef intptr_t isize;
typedef uintptr_t usize;

#define bool _Bool
#define false 0
#define true 1

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef struct Range {
	i32 min;
	i32 max;
} Range;

i32 min_int(i32 a, i32 b);
i32 max_int(i32 a, i32 b);

typedef struct IntegerArray {
	i32 *data;
	u32 usage;
	u32 capacity;
} IntegerArray;
IntegerArray *init_integer_array(void);
bool add_integer(IntegerArray *arr, i32 number);
void remove_integer(IntegerArray *arr, u32 index);
void shuffle_integer_array(IntegerArray *arr, u32 *rng_state);
void free_integer_array(IntegerArray *arr);

typedef struct Coords {
	i32 x;
	i32 y;
} Coords;
typedef struct CoordsArray {
	Coords *data;
	u32 usage;
	u32 capacity;
} CoordsArray;
CoordsArray *init_coords_array(void);
bool add_coords(CoordsArray *arr, i32 x, i32 y);
void remove_coords(CoordsArray *arr, u32 index);
void shuffle_coords_array(CoordsArray *arr, u32 *rng_state);
void free_coords_array(CoordsArray *arr);

typedef struct DynamicString {
	char *data;
	u32 usage;
	u32 capacity;
} DynamicString;
DynamicString *init_dynamic_string(void);
bool add_to_string(DynamicString *str, char *format, ...);
void free_dynamic_string(DynamicString *str);

i32 random(u32 *rng_state, i32 min, i32 max);

void fatal(char *format, ...);
