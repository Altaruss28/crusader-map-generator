#pragma once
#include "utils.h"

bool enable_process_access(void);
void disable_process_access(void);

typedef struct AddressData {
	u32 address;
	bool is_read_type;
	char *aob_str;
} AddressData;
bool aob_scan(AddressData *address_data, u32 entry_count);

bool read_data(u32 address, void *data, u32 size);
bool write_data(u32 address, void *data, u32 size);

bool read_i8(u32 address, i8 *data);
bool read_i16(u32 address, i16 *data);
bool read_i32(u32 address, i32 *data);

bool write_i8(u32 address, i8 data);
bool write_i16(u32 address, i16 data);
bool write_i32(u32 address, i32 data);

bool read_u8(u32 address, u8 *data);
bool read_u16(u32 address, u16 *data);
bool read_u32(u32 address, u32 *data);

bool write_u8(u32 address, u8 data);
bool write_u16(u32 address, u16 data);
bool write_u32(u32 address, u32 data);

bool read_aob(u32 address, u32 byte_count, u8 *buffer);
bool write_aob(u32 address, u32 byte_count, ...);

typedef enum CallingConvetion {
	CNV_STDCALL,
	CNV_THISCALL,
	CNV_CDECL,
} CallingConvetion;
bool execute_code(CallingConvetion calling_convetion, u32 address, u32 this_pointer, u32 arg_count, ...);

typedef enum InjectionLoopEntryType {
	ENTRY_WRITE,
	ENTRY_CALL,
} InjectionLoopEntryType;
typedef struct InjectionLoopArray {
	u8 *data;
	u32 usage;
	u32 capacity;
	u32 entry_count;
	IntegerArray *text_address_array;
} InjectionLoopArray;
InjectionLoopArray *init_injection_loop_array(void);
bool add_write(InjectionLoopArray *arr, u32 address, void *data, u8 size);
bool add_write_i8(InjectionLoopArray *arr, u32 address, i8 data);
bool add_write_i16(InjectionLoopArray *arr, u32 address, i16 data);
bool add_write_i32(InjectionLoopArray *arr, u32 address, i32 data);
bool add_write_u8(InjectionLoopArray *arr, u32 address, u8 data);
bool add_write_u16(InjectionLoopArray *arr, u32 address, u16 data);
bool add_write_u32(InjectionLoopArray *arr, u32 address, u32 data);
bool add_call(InjectionLoopArray *arr, CallingConvetion calling_convetion, u32 address, u32 this_pointer, u32 arg_count, ...);
bool execute_injection_loop(InjectionLoopArray *injection_loop_array);
void free_injection_loop_array(InjectionLoopArray *arr);
