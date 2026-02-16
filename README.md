# 2D Shooter Game (C, SDL2)

## Overview

A modular real-time 2D shooter game developed in C using SDL2, SDL_image, and SDL_ttf.

The project demonstrates low-level systems programming, real-time game loop design, dynamic memory management, and collision detection.

---

## Features

- State-driven game loop (Title Screen and Gameplay)
- Delegate-based separation of logic and rendering
- Custom linked-list entity management system
- Player and enemy mechanics
- Projectile system with vector-based targeting
- Axis-Aligned Bounding Box (AABB) collision detection
- Particle-based explosion and debris effects
- Parallax scrolling background
- Text rendering using SDL_ttf
- Persistent high score using file I/O
- Explicit memory cleanup of all allocated resources

---

## Architecture

The codebase is modular and structured as follows:

- `main.c` – Game loop and state management
- `init.c` – SDL initialization
- `input.c` – Keyboard input handling
- `stage.c` – Gameplay logic and entity updates
- `draw.c` – Rendering pipeline
- `text.c` – Font initialization and text rendering
- `util.c` – Collision detection and vector math

---

## Technologies

- C
- SDL2
- SDL_image
- SDL_ttf

---

## Build Instructions

Ensure SDL2, SDL_image, and SDL_ttf are installed.

Example using GCC:

```bash
gcc *.c -lSDL2 -lSDL2_image -lSDL2_ttf -o shooter
./shooter