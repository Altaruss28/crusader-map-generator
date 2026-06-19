VERSION = v2026.06.20
MAKEFLAGS += -s

CODE = \
	code/main.c \
	$(wildcard code/framework/*.c) \
	$(wildcard code/generators/*.c)
CPPFLAGS = \
	-DVERSION=\"$(VERSION)\" \
	-Icode/framework \
	-Icode/generators
CCFLAGS = \
	-std=c23 \
	-Wall -Wextra -Wshadow \
	-O2 \
	-flto \
	-MMD -MP
LDFLAGS = \
	-flto \
	-s

CC_X64 = x86_64-w64-mingw32-gcc
CC_X86 = i686-w64-mingw32-gcc
WINDRES_X64 = x86_64-w64-mingw32-windres
WINDRES_X86 = i686-w64-mingw32-windres

ZIP = zip -q -r -9

DIR_X64 = build/x64
DIR_X86 = build/x86

OBJ_CODE_X64 = $(patsubst %.c,$(DIR_X64)/intermediates/%.o,$(CODE))
OBJ_CODE_X86 = $(patsubst %.c,$(DIR_X86)/intermediates/%.o,$(CODE))
OBJ_RESOURCES_X64 = $(DIR_X64)/intermediates/resources/resources.o
OBJ_RESOURCES_X86 = $(DIR_X86)/intermediates/resources/resources.o

.PHONY: all x64 x86 clean

all: x64 x86
x64: $(DIR_X64)/cmg/cmg.exe $(DIR_X64)/cmg/config.txt $(DIR_X64)/cmg-$(VERSION)-windows-x64.zip
x86: $(DIR_X86)/cmg/cmg.exe $(DIR_X86)/cmg/config.txt $(DIR_X86)/cmg-$(VERSION)-windows-x86.zip
clean:
	rm -rf build

# x64

-include $(OBJ_CODE_X64:.o=.d)
$(DIR_X64)/intermediates/%.o: %.c
	mkdir -p $(dir $@)
	$(CC_X64) $(CPPFLAGS) $(CCFLAGS) -c $< -o $@
$(OBJ_RESOURCES_X64): resources/resources.rc
	mkdir -p $(dir $@)
	$(WINDRES_X64) $< -o $@

$(DIR_X64)/cmg/cmg.exe: $(OBJ_CODE_X64) $(OBJ_RESOURCES_X64)
	mkdir -p $(DIR_X64)/cmg
	$(CC_X64) $(LDFLAGS) $^ -o $@
$(DIR_X64)/cmg/config.txt: resources/config.txt
	mkdir -p $(DIR_X64)/cmg
	cp $< $@

$(DIR_X64)/cmg-$(VERSION)-windows-x64.zip: $(DIR_X64)/cmg/cmg.exe $(DIR_X64)/cmg/config.txt
	mkdir -p $(DIR_X64)
	cd $(DIR_X64) && $(ZIP) cmg-$(VERSION)-windows-x64.zip cmg/

# x86

-include $(OBJ_CODE_X86:.o=.d)
$(DIR_X86)/intermediates/%.o: %.c
	mkdir -p $(dir $@)
	$(CC_X86) $(CPPFLAGS) $(CCFLAGS) -c $< -o $@
$(OBJ_RESOURCES_X86): resources/resources.rc
	mkdir -p $(dir $@)
	$(WINDRES_X86) $< -o $@

$(DIR_X86)/cmg/cmg.exe: $(OBJ_CODE_X86) $(OBJ_RESOURCES_X86)
	mkdir -p $(DIR_X86)/cmg
	$(CC_X86) $(LDFLAGS) $^ -o $@
$(DIR_X86)/cmg/config.txt: resources/config.txt
	mkdir -p $(DIR_X86)/cmg
	cp $< $@

$(DIR_X86)/cmg-$(VERSION)-windows-x86.zip: $(DIR_X86)/cmg/cmg.exe $(DIR_X86)/cmg/config.txt
	mkdir -p $(DIR_X86)
	cd $(DIR_X86) && $(ZIP) cmg-$(VERSION)-windows-x86.zip cmg/
