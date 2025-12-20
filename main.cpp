/*******************************************************************************************
*
*   raylib [core] example - Basic 3D FPS
*
*   This is a basic FPS example implemented using Raylib.
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 450;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "Raylib FPS Shooter");

    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 4.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    
    // Load Resources
    Image gunImage = LoadImage("resources/gun.png");
    ImageFormat(&gunImage, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8); // Ensure RGBA for transparency
    
    // Manual Green Screen Removal using Pointer Arithmetic for Speed & Precision
    Color *pixels = (Color *)gunImage.data;
    for (int i = 0; i < gunImage.width * gunImage.height; i++)
    {
        Color c = pixels[i];
        // If Green is dominant and bright
        if (c.g > 150 && c.r < 100 && c.b < 100) {
            pixels[i] = BLANK;
        }
    }
    
    Texture2D gunTexture = LoadTextureFromImage(gunImage);
    UnloadImage(gunImage);

    Texture2D flashTexture = LoadTexture("resources/flash.png");

    //--------------------------------------------------------------------------------------

    // Game Loop Variables
    Vector3 cubePosition = { 0.0f, 1.0f, 0.0f };
    Vector3 enemyPosition = { 4.0f, 1.0f, 4.0f };
    bool enemyActive = true;
    
    // Projectile
    Vector3 projectilePos = { 0 };
    bool bulletActive = false;
    Vector3 bulletDirection = { 0 };

    // Gun Dynamics variables
    float recoilOffset = 0.0f;
    Vector2 weaponSway = { 0.0f, 0.0f };
    float weaponBob = 0.0f;

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);

        // Shooting logic
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !bulletActive) {
            bulletActive = true;
            projectilePos = camera.position;
            
            // Recoil impulse
            recoilOffset = 0.4f; // Push gun back/up
            
            // Calculate forward vector from the camera
            // In first person mode, the target is where we are looking.
            // We need a normalized vector from position to target.
            // However, raylib's UpdateCamera modifies target.
            
            Vector3 forward = Vector3Subtract(camera.target, camera.position);
            bulletDirection = Vector3Normalize(forward);
        }

        if (bulletActive) {
            float speed = 0.5f;
            projectilePos = Vector3Add(projectilePos, Vector3Scale(bulletDirection, speed));
            
            // Simple distance check to reset bullet
            if (Vector3Distance(camera.position, projectilePos) > 50.0f) {
                bulletActive = false;
            }

            // Bullet collision with Enemy
            if (enemyActive) {
                // Simple AABB collision check (treating projectile as point for simplicity)
                // Enemy is 2x2x2 cube at enemyPosition
                if (projectilePos.x > enemyPosition.x - 1.0f && projectilePos.x < enemyPosition.x + 1.0f &&
                    projectilePos.z > enemyPosition.z - 1.0f && projectilePos.z < enemyPosition.z + 1.0f &&
                    projectilePos.y > enemyPosition.y - 1.0f && projectilePos.y < enemyPosition.y + 1.0f) {
                        
                        enemyActive = false;
                        bulletActive = false;
                }
            }
        }

        // Weapon Logic Update
        // 1. Recoil decay
        if (recoilOffset > 0) recoilOffset -= 0.02f;
        if (recoilOffset < 0) recoilOffset = 0.0f;

        // 2. Weapon Sway
        Vector2 mouseDelta = GetMouseDelta();
        float swayIntensity = 2.0f;
        float swaySmooth = 0.1f;
        
        // Target sway based on mouse movement
        Vector2 targetSway = { -mouseDelta.x * swayIntensity, -mouseDelta.y * swayIntensity };
        
        // Clamp sway
        float constantSwayClamp = 30.0f;
        if (targetSway.x > constantSwayClamp) targetSway.x = constantSwayClamp;
        if (targetSway.x < -constantSwayClamp) targetSway.x = -constantSwayClamp;
        if (targetSway.y > constantSwayClamp) targetSway.y = constantSwayClamp;
        if (targetSway.y < -constantSwayClamp) targetSway.y = -constantSwayClamp;

        // Lerp towards target
        weaponSway.x = Lerp(weaponSway.x, targetSway.x, swaySmooth);
        weaponSway.y = Lerp(weaponSway.y, targetSway.y, swaySmooth);

        // 3. Bobbing
        // Check if player is moving (simple check on key headers or just assume if keys down)
        bool isMoving = IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D);
        if (isMoving) {
            weaponBob += GetFrameTime() * 10.0f;
        } else {
            // Return to neutral
            weaponBob = Lerp(weaponBob, (float)((int)(weaponBob / PI) * PI), 0.1f); // Reset towards nearest phase, roughly
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 32.0f, 32.0f }, LIGHTGRAY); // Draw ground

                if (enemyActive) {
                    DrawCube(enemyPosition, 2.0f, 2.0f, 2.0f, RED);
                    DrawCubeWires(enemyPosition, 2.0f, 2.0f, 2.0f, MAROON);
                }

                DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, BLUE);     // Draw a blue wall/obstacle
                DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, DARKBLUE);
                
                // Draw some columns
                DrawCube((Vector3){ -4.0f, 1.0f, 4.0f }, 1.0f, 4.0f, 1.0f, GRAY);

                // Draw projectile
                if (bulletActive) {
                    DrawSphere(projectilePos, 0.2f, YELLOW);
                }

            EndMode3D();

            // Draw Gun (Realistic Sprite)
            // Center the gun, apply sway, bob, and recoil
            float scale = 0.4f; // REDUCED SCALE (was 0.8f)
            int gunWidth = gunTexture.width * scale;
            int gunHeight = gunTexture.height * scale;
            
            // Base Position (Bottom Right-ish for standard FPS feel)
            float bobOffsetX = sinf(weaponBob) * 10.0f;
            float bobOffsetY = fabs(cosf(weaponBob)) * 10.0f; // Only bob up/down in arc
            
            float recoilScreenY = recoilOffset * 200.0f; // Convert physics recoil to screen pixels

            int gunX = (screenWidth / 2) + 120 - (gunWidth / 2) + (int)weaponSway.x + (int)bobOffsetX;
            int gunY = screenHeight - gunHeight + 60 + (int)weaponSway.y + (int)bobOffsetY + (int)recoilScreenY; // Added 60 to push it down further if needed

            // Draw gun sprite
            DrawTextureEx(gunTexture, (Vector2){ (float)gunX, (float)gunY }, 0.0f, scale, WHITE);

            // Muzzle Flash
            if (bulletActive && Vector3Distance(camera.position, projectilePos) < 2.0f && recoilOffset > 0.3f) {
                // Flash should be near the tip of the gun. 
                // This usually requires manually finding the "muzzle" point on the sprite.
                // Assuming muzzle is top-left of the gun sprite roughly or centered top.
                // Let's guess relative to gunX/gunY.
                
                int flashX = gunX - 50; // Adjust based on visual
                int flashY = gunY - 50; 
                
                // Randomize flash slightly
                DrawTextureEx(flashTexture, (Vector2){ (float)flashX, (float)flashY }, 0.0f, 0.5f, WHITE);
            }

            // Crosshair
            DrawCircle(screenWidth/2, screenHeight/2, 2, RED);

            DrawText("WASD + Mouse to Move | Left Click to Shoot", 10, 10, 20, DARKGRAY);
            if (!enemyActive) {
                DrawText("ENEMY DESTROYED!", screenWidth/2 - 100, screenHeight/2, 30, GREEN);
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(gunTexture);
    UnloadTexture(flashTexture);

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}