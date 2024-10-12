SILENTCMD := @
SILENTMSG := @echo

INITRD_DIR := ../initrd_root
INITRD_FILE_NAME := ../initramfs

TARGET := init

CC := gcc
LD := gcc

CFLAGS := -Wall -Werror -Wextra -Wpedantic -Wconversion -Wshadow -fsanitize=undefined -g
LDFLAGS := -static -static-libubsan

SRC_DIRS := .

SRC_FILES := $(foreach file,$(SRC_DIRS), $(wildcard $(file)/*c))

OBJ_FILES := $(SRC_FILES:.c=.o)

INCLUDE_DIRS := .

INCLUDES := $(foreach include, $(INCLUDE_DIRS), -I$(include))

LIBS := blkid m ubsan

LIB := $(foreach library, $(LIBS), -l$(library))

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(SILENTMSG) $(LD) -o $@ $^ $(LDFLAGS) $(LIB)
	$(SILENTCMD)$(LD) -o $@ $^ $(LDFLAGS) $(LIB)

%.o: %.c
	$(SILENTMSG) $(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $^
	$(SILENTCMD)$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $^

clean:
	$(SILENTMSG) rm $(OBJ_FILES) $(TARGET) $(INITRD_FILE_NAME).cpio.gz $(INITRD_DIR)/$(TARGET)
	$(SILENTCMD)rm -f $(OBJ_FILES) $(TARGET) $(INITRD_FILE_NAME).cpio.gz $(INITRD_DIR)/$(TARGET)

initrd: $(TARGET)
	cp $(TARGET) $(INITRD_DIR)/init
	cd $(INITRD_DIR); \
	find . | tail -n +2 | cpio -H newc -ov > $(INITRD_FILE_NAME).cpio; \
	gzip -f $(INITRD_FILE_NAME).cpio

