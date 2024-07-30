SILENTCMD := @
SILENTMSG := @echo

CC := gcc
CFLAGS := -Wall -Wextra

LD := gcc
LDFLAGS := -static

SRC_DIRS := ../cotton-init

SRC_FILES := $(foreach file,$(SRC_DIRS), $(wildcard $(file)/*c)) $(TARGET).c
SRC_FILES := $(filter-out ../cotton-init/main.c,$(SRC_FILES))

OBJ_FILES := $(SRC_FILES:.c=.o)

INCLUDE_DIRS := ../cotton-init

INCLUDES := $(foreach include, $(INCLUDE_DIRS), -I$(include))

LIBS := m

LIB := $(foreach library, $(LIBS), -l$(library))

all: check-env $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(SILENTMSG) LD $^ -> $@
	$(SILENTCMD)$(LD) $(LDFLAGS) -o $@ $^ $(LIB)

%.o: %.c
	$(SILENTMSG) CC $^ -> $@
	$(SILENTCMD)$(CC)  -lblkid $(CFLAGS) $(INCLUDES) -lm -c -o $@ $^

clean:
	$(SILENTMSG) rm $(OBJ_FILES) $(TARGET)
	$(SILENTCMD)rm -f $(OBJ_FILES) $(TARGET)

check-env:
ifndef TARGET
	$(error TARGET is undefined)
endif