# Makefile for Pong Game with Windows and Linux Support

# Compiler settings
CXX_LINUX = g++
CXX_WIN = x86_64-w64-mingw32-g++
CXXFLAGS_COMMON = -std=c++17 -Wall -Wextra

# Debug flags
DEBUG_FLAGS = -g -DDEBUG

# SDL configuration (Linux)
SDL_CFLAGS_LINUX := $(shell sdl2-config --cflags)
SDL_LIBS_LINUX := $(shell sdl2-config --libs)

# SDL configuration (Windows)
SDL_CFLAGS_WIN := $(shell mingw64-pkg-config --cflags sdl2)
SDL_LIBS_WIN := $(shell mingw64-pkg-config --libs sdl2)

# Additional SDL libraries
SDL_TTF_LIBS_LINUX = -lSDL2_ttf
SDL_TTF_LIBS_WIN = -lSDL2_ttf

# Source files
SRCS = pong.cpp

# Output names
TARGET_LINUX = pong
TARGET_WIN = pong.exe
TARGET_LINUX_DEBUG = pong_debug
TARGET_WIN_DEBUG = pong_debug.exe

# Build directories
BUILD_DIR = build
BUILD_DIR_LINUX = $(BUILD_DIR)/linux
BUILD_DIR_WIN = $(BUILD_DIR)/windows
BUILD_DIR_LINUX_DEBUG = $(BUILD_DIR)/linux_debug
BUILD_DIR_WIN_DEBUG = $(BUILD_DIR)/windows_debug

# DLL collection script
COLLECT_DLLS_SCRIPT = build/windows/collect_dlls.sh

# Windows DLL settings
DLL_SOURCE_DIR = /usr/x86_64-w64-mingw32/sys-root/mingw/bin

# Platform-specific compiler flags
CXXFLAGS_LINUX = $(CXXFLAGS_COMMON) $(SDL_CFLAGS_LINUX) -DLINUX
CXXFLAGS_WIN = $(CXXFLAGS_COMMON) $(SDL_CFLAGS_WIN) -DWIN32

# Debug-specific flags
CXXFLAGS_LINUX_DEBUG = $(CXXFLAGS_LINUX) $(DEBUG_FLAGS)
CXXFLAGS_WIN_DEBUG = $(CXXFLAGS_WIN) $(DEBUG_FLAGS)

# Linker flags
LDFLAGS_LINUX = $(SDL_LIBS_LINUX) $(SDL_TTF_LIBS_LINUX)
LDFLAGS_WIN = $(SDL_LIBS_WIN) $(SDL_TTF_LIBS_WIN) -lwinmm -static-libgcc -static-libstdc++

# Create necessary directories
$(shell mkdir -p $(BUILD_DIR_LINUX) $(BUILD_DIR_WIN) \
	$(BUILD_DIR_LINUX_DEBUG) $(BUILD_DIR_WIN_DEBUG) \
	build/windows)

# Default target
.PHONY: all
all: linux

# OS-specific builds
.PHONY: linux
linux: $(BUILD_DIR_LINUX)/$(TARGET_LINUX)

.PHONY: windows
windows: $(BUILD_DIR_WIN)/$(TARGET_WIN) collect-windows-dlls

# Debug builds
.PHONY: linux-debug
linux-debug: $(BUILD_DIR_LINUX_DEBUG)/$(TARGET_LINUX_DEBUG)

.PHONY: windows-debug
windows-debug: $(BUILD_DIR_WIN_DEBUG)/$(TARGET_WIN_DEBUG) collect-windows-debug-dlls

# Linux release build
$(BUILD_DIR_LINUX)/$(TARGET_LINUX): pong.cpp
	$(CXX_LINUX) $(CXXFLAGS_LINUX) $< -o $@ $(LDFLAGS_LINUX)

# Windows release build
$(BUILD_DIR_WIN)/$(TARGET_WIN): pong.cpp
	$(CXX_WIN) $(CXXFLAGS_WIN) $< -o $@ $(LDFLAGS_WIN)

# Linux debug build
$(BUILD_DIR_LINUX_DEBUG)/$(TARGET_LINUX_DEBUG): pong.cpp
	$(CXX_LINUX) $(CXXFLAGS_LINUX_DEBUG) $< -o $@ $(LDFLAGS_LINUX)

# Windows debug build
$(BUILD_DIR_WIN_DEBUG)/$(TARGET_WIN_DEBUG): pong.cpp
	$(CXX_WIN) $(CXXFLAGS_WIN_DEBUG) $< -o $@ $(LDFLAGS_WIN)

# Collect Windows DLLs
.PHONY: collect-windows-dlls
collect-windows-dlls: $(BUILD_DIR_WIN)/$(TARGET_WIN)
	@echo "Collecting DLLs for Pong..."
	@chmod +x $(COLLECT_DLLS_SCRIPT)
	@$(COLLECT_DLLS_SCRIPT) $(BUILD_DIR_WIN)/$(TARGET_WIN) $(DLL_SOURCE_DIR) $(BUILD_DIR_WIN)

# Collect Windows Debug DLLs
.PHONY: collect-windows-debug-dlls
collect-windows-debug-dlls: $(BUILD_DIR_WIN_DEBUG)/$(TARGET_WIN_DEBUG)
	@echo "Collecting Debug DLLs for Pong..."
	@chmod +x $(COLLECT_DLLS_SCRIPT)
	@$(COLLECT_DLLS_SCRIPT) $(BUILD_DIR_WIN_DEBUG)/$(TARGET_WIN_DEBUG) $(DLL_SOURCE_DIR) $(BUILD_DIR_WIN_DEBUG)

# Clean target
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# Help target
.PHONY: help
help:
	@echo "Pong Game Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make or make linux       - Build for Linux (release)"
	@echo "  make windows             - Build for Windows (release) and collect DLLs"
	@echo "  make linux-debug         - Build for Linux (debug)"
	@echo "  make windows-debug       - Build for Windows (debug) and collect DLLs"
	@echo "  make collect-windows-dlls     - Collect Windows DLLs"
	@echo "  make collect-windows-debug-dlls - Collect Windows Debug DLLs"
	@echo "  make clean               - Remove all build files"
	@echo "  make help                - Show this help message"
