VERSION = v2026.06.19
MAKEFLAGS += -s

CODE = \
	code/main.c \
	$(wildcard code/framework/*.c) \
	$(wildcard code/generators/*.c)

INCLUDE = \
	-Icode/framework \
	-Icode/generators

FLAGS = \
	-DVERSION=\"$(VERSION)\" \
	$(INCLUDE) \
	-std=c99 \
	-Wall -Wextra -Wshadow \
	-O2 -flto -s \
	-MMD -MP

CC_X64 = x86_64-w64-mingw32-gcc
CC_X86 = i686-w64-mingw32-gcc
WINDRES_X64 = x86_64-w64-mingw32-windres
WINDRES_X86 = i686-w64-mingw32-windres

INTERMEDIATES_X64 = build/x64/intermediates
INTERMEDIATES_X86 = build/x86/intermediates
OUTPUT_X64 = build/x64/output
OUTPUT_X86 = build/x86/output

OBJ_CODE_X64 = $(patsubst %.c,$(INTERMEDIATES_X64)/%.o,$(CODE))
OBJ_CODE_X86 = $(patsubst %.c,$(INTERMEDIATES_X86)/%.o,$(CODE))
OBJ_RESOURCES_X64 = $(INTERMEDIATES_X64)/resources/resources.o
OBJ_RESOURCES_X86 = $(INTERMEDIATES_X86)/resources/resources.o

.PHONY: all x64 x86 clean

all: x64 x86
x64: $(OUTPUT_X64)/cmg.exe $(OUTPUT_X64)/config.txt
x86: $(OUTPUT_X86)/cmg.exe $(OUTPUT_X86)/config.txt
clean:
	rm -rf build

# x64

-include $(OBJ_CODE_X64:.o=.d)
$(INTERMEDIATES_X64)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC_X64) $(FLAGS) -c $< -o $@

$(OBJ_RESOURCES_X64): resources/resources.rc
	mkdir -p $(dir $@)
	$(WINDRES_X64) $< -O coff -o $@

$(OUTPUT_X64)/cmg.exe: $(OBJ_CODE_X64) $(OBJ_RESOURCES_X64)
	mkdir -p $(OUTPUT_X64)
	$(CC_X64) $(FLAGS) $^ -o $@
$(OUTPUT_X64)/config.txt: resources/config.txt
	mkdir -p $(OUTPUT_X64)
	cp $< $@

# x86

-include $(OBJ_CODE_X86:.o=.d)
$(INTERMEDIATES_X86)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC_X86) $(FLAGS) -c $< -o $@

$(OBJ_RESOURCES_X86): resources/resources.rc
	mkdir -p $(dir $@)
	$(WINDRES_X86) $< -O coff -o $@

$(OUTPUT_X86)/cmg.exe: $(OBJ_CODE_X86) $(OBJ_RESOURCES_X86)
	mkdir -p $(OUTPUT_X86)
	$(CC_X86) $(FLAGS) $^ -o $@
$(OUTPUT_X86)/config.txt: resources/config.txt
	mkdir -p $(OUTPUT_X86)
	cp $< $@
