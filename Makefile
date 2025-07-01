PROGRAM_VERSION ?= E0.0.0
.DEFAULT_GOAL := macos-arm64
.SILENT:

# === Project Structure ===

# Common Folders
SRC_DIR       := src
OBJ_DIR       := obj
BIN_DIR       := bin
INCLUDE_DIR   := include

# Common Defaults
CC          ?= clang
CPPFLAGS    := -I$(INCLUDE_DIR)
CFLAGS      += -std=c99 -Wall -Wextra -Wpedantic
CFLAGS      += -g -O0
CFLAGS      += -Wshadow -Wconversion -Wsign-conversion
CFLAGS      += -Wfloat-equal -Wstrict-prototypes -Wundef -Wredundant-decls
CFLAGS      += -fsanitize=address,undefined -fno-omit-frame-pointer
LDFLAGS     += -fsanitize=address,undefined -Wl,-fatal_warnings

# Version header generation for release numbering
VERSION_H := $(INCLUDE_DIR)/version.h
version_h: | $(INCLUDE_DIR)
	@echo '#ifndef VERSION_H' > $(VERSION_H)
	@echo '#define VERSION_H' >> $(VERSION_H)
	@echo '' >> $(VERSION_H)
	@echo '#define PROGRAM_VERSION "$(PROGRAM_VERSION)"' >> $(VERSION_H)
	@echo '' >> $(VERSION_H)
	@echo '#endif' >> $(VERSION_H)

.PHONY: all macos-arm64 macos-x86_64 macos-universal\
        windows windows-x86_64 windows-arm64 windows-i686 clean clean-macos clean-windows

all: macos windows

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# === Source Files ===

ALL_SRC := $(wildcard $(SRC_DIR)/*.c)
SRCS := $(filter-out $(SRC_DIR)/main.c, $(ALL_SRC))

# platform-specific
MACOS_SRCS := $(SRCS) $(SRC_DIR)/platform/os_mac.c $(SRC_DIR)/main.c
WIN_SRCS   := $(SRCS) $(SRC_DIR)/platform/os_win.c $(SRC_DIR)/main.c

# === Native Compilation for macOS ===

macos: macos-arm64 macos-x86_64 macos-universal

MACOS_BIN_DIR := $(BIN_DIR)/macos

# arm64 architecture
MACOS_CFLAGS_arm64   := -arch arm64
MACOS_LDFLAGS_arm64  := -arch arm64
MACOS_OBJ_DIR_arm64  := $(OBJ_DIR)/macos-arm64
MACOS_BIN_arm64      := $(MACOS_BIN_DIR)/arm64
MACOS_OBJS_arm64     := $(patsubst $(SRC_DIR)/%.c,$(MACOS_OBJ_DIR_arm64)/%.o,$(MACOS_SRCS))

macos-arm64: | $(MACOS_BIN_arm64)

$(MACOS_BIN_arm64): $(MACOS_OBJS_arm64) | $(MACOS_BIN_DIR)
	@mkdir -p $(dir $@)
	@echo "Linking macOS arm64 binary: $@"
	$(CC) $(MACOS_CFLAGS_arm64) $(LDFLAGS) -o $@ $^

$(MACOS_OBJ_DIR_arm64)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(MACOS_CFLAGS_arm64) -c $< -o $@

# x86_64 architecture
MACOS_CFLAGS_x86_64  := -arch x86_64
MACOS_LDFLAGS_x86_64 := -arch x86_64
MACOS_OBJ_DIR_x86_64 := $(OBJ_DIR)/macos-x86_64
MACOS_BIN_x86_64     := $(MACOS_BIN_DIR)/x86_64
MACOS_OBJS_x86_64    := $(patsubst $(SRC_DIR)/%.c,$(MACOS_OBJ_DIR_x86_64)/%.o,$(MACOS_SRCS))

macos-x86_64: | $(MACOS_BIN_x86_64)

$(MACOS_BIN_DIR):
	@mkdir -p $(dir $@)

$(MACOS_BIN_x86_64): $(MACOS_OBJS_x86_64) | $(MACOS_BIN_DIR)
	@mkdir -p $(dir $@)
	@echo "Linking macOS x86_64 binary: $@"
	$(CC) $(MACOS_CFLAGS_x86_64) $(LDFLAGS) -o $@ $^

$(MACOS_OBJ_DIR_x86_64)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(MACOS_CFLAGS_x86_64) -c $< -o $@

# Universal binary created from per-arch binaries
macos-universal: macos-arm64 macos-x86_64
	@echo "Creating macOS universal binary with lipo"
	@lipo -create \
		$(MACOS_BIN_DIR)/arm64 \
		$(MACOS_BIN_DIR)/x86_64 \
		-output $(MACOS_BIN_DIR)/universal

clean-macos:
	rm -rf $(MACOS_OBJ_DIR_arm64) $(MACOS_OBJ_DIR_x86_64) $(MAC_BIN_DIR)

# === Cross-compilation to Windows ===

windows: windows-x86_64 windows-i686 windows-arm64

LLVM_MINGW_ROOT ?= $(HOME)/toolchains/llvm-mingw
WIN_CC         ?= $(LLVM_MINGW_ROOT)/bin/clang

ifeq ("$(wildcard $(LLVM_MINGW_ROOT)/bin/clang)","")
  $(warning LLVM_MINGW not found at $(LLVM_MINGW_ROOT); Windows builds may fail)
endif

WIN_BIN_DIR   := $(BIN_DIR)/windows

# x86_64 (64-bit)
WIN_TARGET_x86_64 := x86_64-w64-windows-gnu
WIN_SYSROOT_x86_64 := $(LLVM_MINGW_ROOT)/x86_64-w64-mingw32
WIN_CFLAGS_x86_64 := --target=$(WIN_TARGET_x86_64) --sysroot=$(WIN_SYSROOT_x86_64)
WIN_LDFLAGS_x86_64 := -fuse-ld=lld -Wl,--entry=mainCRTStartup -Wl,--subsystem,console
WIN_OBJ_DIR_x86_64 := $(OBJ_DIR)/win-x86_64
WIN_OBJS_x86_64 := $(patsubst $(SRC_DIR)/%.c,$(WIN_OBJ_DIR_x86_64)/%.o,$(WIN_SRCS))

# i686 (32-bit)
WIN_TARGET_i686 := i686-w64-windows-gnu
WIN_SYSROOT_i686 := $(LLVM_MINGW_ROOT)/i686-w64-mingw32
WIN_CFLAGS_i686 := --target=$(WIN_TARGET_i686) --sysroot=$(WIN_SYSROOT_i686)
WIN_LDFLAGS_i686 := -fuse-ld=lld -Wl,--entry=mainCRTStartup -Wl,--subsystem,console
WIN_OBJ_DIR_i686 := $(OBJ_DIR)/win-i686
WIN_OBJS_i686 := $(patsubst $(SRC_DIR)/%.c,$(WIN_OBJ_DIR_i686)/%.o,$(WIN_SRCS))

# arm64
WIN_TARGET_arm64 := aarch64-w64-windows-gnu
WIN_SYSROOT_arm64 := $(LLVM_MINGW_ROOT)/aarch64-w64-mingw32
WIN_CFLAGS_arm64 := --target=$(WIN_TARGET_arm64) --sysroot=$(WIN_SYSROOT_arm64)
WIN_LDFLAGS_arm64 := -fuse-ld=lld -Wl,--entry=mainCRTStartup -Wl,--subsystem,console
WIN_OBJ_DIRS := $(WIN_OBJ_DIR_x86_64) $(WIN_OBJ_DIR_i686) $(WIN_OBJ_DIR_arm64)
WIN_OBJ_DIR_arm64 := $(OBJ_DIR)/win-arm64
WIN_OBJS_arm64 := $(patsubst $(SRC_DIR)/%.c,$(WIN_OBJ_DIR_arm64)/%.o,$(WIN_SRCS))

$(OBJ_DIR):
	@mkdir -p $@

$(WIN_OBJ_DIRS): | $(OBJ_DIR)
	@mkdir -p $@

$(WIN_OBJ_DIR_x86_64)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(WIN_CC) $(CPPFLAGS) $(WIN_CFLAGS_x86_64) -c $< -o $@

$(WIN_OBJ_DIR_i686)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(WIN_CC) $(CPPFLAGS) $(WIN_CFLAGS_i686) -c $< -o $@

$(WIN_OBJ_DIR_arm64)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(WIN_CC) $(CPPFLAGS) $(WIN_CFLAGS_arm64) -c $< -o $@

$(WIN_BIN_DIR):
	@mkdir -p $@

windows-x86_64: $(WIN_BIN_DIR) $(WIN_OBJS_x86_64)
	@echo "Linking Windows x86_64 binary"
	$(WIN_CC) --target=$(WIN_TARGET_x86_64) -fuse-ld=lld $(WIN_OBJS_x86_64) $(WIN_LDFLAGS_x86_64) -o $(WIN_BIN_DIR)/x86_64.exe

windows-i686: $(WIN_BIN_DIR) $(WIN_OBJS_i686)
	@echo "Linking Windows i686 binary"
	$(WIN_CC) --target=$(WIN_TARGET_i686) -fuse-ld=lld $(WIN_OBJS_i686) $(WIN_LDFLAGS_i686) -o $(WIN_BIN_DIR)/i686.exe

windows-arm64: $(WIN_BIN_DIR) $(WIN_OBJS_arm64)
	@echo "Linking Windows arm64 binary"
	$(WIN_CC) --target=$(WIN_TARGET_arm64) -fuse-ld=lld $(WIN_OBJS_arm64) $(WIN_LDFLAGS_arm64) -o $(WIN_BIN_DIR)/arm64.exe

clean-windows:
	rm -rf $(WIN_OBJ_DIR_x86_64) $(WIN_OBJ_DIR_i686) $(WIN_OBJ_DIR_arm64) $(WIN_BIN_DIR)

# === Testing ===

TEST_DIR       := test
TEST_SRC       := $(TEST_DIR)/test.c
TEST_BIN       := $(BIN_DIR)/test
TEST_FILE      := $(TEST_DIR)/testfile.txt

.PHONY: test

test: $(TEST_BIN) $(TEST_FILE)
	@echo "Running test suite..."
	@$(TEST_BIN)

$(TEST_BIN): $(TEST_SRC) | $(BIN_DIR)
	@echo "Compiling test binary: $@"
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $<

$(TEST_FILE):
	@mkdir -p $(dir $@)
	@echo "Line 1" >  $@
	@echo "Line 2" >> $@
	@echo "Line 3" >> $@
	@echo "Line 4" >> $@
	@echo "Line 5" >> $@

.PHONY: clean-test

clean-test:
	rm -f $(TEST_BIN) $(TEST_FILE)
