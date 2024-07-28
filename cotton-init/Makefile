SILENTCMD := @
SILENTMSG := @echo

INITRD_DIR := ../initrd_root
INITRD_FILE_NAME := ../initramfs

TARGET := init

CC := gcc
CFLAGS := -Wall -Wextra

LD := gcc
LDFLAGS := -static

SRC_DIRS := .

SRC_FILES := $(foreach file,$(SRC_DIRS), $(wildcard $(file)/*c))

OBJ_FILES := $(SRC_FILES:.c=.o)

INCLUDE_DIRS := .

INCLUDES := $(foreach include, $(INCLUDE_DIRS), -I$(include))

LIBS := m

LIB := $(foreach library, $(LIBS), -l$(library))

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(SILENTMSG) LD $^ -> $@
	$(SILENTCMD)$(LD) $(LDFLAGS) -o $@ $^ $(LIB)

%.o: %.c
	$(SILENTMSG) CC $^ -> $@
	$(SILENTCMD)$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $^

clean:
	$(SILENTMSG) rm $(OBJ_FILES) $(TARGET) $(INITRD_FILE_NAME) $(INITRD_DIR)/$(TARGET)
	$(SILENTCMD)rm -f $(OBJ_FILES) $(TARGET) $(INITRD_FILE_NAME) $(INITRD_DIR)/$(TARGET)

initrd: $(TARGET)
	cp $(TARGET) $(INITRD_DIR)/init
	cd $(INITRD_DIR)
	find . | tail -n +2 | cpio -H newc -ov > $(INITRD_FILE_NAME).cpio
	gzip -f $(INITRD_FILE_NAME).cpio
	