#include "process_access.h"
#include "utils.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

static HANDLE process_handle;

bool enable_process_access(void)
{
	PROCESSENTRY32 process_entry;
	process_entry.dwSize = sizeof(process_entry);
	
	HANDLE snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot_handle == INVALID_HANDLE_VALUE) goto err;
	
	if (!Process32First(snapshot_handle, &process_entry)) goto err;
	
	do {
		if (_tcsicmp(process_entry.szExeFile, "Stronghold Crusader.exe") == 0
		|| _tcsicmp(process_entry.szExeFile, "Stronghold_Crusader_Extreme.exe") == 0) {
			process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_entry.th32ProcessID);
			if (!process_handle) goto err;
			break;
		}
	} while (Process32Next(snapshot_handle, &process_entry));
	
	if (!process_handle) goto err;
	
	CloseHandle(snapshot_handle);
	
	return true;
	
err:
	if (snapshot_handle) CloseHandle(snapshot_handle);
	return false;
}
void disable_process_access(void)
{
	if (!process_handle) return;
	
	CloseHandle(process_handle);
	process_handle = NULL;
}

u8 byte_frequencies[256] = {
	255,
	243,
	229,
	219,
	240,
	194,
	180,
	193,
	242,
	135,
	145,
	133,
	236,
	151,
	134,
	241,
	231,
	107,
	126,
	103,
	203,
	234,
	87,
	65,
	190,
	85,
	147,
	110,
	167,
	46,
	66,
	89,
	213,
	90,
	69,
	93,
	237,
	101,
	28,
	33,
	140,
	62,
	21,
	125,
	121,
	78,
	83,
	14,
	192,
	99,
	92,
	233,
	122,
	88,
	42,
	50,
	157,
	144,
	48,
	175,
	115,
	109,
	49,
	86,
	209,
	181,
	132,
	164,
	215,
	245,
	182,
	161,
	206,
	146,
	67,
	72,
	166,
	232,
	143,
	123,
	238,
	208,
	131,
	212,
	155,
	223,
	225,
	217,
	119,
	159,
	53,
	154,
	124,
	198,
	200,
	201,
	139,
	176,
	57,
	148,
	178,
	224,
	197,
	105,
	221,
	188,
	226,
	38,
	187,
	112,
	173,
	185,
	218,
	165,
	210,
	230,
	251,
	249,
	228,
	204,
	179,
	137,
	26,
	25,
	116,
	171,
	113,
	80,
	199,
	150,
	43,
	244,
	202,
	247,
	96,
	52,
	162,
	248,
	94,
	253,
	68,
	246,
	41,
	10,
	149,
	13,
	16,
	35,
	81,
	95,
	11,
	2,
	73,
	17,
	0,
	8,
	63,
	24,
	23,
	39,
	120,
	138,
	15,
	30,
	74,
	117,
	4,
	12,
	79,
	7,
	3,
	31,
	76,
	1,
	5,
	47,
	142,
	22,
	9,
	18,
	100,
	91,
	64,
	118,
	189,
	108,
	153,
	71,
	98,
	55,
	97,
	75,
	239,
	186,
	207,
	191,
	170,
	102,
	172,
	211,
	174,
	195,
	70,
	104,
	252,
	54,
	184,
	127,
	168,
	77,
	128,
	45,
	106,
	6,
	158,
	61,
	160,
	111,
	32,
	141,
	130,
	56,
	20,
	36,
	183,
	60,
	44,
	34,
	163,
	58,
	29,
	19,
	250,
	216,
	40,
	214,
	235,
	27,
	37,
	51,
	220,
	129,
	84,
	82,
	169,
	59,
	205,
	152,
	222,
	156,
	114,
	136,
	227,
	177,
	196,
	254,
};
bool aob_scan(AddressData *address_data, u32 entry_count)
{
	bool ret = false;
	
	u8 *section_buffer = NULL;
	u32 section_buffer_size = 0;
	
	struct {
		u32 first_byte_offset;
		u32 last_byte_offset;
		u32 check_byte_count;
		u32 check_byte_offset;
		bool found;
	} entry_data[entry_count];
	
	struct {
		u32 entry_count;
		u32 entry_data_index_offset;
	} entry_buckets[256] = {0};
	
	u32 entry_data_indices[entry_count];
	
	u32 total_check_byte_count = 0;
	
	u32 total_byte_counts[entry_count];
	u32 value_byte_counts[entry_count];
	
	for (u32 entry_index = 0; entry_index < entry_count; entry_index++) {
		
		char *token = address_data[entry_index].aob_str;
		
		u32 total_byte_count = (strlen(token) + 1) / 3;
		u32 value_byte_count = 0;
		
		for (u32 byte_index = 0; byte_index < total_byte_count; byte_index++) {
			
			if (token[0] != '?' && token[1] != '?') value_byte_count++;
			token += 3;
			
		}
		
		total_byte_counts[entry_index] = total_byte_count;
		value_byte_counts[entry_index] = value_byte_count;
		
		total_check_byte_count += value_byte_count - 1;
		
	}
	
	struct {
		i32 offset;
		u8 value;
	} check_bytes[total_check_byte_count];
	
	u32 check_byte_load_index = 0;
	
	u32 anchor_byte_values[entry_count];
	
	for (u32 entry_index = 0; entry_index < entry_count; entry_index++) {
		
		u32 value_byte_count = value_byte_counts[entry_index];
		
		typedef struct {
			u32 index;
			u8 value;
			u8 frequency;
		} ValueByte;
		
		ValueByte value_bytes[value_byte_count];
		
		char *token = address_data[entry_index].aob_str;
		u32 value_byte_index = 0;
		
		for (u32 byte_index = 0; byte_index < total_byte_counts[entry_index]; byte_index++) {
			
			if (token[0] != '?' && token[1] != '?') {
				
				value_bytes[value_byte_index].index = byte_index;
				
				char *end_ptr;
				u8 value = (u8)strtol(token, &end_ptr, 16);
				
				value_bytes[value_byte_index].value = value;
				value_bytes[value_byte_index].frequency = byte_frequencies[value];
				
				value_byte_index++;
				
			}
			
			token += 3;
			
		}
		
		for (u32 i = 1; i < value_byte_count; i++) {
			
			ValueByte byte = value_bytes[i];
			
			i32 w = i - 1;
			
			while (w >= 0 && value_bytes[w].frequency > byte.frequency) {
				value_bytes[w + 1] = value_bytes[w];
				w--;
			}
			
			value_bytes[w + 1] = byte;
			
		}
		
		u32 anchor_index = value_bytes[0].index;
		u8 anchor_value = value_bytes[0].value;
		
		anchor_byte_values[entry_index] = anchor_value;
		entry_buckets[anchor_value].entry_count++;
		
		entry_data[entry_index].first_byte_offset = anchor_index;
		entry_data[entry_index].last_byte_offset = total_byte_counts[entry_index] - anchor_index - 1;
		
		entry_data[entry_index].check_byte_count = value_byte_count - 1;
		entry_data[entry_index].check_byte_offset = check_byte_load_index;
		
		entry_data[entry_index].found = false;
		
		for (u32 byte_index = 1; byte_index < value_byte_count; byte_index++) {
			check_bytes[check_byte_load_index].offset = value_bytes[byte_index].index - anchor_index;
			check_bytes[check_byte_load_index].value = value_bytes[byte_index].value;
			check_byte_load_index++;
		}
		
	}
	
	u32 bucket_entry_data_load_index = 0;
	u32 bucket_entry_data_load_index_offsets[256];
	
	for (u32 bucket_index = 0; bucket_index < 256; bucket_index++) {
		entry_buckets[bucket_index].entry_data_index_offset = bucket_entry_data_load_index;
		bucket_entry_data_load_index_offsets[bucket_index] = bucket_entry_data_load_index;
		bucket_entry_data_load_index += entry_buckets[bucket_index].entry_count;
	}
	
	for (u32 bucket_index = 0; bucket_index < 256; bucket_index++) {
		for (u32 entry_index = 0; entry_index < entry_count; entry_index++) {
			if (anchor_byte_values[entry_index] != bucket_index) continue;
			entry_data_indices[bucket_entry_data_load_index_offsets[bucket_index]++] = entry_index;
		}
	}
	
	MEMORY_BASIC_INFORMATION mbi;
	usize scan_address = 0;
	
	while (VirtualQueryEx(process_handle, (LPCVOID)scan_address, &mbi, sizeof(mbi))) {
		
		if (mbi.State == MEM_COMMIT
		&& (mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY))
		&& !(mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))) {
			
			usize section_start = (usize)mbi.BaseAddress;
			usize section_size = mbi.RegionSize;
			
			if (section_buffer_size < section_size) {
				free(section_buffer);
				if (!(section_buffer = malloc(section_size))) goto out;
				section_buffer_size = section_size;
			}
			
			SIZE_T read_byte_count;
			if (!ReadProcessMemory(process_handle, mbi.BaseAddress, section_buffer, section_size, &read_byte_count)) goto out;
			if (read_byte_count == 0) continue;
			
			for (u32 section_byte_index = 0; section_byte_index < read_byte_count; section_byte_index++) {
				
				u8 *section_byte_ptr = section_buffer + section_byte_index;
				u8 section_byte_value = *section_byte_ptr;
				
				u32 bucket_entry_count = entry_buckets[section_byte_value].entry_count;
				
				if (bucket_entry_count == 0) continue;
				
				u32 bucket_entry_data_index_offset = entry_buckets[section_byte_value].entry_data_index_offset;
				
				for (u32 bucket_entry_index = 0; bucket_entry_index < bucket_entry_count; bucket_entry_index++) {
					
					u32 entry_index = entry_data_indices[bucket_entry_data_index_offset + bucket_entry_index];
					
					u32 first_byte_offset = entry_data[entry_index].first_byte_offset;
					u32 last_byte_offset = entry_data[entry_index].last_byte_offset;
					
					if (section_byte_index < first_byte_offset 
					|| section_byte_index + last_byte_offset >= read_byte_count) continue;
					
					u32 entry_check_byte_count = entry_data[entry_index].check_byte_count;
					u32 entry_check_byte_offset = entry_data[entry_index].check_byte_offset;
					
					bool matched = true;
					
					for (u32 entry_check_byte_index = 0; entry_check_byte_index < entry_check_byte_count; entry_check_byte_index++) {
						
						u32 check_byte_offset = entry_check_byte_offset + entry_check_byte_index;
						
						i32 offset = check_bytes[check_byte_offset].offset;
						u8 check_byte_value = check_bytes[check_byte_offset].value;
						
						u8 offset_byte_value = section_byte_ptr[offset];
						
						if (check_byte_value == offset_byte_value) continue;
						
						matched = false;
						break;
						
					}
					
					if (!matched) continue;
					
					if (entry_data[entry_index].found) goto out;
					
					if (address_data[entry_index].is_read_type) {
						memcpy(&address_data[entry_index].address, section_buffer + section_byte_index - first_byte_offset, sizeof(uint32_t));
					} else {
						address_data[entry_index].address = section_start + section_byte_index - first_byte_offset;
					}
					
					entry_data[entry_index].found = true;
					
				}
				
			}
			
		}
		
		scan_address = (usize)mbi.BaseAddress + mbi.RegionSize;
		
	}
	
	for (u32 entry_index = 0; entry_index < entry_count; entry_index++) {
		if (!entry_data[entry_index].found) goto out;
	}
	
	ret = true;
	
out:
	free(section_buffer);
	return ret;
}

bool read_data(u32 address, void *data, u32 size)
{
	DWORD saved_protection_state;
	if (!VirtualProtectEx(process_handle, (LPVOID)(usize)address, (SIZE_T)size, PAGE_EXECUTE_READWRITE, &saved_protection_state)) return false;
	
	SIZE_T read_byte_count;
	if (!ReadProcessMemory(process_handle, (LPVOID)(usize)address, data, (SIZE_T)size, &read_byte_count) || read_byte_count != size) return false;
	
	DWORD tmp;
	if (!VirtualProtectEx(process_handle, (LPVOID)(usize)address, (SIZE_T)size, saved_protection_state, &tmp)) return false;
	
	return true;
}
bool write_data(u32 address, void *data, u32 size)
{
	DWORD saved_protection_state;
	if (!VirtualProtectEx(process_handle, (LPVOID)(usize)address, (SIZE_T)size, PAGE_EXECUTE_READWRITE, &saved_protection_state)) return false;
	
	SIZE_T written_byte_count;
	if (!WriteProcessMemory(process_handle, (LPVOID)(usize)address, data, (SIZE_T)size, &written_byte_count) || written_byte_count != size) return false;
	
	if (!FlushInstructionCache(process_handle, (LPVOID)(usize)address, (SIZE_T)size)) return false;
	
	DWORD tmp;
	if (!VirtualProtectEx(process_handle, (LPVOID)(usize)address, (SIZE_T)size, saved_protection_state, &tmp)) return false;
	
	return true;
}

bool read_i8(u32 address, i8 *data) {return read_data(address, data, sizeof(data));}
bool read_i16(u32 address, i16 *data) {return read_data(address, data, sizeof(data));}
bool read_i32(u32 address, i32 *data) {return read_data(address, data, sizeof(data));}

bool write_i8(u32 address, i8 data) {return write_data(address, &data, sizeof(data));}
bool write_i16(u32 address, i16 data) {return write_data(address, &data, sizeof(data));}
bool write_i32(u32 address, i32 data) {return write_data(address, &data, sizeof(data));}

bool read_u8(u32 address, u8 *data) {return read_data(address, data, sizeof(data));}
bool read_u16(u32 address, u16 *data) {return read_data(address, data, sizeof(data));}
bool read_u32(u32 address, u32 *data) {return read_data(address, data, sizeof(data));}

bool write_u8(u32 address, u8 data) {return write_data(address, &data, sizeof(data));}
bool write_u16(u32 address, u16 data) {return write_data(address, &data, sizeof(data));}
bool write_u32(u32 address, u32 data) {return write_data(address, &data, sizeof(data));}

bool read_aob(u32 address, u32 byte_count, u8 *buffer)
{
	return read_data(address, buffer, byte_count);
}
bool write_aob(u32 address, u32 byte_count, ...)
{
	va_list(bytes);
	va_start(bytes, byte_count);
	
	u8 byte_array[byte_count];
	
	for (u32 i = 0; i < byte_count; i++) {
		byte_array[i] = (u8)va_arg(bytes, int);
	}
	
	va_end(bytes);
	
	return write_data(address, byte_array, byte_count);
}

bool execute_code(CallingConvetion calling_convetion, u32 address, u32 this_pointer, u32 arg_count, ...)
{
	va_list args;
	va_start(args, arg_count);
	
	i32 arg_arr[arg_count];
	
	for (u32 i = 0; i < arg_count; i++) {
		arg_arr[i] = (i32)va_arg(args, int);
	}
	
	va_end(args);
	
	BYTE stub_buffer[128 + (arg_count * 5)];
	BYTE *p = stub_buffer;
	
	for (u32 i = 0; i < arg_count; i++) {
		*p++ = 0x68;
		memcpy(p, &arg_arr[arg_count - 1 - i], 4);
		p += 4;
	}
	
	if (calling_convetion == CNV_THISCALL) {
		*p++ = 0xB9;
		memcpy(p, &this_pointer, 4);
		p += 4;
	}
	
	*p++ = 0xB8;
	memcpy(p, &address, 4);
	p += 4;
	
	*p++ = 0xFF;
	*p++ = 0xD0;
	
	if (calling_convetion == CNV_CDECL) {
		*p++ = 0x81;
		*p++ = 0xC4;
		u32 arg_size = arg_count * 4;
		memcpy(p, &arg_size, 4);
		p += 4;
	}
	
	*p++ = 0xC3;
	
	LPVOID remote_mem = NULL;
	HANDLE thread_handle = NULL;
	
	SIZE_T stub_size = p - stub_buffer;
	
	remote_mem = VirtualAllocEx(process_handle, NULL, stub_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!remote_mem) goto err;
	
	if (!WriteProcessMemory(process_handle, remote_mem, stub_buffer, stub_size, NULL)) goto err;
	
	thread_handle = CreateRemoteThread(process_handle, NULL, 0, (LPTHREAD_START_ROUTINE)remote_mem, NULL, 0, NULL);
	if (!thread_handle) goto err;
	
	WaitForSingleObject(thread_handle, INFINITE);
	
	CloseHandle(thread_handle);
	VirtualFreeEx(process_handle, remote_mem, 0, MEM_RELEASE);
	
	return true;
	
err:
	if (thread_handle) CloseHandle(thread_handle);
	if (remote_mem) VirtualFreeEx(process_handle, remote_mem, 0, MEM_RELEASE);
	return false;
}

InjectionLoopArray *init_injection_loop_array(void)
{
	InjectionLoopArray *arr = malloc(sizeof(InjectionLoopArray));
	if (!arr) goto err;
	
	arr->data = NULL;
	arr->usage = 0;
	arr->capacity = 256;
	arr->entry_count = 0;
	arr->text_address_array = NULL;
	
	arr->data = malloc(arr->capacity);
	if (!arr->data) goto err;
	
	arr->text_address_array = init_integer_array();
	if (!arr->text_address_array) goto err;
	
	return arr;
	
err:
	free_injection_loop_array(arr);
	return NULL;
}
bool add_write(InjectionLoopArray *arr, u32 address, void *data, u8 size)
{
	while (arr->usage + 6 + size > arr->capacity) {
		u32 new_capacity = arr->capacity * 2;
		u8 *new_data = realloc(arr->data, new_capacity);
		if (!new_data) return false;
		arr->data = new_data;
		arr->capacity = new_capacity;
	}
	
	arr->data[arr->usage++] = ENTRY_WRITE;
	
	memcpy(arr->data + arr->usage, &address, 4);
	arr->usage += 4;
	
	arr->data[arr->usage++] = size;
	
	memcpy(arr->data + arr->usage, data, size);
	arr->usage += size;
	
	arr->entry_count++;
	
	return true;
}
bool add_write_i8(InjectionLoopArray *arr, u32 address, i8 data) {return add_write(arr, address, &data, sizeof(data));}
bool add_write_i16(InjectionLoopArray *arr, u32 address, i16 data) {return add_write(arr, address, &data, sizeof(data));}
bool add_write_i32(InjectionLoopArray *arr, u32 address, i32 data) {return add_write(arr, address, &data, sizeof(data));}
bool add_write_u8(InjectionLoopArray *arr, u32 address, u8 data) {return add_write(arr, address, &data, sizeof(data));}
bool add_write_u16(InjectionLoopArray *arr, u32 address, u16 data) {return add_write(arr, address, &data, sizeof(data));}
bool add_write_u32(InjectionLoopArray *arr, u32 address, u32 data) {return add_write(arr, address, &data, sizeof(data));}
bool add_call(InjectionLoopArray *arr, CallingConvetion calling_convetion, u32 address, u32 this_pointer, u32 arg_count, ...)
{
	while (arr->usage + 11 + (arg_count * 4) > arr->capacity) {
		size_t new_capacity = arr->capacity * 2;
		uint8_t *new_data = realloc(arr->data, new_capacity);
		if (!new_data) return false;
		arr->data = new_data;
		arr->capacity = new_capacity;
	}
	
	arr->data[arr->usage++] = ENTRY_CALL;
	arr->data[arr->usage++] = calling_convetion;
	memcpy(arr->data + arr->usage, &address, 4);
	arr->usage += 4;
	memcpy(arr->data + arr->usage, &this_pointer, 4);
	arr->usage += 4;
	arr->data[arr->usage++] = arg_count;
	
	va_list args;
	va_start(args, arg_count);
	
	int32_t arg_arr[arg_count];
	
	for (size_t i = 0; i < arg_count; i++) {
		arg_arr[i] = (int32_t)va_arg(args, int);
	}
	
	for (size_t i = 0; i < arg_count; i++) {
		memcpy(arr->data + arr->usage, &arg_arr[arg_count - 1 - i], 4);
		arr->usage += 4;
	}
	
	va_end(args);
	
	arr->entry_count++;
	
	return true;
}
bool execute_injection_loop(InjectionLoopArray *injection_loop_array)
{
	LPVOID data_mem = NULL;
	LPVOID code_mem = NULL;
	HANDLE thread_handle = NULL;
	
	IntegerArray *text_address_array = injection_loop_array->text_address_array;
	DWORD saved_protection_states[text_address_array->usage];
	
	for (u32 i = 0; i < text_address_array->usage; i++) {
		if (!VirtualProtectEx(process_handle, (LPVOID)(usize)text_address_array->data[i], (SIZE_T)4, PAGE_EXECUTE_READWRITE, &saved_protection_states[i])) goto err;
	}
	
	u8 injection_loop_code[] = { 
		0xBE, 0x00, 0x00, 0x00, 0x00,       // mov esi, 0 ; later set to data_mem ptr
		0xBF, 0x00, 0x00, 0x00, 0x00,       // mov edi, 0 ; later set to entry count
		                                    // main_loop:
		0x8A, 0x06,                         // mov al, [esi]
		0x83, 0xC6, 0x01,                   // add esi, 1
		0x3C, ENTRY_WRITE,                  // cmp al, ENTRY_WRITE
		0x74, 0x0A,                         // je do_write
		0x3C, ENTRY_CALL,                   // cmp al, ENTRY_CALL
		0x74, 0x42,                         // je do_call
		0xB8, 0x01, 0x00, 0x00, 0x00,       // mov eax, 1
		0xC3,                               // ret
		                                    // do_write:
		0x8B, 0x06,                         // mov eax, [esi] ; address
		0x83, 0xC6, 0x04,                   // add esi, 4
		0x8A, 0x1E,                         // mov bl, [esi] ; size
		0x83, 0xC6, 0x01,                   // add esi, 1
		0x80, 0xFB, 0x01,                   // cmp bl, 1
		0x74, 0x10,                         // je write_one
		0x80, 0xFB, 0x02,                   // cmp bl, 2
		0x74, 0x14,                         // je write_two
		0x80, 0xFB, 0x04,                   // cmp bl, 4
		0x74, 0x1A,                         // je write_four
		0xB8, 0x01, 0x00, 0x00, 0x00,       // mov eax, 1
		0xC3,                               // ret
		                                    // write_one:
		0x8A, 0x0E,                         // mov cl, [esi]
		0x83, 0xC6, 0x01,                   // add esi, 1
		0x88, 0x08,                         // mov [eax], cl
		0xEB, 0x48,                         // jmp next
		                                    // write_two:
		0x66, 0x8B, 0x0E,                   // mov cx, [esi]
		0x83, 0xC6, 0x02,                   // add esi, 2
		0x66, 0x89, 0x08,                   // mov [eax], cx
		0xEB, 0x3D,                         // jmp next
		                                    // write_four:
		0x8B, 0x0E,                         // mov ecx, [esi]
		0x83, 0xC6, 0x04,                   // add esi, 4
		0x89, 0x08,                         // mov [eax], ecx
		0xEB, 0x34,                         // jmp next
		                                    // do_call:
		0x8A, 0x06,                         // mov al, [esi] ; calling convetion
		0x83, 0xC6, 0x01,                   // add esi, 1
		0x8B, 0x16,                         // mov edx, [esi] ; address
		0x83, 0xC6, 0x04,                   // add esi, 4
		0x8B, 0x0E,                         // mov ecx, [esi] ; this ptr
		0x83, 0xC6, 0x04,                   // add esi, 4
		0x31, 0xDB,                         // xor ebx, ebx
		0x8A, 0x1E,                         // mov bl, [esi] ; arg count
		0x83, 0xC6, 0x01,                   // add esi, 1
		0x83, 0xFB, 0x00,                   // cmp ebx, 0
		0x7E, 0x0A,                         // jle args_done
		0x89, 0xDD,                         // mov ebp, ebx
		                                    // arg_loop:
		0xFF, 0x36,                         // push [esi]
		0x83, 0xC6, 0x04,                   // add esi, 4
		0x4D,                               // dec ebp
		0x75, 0xF8,                         // jnz arg_loop
		                                    // args_done:
		0x3C, CNV_CDECL,                    // cmp al, CNV_CDECL
		0x75, 0x09,                         // jne not_cdecl
		0xFF, 0xD2,                         // call edx
		0xC1, 0xE3, 0x02,                   // shl ebx, 2
		0x01, 0xDC,                         // add esp, ebx
		0xEB, 0x02,                         // jmp next
		                                    // not_cdecl:
		0xFF, 0xD2,                         // call edx
		                                    // next:
		0x4F,                               // dec edi
		0x0F, 0x85, 0x76, 0xFF, 0xFF, 0xFF, // jnz main_loop
		0x31, 0xC0,                         // xor eax, eax
		0xC3                                // ret
	};
	
	data_mem = VirtualAllocEx(process_handle, NULL, injection_loop_array->usage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!data_mem) goto err;
	
	if (!WriteProcessMemory(process_handle, data_mem, injection_loop_array->data, injection_loop_array->usage, NULL)) goto err;
	
	memcpy(injection_loop_code + 1, &data_mem, 4);
	memcpy(injection_loop_code + 6, &injection_loop_array->entry_count, 4);
	
	code_mem = VirtualAllocEx(process_handle, NULL, sizeof(injection_loop_code), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!code_mem) goto err;
	
	if (!WriteProcessMemory(process_handle, code_mem, injection_loop_code, sizeof(injection_loop_code), NULL)) goto err;
	
	thread_handle = CreateRemoteThread(process_handle, NULL, 0, (LPTHREAD_START_ROUTINE)code_mem, NULL, 0, NULL);
	if (!thread_handle) goto err;
	
	WaitForSingleObject(thread_handle, INFINITE);
	
	CloseHandle(thread_handle);
	VirtualFreeEx(process_handle, code_mem, 0, MEM_RELEASE);
	VirtualFreeEx(process_handle, data_mem, 0, MEM_RELEASE);
	
	for (size_t i = 0; i < text_address_array->usage; i++) {
		DWORD tmp;
		if (!VirtualProtectEx(process_handle, (LPVOID)(usize)text_address_array->data[i], (SIZE_T)4, saved_protection_states[i], &tmp)) goto err;
	}
	
	return true;
	
err:
	if (thread_handle) CloseHandle(thread_handle);
	if (code_mem) VirtualFreeEx(process_handle, code_mem, 0, MEM_RELEASE);
	if (data_mem) VirtualFreeEx(process_handle, data_mem, 0, MEM_RELEASE);
	return false;
}
void free_injection_loop_array(InjectionLoopArray *arr)
{
	if (!arr) return;
	
	free(arr->data);
	free_integer_array(arr->text_address_array);
	
	free(arr);
}
