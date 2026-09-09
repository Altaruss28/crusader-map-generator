#include "common.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int min_int(int a, int b) {return a < b ? a : b;}
int max_int(int a, int b) {return a > b ? a : b;}

int random(rng_state_t *rng_state, int min, int max)
{
	static_assert(TYPE_MATCH(rng_state_t, u32));
	rng_state_t n = *rng_state;

	n ^= n << 13;
	n ^= n >> 17;
	n ^= n << 5;

	*rng_state = n;

	return min + (n % (max - min + 1));
}

IntegerArray *integer_array_init(void)
{
	IntegerArray *arr = malloc(sizeof(IntegerArray));
	if (!arr)
		goto err;

	arr->usage = 0;
	arr->capacity = 8;

	if (!(arr->data = malloc(arr->capacity * sizeof(*arr->data))))
		goto err;

	return arr;

err:
	integer_array_free(arr);
	return nullptr;
}
bool integer_array_add(IntegerArray *arr, int num)
{
	if (arr->usage == arr->capacity) {
		usize new_capacity = arr->capacity * 2;
		int *new_data = realloc(arr->data, new_capacity * sizeof(*arr->data));
		if (!new_data)
			return false;
		arr->data = new_data;
		arr->capacity = new_capacity;
	}
	arr->data[arr->usage++] = num;
	return true;
}
void integer_array_remove(IntegerArray *arr, usize idx)
{
	arr->data[idx] = arr->data[--arr->usage];
}
void integer_array_shuffle(IntegerArray *arr, rng_state_t *rng_state)
{
	for (usize i = 0; i < arr->usage; ++i) {
		int r = random(rng_state, i, arr->usage - 1);
		int tmp = arr->data[i];
		arr->data[i] = arr->data[r];
		arr->data[r] = tmp;
	}
}
void integer_array_free(IntegerArray *arr)
{
	if (!arr)
		return;

	free(arr->data);
	free(arr);
}

CoordsArray *coords_array_init(void)
{
	CoordsArray *arr = malloc(sizeof(CoordsArray));
	if (!arr)
		goto err;

	arr->usage = 0;
	arr->capacity = 8;

	if (!(arr->data = malloc(arr->capacity * sizeof(*arr->data))))
		goto err;

	return arr;

err:
	coords_array_free(arr);
	return nullptr;
}
bool coords_array_add(CoordsArray *arr, int x, int y)
{
	if (arr->usage == arr->capacity) {
		usize new_capacity = arr->capacity * 2;
		Coords *new_data = realloc(arr->data, new_capacity * sizeof(*arr->data));
		if (!new_data)
			return false;
		arr->data = new_data;
		arr->capacity = new_capacity;
	}
	arr->data[arr->usage++] = (Coords){x, y};
	return true;
}
void coords_array_remove(CoordsArray *arr, usize idx)
{
	arr->data[idx] = arr->data[--arr->usage];
}
void coords_array_shuffle(CoordsArray *arr, rng_state_t *rng_state)
{
	for (usize i = 0; i < arr->usage; ++i) {
		int r = random(rng_state, i, arr->usage - 1);
		Coords tmp = arr->data[i];
		arr->data[i] = arr->data[r];
		arr->data[r] = tmp;
	}
}
void coords_array_free(CoordsArray *arr)
{
	if (!arr)
		return;

	free(arr->data);
	free(arr);
}

DynamicString *dynamic_string_init(void)
{
	DynamicString *str = malloc(sizeof(DynamicString));
	if (!str)
		goto err;

	str->usage = 0;
	str->capacity = 8;

	if (!(str->data = calloc(str->capacity, sizeof(*str->data))))
		goto err;

	return str;

err:
	dynamic_string_free(str);
	return nullptr;
}
bool dynamic_string_add(DynamicString *str, char *format, ...)
{
	va_list args;
	va_start(args, format);

	va_list args_copy;
	va_copy(args_copy, args);
	int new_str_len = vsnprintf(nullptr, 0, format, args_copy) + 1;
	va_end(args_copy);

	char new_str_buf[new_str_len];

	vsnprintf(new_str_buf, new_str_len, format, args);

	va_end(args);

	while (str->usage + new_str_len > str->capacity) {
		usize new_capacity = str->capacity * 2;
		char *new_data = realloc(str->data, new_capacity * sizeof(*str->data));
		if (!new_data)
			return false;
		str->data = new_data;
		str->capacity = new_capacity;
	}

	memcpy(str->data + str->usage, new_str_buf, new_str_len);
	str->usage += new_str_len - 1;

	return true;
}
void dynamic_string_free(DynamicString *str)
{
	if (!str)
		return;

	free(str->data);
	free(str);
}
