# Console Pac-Man

A playable Pac-Man clone that runs entirely in the Windows terminal, written in C. Pac-Man moves in real time on each keypress, two ghosts roam the maze and hunt him down when they have a clear line of sight, and the board is redrawn in colour every turn.

---

## Features

- **Line-of-sight ghost AI:** each ghost wanders the maze and only turns when it hits a wall. When Pac-Man is in the same row or column with no walls in between, the ghost locks on and chases him.
- **Dot-preserving ghosts:** a ghost "holds" any dot it passes over and puts it back when it moves on, so ghosts never eat the player's dots.
- **Collision detection that can't be dodged:** the game checks both where each ghost is now and where it just was. Pac-Man can't slip past a ghost by swapping squares with it in the same turn.
- **File-driven maps:** the maze layout is loaded from `map.txt` at startup, and the game builds the outer walls itself. You can change the level without touching the code.
- **Coloured rendering:** walls are blue, Pac-Man is yellow, ghosts are pink, and dots are white.
- **Win and lose states:** clear every dot to win. Get caught by a ghost and the game ends.

---

## How to Play

| Key | Action |
|---|---|
| `W` | Move up |
| `A` | Move left |
| `S` | Move down |
| `D` | Move right |

Eat all the dots to win. Avoid the ghosts (`G`).

| Symbol | Meaning |
|---|---|
| `P` | Pac-Man |
| `G` | Ghost |
| `.` | Dot |
| `W` | Wall |

---

## Building and Running

The game uses `conio.h` for instant keypress input, so it runs on **Windows**.

With MinGW / g++:

```bash
g++ pacman.cpp colours.cpp -o pacman.exe
pacman.exe
```

Keep `map.txt` in the same folder as the executable.

---

## Map Format

`map.txt` describes the 9 × 9 interior of the board as 9 rows of 9 characters, separated by spaces. The outer walls are added automatically, making the full board 11 × 11. Pac-Man starts in the centre, and the two ghosts start in the top-left and bottom-right corners.

---

## How It Works

Each turn runs in this order:

1. **Ghosts pick a direction.** A ghost keeps its current heading unless the next square is a wall, in which case it picks a new random direction until it finds an open one.
2. **Ghosts check for Pac-Man.** If Pac-Man is in the same row or column and the path between them has no walls, the ghost turns toward him.
3. **Ghosts move**, picking up or putting back any dot they're standing on.
4. **Pac-Man moves** based on the key pressed. Moves into walls are blocked.
5. **The game checks for a win or loss** and redraws the board.

The board is stored as a dynamically allocated 2D character array. Ghost positions (current and previous) are tracked separately so that collisions are detected correctly.

---

## Project Structure

| File | Purpose |
|---|---|
| `pacman.cpp` | Game loop, movement, ghost AI, collision detection, and rendering |
| `colours.cpp`, `colours.h` | Console colour helper (provided starter code) |
| `map.txt` | Maze layout |
