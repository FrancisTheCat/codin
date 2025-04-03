CC := gcc
AS := gcc
CFLAGS  := -nostdlib -fno-builtin -I. -fno-stack-protector
ASFLAGS := $(CFLAGS)

SRC := $(wildcard *.c) $(wildcard *.S)

PLATFORM ?= linux

PLATFORMS := linux windows macos FreeBSD OpenBSD

PLATFORM_SPECIFIC := $(foreach plat, $(PLATFORMS), \
    $(filter %_$(plat).c %_$(plat).S, $(SRC)) \
)

ifneq ($(filter $(PLATFORM), $(PLATFORMS)), $(PLATFORM))
    $(error "$(PLATFORM) is not a known platform")
endif

SRC := $(filter-out $(PLATFORM_SPECIFIC), $(SRC))

SRC += $(wildcard *$(PLATFORM).c) $(wildcard *$(PLATFORM).S)

ifdef MODE
    ifeq ($(MODE),release)
        CFLAGS += -O3
        MODE_FLAG := release
    else ifeq ($(MODE),release-native)
        CFLAGS += -O3 -march=native
        MODE_FLAG := release-native
    else ifeq ($(MODE),debug)
        CFLAGS += -g
        MODE_FLAG := debug
    else
    	$(error "$(MODE) is not a known mode")
    endif
else
    CFLAGS += -O2 -g
    MODE_FLAG := default
endif

MODE_FLAG += $(PLATFORM)

OBJ_DIR := obj
OBJ := $(patsubst %.c, $(OBJ_DIR)/%.o, $(filter %.c, $(SRC))) \
       $(patsubst %.S, $(OBJ_DIR)/%.o, $(filter %.S, $(SRC)))

MODE_FILE := $(OBJ_DIR)/.mode

OUT := libcodin.a

all: build

build: check_mode $(OUT)

.NOTPARALLEL: check_mode
.NOTPARALLEL: build

check_mode:
	@if [ -f "$(MODE_FILE)" ] && [ "$$(cat $(MODE_FILE))" != "$(MODE_FLAG)" ]; then \
	    $(MAKE) clean; \
	    $(MAKE) --no-print-directory build; \
	    exit 0; \
	fi
	@mkdir -p $(OBJ_DIR)
	@echo "$(MODE_FLAG)" > $(MODE_FILE)

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.S | $(OBJ_DIR)
	$(AS) $(ASFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)

install: build
	sudo cp $(OUT) /usr/lib/$(OUT)
	sudo rm -rf /usr/include/codin
	sudo mkdir  /usr/include/codin
	sudo cp *.h /usr/include/codin
	sudo ldconfig

uninstall:
	sudo rm -rf /usr/include/codin
	sudo rm /usr/lib/$(OUT)
	sudo ldconfig

clean:
	rm -rf $(OBJ_DIR) $(OUT)
