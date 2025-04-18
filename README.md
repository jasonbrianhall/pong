# SDL2 Pong Game

A classic Pong game implementation using SDL2 with cross-platform support for Linux and Windows.

## Features

- Smooth gameplay at 60 FPS
- Single-player mode with adjustable AI difficulty
- Two-player mode for local multiplayer
- Joystick/gamepad support (first player only)
- Cross-platform (Linux and Windows)
- Automatic font detection and loading

## Controls

### Player 1 (Left Paddle)
- **W key**: Move paddle up
- **S key**: Move paddle down
- **Joystick**: Use vertical axis to control paddle movement

### Player 2 (Right Paddle)
- **Up arrow**: Move paddle up
- **Down arrow**: Move paddle down
- Can be controlled by AI with adjustable difficulty

### Game Controls
- **1**: Set AI difficulty to Easy
- **2**: Set AI difficulty to Medium
- **3**: Set AI difficulty to Hard

## Building from Source

### Linux

Prerequisites:
- GCC compiler
- SDL2 development libraries
- SDL2_ttf development libraries

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install libsdl2-dev libsdl2-ttf-dev g++

# Install dependencies (Fedora)
sudo dnf install SDL2-devel SDL2_ttf-devel gcc-c++

# Compile the game
make linux
```

The executable will be created in `build/linux/pong`.

### Windows (Cross-compilation from Linux)

Prerequisites:
- MinGW-w64 cross-compiler
- SDL2 and SDL2_ttf libraries for MinGW

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install mingw-w64 mingw-w64-tools

# Install dependencies (Fedora)
sudo dnf install mingw64-gcc mingw64-gcc-c++ mingw64-SDL2 mingw64-SDL2_ttf

# Compile the game for Windows
make windows
```

The executable and required DLLs will be created in `build/windows/`.

### Debug Builds

```bash
# Linux debug build
make linux-debug

# Windows debug build
make windows-debug
```

## Command Line Options

```
Pong Game Usage:
  --2player, -2   : Two-player mode
  --easy, -e      : Set AI difficulty to Easy
  --hard, -h      : Set AI difficulty to Hard
  --help          : Show help message
```

## Automatic Builds with GitHub Actions

This project includes a GitHub Actions workflow that automatically builds the game for both Linux and Windows whenever changes are pushed to the repository. The workflow:

1. Builds the Linux version
2. Cross-compiles the Windows version
3. Creates a self-contained Windows package with all necessary DLLs
4. Uploads build artifacts
5. Creates GitHub releases with versioned builds

## Project Structure

```
.
├── build/              # Build outputs
│   ├── linux/          # Linux build
│   ├── linux_debug/    # Linux debug build
│   ├── windows/        # Windows build
│   └── windows_debug/  # Windows debug build
├── .github/workflows/  # GitHub Actions workflow files
├── pong.cpp            # Main source code
└── Makefile            # Build configuration
```

## License

[MIT License](LICENSE.md)

## Credits

Created by Jason Hall (jbhall) - 2025
