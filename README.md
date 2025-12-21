# Simple Raylib FPS Shooter

A basic First-Person Shooter (FPS) prototype built using C++ and [Raylib](https://www.raylib.com/).

## Project Overview

This project demonstrates how to create a 3D environment, implement a first-person camera, handle collision detection, and create simple gameplay mechanics (shooting) using Raylib's easy-to-use API.

## Code Structure & Raylib 3D Concepts

The core logic is contained in `main.cpp`. Here's a breakdown of how the 3D magic happens:

### 1. The Camera (`Camera3D`)
In Raylib, a 3D scene requires a `Camera3D` struct.
```cpp
Camera camera = { 0 };
camera.position = (Vector3){ 0.0f, 2.0f, 4.0f }; // Where you are
camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };   // Where you look
camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };       // Vertical axis
camera.fovy = 60.0f;                             // Field of View
camera.projection = CAMERA_PERSPECTIVE;
```

### 2. Player Movement (`UpdateCamera`)
Raylib provides a helper function to handle complex camera math (pitch, yaw, movement) automatically.
```cpp
// CAMERA_FIRST_PERSON mode handles WASD movement and Mouse Look
UpdateCamera(&camera, CAMERA_FIRST_PERSON);
```

### 3. Rendering 3D Objects (`BeginMode3D`)
To draw 3D shapes, we must switch the rendering context to 3D mode using the camera we defined.
```cpp
BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);
        // All 3D drawing happens here!
        DrawPlane(...); // The ground
        DrawCube(...);  // Walls and enemies
    EndMode3D();

    // 2D drawing (UI, Gun overlay) happens here
EndDrawing();
```

### 4. Shooting Mechanics (Raycasting logic)
Since this is a simple prototype, "shooting" is calculated using vector math.
- **Direction**: A vector from the camera position to the camera target.
- **Collision**: Checks if the "bullet" point overlaps with the Enemy cube's 3D coordinates (AABB collision).

## How to Compile

You need a C++ compiler (like GCC/MinGW) and the Raylib library installed.

**Example Command (MinGW):**
```bash
g++ main.cpp -o fps_game.exe -lraylib -lopengl32 -lgdi32 -lwinmm
```
*Note: Ensure your compiler can find the Raylib headers and library files.*

## Controls
- **Space**: Jump
- **1 / 2**: Switch Weapons (Rifle / Revolver)
- **R**: Reload (Manual)

## Customizing Muzzle Flash

You can adjust the position of the muzzle flash by editing `main.cpp`. Look for the initialization section:

```cpp
// Rifle
weapons[0].flashOffsetX = 60;   
weapons[0].flashOffsetY = 100;  

// Revolver
weapons[1].flashOffsetX = 60; 
weapons[1].flashOffsetY = 110; 
```

### How Coordinates (Pixels) Work
In computer graphics (and this game), the screen coordinates work like a grid:
- **X Axis (Horizontal)**: 0 is the Left side. Increasing the value moves **Right**.
- **Y Axis (Vertical)**: 0 is the Top. Increasing the value moves **Down**.

**Visual Guide:**
- To move the flash **Left**, **Decrease** `flashOffsetX`.
- To move the flash **Right**, **Increase** `flashOffsetX`.
- To move the flash **Up**, **Decrease** `flashOffsetY`.
- To move the flash **Down**, **Increase** `flashOffsetY`.

Start by changing values by 10 or 20 pixels at a time to fine-tune the position.

After changing these values, re-compile the game to see the changes.

