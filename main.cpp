/*******************************************************************************************
*
*   raylib [core] example - Basic 3D FPS
*
*   WWII Industrial Bunker Level - Eerie atmosphere with concrete, pipes, and debris
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <math.h>

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 450;

typedef struct Weapon {
    Texture2D texture;
    int maxAmmo;
    int currentAmmo;
    float scale;
    float cooldown;
    float timeSinceLastShot;
    bool automatic; 
    bool isReloading;
    float reloadTime;
    float reloadTimer;
    int flashOffsetX;
    int flashOffsetY;
    float flashScale;
} Weapon;

//------------------------------------------------------------------------------------
// Level Geometry Structures
//------------------------------------------------------------------------------------
typedef struct Wall {
    Vector3 position;
    Vector3 size;
    Color color;
} Wall;

typedef struct Pillar {
    Vector3 position;
    float width;
    float height;
} Pillar;

typedef struct Prop {
    Vector3 position;
    Vector3 size;
    Color color;
    int type; // 0=crate, 1=table, 2=shelf, 3=debris, 4=pipe, 5=beam
} Prop;

typedef struct Stair {
    Vector3 position;
    Vector3 size;
} Stair;

typedef struct LightSource {
    Vector3 position;
    float flickerTimer;
    float flickerSpeed;
    bool isOn;
} LightSource;

// Level Arrays
#define MAX_WALLS 80
#define MAX_PILLARS 20
#define MAX_PROPS 60
#define MAX_STAIRS 12
#define MAX_LIGHTS 16

Wall walls[MAX_WALLS];
int wallCount = 0;

Pillar pillars[MAX_PILLARS];
int pillarCount = 0;

Prop props[MAX_PROPS];
int propCount = 0;

Stair stairs[MAX_STAIRS];
int stairCount = 0;

LightSource lights[MAX_LIGHTS];
int lightCount = 0;

// Color Palette - WWII Industrial Bunker
#define CONCRETE_DARK (Color){55, 55, 60, 255}
#define CONCRETE_MED (Color){70, 70, 75, 255}
#define CONCRETE_LIGHT (Color){85, 85, 90, 255}
#define WORN_PAINT (Color){75, 80, 70, 255}
#define RUST_METAL (Color){90, 60, 45, 255}
#define DARK_METAL (Color){45, 48, 52, 255}
#define WOOD_CRATE (Color){120, 80, 40, 255}
#define WOOD_DARK (Color){80, 55, 30, 255}
#define DEBRIS_COLOR (Color){65, 60, 55, 255}
#define PIPE_COLOR (Color){60, 65, 70, 255}
#define FLOOR_COLOR (Color){50, 50, 55, 255}
#define CEILING_COLOR (Color){40, 40, 45, 255}
#define FOG_COLOR (Color){35, 38, 42, 255}

// Global flickering state
float globalFlicker = 1.0f;
float flickerTimer = 0.0f;

//------------------------------------------------------------------------------------
// Level Initialization
//------------------------------------------------------------------------------------
void InitializeLevel()
{
    wallCount = 0;
    pillarCount = 0;
    propCount = 0;
    stairCount = 0;
    lightCount = 0;
    
    // ============================================
    // LEFT HALL - Long narrow room (12 wide x 24 deep)
    // Centered at X = -24
    // ============================================
    
    // Left Hall - West Wall (outer)
    walls[wallCount++] = (Wall){{-30.0f, 3.0f, 0.0f}, {0.5f, 6.0f, 26.0f}, CONCRETE_DARK};
    // Left Hall - North Wall
    walls[wallCount++] = (Wall){{-24.0f, 3.0f, -13.0f}, {13.0f, 6.0f, 0.5f}, CONCRETE_MED};
    // Left Hall - South Wall
    walls[wallCount++] = (Wall){{-24.0f, 3.0f, 13.0f}, {13.0f, 6.0f, 0.5f}, CONCRETE_MED};
    // Left Hall - East Wall (with doorway gap in middle)
    walls[wallCount++] = (Wall){{-18.0f, 3.0f, -8.0f}, {0.5f, 6.0f, 10.0f}, CONCRETE_DARK};
    walls[wallCount++] = (Wall){{-18.0f, 3.0f, 8.0f}, {0.5f, 6.0f, 10.0f}, CONCRETE_DARK};
    // Doorway lintel (above door)
    walls[wallCount++] = (Wall){{-18.0f, 5.0f, 0.0f}, {0.5f, 2.0f, 6.0f}, CONCRETE_DARK};
    
    // Left Hall - Ceiling
    walls[wallCount++] = (Wall){{-24.0f, 6.0f, 0.0f}, {12.0f, 0.3f, 26.0f}, CEILING_COLOR};
    
    // Left Hall Pillars (2 pillars)
    pillars[pillarCount++] = (Pillar){{-27.0f, 0.0f, -6.0f}, 1.2f, 6.0f};
    pillars[pillarCount++] = (Pillar){{-27.0f, 0.0f, 6.0f}, 1.2f, 6.0f};
    
    // ============================================
    // CENTRAL ROOM - Large main area (24 wide x 28 deep)
    // Centered at X = 0
    // ============================================
    
    // Central Room - North Wall
    walls[wallCount++] = (Wall){{0.0f, 4.0f, -15.0f}, {26.0f, 8.0f, 0.5f}, CONCRETE_MED};
    // Central Room - South Wall
    walls[wallCount++] = (Wall){{0.0f, 4.0f, 15.0f}, {26.0f, 8.0f, 0.5f}, CONCRETE_MED};
    // Central Room - West Wall segments (with doorway to left hall)
    walls[wallCount++] = (Wall){{-12.5f, 4.0f, -10.0f}, {0.5f, 8.0f, 10.0f}, WORN_PAINT};
    walls[wallCount++] = (Wall){{-12.5f, 4.0f, 10.0f}, {0.5f, 8.0f, 10.0f}, WORN_PAINT};
    walls[wallCount++] = (Wall){{-12.5f, 6.0f, 0.0f}, {0.5f, 4.0f, 6.0f}, WORN_PAINT};
    // Central Room - East Wall segments (with doorway to right room)
    walls[wallCount++] = (Wall){{12.5f, 4.0f, -10.0f}, {0.5f, 8.0f, 10.0f}, WORN_PAINT};
    walls[wallCount++] = (Wall){{12.5f, 4.0f, 10.0f}, {0.5f, 8.0f, 10.0f}, WORN_PAINT};
    walls[wallCount++] = (Wall){{12.5f, 6.0f, 0.0f}, {0.5f, 4.0f, 6.0f}, WORN_PAINT};
    
    // Central Room - Higher Ceiling
    walls[wallCount++] = (Wall){{0.0f, 8.0f, 0.0f}, {25.0f, 0.3f, 30.0f}, CEILING_COLOR};
    
    // Central Room - Ceiling Beams (horizontal metal beams)
    props[propCount++] = (Prop){{0.0f, 7.5f, -7.0f}, {24.0f, 0.4f, 0.6f}, DARK_METAL, 5};
    props[propCount++] = (Prop){{0.0f, 7.5f, 0.0f}, {24.0f, 0.4f, 0.6f}, DARK_METAL, 5};
    props[propCount++] = (Prop){{0.0f, 7.5f, 7.0f}, {24.0f, 0.4f, 0.6f}, DARK_METAL, 5};
    
    // Central Room Pillars (6 pillars - 2 rows of 3)
    pillars[pillarCount++] = (Pillar){{-8.0f, 0.0f, -10.0f}, 1.5f, 8.0f};
    pillars[pillarCount++] = (Pillar){{-8.0f, 0.0f, 10.0f}, 1.5f, 8.0f};
    pillars[pillarCount++] = (Pillar){{8.0f, 0.0f, -10.0f}, 1.5f, 8.0f};
    pillars[pillarCount++] = (Pillar){{8.0f, 0.0f, 10.0f}, 1.5f, 8.0f};
    // Near stairwell
    pillars[pillarCount++] = (Pillar){{-4.0f, 0.0f, -3.0f}, 1.0f, 8.0f};
    pillars[pillarCount++] = (Pillar){{4.0f, 0.0f, -3.0f}, 1.0f, 8.0f};
    
    // ============================================
    // CENTRAL STAIRWELL - Going up in center
    // ============================================
    
    // Stair steps (going from Z=0 to Z=6, rising)
    for (int i = 0; i < 10; i++) {
        float stepY = 0.25f + i * 0.4f;
        float stepZ = 1.0f + i * 0.5f;
        stairs[stairCount++] = (Stair){{0.0f, stepY, stepZ}, {4.0f, 0.25f, 0.5f}};
    }
    
    // Stair railing posts (left side)
    props[propCount++] = (Prop){{-2.2f, 1.5f, 1.5f}, {0.1f, 1.5f, 0.1f}, RUST_METAL, 4};
    props[propCount++] = (Prop){{-2.2f, 2.3f, 3.5f}, {0.1f, 1.5f, 0.1f}, RUST_METAL, 4};
    props[propCount++] = (Prop){{-2.2f, 3.1f, 5.5f}, {0.1f, 1.5f, 0.1f}, RUST_METAL, 4};
    // Stair railing posts (right side)
    props[propCount++] = (Prop){{2.2f, 1.5f, 1.5f}, {0.1f, 1.5f, 0.1f}, RUST_METAL, 4};
    props[propCount++] = (Prop){{2.2f, 2.3f, 3.5f}, {0.1f, 1.5f, 0.1f}, RUST_METAL, 4};
    props[propCount++] = (Prop){{2.2f, 3.1f, 5.5f}, {0.1f, 1.5f, 0.1f}, RUST_METAL, 4};
    // Horizontal railing bars
    props[propCount++] = (Prop){{-2.2f, 2.5f, 3.5f}, {0.08f, 0.08f, 5.0f}, RUST_METAL, 4};
    props[propCount++] = (Prop){{2.2f, 2.5f, 3.5f}, {0.08f, 0.08f, 5.0f}, RUST_METAL, 4};
    
    // Upper platform at top of stairs
    walls[wallCount++] = (Wall){{0.0f, 4.2f, 7.5f}, {5.0f, 0.25f, 3.0f}, CONCRETE_LIGHT};
    
    // ============================================
    // RIGHT ROOM - Large room (16 wide x 20 deep)
    // Centered at X = 24
    // ============================================
    
    // Right Room - East Wall (outer)
    walls[wallCount++] = (Wall){{32.0f, 3.0f, 0.0f}, {0.5f, 6.0f, 22.0f}, CONCRETE_DARK};
    // Right Room - North Wall
    walls[wallCount++] = (Wall){{24.0f, 3.0f, -11.0f}, {17.0f, 6.0f, 0.5f}, CONCRETE_MED};
    // Right Room - South Wall
    walls[wallCount++] = (Wall){{24.0f, 3.0f, 11.0f}, {17.0f, 6.0f, 0.5f}, CONCRETE_MED};
    // Right Room - West Wall segments (with doorway)
    walls[wallCount++] = (Wall){{16.0f, 3.0f, -7.0f}, {0.5f, 6.0f, 8.0f}, WORN_PAINT};
    walls[wallCount++] = (Wall){{16.0f, 3.0f, 7.0f}, {0.5f, 6.0f, 8.0f}, WORN_PAINT};
    walls[wallCount++] = (Wall){{16.0f, 5.0f, 0.0f}, {0.5f, 2.0f, 6.0f}, WORN_PAINT};
    
    // Right Room - Ceiling
    walls[wallCount++] = (Wall){{24.0f, 6.0f, 0.0f}, {16.0f, 0.3f, 22.0f}, CEILING_COLOR};
    
    // Right Room Pillars (4 pillars)
    pillars[pillarCount++] = (Pillar){{20.0f, 0.0f, -6.0f}, 1.2f, 6.0f};
    pillars[pillarCount++] = (Pillar){{20.0f, 0.0f, 6.0f}, 1.2f, 6.0f};
    pillars[pillarCount++] = (Pillar){{28.0f, 0.0f, -6.0f}, 1.2f, 6.0f};
    pillars[pillarCount++] = (Pillar){{28.0f, 0.0f, 6.0f}, 1.2f, 6.0f};
    
    // ============================================
    // CORRIDOR between Central and Right rooms
    // ============================================
    walls[wallCount++] = (Wall){{14.25f, 3.0f, -2.5f}, {4.0f, 6.0f, 0.3f}, CONCRETE_DARK};
    walls[wallCount++] = (Wall){{14.25f, 3.0f, 2.5f}, {4.0f, 6.0f, 0.3f}, CONCRETE_DARK};
    walls[wallCount++] = (Wall){{14.25f, 5.5f, 0.0f}, {4.0f, 0.3f, 5.0f}, CEILING_COLOR};
    
    // ============================================
    // PROPS - Crates, Tables, Shelves, Debris
    // ============================================
    
    // LEFT HALL PROPS
    // Wooden crates stacked
    props[propCount++] = (Prop){{-28.0f, 0.75f, -10.0f}, {1.5f, 1.5f, 1.5f}, WOOD_CRATE, 0};
    props[propCount++] = (Prop){{-26.5f, 0.75f, -10.0f}, {1.5f, 1.5f, 1.5f}, WOOD_DARK, 0};
    props[propCount++] = (Prop){{-27.25f, 2.0f, -10.0f}, {1.3f, 1.0f, 1.3f}, WOOD_CRATE, 0};
    // Table with debris
    props[propCount++] = (Prop){{-22.0f, 1.0f, 8.0f}, {3.0f, 0.15f, 1.5f}, WOOD_DARK, 1};
    props[propCount++] = (Prop){{-22.0f, 0.5f, 7.5f}, {0.2f, 1.0f, 0.2f}, WOOD_DARK, 1};
    props[propCount++] = (Prop){{-22.0f, 0.5f, 8.5f}, {0.2f, 1.0f, 0.2f}, WOOD_DARK, 1};
    // Debris pile
    props[propCount++] = (Prop){{-25.0f, 0.3f, 3.0f}, {2.0f, 0.6f, 1.5f}, DEBRIS_COLOR, 3};
    // Pipes on wall
    props[propCount++] = (Prop){{-29.8f, 4.0f, 0.0f}, {0.2f, 0.2f, 20.0f}, PIPE_COLOR, 4};
    props[propCount++] = (Prop){{-29.8f, 2.5f, -5.0f}, {0.15f, 0.15f, 8.0f}, PIPE_COLOR, 4};
    
    // CENTRAL ROOM PROPS
    // Large crate pile for cover
    props[propCount++] = (Prop){{-6.0f, 1.0f, -12.0f}, {2.0f, 2.0f, 2.0f}, WOOD_CRATE, 0};
    props[propCount++] = (Prop){{-4.0f, 1.0f, -12.0f}, {2.0f, 2.0f, 2.0f}, WOOD_DARK, 0};
    props[propCount++] = (Prop){{-5.0f, 2.5f, -12.0f}, {1.5f, 1.0f, 1.5f}, WOOD_CRATE, 0};
    // Metal shelves
    props[propCount++] = (Prop){{10.0f, 1.5f, -13.5f}, {4.0f, 3.0f, 0.8f}, DARK_METAL, 2};
    // Overturned table
    props[propCount++] = (Prop){{6.0f, 0.6f, 12.0f}, {2.5f, 0.15f, 1.2f}, WOOD_DARK, 1};
    // Debris and papers
    props[propCount++] = (Prop){{-10.0f, 0.15f, 5.0f}, {1.5f, 0.3f, 1.0f}, DEBRIS_COLOR, 3};
    props[propCount++] = (Prop){{3.0f, 0.1f, -8.0f}, {0.8f, 0.02f, 0.6f}, (Color){200, 195, 180, 255}, 3};
    // Broken chair
    props[propCount++] = (Prop){{-9.0f, 0.4f, 8.0f}, {0.5f, 0.8f, 0.5f}, WOOD_DARK, 3};
    // Cables hanging from ceiling
    props[propCount++] = (Prop){{5.0f, 6.0f, 5.0f}, {0.05f, 2.5f, 0.05f}, (Color){30, 30, 35, 255}, 4};
    props[propCount++] = (Prop){{-3.0f, 5.5f, -5.0f}, {0.05f, 3.0f, 0.05f}, (Color){30, 30, 35, 255}, 4};
    
    // RIGHT ROOM PROPS
    // Crate cover positions
    props[propCount++] = (Prop){{22.0f, 1.0f, -8.0f}, {2.0f, 2.0f, 2.0f}, WOOD_CRATE, 0};
    props[propCount++] = (Prop){{30.0f, 0.75f, 8.0f}, {1.5f, 1.5f, 1.5f}, WOOD_DARK, 0};
    props[propCount++] = (Prop){{30.0f, 0.75f, 6.5f}, {1.5f, 1.5f, 1.5f}, WOOD_CRATE, 0};
    // Old desk/table
    props[propCount++] = (Prop){{26.0f, 1.0f, -9.0f}, {2.5f, 0.15f, 1.2f}, WOOD_DARK, 1};
    props[propCount++] = (Prop){{26.0f, 0.5f, -8.5f}, {0.15f, 1.0f, 0.15f}, WOOD_DARK, 1};
    props[propCount++] = (Prop){{26.0f, 0.5f, -9.5f}, {0.15f, 1.0f, 0.15f}, WOOD_DARK, 1};
    // Shelf unit
    props[propCount++] = (Prop){{31.0f, 2.0f, 0.0f}, {0.8f, 4.0f, 3.0f}, DARK_METAL, 2};
    // Debris
    props[propCount++] = (Prop){{24.0f, 0.25f, 4.0f}, {1.8f, 0.5f, 1.2f}, DEBRIS_COLOR, 3};
    // Wall pipes
    props[propCount++] = (Prop){{31.8f, 3.5f, 0.0f}, {0.15f, 0.15f, 18.0f}, PIPE_COLOR, 4};
    props[propCount++] = (Prop){{31.8f, 2.0f, 0.0f}, {0.1f, 0.1f, 18.0f}, RUST_METAL, 4};
    
    // ============================================
    // LIGHTS - Dim industrial lighting
    // ============================================
    lights[lightCount++] = (LightSource){{-24.0f, 5.5f, 0.0f}, 0.0f, 3.5f, true};
    lights[lightCount++] = (LightSource){{-24.0f, 5.5f, -8.0f}, 0.0f, 4.2f, true};
    lights[lightCount++] = (LightSource){{0.0f, 7.5f, -7.0f}, 0.0f, 2.8f, true};
    lights[lightCount++] = (LightSource){{0.0f, 7.5f, 7.0f}, 0.0f, 5.0f, true};
    lights[lightCount++] = (LightSource){{24.0f, 5.5f, 0.0f}, 0.0f, 3.0f, true};
    lights[lightCount++] = (LightSource){{24.0f, 5.5f, -6.0f}, 0.0f, 6.0f, true};
}

//------------------------------------------------------------------------------------
// Collision Detection
//------------------------------------------------------------------------------------
bool CheckBoxCollision(Vector3 playerPos, float radius, Vector3 boxPos, Vector3 boxSize)
{
    // Check if player sphere intersects with AABB
    float closestX = Clamp(playerPos.x, boxPos.x - boxSize.x/2, boxPos.x + boxSize.x/2);
    float closestY = Clamp(playerPos.y, boxPos.y - boxSize.y/2, boxPos.y + boxSize.y/2);
    float closestZ = Clamp(playerPos.z, boxPos.z - boxSize.z/2, boxPos.z + boxSize.z/2);
    
    float distX = playerPos.x - closestX;
    float distY = playerPos.y - closestY;
    float distZ = playerPos.z - closestZ;
    
    float distSquared = distX*distX + distY*distY + distZ*distZ;
    return distSquared < radius*radius;
}

Vector3 ResolveCollision(Vector3 playerPos, Vector3 oldPos, float radius)
{
    Vector3 resolved = playerPos;
    
    // Check walls
    for (int i = 0; i < wallCount; i++) {
        if (CheckBoxCollision(resolved, radius, walls[i].position, walls[i].size)) {
            // Push player back
            resolved = oldPos;
            break;
        }
    }
    
    // Check pillars
    for (int i = 0; i < pillarCount; i++) {
        Vector3 pillarBox = {pillars[i].position.x, pillars[i].height/2, pillars[i].position.z};
        Vector3 pillarSize = {pillars[i].width, pillars[i].height, pillars[i].width};
        if (CheckBoxCollision(resolved, radius, pillarBox, pillarSize)) {
            resolved = oldPos;
            break;
        }
    }
    
    // Check large props (crates only for collision)
    for (int i = 0; i < propCount; i++) {
        if (props[i].type == 0 && props[i].size.x > 1.0f) { // Only large crates
            if (CheckBoxCollision(resolved, radius, props[i].position, props[i].size)) {
                resolved = oldPos;
                break;
            }
        }
    }
    
    return resolved;
}

//------------------------------------------------------------------------------------
// Drawing Functions
//------------------------------------------------------------------------------------
void DrawLevelGeometry()
{
    // Draw Floor - Dark concrete
    DrawPlane((Vector3){0.0f, 0.0f, 0.0f}, (Vector2){80.0f, 50.0f}, FLOOR_COLOR);
    
    // Draw all walls
    for (int i = 0; i < wallCount; i++) {
        DrawCubeV(walls[i].position, walls[i].size, walls[i].color);
        // Add subtle edge lines
        DrawCubeWiresV(walls[i].position, walls[i].size, (Color){30, 30, 35, 100});
    }
    
    // Draw pillars (square concrete pillars)
    for (int i = 0; i < pillarCount; i++) {
        Vector3 pillarPos = {pillars[i].position.x, pillars[i].height/2, pillars[i].position.z};
        DrawCube(pillarPos, pillars[i].width, pillars[i].height, pillars[i].width, CONCRETE_LIGHT);
        DrawCubeWires(pillarPos, pillars[i].width, pillars[i].height, pillars[i].width, (Color){40, 40, 45, 150});
    }
    
    // Draw stairs
    for (int i = 0; i < stairCount; i++) {
        DrawCubeV(stairs[i].position, stairs[i].size, CONCRETE_MED);
        DrawCubeWiresV(stairs[i].position, stairs[i].size, (Color){50, 50, 55, 100});
    }
    
    // Draw props
    for (int i = 0; i < propCount; i++) {
        DrawCubeV(props[i].position, props[i].size, props[i].color);
        // Add wire edges for definition
        if (props[i].type != 4) { // Not for pipes/cables
            DrawCubeWiresV(props[i].position, props[i].size, (Color){25, 25, 30, 80});
        }
    }
}

void DrawAtmosphericLights()
{
    // Draw light fixtures (simple boxes representing lamps)
    for (int i = 0; i < lightCount; i++) {
        Color lightColor = lights[i].isOn ? (Color){180, 160, 120, 255} : (Color){60, 55, 50, 255};
        DrawCube(lights[i].position, 0.6f, 0.2f, 0.6f, DARK_METAL);
        
        if (lights[i].isOn) {
            // Light glow cone (simple representation)
            Vector3 glowPos = {lights[i].position.x, lights[i].position.y - 1.5f, lights[i].position.z};
            DrawCube(glowPos, 2.0f, 0.05f, 2.0f, (Color){100, 90, 70, 40});
        }
    }
}

void UpdateLightFlicker(float deltaTime)
{
    flickerTimer += deltaTime;
    
    // Random flicker effect
    if (flickerTimer > 0.1f) {
        flickerTimer = 0.0f;
        globalFlicker = 0.85f + ((float)(rand() % 30) / 100.0f);
        
        // Randomly toggle individual lights for dramatic effect
        for (int i = 0; i < lightCount; i++) {
            lights[i].flickerTimer += deltaTime * lights[i].flickerSpeed;
            if (lights[i].flickerTimer > 1.0f) {
                lights[i].flickerTimer = 0.0f;
                // 5% chance to flicker off briefly
                if (rand() % 100 < 5) {
                    lights[i].isOn = !lights[i].isOn;
                } else {
                    lights[i].isOn = true;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "WWII Industrial Bunker - FPS");

    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ -24.0f, 2.0f, 0.0f };    // Start in left hall
    camera.target = (Vector3){ -18.0f, 2.0f, 0.0f };      // Looking towards doorway
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    DisableCursor();
    SetTargetFPS(60);
    
    // Initialize Level
    InitializeLevel();
    
    // Load Resources
    Image gunImage = LoadImage("resources/gun.png");
    ImageFormat(&gunImage, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    
    Color *pixels = (Color *)gunImage.data;
    for (int i = 0; i < gunImage.width * gunImage.height; i++)
    {
        Color c = pixels[i];
        if (c.g > 150 && c.r < 100 && c.b < 100) {
            pixels[i] = BLANK;
        }
    }
    
    Texture2D gunTexture = LoadTextureFromImage(gunImage);
    UnloadImage(gunImage);

    Texture2D flashTexture = LoadTexture("resources/muzzle_flash.png");
    
    // Load Revolver
    Image revolverImage = LoadImage("resources/revolver.png");
    ImageFormat(&revolverImage, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Color *revPixels = (Color *)revolverImage.data;
    for (int i = 0; i < revolverImage.width * revolverImage.height; i++)
    {
        if (revPixels[i].g > 150 && revPixels[i].r < 100 && revPixels[i].b < 100) revPixels[i] = BLANK;
    }
    Texture2D revolverTexture = LoadTextureFromImage(revolverImage);
    UnloadImage(revolverImage);

    // Initialize Weapons
    Weapon weapons[2] = { 0 };
    
    // Rifle
    weapons[0].texture = gunTexture;
    weapons[0].maxAmmo = 32;
    weapons[0].currentAmmo = 32;
    weapons[0].scale = 0.4f;
    weapons[0].reloadTime = 2.0f;
    weapons[0].cooldown = 0.1f;    
    weapons[0].automatic = true;   
    weapons[0].flashOffsetX = 60;
    weapons[0].flashOffsetY = 100;
    weapons[0].flashScale = 0.2f; 
    
    // Revolver
    weapons[1].texture = revolverTexture;
    weapons[1].maxAmmo = 6;
    weapons[1].currentAmmo = 6;
    weapons[1].scale = 0.4f; 
    weapons[1].reloadTime = 1.5f;
    weapons[1].cooldown = 0.5f;    
    weapons[1].automatic = false;  
    weapons[1].flashOffsetX = 60;
    weapons[1].flashOffsetY = 110;
    weapons[1].flashScale = 0.15f; 

    int currentWeapon = 0;

    //--------------------------------------------------------------------------------------

    // Game Loop Variables
    Vector3 enemyPositions[5] = {
        {0.0f, 1.0f, -10.0f},   // Central room
        {24.0f, 1.0f, 0.0f},    // Right room
        {24.0f, 1.0f, 7.0f},    // Right room
        {-24.0f, 1.0f, -8.0f},  // Left hall
        {8.0f, 1.0f, 5.0f}      // Central room
    };
    bool enemyActive[5] = {true, true, true, true, true};
    
    // Projectiles
    typedef struct Bullet {
        Vector3 position;
        Vector3 direction;
        bool active;
    } Bullet;
    
    #define MAX_BULLETS 100
    Bullet bullets[MAX_BULLETS] = { 0 };

    // Gun Dynamics variables
    float recoilOffset = 0.0f;
    Vector2 weaponSway = { 0.0f, 0.0f };
    float weaponBob = 0.0f;

    // Physics & Movement
    float verticalVelocity = 0.0f;
    const float GRAVITY = 13.0f;
    const float JUMP_FORCE = 6.0f;
    bool isGrounded = true;
    float playerHeight = 2.0f;
    float playerRadius = 0.5f;

    // Weapon Switch Logic
    int targetWeapon = currentWeapon;
    bool isSwitching = false;
    float switchTimer = 0.0f;
    float switchDuration = 0.6f;

    // Main game loop
    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        
        // Update atmospheric effects
        UpdateLightFlicker(deltaTime);
        
        // Store old position for collision
        Vector3 oldPosition = camera.position;
        
        // Weapon Switching Input
        if (!isSwitching) {
            if (IsKeyPressed(KEY_ONE) && currentWeapon != 0) {
                targetWeapon = 0;
                isSwitching = true;
                switchTimer = 0.0f;
            }
            if (IsKeyPressed(KEY_TWO) && currentWeapon != 1) {
                targetWeapon = 1;
                isSwitching = true;
                switchTimer = 0.0f;
            }
            
            float wheel = GetMouseWheelMove();
            if (wheel != 0) {
                int next = currentWeapon + (int)wheel;
                 if (next > 1) next = 0;
                 if (next < 0) next = 1;
                 
                 if (next != currentWeapon) {
                     targetWeapon = next;
                     isSwitching = true;
                     switchTimer = 0.0f;
                 }
            }
        }
        
        // Weapon Switch Animation Logic
        if (isSwitching) {
            switchTimer += deltaTime;
            
            if (switchTimer >= switchDuration / 2.0f && currentWeapon != targetWeapon) {
                currentWeapon = targetWeapon;
            }
            
            if (switchTimer >= switchDuration) {
                isSwitching = false;
                switchTimer = 0.0f;
            }
        }

        float oldCamY = camera.position.y;

        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        
        // Apply collision detection
        Vector3 feetPos = {camera.position.x, camera.position.y - playerHeight + 0.5f, camera.position.z};
        camera.position = ResolveCollision(camera.position, oldPosition, playerRadius);
        
        // Physics: Apply Gravity
        if (!isGrounded) {
             verticalVelocity -= GRAVITY * deltaTime;
        } else {
            if (IsKeyPressed(KEY_SPACE)) {
                verticalVelocity = JUMP_FORCE;
                isGrounded = false;
            }
        }
        
        camera.position.y += verticalVelocity * deltaTime;
        
        // Floor and stair collision
        float groundLevel = playerHeight;
        
        // Check if on stairs
        for (int i = 0; i < stairCount; i++) {
            float stepTop = stairs[i].position.y + stairs[i].size.y/2;
            if (camera.position.x > stairs[i].position.x - stairs[i].size.x/2 - 0.5f &&
                camera.position.x < stairs[i].position.x + stairs[i].size.x/2 + 0.5f &&
                camera.position.z > stairs[i].position.z - stairs[i].size.z/2 - 0.3f &&
                camera.position.z < stairs[i].position.z + stairs[i].size.z/2 + 0.3f) {
                if (stepTop + playerHeight > groundLevel) {
                    groundLevel = stepTop + playerHeight;
                }
            }
        }
        
        // Check if on upper platform
        if (camera.position.x > -2.5f && camera.position.x < 2.5f &&
            camera.position.z > 6.0f && camera.position.z < 9.0f) {
            groundLevel = 4.2f + playerHeight;
        }
        
        if (camera.position.y <= groundLevel) {
            camera.position.y = groundLevel;
            verticalVelocity = 0;
            isGrounded = true;
        } else {
            isGrounded = false;
        }

        camera.target.y += (camera.position.y - oldCamY);

        // Reload Logic
        Weapon *w = &weapons[currentWeapon];
        
        if (w->isReloading) {
            w->reloadTimer -= deltaTime;
            if (w->reloadTimer <= 0) {
                w->currentAmmo = w->maxAmmo;
                w->isReloading = false;
            }
        } else {
             if (w->currentAmmo <= 0 || IsKeyPressed(KEY_R)) {
                 if (w->currentAmmo < w->maxAmmo) {
                    w->isReloading = true;
                    w->reloadTimer = w->reloadTime;
                 }
             }
        }
        
        w->timeSinceLastShot += deltaTime;

        // Shooting logic
        bool shootInput = false;
        if (w->automatic) shootInput = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        else shootInput = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (shootInput && !w->isReloading && !isSwitching && w->currentAmmo > 0 && w->timeSinceLastShot >= w->cooldown) {
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullets[i].active) {
                    bullets[i].active = true;
                    bullets[i].position = camera.position;
                    Vector3 forward = Vector3Subtract(camera.target, camera.position);
                    bullets[i].direction = Vector3Normalize(forward);
                    
                    w->currentAmmo--;
                    w->timeSinceLastShot = 0.0f;
                    recoilOffset = 0.4f;
                    break;
                }
            }
        }

        // Update Projectiles
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].active) {
                float speed = 2.0f; 
                bullets[i].position = Vector3Add(bullets[i].position, Vector3Scale(bullets[i].direction, speed));
               
                // Collision with Enemies
                for (int e = 0; e < 5; e++) {
                    if (enemyActive[e]) {
                        if (bullets[i].position.x > enemyPositions[e].x - 1.0f && bullets[i].position.x < enemyPositions[e].x + 1.0f &&
                            bullets[i].position.z > enemyPositions[e].z - 1.0f && bullets[i].position.z < enemyPositions[e].z + 1.0f &&
                            bullets[i].position.y > enemyPositions[e].y - 1.0f && bullets[i].position.y < enemyPositions[e].y + 1.0f) {
                                enemyActive[e] = false;
                                bullets[i].active = false;
                        }
                    }
                }
                
                // Wall collision for bullets
                for (int wi = 0; wi < wallCount; wi++) {
                    if (CheckBoxCollision(bullets[i].position, 0.1f, walls[wi].position, walls[wi].size)) {
                        bullets[i].active = false;
                        break;
                    }
                }
                
                // Despawn distance
                 if (Vector3Distance(camera.position, bullets[i].position) > 100.0f) {
                    bullets[i].active = false;
                }
            }
        }

        // Weapon dynamics
        if (recoilOffset > 0) recoilOffset -= 0.02f;
        if (recoilOffset < 0) recoilOffset = 0.0f;

        Vector2 mouseDelta = GetMouseDelta();
        float swayIntensity = 2.0f;
        float swaySmooth = 0.1f;
        
        Vector2 targetSway = { -mouseDelta.x * swayIntensity, -mouseDelta.y * swayIntensity };
        
        float constantSwayClamp = 30.0f;
        if (targetSway.x > constantSwayClamp) targetSway.x = constantSwayClamp;
        if (targetSway.x < -constantSwayClamp) targetSway.x = -constantSwayClamp;
        if (targetSway.y > constantSwayClamp) targetSway.y = constantSwayClamp;
        if (targetSway.y < -constantSwayClamp) targetSway.y = -constantSwayClamp;

        weaponSway.x = Lerp(weaponSway.x, targetSway.x, swaySmooth);
        weaponSway.y = Lerp(weaponSway.y, targetSway.y, swaySmooth);

        bool isMoving = IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D);
        if (isMoving) {
            weaponBob += deltaTime * 10.0f;
        } else {
            weaponBob = Lerp(weaponBob, (float)((int)(weaponBob / PI) * PI), 0.1f);
        }

        // Draw
        //--------------------------------------------------------------------------------------
        BeginDrawing();

            // Fog-like background color for atmosphere
            ClearBackground(FOG_COLOR);

            BeginMode3D(camera);

                // Draw level geometry
                DrawLevelGeometry();
                
                // Draw atmospheric lights
                DrawAtmosphericLights();

                // Draw enemies
                for (int e = 0; e < 5; e++) {
                    if (enemyActive[e]) {
                        DrawCube(enemyPositions[e], 1.8f, 2.0f, 1.8f, (Color){140, 50, 50, 255});
                        DrawCubeWires(enemyPositions[e], 1.8f, 2.0f, 1.8f, (Color){100, 30, 30, 255});
                    }
                }

                // Draw projectiles
                for (int i = 0; i < MAX_BULLETS; i++) {
                    if (bullets[i].active) {
                        DrawSphere(bullets[i].position, 0.08f, (Color){255, 220, 100, 255});
                    }
                }

            EndMode3D();

            // Draw Gun
            float scale = w->scale;
            int gunWidth = (int)(w->texture.width * scale);
            int gunHeight = (int)(w->texture.height * scale);
            
            float bobOffsetX = sinf(weaponBob) * 10.0f;
            float bobOffsetY = fabsf(cosf(weaponBob)) * 10.0f;
            
            float recoilScreenY = recoilOffset * 200.0f;
            
            float reloadOffsetY = 0.0f;
            if (w->isReloading) {
                float t = 1.0f - (w->reloadTimer / w->reloadTime); 
                if (t < 0.5f) reloadOffsetY = Lerp(0.0f, 200.0f, t * 2.0f);
                else reloadOffsetY = Lerp(200.0f, 0.0f, (t - 0.5f) * 2.0f);
            }
            
            float switchOffsetY = 0.0f;
            if (isSwitching) {
                float t = switchTimer / switchDuration;
                if (t < 0.5f) {
                    float halfT = t * 2.0f; 
                    switchOffsetY = Lerp(0.0f, 300.0f, halfT); 
                } else {
                    float halfT = (t - 0.5f) * 2.0f;
                    switchOffsetY = Lerp(300.0f, 0.0f, halfT);
                }
            }

            int gunX = (screenWidth / 2) + 120 - (gunWidth / 2) + (int)weaponSway.x + (int)bobOffsetX;
            int gunY = screenHeight - gunHeight + 60 + (int)weaponSway.y + (int)bobOffsetY + (int)recoilScreenY + (int)reloadOffsetY + (int)switchOffsetY; 

            DrawTextureEx(w->texture, (Vector2){ (float)gunX, (float)gunY }, 0.0f, scale, WHITE);
            
            // UI: Ammo
            Color ammoColor = (w->currentAmmo <= w->maxAmmo / 4) ? RED : (Color){180, 180, 160, 255};
            if (w->isReloading) {
                 const char *text = "RELOADING...";
                 int textWidth = MeasureText(text, 30);
                 DrawText(text, screenWidth - textWidth - 20, screenHeight - 40, 30, (Color){200, 150, 50, 255});
            } else {
                 const char *text = TextFormat("AMMO: %d / %d", w->currentAmmo, w->maxAmmo);
                 int textWidth = MeasureText(text, 40);
                 DrawText(text, screenWidth - textWidth - 20, screenHeight - 50, 40, ammoColor);
            }

            // Muzzle Flash
            if (w->timeSinceLastShot < 0.1f && recoilOffset > 0.1f) {
                int flashX = gunX + w->flashOffsetX - (int)(flashTexture.width * w->flashScale / 2);
                int flashY = gunY + w->flashOffsetY - (int)(flashTexture.height * w->flashScale / 2);
                
                BeginBlendMode(BLEND_ADDITIVE);
                    DrawTextureEx(flashTexture, (Vector2){ (float)flashX, (float)flashY }, 0.0f, w->flashScale, WHITE);
                EndBlendMode();
            }

            // Crosshair
            DrawCircle(screenWidth/2, screenHeight/2, 2, (Color){200, 50, 50, 200});
            DrawCircleLines(screenWidth/2, screenHeight/2, 8, (Color){200, 50, 50, 150});

            // Controls hint
            DrawText("WASD: Move | Mouse: Look | LMB: Shoot | R: Reload | 1/2: Switch", 10, 10, 16, (Color){150, 150, 140, 200});
            
            // Enemy counter
            int activeEnemies = 0;
            for (int e = 0; e < 5; e++) if (enemyActive[e]) activeEnemies++;
            DrawText(TextFormat("Enemies: %d", activeEnemies), 10, 30, 20, (Color){180, 100, 100, 255});
            
            if (activeEnemies == 0) {
                const char* victoryText = "AREA CLEARED!";
                int victoryWidth = MeasureText(victoryText, 40);
                DrawText(victoryText, screenWidth/2 - victoryWidth/2, screenHeight/2 - 50, 40, (Color){100, 200, 100, 255});
            }

        EndDrawing();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(gunTexture);
    UnloadTexture(revolverTexture);
    UnloadTexture(flashTexture);

    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}