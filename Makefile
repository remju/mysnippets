CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wswitch-enum -Werror -std=c17
DBG := gdb

SRC_DIR := src
BUILD_DIR := build
SRCS := $(wildcard $(SRC_DIR)/*.c)
INCLUDES := $(wildcard $(INCLUDE_DIR)/*.h)

TARGET_NAME := mysnippets
TARGET := $(BUILD_DIR)/$(TARGET_NAME)
DGB_TARGET := $(BUILD_DIR)/$(TARGET_NAME).debug

.PHONY: dgb test install clean

$(TARGET): $(SRCS) $(INCLUDES) $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

dbg: $(DGB_TARGET)
$(DGB_TARGET): $(SRCS) $(INCLUDES) $(BUILD_DIR)
	$(CC) $(CFLAGS) -g -o $@ $(SRCS)

$(BUILD_DIR):
	@mkdir -p $@

test: $(TARGET)
	@./build/mysnippets save mysnippets.test 'test'
	@./build/mysnippets get mysnippets.test 1>/dev/null
	@rm ~/.mysnippets/mysnippets.test

install: $(SRCS) $(INCLUDES)
	$(CC) $(CFLAGS) -O3 -o /usr/local/bin/$(TARGET_NAME) $(SRCS)

clean:
	rm -rf $(BUILD_DIR)