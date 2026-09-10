# MazeMotion

A Qt 6 desktop application in which a ball inside a randomly generated maze is controlled
by physically tilting an Arduino Nano 33 IoT board. IMU data (accelerometer and gyroscope)
is streamed to the computer over a serial port, drives the ball physics, and is plotted in
real time. The board can be rendered in two modes: 2D (QPainter) and 3D (OpenGL 3.3 Core
with Phong lighting).

## Features

- Maze generation with Recursive Backtracking (DFS) - exactly one path between any two cells.
- Goal placed in the cell farthest from the start, found with BFS.
- Ball physics: acceleration from the accelerometer or the keyboard, friction, wall
  collisions with reflection along the surface normal and position correction.
- Keyboard fallback with the arrow keys - works without a connected sensor.
- 2D / 3D view switching during play; both views share the same game logic.
- Live charts of linear acceleration [g] and angular velocity [°/s] (Qt Charts) with a
  sliding window of 200 samples.
- CSV frames received with an XOR checksum - corrupted frames are discarded.

## Architecture

```
main.cpp                 entry point, QApplication
src/mainwindow.*         main window: toolbar, port selection, splitter, 2D/3D QStackedWidget
src/gamelogic.*          ball physics, collisions, game state, 16 ms timer (~60 Hz)
src/mazegenerator.*      maze generator (DFS) and goal selection (BFS)
src/gamewidget.*         2D rendering (QPainter) and keyboard handling
src/gamewidget3d.*       3D rendering (QOpenGLWidget, VAO/VBO, Phong, 4x MSAA)
src/sensorreader.*       serial port, buffering and parsing of IMU frames
src/chartpanel.*         accelerometer and gyroscope charts
hardware/GyroAcc/        Arduino sketch that streams IMU data
docs/                    Doxygen configuration and generated HTML documentation
```

The game logic is fully separated from the rendering layer. `GameLogic` emits the
`stateUpdated`, `mazeChanged` and `gameWonSignal` signals, which both widgets react to -
switching the view neither interrupts nor resets the game. Board, wall, ball and goal
coordinates are normalized to the 0.0-1.0 range, so they are independent of the window
size and shared by both rendering modes.

## Communication protocol

The board sends one frame roughly every 16 ms at 115200 baud:

```
ax,ay,az,gx,gy,gz*CS\r\n
```

- `ax, ay, az` - linear acceleration in [g], two decimal places
- `gx, gy, gz` - angular velocity in [°/s], two decimal places
- `CS` - XOR checksum of all bytes before the `*` character, two hexadecimal digits

Example: `-0.12,0.98,0.05,1.20,-0.34,0.11*3F`

## Requirements

- Qt 6.5 or newer, modules: Core, Widgets, SerialPort, Charts, OpenGLWidgets
- CMake 3.19 or newer, a C++17 capable compiler
- OpenGL 3.3 Core Profile (for the 3D view) and GLU
- Arduino Nano 33 IoT with the `Arduino_LSM6DS3` library (optional - keyboard control
  works without the board)

## Building

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/MazeMotion
```

## Running with the sensor

1. Upload `hardware/GyroAcc/GyroAcc.ino` to the Arduino Nano 33 IoT board.
2. Connect the board over USB and start the application.
3. Pick the serial port from the list (e.g. `ttyACM0`) and press **Połącz** (Connect).
4. Tilt the board to move the ball; **3D** switches the rendering mode and
   **Generuj labirynt** (New maze) creates a new board.

On Linux, access to the serial port may require adding the user to the `dialout` group.

## Controls

| Element | Action |
|---|---|
| Tilting the board | moves the ball (acceleration from the accelerometer) |
| Arrow keys | move the ball without the sensor |
| **3D** / **2D** button | switches the view |
| **Generuj labirynt** button | new maze and position reset |
| **Połącz** / **Rozłącz** button | opens and closes the serial port |

## Documentation

The code is annotated with Doxygen comments. To generate the documentation:

```bash
cd docs && doxygen Doxyfile
```

The output is written to `docs/html/index.html`.

## Note on language

The user interface labels and the in-code Doxygen comments are in Polish; this README is
in English and gives the English meaning of each button label in parentheses.
