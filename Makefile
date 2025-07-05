.DEFAULT_GOAL := macos-arm64
.SILENT:

# === GNU Standard Variables ===
INSTALL ?= install    # GNU: support 'install' variable
prefix ?= /usr/local  # GNU: standard install prefix
bindir ?= $(prefix)/bin

# === Project Structure ===

# Common Folders
SRC_DIR       := src
OBJ_DIR       := obj
BIN_DIR       := bin
DST_DIR       ?= dist
INCLUDE_DIR   := include

# Common Defaults
CC          ?= clang
CPPFLAGS    := -I$(INCLUDE_DIR)
CFLAGS      += -std=c99 -Wall -Wextra -Wpedantic
CFLAGS      += -g -O0
CFLAGS      += -Wshadow -Wconversion -Wsign-conversion
CFLAGS      += -Wfloat-equal -Wstrict-prototypes -Wundef -Wredundant-decls
CFLAGS      += -fno-omit-frame-pointer
LDFLAGS     += -Wl,-fatal_warnings

# Version header generation for release numbering
$(INCLUDE_DIR):
	mkdir -p $@

GIT_HASH := $(shell git rev-parse --short HEAD)
GIT_BRANCH := $(shell git rev-parse --abbrev-ref HEAD)

VERSION_H := $(INCLUDE_DIR)/version.h

# Make builds depend on the version header
ALL_TARGETS := macos windows linux

all: $(VERSION_H) $(ALL_TARGETS)

# Make every target depend on version.h
macos: $(VERSION_H) macos-arm64 macos-x86_64 macos-universal
windows: $(VERSION_H) windows-x86_64 windows-i686 windows-arm64
linux: $(VERSION_H) linux-x86_64

$(VERSION_H): | $(INCLUDE_DIR)
	@echo "Generating version header: $@"
	@if [ -z "$(PROGRAM_VERSION)" ]; then \
		cat > $@ << EOF
#ifndef VERSION_H
#define VERSION_H

#define GIT_COMMIT_HASH "$(GIT_HASH)"
#define GIT_BRANCH "$(GIT_BRANCH)"

#endif /* VERSION_H */
EOF
	else \
		cat > $@ << EOF
#ifndef VERSION_H
#define VERSION_H

#define PROGRAM_VERSION "$(PROGRAM_VERSION)"
#define GIT_COMMIT_HASH "$(GIT_HASH)"
#define GIT_BRANCH "$(GIT_BRANCH)"

#endif /* VERSION_H */
EOF
	fi

.PHONY: all install uninstall distclean check macos-arm64 macos-x86_64 macos-universal \
        windows windows-x86_64 windows-arm64 windows-i686 linux linux-x86_64 \
		clean clean-macos clean-windows clean-linux clean-test test

# === Meta Targets ===

all: macos windows linux

install:
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) -m 755 $(BIN_DIR)/macos/universal $(DESTDIR)$(bindir)/yourprog  # GNU: standard install

uninstall:
	rm -f $(DESTDIR)$(bindir)/yourprog  # GNU: uninstall target

distclean: clean
	rm -f $(VERSION_H)  # GNU: remove generated config files too

check: test  # GNU: 'check' is the standard name for running tests

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# === Source Files ===

ALL_SRC := $(wildcard $(SRC_DIR)/*.c)
SRCS := $(filter-out $(SRC_DIR)/main.c, $(ALL_SRC))

# platform-specific
MACOS_SRCS := $(SRCS) $(SRC_DIR)/platform/os_mac.c $(SRC_DIR)/main.c
WIN_SRCS   := $(SRCS) $(SRC_DIR)/platform/os_win.c $(SRC_DIR)/main.c
LINUX_SRCS := $(SRCS) $(SRC_DIR)/platform/os_linux.c $(SRC_DIR)/main.c

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

$(MACOS_OBJ_DIR_arm64)/%.o: $(SRC_DIR)/%.c $(VERSION_H)
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

$(MACOS_OBJ_DIR_x86_64)/%.o: $(SRC_DIR)/%.c $(VERSION_H)
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
	rm -rf $(MACOS_OBJ_DIR_arm64) $(MACOS_OBJ_DIR_x86_64) $(MACOS_BIN_DIR)

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
WIN_OBJ_DIR_arm64 := $(OBJ_DIR)/win-arm64
WIN_OBJS_arm64 := $(patsubst $(SRC_DIR)/%.c,$(WIN_OBJ_DIR_arm64)/%.o,$(WIN_SRCS))
WIN_OBJ_DIRS := $(WIN_OBJ_DIR_x86_64) $(WIN_OBJ_DIR_i686) $(WIN_OBJ_DIR_arm64)

$(OBJ_DIR):
	@mkdir -p $@

$(WIN_OBJ_DIRS): | $(OBJ_DIR)
	@mkdir -p $@

$(WIN_OBJ_DIR_x86_64)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR) $(VERSION_H)
	@mkdir -p $(dir $@)
	$(WIN_CC) $(CPPFLAGS) $(WIN_CFLAGS_x86_64) -c $< -o $@

$(WIN_OBJ_DIR_i686)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR) $(VERSION_H)
	@mkdir -p $(dir $@)
	$(WIN_CC) $(CPPFLAGS) $(WIN_CFLAGS_i686) -c $< -o $@

$(WIN_OBJ_DIR_arm64)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR) $(VERSION_H)
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

# === Cross-compilation to Linux ===

linux: linux-x86_64

LINUX_BIN_DIR := $(BIN_DIR)/linux
LINUX_OBJ_DIR := $(OBJ_DIR)/linux-x86_64
LINUX_BIN     := $(LINUX_BIN_DIR)/x86_64
LINUX_OBJS    := $(patsubst $(SRC_DIR)/%.c,$(LINUX_OBJ_DIR)/%.o,$(LINUX_SRCS))

linux-x86_64: $(LINUX_BIN)

$(LINUX_BIN): $(LINUX_OBJS) | $(LINUX_BIN_DIR)
	@mkdir -p $(dir $@)
	@echo "Linking Linux x86_64 binary"
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(LINUX_OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(VERSION_H)
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(LINUX_BIN_DIR):
	@mkdir -p $@

# Distribution copies
dist: clean all $(VERSION_H) | $(DST_DIR)
	@echo "Copying and renaming binaries to $(DST_DIR)/"

	# Copy the universal macOS binary only, with version suffix
	@if [ -f "$(MACOS_BIN_DIR)/universal" ]; then \
		cp -a "$(MACOS_BIN_DIR)/universal" "$(DST_DIR)/typebelow-$(PROGRAM_VERSION)-macos-universal"; \
	fi

	# Optionally copy arch-specific macOS binaries with arch suffixes
	@for arch in arm64 x86_64; do \
		file="$(MACOS_BIN_DIR)/$$arch"; \
		if [ -f "$$file" ]; then \
			cp -a "$$file" "$(DST_DIR)/typebelow-$(PROGRAM_VERSION)-macos-$$arch"; \
		fi; \
	done

	# Copy Windows executables with architecture suffixes
	@for file in $(WIN_BIN_DIR)/*.exe; do \
		if [ -f "$$file" ]; then \
			base=$$(basename $$file .exe); \
			cp "$$file" "$(DST_DIR)/typebelow-$(PROGRAM_VERSION)-windows-$$base.exe"; \
		fi \
	done

	# Similarly, you could add Linux binaries if desired
	@if [ -f "$(LINUX_BIN_DIR)/x86_64" ]; then \
		cp "$(LINUX_BIN_DIR)/x86_64" "$(DST_DIR)/typebelow-$(PROGRAM_VERSION)-linux-x86_64"; \
	fi

# GitHub release automation
ifeq ($(findstring -, $(PROGRAM_VERSION)),)
  NOTE ?= "Automated release of version $(PROGRAM_VERSION)"
else
  NOTE ?= "Beta prerelease of version $(PROGRAM_VERSION)"
endif
IS_PRERELEASE := $(findstring -, $(PROGRAM_VERSION))
RELEASE_FLAGS := $(if $(IS_PRERELEASE),--prerelease,)

release: dist
	gh release create $(PROGRAM_VERSION) \
		--title "Release $(PROGRAM_VERSION)" \
		--notes "$(NOTE)" \
		$(RELEASE_FLAGS) \
		$(wildcard $(DST_DIR)/*)

# === Testing ===

TEST_DIR       := test
TEST_SRC       := $(TEST_DIR)/test.c
TEST_BIN       := $(BIN_DIR)/test
TEST_FILE      := $(TEST_DIR)/testfile.txt

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

clean-test:
	rm -f $(TEST_BIN) $(TEST_FILE)
