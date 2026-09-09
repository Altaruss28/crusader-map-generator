#pragma once

#include "common.h"

bool process_access_enable(void);
void process_access_disable(void);

typedef struct {
	u32 address;
	bool is_read_type;
	char *aob_str;
} AobScanEntry;
bool aob_scan(AobScanEntry *entries, int entry_count);

bool read_data(u32 address, void *data, usize size);
bool write_data(u32 address, const void *data, usize size);

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

bool read_aob(u32 address, int byte_count, u8 *buffer);
bool write_aob(u32 address, int byte_count, ...);

typedef enum {
	CNV_STDCALL,
	CNV_THISCALL,
	CNV_CDECL,
} CallingConvetion;
bool execute_code(CallingConvetion calling_convetion, u32 address, u32 this_ptr, int arg_count, ...);

typedef enum {
	BATCH_INJECTION_ENTRY_WRITE,
	BATCH_INJECTION_ENTRY_CALL,
} BatchInjectionEntryType;
typedef struct {
	u8 *data;
	usize usage;
	usize capacity;
	usize entry_count;
	IntegerArray *text_addresses;
} BatchInjectionData;
BatchInjectionData *batch_injection_data_init(void);
bool batch_injection_data_add_write(BatchInjectionData *bid, u32 address, void *data, int size);
bool batch_injection_data_add_write_i8(BatchInjectionData *bid, u32 address, i8 data);
bool batch_injection_data_add_write_i16(BatchInjectionData *bid, u32 address, i16 data);
bool batch_injection_data_add_write_i32(BatchInjectionData *bid, u32 address, i32 data);
bool batch_injection_data_add_write_u8(BatchInjectionData *bid, u32 address, u8 data);
bool batch_injection_data_add_write_u16(BatchInjectionData *bid, u32 address, u16 data);
bool batch_injection_data_add_write_u32(BatchInjectionData *bid, u32 address, u32 data);
bool batch_injection_data_add_call(BatchInjectionData *bid, CallingConvetion calling_convetion, u32 address, u32 this_ptr, int arg_count, ...);
bool batch_injection_data_inject(const BatchInjectionData *bid);
void batch_injection_data_free(BatchInjectionData *bid);
