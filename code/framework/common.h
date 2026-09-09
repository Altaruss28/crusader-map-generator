#pragma once

#include <stddef.h>
#include <stdint.h>

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))
#define TYPE_MATCH(T1, T2) _Generic((T1 *)0, T2 *: true, default: false)

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

static_assert(TYPE_MATCH(size_t, uintptr_t));
typedef intptr_t isize;
typedef uintptr_t usize;

typedef struct {int min, max;} Range;
typedef struct {int x, y;} Coords;

int min_int(int a, int b);
int max_int(int a, int b);

typedef u32 rng_state_t;
int random(rng_state_t *rng_state, int min, int max);

typedef struct {
	int *data;
	usize usage;
	usize capacity;
} IntegerArray;
IntegerArray *integer_array_init(void);
bool integer_array_add(IntegerArray *arr, int num);
void integer_array_remove(IntegerArray *arr, usize idx);
void integer_array_shuffle(IntegerArray *arr, rng_state_t *rng_state);
void integer_array_free(IntegerArray *arr);

typedef struct {
	Coords *data;
	usize usage;
	usize capacity;
} CoordsArray;
CoordsArray *coords_array_init(void);
bool coords_array_add(CoordsArray *arr, int x, int y);
void coords_array_remove(CoordsArray *arr, usize idx);
void coords_array_shuffle(CoordsArray *arr, rng_state_t *rng_state);
void coords_array_free(CoordsArray *arr);

typedef struct {
	char *data;
	usize usage;
	usize capacity;
} DynamicString;
DynamicString *dynamic_string_init(void);
bool dynamic_string_add(DynamicString *str, char *format, ...);
void dynamic_string_free(DynamicString *str);
