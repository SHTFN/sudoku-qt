# Sudoku Qt

Sudoku Qt is a desktop Sudoku application written in C++ with Qt Widgets. The project includes a playable graphical interface, board validation, save/load support, a timer, undo/redo history, candidate notes, hints, and several solving strategies.

## Features

- 9x9 Sudoku board with interactive cell input.
- Move validation and solved-board detection.
- Candidate mode for pencil-mark notes.
- Automatic candidate generation.
- Undo and redo for game moves.
- Built-in hint system based on Sudoku solving strategies.
- Save and load games as JSON files.
- Game timer.
- Interface translations for English, Russian, and Romanian.

## Project Structure

```text
include/       Public headers grouped by module
src/           Application source files
test/          GoogleTest-based unit tests
tests/         Test data, including sample boards
translations/  Qt translation files
resources.qrc  Qt resource file
qt.pro         qmake project file
```

Main modules:

- `core` - Sudoku board model, timer, errors, and undo/redo stack.
- `game` - game state and high-level game actions.
- `solver` - validation and solving/hint logic.
- `storage` - JSON save/load support.
- `ui` - Qt Widgets interface.

## Requirements

- Qt 5.12 or newer.
- C++20-compatible compiler.
- qmake.
- GoogleTest only if you want to build the test target.

The project was built with Qt 5.12.12 on macOS using the `clang_64` kit.

## Build the Application

From the project root:

```bash
qmake qt.pro CONFIG+=release CONFIG+=solution
make -j8
```

The `CONFIG+=solution` flag is required because the GUI application sources are enabled in the `solution` block of `qt.pro`.

On macOS, Qt Creator usually creates a separate build directory, for example:

```text
~/Desktop/build-qt-Desktop_Qt_5_12_12_clang_64bit-Release
```

The built application will be:

```text
qt.app
```

Run it with:

```bash
open qt.app
```

## Package for macOS

To run the app on another Mac, deploy the Qt frameworks into the `.app` bundle:

```bash
cd ~/Desktop/build-qt-Desktop_Qt_5_12_12_clang_64bit-Release
/Users/stefanalbu/Qt5.12.12/5.12.12/clang_64/bin/macdeployqt qt.app
```

Then create an archive:

```bash
ditto -c -k --keepParent qt.app qt-macos-release.zip
```

Send `qt-macos-release.zip` to another user. After extracting it, they can launch `qt.app`.

## Build on Windows

Open `qt.pro` in Qt Creator and add these qmake arguments:

```text
CONFIG+=release CONFIG+=solution
```

For an MSVC kit, build with:

```powershell
qmake qt.pro CONFIG+=release CONFIG+=solution
nmake -f Makefile.Release
```

For a MinGW kit, use:

```powershell
qmake qt.pro CONFIG+=release CONFIG+=solution
mingw32-make
```

After building, deploy Qt DLLs:

```powershell
windeployqt release\qt.exe
```

Distribute the whole `release` directory, not only `qt.exe`.
