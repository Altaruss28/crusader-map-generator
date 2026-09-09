#include "process_access.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <psapi.h>
#include <tchar.h>
#include <tlhelp32.h>

#include "common.h"

static HANDLE process_handle;

bool process_access_enable(void)
{
	PROCESSENTRY32 process_entry;
	process_entry.dwSize = sizeof(process_entry);

	HANDLE snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (snapshot_handle == INVALID_HANDLE_VALUE
		|| !Process32First(snapshot_handle, &process_entry))
		goto err;

	do {
		if (_tcsicmp(process_entry.szExeFile, "Stronghold Crusader.exe") == 0
			|| _tcsicmp(process_entry.szExeFile, "Stronghold_Crusader_Extreme.exe") == 0) {
			if(!(process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_entry.th32ProcessID)))
				goto err;
			break;
		}
	} while (Process32Next(snapshot_handle, &process_entry));

	if (!process_handle)
		goto err;

	CloseHandle(snapshot_handle);

	return true;

err:
	if (snapshot_handle)
		CloseHandle(snapshot_handle);
	return false;
}
void process_access_disable(void)
{
	if (!process_handle)
		return;

	CloseHandle(process_handle);
	process_handle = nullptr;
}

static const u8 byte_value_frequencies[256] = {
	255, 243, 229, 219, 240, 194, 180, 193, 242, 135, 145, 133, 236, 151, 134, 241,
	231, 107, 126, 103, 203, 234,  87,  65, 190,  85, 147, 110, 167,  46,  66,  89,
	213,  90,  69,  93, 237, 101,  28,  33, 140,  62,  21, 125, 121,  78,  83,  14,
	192,  99,  92, 233, 122,  88,  42,  50, 157, 144,  48, 175, 115, 109,  49,  86,
	209, 181, 132, 164, 215, 245, 182, 161, 206, 146,  67,  72, 166, 232, 143, 123,
	238, 208, 131, 212, 155, 223, 225, 217, 119, 159,  53, 154, 124, 198, 200, 201,
	139, 176,  57, 148, 178, 224, 197, 105, 221, 188, 226,  38, 187, 112, 173, 185,
	218, 165, 210, 230, 251, 249, 228, 204, 179, 137,  26,  25, 116, 171, 113,  80,
	199, 150,  43, 244, 202, 247,  96,  52, 162, 248,  94, 253,  68, 246,  41,  10,
	149,  13,  16,  35,  81,  95,  11,   2,  73,  17,   0,   8,  63,  24,  23,  39,
	120, 138,  15,  30,  74, 117,   4,  12,  79,   7,   3,  31,  76,   1,   5,  47,
	142,  22,   9,  18, 100,  91,  64, 118, 189, 108, 153,  71,  98,  55,  97,  75,
	239, 186, 207, 191, 170, 102, 172, 211, 174, 195,  70, 104, 252,  54, 184, 127,
	168,  77, 128,  45, 106,   6, 158,  61, 160, 111,  32, 141, 130,  56,  20,  36,
	183,  60,  44,  34, 163,  58,  29,  19, 250, 216,  40, 214, 235,  27,  37,  51,
	220, 129,  84,  82, 169,  59, 205, 152, 222, 156, 114, 136, 227, 177, 196, 254,
};
bool aob_scan(AobScanEntry *entries, int entry_count)
{
	bool ret = false;

	u8 *region_buffer = nullptr;
	int region_buffer_size = 0;

	struct {
		int first_byte_offset;
		int last_byte_offset;
		int check_byte_count;
		int check_byte_offset;
		bool found;
	} entry_scan_data[entry_count];

	struct {
		int entry_count;
		int entry_scan_data_index_offset;
	} entry_buckets[256] = {};

	int entry_scan_data_indices[entry_count];

	int total_check_byte_count = 0;

	int total_byte_counts[entry_count];
	int value_byte_counts[entry_count];

	for (int i = 0; i < entry_count; ++i) {
		char *token = entries[i].aob_str;

		int total_byte_count = (strlen(token) + 1) / 3;
		int value_byte_count = 0;

		for (int j = 0; j < total_byte_count; ++j) {
			if (token[0] != '?' && token[1] != '?')
				++value_byte_count;
			token += 3;
		}

		total_byte_counts[i] = total_byte_count;
		value_byte_counts[i] = value_byte_count;

		total_check_byte_count += value_byte_count - 1;
	}

	struct {
		int offset;
		u8 value;
	} check_bytes[total_check_byte_count];

	int check_byte_load_index = 0;

	int anchor_byte_values[entry_count];

	for (int i = 0; i < entry_count; ++i) {
		int value_byte_count = value_byte_counts[i];

		typedef struct {
			int index;
			u8 value;
			u8 frequency;
		} ValueByte;

		ValueByte value_bytes[value_byte_count];

		char *token = entries[i].aob_str;
		int value_byte_index = 0;

		for (int j = 0; j < total_byte_counts[i]; ++j) {
			if (token[0] != '?' && token[1] != '?') {
				value_bytes[value_byte_index].index = j;

				char *end_ptr;
				u8 value = (u8)strtol(token, &end_ptr, 16);

				value_bytes[value_byte_index].value = value;
				value_bytes[value_byte_index].frequency = byte_value_frequencies[value];

				++value_byte_index;
			}

			token += 3;
		}

		for (int j = 1; j < value_byte_count; ++j) {
			ValueByte byte = value_bytes[j];

			int k = j - 1;

			while (k >= 0 && value_bytes[k].frequency > byte.frequency) {
				value_bytes[k + 1] = value_bytes[k];
				k--;
			}

			value_bytes[k + 1] = byte;
		}

		int anchor_index = value_bytes[0].index;
		u8 anchor_value = value_bytes[0].value;

		anchor_byte_values[i] = anchor_value;
		++entry_buckets[anchor_value].entry_count;

		entry_scan_data[i].first_byte_offset = anchor_index;
		entry_scan_data[i].last_byte_offset = total_byte_counts[i] - anchor_index - 1;
		entry_scan_data[i].check_byte_count = value_byte_count - 1;
		entry_scan_data[i].check_byte_offset = check_byte_load_index;
		entry_scan_data[i].found = false;

		for (int j = 1; j < value_byte_count; ++j) {
			check_bytes[check_byte_load_index].offset = value_bytes[j].index - anchor_index;
			check_bytes[check_byte_load_index].value = value_bytes[j].value;
			++check_byte_load_index;
		}
	}

	int bucket_entry_scan_data_load_index = 0;
	int bucket_entry_scan_data_load_index_offsets[256];

	for (int i = 0; i < 256; ++i) {
		entry_buckets[i].entry_scan_data_index_offset = bucket_entry_scan_data_load_index;
		bucket_entry_scan_data_load_index_offsets[i] = bucket_entry_scan_data_load_index;
		bucket_entry_scan_data_load_index += entry_buckets[i].entry_count;
	}

	for (int i = 0; i < 256; ++i)
		for (int j = 0; j < entry_count; ++j) {
			if (anchor_byte_values[j] != i)
				continue;
			entry_scan_data_indices[bucket_entry_scan_data_load_index_offsets[i]++] = j;
		}

	HMODULE h_mods[1024];
	DWORD tmp;

	if (!EnumProcessModules(process_handle, h_mods, sizeof(h_mods), &tmp))
		goto err;

	MODULEINFO module_info;
	if (!GetModuleInformation(process_handle, h_mods[0], &module_info, sizeof(module_info)))
		goto err;

	BYTE *module_base = (BYTE *)module_info.lpBaseOfDll;

	IMAGE_DOS_HEADER dos_header;
	if (!ReadProcessMemory(process_handle, module_base, &dos_header, sizeof(dos_header), nullptr)
		|| dos_header.e_magic != IMAGE_DOS_SIGNATURE)
		goto err;

	IMAGE_NT_HEADERS32 nt_headers;
	if (!ReadProcessMemory(process_handle, module_base + dos_header.e_lfanew, &nt_headers, sizeof(nt_headers), nullptr)
		|| nt_headers.Signature != IMAGE_NT_SIGNATURE)
		goto err;

	int region_count = nt_headers.FileHeader.NumberOfSections;
	usize region_table = (usize)(module_base + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS32));

	usize scan_address = 0;
	usize scan_end = 0;

	for (int i = 0; i < region_count; ++i) {
		IMAGE_SECTION_HEADER section;

		if (!ReadProcessMemory(process_handle, (LPCVOID)(region_table + (i * sizeof(IMAGE_SECTION_HEADER))), &section, sizeof(section), nullptr))
			goto err;

		if (memcmp(section.Name, ".text", 5) == 0) {
			scan_address = (usize)module_base + section.VirtualAddress;
			scan_end = scan_address + section.Misc.VirtualSize;
			break;
		}
	}

	if (scan_address == 0)
		goto err;

	MEMORY_BASIC_INFORMATION mbi;

	while (scan_address < scan_end && VirtualQueryEx(process_handle, (LPCVOID)scan_address, &mbi, sizeof(mbi))) {
		usize region_start = (usize)mbi.BaseAddress;
		if (region_start >= scan_end)
			break;

		usize region_end = region_start + mbi.RegionSize;
		if (region_end > scan_end)
			region_end = scan_end;

		if (mbi.State == MEM_COMMIT
			&& (mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY))
			&& !(mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))) {
			int region_size = region_end - region_start;

			if (region_buffer_size < region_size) {
				free(region_buffer);
				if (!(region_buffer = malloc(region_size)))
					goto err;
				region_buffer_size = region_size;
			}

			SIZE_T read_byte_count;
			if (!ReadProcessMemory(process_handle, (LPCVOID)region_start, region_buffer, region_size, &read_byte_count))
				goto err;
			if (read_byte_count == 0) {
				scan_address = region_end;
				continue;
			}

			for (usize i = 0; i < read_byte_count; ++i) {
				u8 *region_byte_ptr = region_buffer + i;
				u8 region_byte_value = *region_byte_ptr;

				int bucket_entry_count = entry_buckets[region_byte_value].entry_count;
				if (bucket_entry_count == 0)
					continue;

				int bucket_entry_scan_data_index_offset = entry_buckets[region_byte_value].entry_scan_data_index_offset;

				for (int j = 0; j < bucket_entry_count; ++j) {
					int entry_index = entry_scan_data_indices[bucket_entry_scan_data_index_offset + j];

					int first_byte_offset = entry_scan_data[entry_index].first_byte_offset;
					int last_byte_offset = entry_scan_data[entry_index].last_byte_offset;

					if (i < (usize)first_byte_offset
						|| i + last_byte_offset >= read_byte_count)
						continue;

					int entry_check_byte_count = entry_scan_data[entry_index].check_byte_count;
					int entry_check_byte_offset = entry_scan_data[entry_index].check_byte_offset;

					bool matched = true;

					for (int k = 0; k < entry_check_byte_count; ++k) {
						int check_byte_offset = entry_check_byte_offset + k;
						u8 check_byte_value = check_bytes[check_byte_offset].value;

						int offset = check_bytes[check_byte_offset].offset;
						u8 offset_byte_value = region_byte_ptr[offset];

						if (check_byte_value == offset_byte_value)
							continue;

						matched = false;
						break;
					}

					if (!matched)
						continue;
					if (entry_scan_data[entry_index].found)
						goto err;

					if (entries[entry_index].is_read_type) {
						memcpy(&entries[entry_index].address, region_buffer + i - first_byte_offset, sizeof(u32));
					} else {
						entries[entry_index].address = region_start + i - first_byte_offset;
					}

					entry_scan_data[entry_index].found = true;
				}
			}
		}

		scan_address = region_end;
	}

	for (int i = 0; i < entry_count; ++i)
		if (!entry_scan_data[i].found)
			goto err;

	ret = true;

err:
	free(region_buffer);
	return ret;
}

bool read_data(u32 address, void *data, usize size)
{
	DWORD saved_protection_state;
	if (!VirtualProtectEx(process_handle, (LPVOID)(usize)address, (SIZE_T)size, PAGE_EXECUTE_READWRITE, &saved_protection_state))
		return false;

	SIZE_T read_byte_count;
	if (!ReadProcessMemory(process_handle, (LPVOID)(usize)address, data, (SIZE_T)size, &read_byte_count)
		|| read_byte_count != size)
		return false;

	DWORD tmp;
	if (!VirtualProtectEx(process_handle, (LPVOID)(usize)address, (SIZE_T)size, saved_protection_state, &tmp))
		return false;

	return true;
}
bool write_data(u32 address, const void *data, usize size)
{
	DWORD saved_protection_state;
	if (!VirtualProtectEx(process_handle, (LPVOID)(usize)address, (SIZE_T)size, PAGE_EXECUTE_READWRITE, &saved_protection_state))
		return false;

	SIZE_T written_byte_count;
	if (!WriteProcessMemory(process_handle, (LPVOID)(usize)address, data, (SIZE_T)size, &written_byte_count)
		|| written_byte_count != size
		|| !FlushInstructionCache(process_handle, (LPVOID)(usize)address, (SIZE_T)size))
		return false;

	DWORD tmp;
	if (!VirtualProtectEx(process_handle, (LPVOID)(usize)address, (SIZE_T)size, saved_protection_state, &tmp))
		return false;

	return true;
}

bool read_i8(u32 address, i8 *data) {return read_data(address, data, sizeof(*data));}
bool read_i16(u32 address, i16 *data) {return read_data(address, data, sizeof(*data));}
bool read_i32(u32 address, i32 *data) {return read_data(address, data, sizeof(*data));}
bool write_i8(u32 address, i8 data) {return write_data(address, &data, sizeof(data));}
bool write_i16(u32 address, i16 data) {return write_data(address, &data, sizeof(data));}
bool write_i32(u32 address, i32 data) {return write_data(address, &data, sizeof(data));}

bool read_u8(u32 address, u8 *data) {return read_data(address, data, sizeof(*data));}
bool read_u16(u32 address, u16 *data) {return read_data(address, data, sizeof(*data));}
bool read_u32(u32 address, u32 *data) {return read_data(address, data, sizeof(*data));}
bool write_u8(u32 address, u8 data) {return write_data(address, &data, sizeof(data));}
bool write_u16(u32 address, u16 data) {return write_data(address, &data, sizeof(data));}
bool write_u32(u32 address, u32 data) {return write_data(address, &data, sizeof(data));}

bool read_aob(u32 address, int byte_count, u8 *buffer)
{
	return read_data(address, buffer, byte_count);
}
bool write_aob(u32 address, int byte_count, ...)
{
	va_list(bytes);
	va_start(bytes, byte_count);

	u8 byte_arr[byte_count];

	for (int i = 0; i < byte_count; ++i)
		byte_arr[i] = (u8)va_arg(bytes, int);

	va_end(bytes);

	return write_data(address, byte_arr, byte_count);
}

bool execute_code(CallingConvetion calling_convetion, u32 address, u32 this_ptr, int arg_count, ...)
{
	va_list args;
	va_start(args, arg_count);

	int arg_arr[arg_count];

	for (int i = 0; i < arg_count; ++i)
		arg_arr[i] = (int)va_arg(args, int);

	va_end(args);

	u8 stub_buffer[32 + (arg_count * 5)];
	u8 *p = stub_buffer;

	for (int i = 0; i < arg_count; ++i) {
		*p++ = 0x68; // push
		memcpy(p, &arg_arr[arg_count - 1 - i], 4);
		p += 4;
	}

	if (calling_convetion == CNV_THISCALL) {
		*p++ = 0xB9; // mov ecx
		memcpy(p, &this_ptr, 4);
		p += 4;
	}

	*p++ = 0xB8; // mov eax
	memcpy(p, &address, 4);
	p += 4;

	*p++ = 0xFF; // call eax
	*p++ = 0xD0;

	if (calling_convetion == CNV_CDECL) {
		*p++ = 0x81; // add esp
		*p++ = 0xC4;
		int arg_size = arg_count * 4;
		memcpy(p, &arg_size, 4);
		p += 4;
	}

	*p++ = 0x31; // xor eax, eax
	*p++ = 0xC0;

	*p++ = 0xC3; // ret

	LPVOID remote_mem = nullptr;
	HANDLE thread_handle = nullptr;

	SIZE_T stub_size = p - stub_buffer;

	if (!(remote_mem = VirtualAllocEx(process_handle, nullptr, stub_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))
		|| !WriteProcessMemory(process_handle, remote_mem, stub_buffer, stub_size, nullptr)
		|| !(thread_handle = CreateRemoteThread(process_handle, nullptr, 0, (LPTHREAD_START_ROUTINE)remote_mem, nullptr, 0, nullptr)))
		goto err;

	WaitForSingleObject(thread_handle, INFINITE);

	CloseHandle(thread_handle);
	VirtualFreeEx(process_handle, remote_mem, 0, MEM_RELEASE);

	return true;

err:
	if (thread_handle)
		CloseHandle(thread_handle);
	if (remote_mem)
		VirtualFreeEx(process_handle, remote_mem, 0, MEM_RELEASE);
	return false;
}

BatchInjectionData *batch_injection_data_init(void)
{
	BatchInjectionData *bid = malloc(sizeof(BatchInjectionData));
	if (!bid)
		goto err;

	bid->data = nullptr;
	bid->usage = 0;
	bid->capacity = 256;
	bid->entry_count = 0;
	bid->text_addresses = nullptr;

	if (!(bid->data = malloc(bid->capacity))
		|| !(bid->text_addresses = integer_array_init()))
		goto err;

	return bid;

err:
	batch_injection_data_free(bid);
	return nullptr;
}
bool batch_injection_data_add_write(BatchInjectionData *bid, u32 address, void *data, int size)
{
	while (bid->usage + 6 + size > bid->capacity) {
		usize new_capacity = bid->capacity * 2;
		u8 *new_data = realloc(bid->data, new_capacity);
		if (!new_data)
			return false;
		bid->data = new_data;
		bid->capacity = new_capacity;
	}

	bid->data[bid->usage++] = BATCH_INJECTION_ENTRY_WRITE;

	memcpy(bid->data + bid->usage, &address, 4);
	bid->usage += 4;

	bid->data[bid->usage++] = size;

	memcpy(bid->data + bid->usage, data, size);
	bid->usage += size;

	++bid->entry_count;

	return true;
}
bool batch_injection_data_add_write_i8(BatchInjectionData *bid, u32 address, i8 data) {return batch_injection_data_add_write(bid, address, &data, sizeof(data));}
bool batch_injection_data_add_write_i16(BatchInjectionData *bid, u32 address, i16 data) {return batch_injection_data_add_write(bid, address, &data, sizeof(data));}
bool batch_injection_data_add_write_i32(BatchInjectionData *bid, u32 address, i32 data) {return batch_injection_data_add_write(bid, address, &data, sizeof(data));}
bool batch_injection_data_add_write_u8(BatchInjectionData *bid, u32 address, u8 data) {return batch_injection_data_add_write(bid, address, &data, sizeof(data));}
bool batch_injection_data_add_write_u16(BatchInjectionData *bid, u32 address, u16 data) {return batch_injection_data_add_write(bid, address, &data, sizeof(data));}
bool batch_injection_data_add_write_u32(BatchInjectionData *bid, u32 address, u32 data) {return batch_injection_data_add_write(bid, address, &data, sizeof(data));}
bool batch_injection_data_add_call(BatchInjectionData *bid, CallingConvetion calling_convetion, u32 address, u32 this_ptr, int arg_count, ...)
{
	while (bid->usage + 11 + (arg_count * 4) > bid->capacity) {
		usize new_capacity = bid->capacity * 2;
		u8 *new_data = realloc(bid->data, new_capacity);
		if (!new_data)
			return false;
		bid->data = new_data;
		bid->capacity = new_capacity;
	}

	bid->data[bid->usage++] = BATCH_INJECTION_ENTRY_CALL;
	bid->data[bid->usage++] = calling_convetion;
	memcpy(bid->data + bid->usage, &address, 4);
	bid->usage += 4;
	memcpy(bid->data + bid->usage, &this_ptr, 4);
	bid->usage += 4;
	bid->data[bid->usage++] = arg_count;

	va_list args;
	va_start(args, arg_count);

	int arg_arr[arg_count];

	for (int i = 0; i < arg_count; ++i)
		arg_arr[i] = (int)va_arg(args, int);

	for (int i = 0; i < arg_count; ++i) {
		memcpy(bid->data + bid->usage, &arg_arr[arg_count - 1 - i], 4);
		bid->usage += 4;
	}

	va_end(args);

	++bid->entry_count;

	return true;
}
u8 batch_injection_stub[] = {
	0xBE, 0x00, 0x00, 0x00, 0x00,       // mov esi, 0 ; data ptr
	0xBF, 0x00, 0x00, 0x00, 0x00,       // mov edi, 0 ; entry count
	                                    // main_loop:
	0x8A, 0x06,                         // mov al, [esi]
	0x83, 0xC6, 0x01,                   // add esi, 1
	0x3C, BATCH_INJECTION_ENTRY_WRITE,  // cmp al, BATCH_INJECTION_ENTRY_WRITE
	0x74, 0x0A,                         // je do_write
	0x3C, BATCH_INJECTION_ENTRY_CALL,   // cmp al, BATCH_INJECTION_ENTRY_CALL
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
bool batch_injection_data_inject(const BatchInjectionData *bid)
{
	LPVOID data_mem = nullptr;
	LPVOID stub_mem = nullptr;
	HANDLE thread_handle = nullptr;

	IntegerArray *text_addresses = bid->text_addresses;
	DWORD saved_protection_states[text_addresses->usage];

	for (usize i = 0; i < text_addresses->usage; ++i)
		if (!VirtualProtectEx(process_handle, (LPVOID)(usize)text_addresses->data[i], sizeof(u32), PAGE_EXECUTE_READWRITE, &saved_protection_states[i]))
			goto err;

	if (!(data_mem = VirtualAllocEx(process_handle, nullptr, bid->usage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))
		|| !WriteProcessMemory(process_handle, data_mem, bid->data, bid->usage, nullptr))
		goto err;

	memcpy(batch_injection_stub + 1, &data_mem, 4);
	memcpy(batch_injection_stub + 6, &bid->entry_count, 4);

	if (!(stub_mem = VirtualAllocEx(process_handle, nullptr, sizeof(batch_injection_stub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))
		|| !WriteProcessMemory(process_handle, stub_mem, batch_injection_stub, sizeof(batch_injection_stub), nullptr)
		|| !(thread_handle = CreateRemoteThread(process_handle, nullptr, 0, (LPTHREAD_START_ROUTINE)stub_mem, nullptr, 0, nullptr)))
		goto err;

	WaitForSingleObject(thread_handle, INFINITE);

	CloseHandle(thread_handle);
	VirtualFreeEx(process_handle, stub_mem, 0, MEM_RELEASE);
	VirtualFreeEx(process_handle, data_mem, 0, MEM_RELEASE);

	for (usize i = 0; i < text_addresses->usage; ++i) {
		DWORD tmp;
		if (!VirtualProtectEx(process_handle, (LPVOID)(usize)text_addresses->data[i], sizeof(u32), saved_protection_states[i], &tmp))
			goto err;
	}

	return true;

err:
	if (thread_handle)
		CloseHandle(thread_handle);
	if (stub_mem)
		VirtualFreeEx(process_handle, stub_mem, 0, MEM_RELEASE);
	if (data_mem)
		VirtualFreeEx(process_handle, data_mem, 0, MEM_RELEASE);
	return false;
}
void batch_injection_data_free(BatchInjectionData *bid)
{
	if (!bid)
		return;

	free(bid->data);
	integer_array_free(bid->text_addresses);

	free(bid);
}
