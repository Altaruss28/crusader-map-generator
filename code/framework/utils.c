#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

i32 min_int(i32 a, i32 b)
{
	return a < b ? a : b;
}
i32 max_int(i32 a, i32 b)
{
	return a > b ? a : b;
}

IntegerArray *init_integer_array(void)
{
	IntegerArray *arr = malloc(sizeof(IntegerArray));
	if (!arr) goto err;
	
	arr->data = NULL;
	arr->usage = 0;
	arr->capacity = 8;
	
	arr->data = malloc(arr->capacity * sizeof *arr->data);
	if (!arr->data) goto err;
	
	return arr;
	
err:
	free_integer_array(arr);
	return NULL;
}
bool add_integer(IntegerArray *arr, i32 number)
{
	if (arr->usage == arr->capacity) {
		u32 new_capacity = arr->capacity * 2;
		i32 *new_data = realloc(arr->data, new_capacity * sizeof *arr->data);
		if (!new_data) return false;
		arr->data = new_data;
		arr->capacity = new_capacity;
	}
	arr->data[arr->usage++] = number;
	return true;
}
void remove_integer(IntegerArray *arr, u32 index)
{
	arr->data[index] = arr->data[arr->usage - 1];
	arr->usage--;
}
void shuffle_integer_array(IntegerArray *arr, u32 *rng_state)
{
	for (u32 i = 0; i < arr->usage; i++) {
		u32 r = random(rng_state, i, arr->usage - 1);
		i32 tmp = arr->data[i];
		arr->data[i] = arr->data[r];
		arr->data[r] = tmp;
	}
}
void free_integer_array(IntegerArray *arr)
{
	if (!arr) return;
	
	free(arr->data);
	free(arr);
}

CoordsArray *init_coords_array(void)
{
	CoordsArray *arr = malloc(sizeof(CoordsArray));
	if (!arr) goto err;
	
	arr->data = NULL;
	arr->usage = 0;
	arr->capacity = 8;
	
	arr->data = malloc(arr->capacity * sizeof *arr->data);
	if (!arr->data) goto err;
	
	return arr;
	
err:
	free_coords_array(arr);
	return NULL;
}
bool add_coords(CoordsArray *arr, i32 x, i32 y)
{
	if (arr->usage == arr->capacity) {
		u32 new_capacity = arr->capacity * 2;
		Coords *new_data = realloc(arr->data, new_capacity * sizeof *arr->data);
		if (!new_data) return false;
		arr->data = new_data;
		arr->capacity = new_capacity;
	}
	arr->data[arr->usage++] = (Coords){x, y};
	return true;
}
void remove_coords(CoordsArray *arr, u32 index)
{
	arr->data[index] = arr->data[arr->usage - 1];
	arr->usage--;
}
void shuffle_coords_array(CoordsArray *arr, u32 *rng_state)
{
	for (u32 i = 0; i < arr->usage; i++) {
		u32 r = random(rng_state, i, arr->usage - 1);
		Coords tmp = arr->data[i];
		arr->data[i] = arr->data[r];
		arr->data[r] = tmp;
	}
}
void free_coords_array(CoordsArray *arr)
{
	if (!arr) return;
	
	free(arr->data);
	free(arr);
}

DynamicString *init_dynamic_string(void)
{
	DynamicString *str = malloc(sizeof(DynamicString));
	if (!str) goto err;
	
	str->data = NULL;
	str->usage = 0;
	str->capacity = 8;
	
	str->data = calloc(str->capacity, sizeof *str->data);
	if (!str->data) goto err;
	
	return str;
	
err:
	free_dynamic_string(str);
	return NULL;
}
bool add_to_string(DynamicString *str, char *format, ...)
{
	va_list args;
	va_start(args, format);
	
	va_list args_copy;
	va_copy(args_copy, args);
	u32 new_str_len = vsnprintf(NULL, 0, format, args_copy) + 1;
	va_end(args_copy);
	
	char new_str_buf[new_str_len];
	
	vsnprintf(new_str_buf, new_str_len, format, args);
	
	va_end(args);
	
	while (str->usage + new_str_len > str->capacity) {
		u32 new_capacity = str->capacity * 2;
		char *new_data = realloc(str->data, new_capacity * sizeof *str->data);
		if (!new_data) return false;
		str->data = new_data;
		str->capacity = new_capacity;
	}
	
	memcpy(str->data + str->usage, new_str_buf, new_str_len);
	str->usage += new_str_len - 1;
	
	return true;
}
void free_dynamic_string(DynamicString *str)
{
	if (!str) return;
	
	free(str->data);
	free(str);
}

i32 random(u32 *rng_state, i32 min, i32 max)
{
	u32 n = *rng_state;
	
	n ^= n << 13;
	n ^= n >> 17;
	n ^= n << 5;
	
	*rng_state = n;
	
	return min + (n % (max - min + 1));
}

void fatal(char *format, ...)
{
	va_list args;
	va_start(args, format);
	
	va_list args_copy;
	va_copy(args_copy, args);
	u32 msg_len = vsnprintf(NULL, 0, format, args_copy) + 1;
	va_end(args_copy);
	
	char msg_buf[msg_len];
	
	vsnprintf(msg_buf, msg_len, format, args);
	
	va_end(args);
	
	printf("%s", msg_buf);
	printf("Press Enter to exit...");
	getchar();
	
	exit(EXIT_FAILURE);
}
