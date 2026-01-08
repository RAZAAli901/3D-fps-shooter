# Raylib FPS Shooter - WWII Industrial Bunker

A First-Person Shooter (FPS) prototype built using C++ and [Raylib](https://www.raylib.com/), featuring a WWII-era industrial bunker level with atmospheric effects, multiple rooms, and tactical gameplay.

![Game Engine: Raylib](https://img.shields.io/badge/Engine-Raylib-red)
![Language: C++](https://img.shields.io/badge/Language-C++-blue)
![Platform: Windows](https://img.shields.io/badge/Platform-Windows-green)

---

## 📂 Project Structure

```
FPS shooter/
├── main.cpp           # Complete game source (880+ lines)
├── main.exe           # Compiled executable
├── README.md          # This documentation
└── resources/
    ├── gun.png        # Rifle sprite
    ├── revolver.png   # Revolver sprite
    └── muzzle_flash.png
```

---

## 🗺️ Level Map

```
═══════════════════════════════════════════════════════════════════════════════
                        WWII INDUSTRIAL BUNKER - TOP-DOWN MAP
═══════════════════════════════════════════════════════════════════════════════

    NORTH
      ↑
WEST ←─┼─→ EAST        Scale: Each █ = ~2 game units
      ↓
    SOUTH

┌─────────────────────────────────────────────────────────────────────────────┐
│                                                                             │
│   ╔═══════════════╗            ╔═══════════════════════════╗                │
│   ║ LEFT HALL     ║            ║    CENTRAL ROOM           ║                │
│   ║               ║            ║                           ║                │
│   ║   ┌───┐       ║            ║  ┌───┐           ┌───┐    ║                │
│   ║   │ ■ │  [C]  ║            ║  │ ■ │   ┌───┐   │ ■ │    ║                │
│   ║   └───┘       ║            ║  └───┘   │ ■ │   └───┘    ║                │
│   ║               ║            ║          └───┘            ║                │
│   ║   ~~~~        ╠════════════╣     ╔═══════════╗         ║                │
│   ║   PIPE        ║   DOOR     ║     ║ STAIRWELL ║         ║                │
│   ║               ║   (4 units)║     ║   ↗ ↗ ↗   ║  [E]    ║                │
│   ║   [T]         ║            ║     ║   ↗ ↗ ↗   ║         ║                │
│   ║               ╠════════════╣     ║   ↗ ↗ ↗   ║         ╠════════════════╣
│   ║   ┌───┐       ║            ║     ╚═══════════╝         ║   CORRIDOR     ║
│   ║   │ ■ │       ║            ║          ┌───┐            ╠════════════════╣
│   ║   └───┘       ║            ║  ┌───┐   │ ■ │   ┌───┐    ║                │
│   ║               ║            ║  │ ■ │   └───┘   │ ■ │    ║                │
│   ║  [C][C] [E]   ║            ║  └───┘           └───┘    ║                │
│   ║   stacked     ║            ║                    [S]    ║                │
│   ║               ║            ║  [C]        [D]           ║                │
│   ╚═══════════════╝            ╚═══════════════════════════╝                │
│                                                                             │
│                                                                             │
│                                ╔═════════════════════════╗                  │
│                                ║     RIGHT ROOM          ║                  │
│                                ║                         ║                  │
│                                ║  ┌───┐           ┌───┐  ║                  │
│                                ║  │ ■ │   [T]     │ ■ │  ║                  │
│                                ║  └───┘           └───┘  ║                  │
│                                ║         [E]             ║                  │
│                                ║  [C]            ~~~~    ║                  │
│                                ║                 PIPE    ║                  │
│                                ║  ┌───┐    [E]   ┌───┐   ║                  │
│                                ║  │ ■ │  [C][C]  │ ■ │   ║                  │
│                                ║  └───┘  stacked └───┘   ║                  │
│                                ║              [SHELF]    ║                  │
│                                ║  [D]                    ║                  │
│                                ╚═════════════════════════╝                  │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
                                    LEGEND
═══════════════════════════════════════════════════════════════════════════════

  ┌───┐
  │ ■ │  = Concrete Support Pillar (blocks movement, provides cover)
  └───┘

  [C]    = Wooden Crate (cover position, large ones block movement)
  [T]    = Wooden Table
  [S]    = Metal Shelf
  [D]    = Debris Pile
  [E]    = Enemy Spawn Point (5 total)
  
  ~~~~   = Wall-mounted Pipes
  ════   = Doorway/Passage (wide enough for easy navigation)
  ↗ ↗    = Stairs going UP (climbable)

═══════════════════════════════════════════════════════════════════════════════
```

### Room Dimensions

| Room | Width (X) | Depth (Z) | Height (Y) | Pillars |
|------|-----------|-----------|------------|---------|
| Left Hall | 12 units | 26 units | 6 units | 2 |
| Central Room | 25 units | 30 units | 8 units | 6 |
| Right Room | 16 units | 22 units | 6 units | 4 |
| Corridors | 4 units | 5 units | 6 units | 0 |

### Spawn Points

| Location | Coordinates (X, Y, Z) |
|----------|----------------------|
| Player Start | Left Hall (-24, 2, 0) |
| Enemy 1 | Central Room (0, 1, -10) |
| Enemy 2 | Right Room (24, 1, 0) |
| Enemy 3 | Right Room (24, 1, 7) |
| Enemy 4 | Left Hall (-24, 1, -8) |
| Enemy 5 | Central Room (8, 1, 5) |

---

## 🎮 Controls

| Key | Action |
|-----|--------|
| `W` `A` `S` `D` | Move forward/left/back/right |
| `Mouse` | Look around |
| `Left Click` | Fire weapon |
| `R` | Reload |
| `1` | Switch to Rifle |
| `2` | Switch to Revolver |
| `Space` | Jump |
| `Mouse Wheel` | Cycle weapons |
| `ESC` | Exit game |

---

## 🔫 Weapons

### Rifle (Slot 1)
| Stat | Value |
|------|-------|
| Fire Mode | Automatic (hold to fire) |
| Magazine | 32 rounds |
| Fire Rate | 0.1s cooldown |
| Reload Time | 2.0 seconds |

### Revolver (Slot 2)
| Stat | Value |
|------|-------|
| Fire Mode | Semi-automatic (click to fire) |
| Magazine | 6 rounds |
| Fire Rate | 0.5s cooldown |
| Reload Time | 1.5 seconds |

---

## 🏗️ Level Architecture

### Color Palette (WWII Industrial)

| Material | RGB Values | Description |
|----------|------------|-------------|
| Concrete (Dark) | `55, 55, 60` | Main wall surfaces |
| Concrete (Medium) | `70, 70, 75` | Secondary walls |
| Concrete (Light) | `85, 85, 90` | Pillars |
| Worn Paint | `75, 80, 70` | Faded wall sections |
| Rust Metal | `90, 60, 45` | Stair railings |
| Dark Metal | `45, 48, 52` | Ceiling beams, shelves |
| Wood (Crate) | `120, 80, 40` | Wooden crates |
| Wood (Dark) | `80, 55, 30` | Tables, old furniture |
| Pipes | `60, 65, 70` | Wall-mounted pipes |
| Floor | `50, 50, 55` | Ground surface |
| Ceiling | `40, 40, 45` | Overhead surfaces |
| Fog | `35, 38, 42` | Background/atmosphere |

### Atmospheric Effects

1. **Fog**: Dark grey background simulates dusty air
2. **Flickering Lights**: 6 industrial lamps with random flicker (5% toggle chance)
3. **Dim Ambient**: Low-saturation color scheme throughout
4. **Light Pools**: Subtle glow beneath active light fixtures

---

## 📖 Code Architecture

### Data Structures

```cpp
// Level geometry building blocks
typedef struct Wall {
    Vector3 position;    // Center position
    Vector3 size;        // Width, Height, Depth
    Color color;         // Surface color
} Wall;

typedef struct Pillar {
    Vector3 position;    // Base position (Y=0)
    float width;         // Square cross-section size
    float height;        // Total height
} Pillar;

typedef struct Prop {
    Vector3 position;    // Center position
    Vector3 size;        // Bounding box
    Color color;         // Surface color
    int type;            // 0=crate, 1=table, 2=shelf, 3=debris, 4=pipe, 5=beam
} Prop;

typedef struct Stair {
    Vector3 position;    // Step center
    Vector3 size;        // Step dimensions
} Stair;

typedef struct LightSource {
    Vector3 position;    // Fixture location
    float flickerTimer;  // Animation state
    float flickerSpeed;  // Flicker frequency
    bool isOn;           // Current state
} LightSource;

typedef struct Weapon {
    Texture2D texture;   // Gun sprite
    int maxAmmo;         // Magazine capacity
    int currentAmmo;     // Current rounds
    float cooldown;      // Fire rate
    bool automatic;      // Hold vs click to fire
    // ... reload, flash settings
} Weapon;
```

### Key Functions

| Function | Purpose |
|----------|---------|
| `InitializeLevel()` | Populates all geometry arrays (~300 lines) |
| `DrawLevelGeometry()` | Renders floor, walls, pillars, stairs, props |
| `DrawAtmosphericLights()` | Renders light fixtures and glow cones |
| `UpdateLightFlicker(dt)` | Random flicker animation |
| `CheckBoxCollision(pos, r, box, size)` | AABB vs sphere collision |
| `ResolveCollision(newPos, oldPos, r)` | Push player out of solids |

### Array Limits

```cpp
#define MAX_WALLS 80
#define MAX_PILLARS 20
#define MAX_PROPS 60
#define MAX_STAIRS 12
#define MAX_LIGHTS 16
#define MAX_BULLETS 100
```

---

## 🛠️ How to Modify

### Add a New Room

1. Add walls in `InitializeLevel()`:
```cpp
walls[wallCount++] = (Wall){{X, Y, Z}, {WIDTH, HEIGHT, DEPTH}, CONCRETE_MED};
```

2. Add pillars:
```cpp
pillars[pillarCount++] = (Pillar){{X, 0.0f, Z}, 1.2f, 6.0f};
```

3. Add props:
```cpp
props[propCount++] = (Prop){{X, Y, Z}, {W, H, D}, WOOD_CRATE, 0};
```

### Add a New Enemy

In the game loop variables section:
```cpp
Vector3 enemyPositions[6] = {
    // ... existing 5 enemies
    {NEW_X, 1.0f, NEW_Z}  // Add 6th enemy
};
bool enemyActive[6] = {true, true, true, true, true, true};
```

Update collision loop to check 6 enemies instead of 5.

### Change Weapon Stats

```cpp
// Make rifle shoot faster
weapons[0].cooldown = 0.05f;  // Was 0.1f

// Increase revolver damage (add damage field)
// Or increase magazine size
weapons[1].maxAmmo = 8;  // Was 6
```

### Adjust Atmosphere

```cpp
// Darker fog
#define FOG_COLOR (Color){25, 28, 32, 255}

// More frequent flicker
if (rand() % 100 < 15) {  // Was 5%
    lights[i].isOn = !lights[i].isOn;
}
```

---

## 🚀 Compilation

### Prerequisites
- Raylib library installed
- MinGW/GCC compiler (w64devkit recommended)

### Build Command

```bash
# Windows (PowerShell/CMD)
cmd /c "set PATH=C:\raylib\w64devkit\bin;%PATH% && g++ main.cpp -o main.exe -IC:\raylib\raylib\src -LC:\raylib\raylib\src -lraylib -lopengl32 -lgdi32 -lwinmm"
```

### Run

```bash
.\main.exe
```

---

## 🎯 Gameplay Tips

1. **Use Cover**: Pillars and crates block bullets - peek and shoot!
2. **High Ground**: Climb the central stairwell for a tactical advantage
3. **Rifle for Range**: Automatic fire is great for medium distance
4. **Revolver for Power**: Higher damage per shot, harder to use
5. **Watch Your Ammo**: Reload behind cover, not in the open
6. **Clear Room by Room**: Start in Left Hall, move to Central, then Right

---

## 📊 Technical Specs

| Metric | Value |
|--------|-------|
| Source Lines | ~880 |
| Executable Size | ~2.1 MB |
| Target FPS | 60 |
| Resolution | 800 × 450 |
| Collision Type | AABB (walls, pillars, props) |
| Max Projectiles | 100 simultaneous |

---

## 🔮 Future Enhancements

- [ ] Audio: Ambient sounds, gunshots, footsteps
- [ ] Textures: Load actual image textures for walls
- [ ] Lighting Shaders: Dynamic shadows and proper lighting
- [ ] Enemy AI: Patrol paths, shooting back
- [ ] Health System: Player can take damage
- [ ] Pickups: Ammo crates, health kits
- [ ] Second Floor: Full upper level from stairwell
- [ ] Destructible Props: Shoot crates to break them

---

## 📜 License

This project uses [Raylib](https://www.raylib.com/) which is licensed under the zlib/libpng license.
