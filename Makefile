MAKEFLAGS += -s

VERSION_FILE := VERSION
VERSION_STR := $(shell cat $(VERSION_FILE))

CODE := \
	code/main.c \
	$(wildcard code/framework/*.c) \
	$(wildcard code/generators/*.c)
CPPFLAGS += \
	-Icode/framework \
	-Icode/generators
CFLAGS += \
	-std=c23 \
	-Wall -Wextra -Wshadow \
	-O2 \
	-flto \
	-MMD -MP
LDFLAGS += \
	-flto \
	-s

CC_X64 := x86_64-w64-mingw32-gcc
CC_X86 := i686-w64-mingw32-gcc
WINDRES_X64 := x86_64-w64-mingw32-windres
WINDRES_X86 := i686-w64-mingw32-windres

ZIP := zip -q -r -9

DIR_X64 := build/x64
DIR_X86 := build/x86

OBJ_CODE_X64 := $(patsubst %.c,$(DIR_X64)/intermediates/%.o,$(CODE))
OBJ_CODE_X86 := $(patsubst %.c,$(DIR_X86)/intermediates/%.o,$(CODE))
OBJ_RESOURCES_X64 := $(DIR_X64)/intermediates/resources/resources.o
OBJ_RESOURCES_X86 := $(DIR_X86)/intermediates/resources/resources.o

.DELETE_ON_ERROR:
.PHONY: all x64 x86 clean

all: x64 x86
x64: $(DIR_X64)/cmg-$(VERSION_STR)-windows-x64.zip
x86: $(DIR_X86)/cmg-$(VERSION_STR)-windows-x86.zip
clean:
	rm -rf build

# x64

$(DIR_X64)/intermediates/version.h: $(VERSION_FILE)
	mkdir -p $(dir $@)
	echo '#define VERSION_STR "$(VERSION_STR)"' > $@
-include $(OBJ_CODE_X64:.o=.d)
$(DIR_X64)/intermediates/%.o: %.c $(DIR_X64)/intermediates/version.h
	mkdir -p $(dir $@)
	$(CC_X64) $(CPPFLAGS) $(CFLAGS) -I$(DIR_X64)/intermediates -c $< -o $@
$(OBJ_RESOURCES_X64): resources/resources.rc
	mkdir -p $(dir $@)
	$(WINDRES_X64) $< -o $@

$(DIR_X64)/cmg/cmg.exe: $(OBJ_CODE_X64) $(OBJ_RESOURCES_X64)
	mkdir -p $(dir $@)
	$(CC_X64) $(LDFLAGS) $^ -o $@
$(DIR_X64)/cmg/config.txt: resources/config.txt
	mkdir -p $(dir $@)
	cp $< $@

$(DIR_X64)/cmg-$(VERSION_STR)-windows-x64.zip: $(DIR_X64)/cmg/cmg.exe $(DIR_X64)/cmg/config.txt
	mkdir -p $(dir $@)
	cd $(dir $@) && $(ZIP) cmg-$(VERSION_STR)-windows-x64.zip cmg/

# x86

$(DIR_X86)/intermediates/version.h: $(VERSION_FILE)
	mkdir -p $(dir $@)
	echo '#define VERSION_STR "$(VERSION_STR)"' > $@
-include $(OBJ_CODE_X86:.o=.d)
$(DIR_X86)/intermediates/%.o: %.c $(DIR_X86)/intermediates/version.h
	mkdir -p $(dir $@)
	$(CC_X86) $(CPPFLAGS) $(CFLAGS) -I$(DIR_X86)/intermediates -c $< -o $@
$(OBJ_RESOURCES_X86): resources/resources.rc
	mkdir -p $(dir $@)
	$(WINDRES_X86) $< -o $@

$(DIR_X86)/cmg/cmg.exe: $(OBJ_CODE_X86) $(OBJ_RESOURCES_X86)
	mkdir -p $(dir $@)
	$(CC_X86) $(LDFLAGS) $^ -o $@
$(DIR_X86)/cmg/config.txt: resources/config.txt
	mkdir -p $(dir $@)
	cp $< $@

$(DIR_X86)/cmg-$(VERSION_STR)-windows-x86.zip: $(DIR_X86)/cmg/cmg.exe $(DIR_X86)/cmg/config.txt
	mkdir -p $(dir $@)
	cd $(dir $@) && $(ZIP) cmg-$(VERSION_STR)-windows-x86.zip cmg/
