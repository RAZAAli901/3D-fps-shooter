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
    weapons[0].flashOffsetX = 60;  // Moved Left closer to center
    weapons[0].flashOffsetY = 100; // Moved Higher
    weapons[0].flashScale = 0.2f; 
    
    // Revolver
    weapons[1].texture = revolverTexture;
    weapons[1].maxAmmo = 6;
    weapons[1].currentAmmo = 6;
    weapons[1].scale = 0.4f; 
    weapons[1].reloadTime = 1.5f;
    weapons[1].cooldown = 0.5f;    
    weapons[1].automatic = false;  
    weapons[1].flashOffsetX = 60;  // Moved much further Left (135 -> 60)
    weapons[1].flashOffsetY = 110; // Moved Up slightly (125 -> 110)
    weapons[1].flashScale = 0.15f; 

    int currentWeapon = 0;

    //--------------------------------------------------------------------------------------

    // Game Loop Variables
    Vector3 cubePosition = { 0.0f, 1.0f, 0.0f };
    Vector3 enemyPosition = { 4.0f, 1.0f, 4.0f };
    bool enemyActive = true;
    
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
    const float JUMP_FORCE = 8.0f;
    bool isGrounded = true;
    float playerHeight = 2.0f; // Eye height

    // Weapon Switch Logic
    int targetWeapon = currentWeapon;
    bool isSwitching = false;
    float switchTimer = 0.0f;
    float switchDuration = 0.6f;

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        
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
            
            // Mouse Wheel Switching
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
            switchTimer += GetFrameTime();
            
            // Halfway point: Swap weapon
            if (switchTimer >= switchDuration / 2.0f && currentWeapon != targetWeapon) {
                currentWeapon = targetWeapon;
            }
            
            // End switch
            if (switchTimer >= switchDuration) {
                isSwitching = false;
                switchTimer = 0.0f;
            }
        }

        // Store old camera Y to calculate delta for target
        float oldCamY = camera.position.y;

        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        
        // Physics: Apply Gravity
        if (!isGrounded) {
             verticalVelocity -= GRAVITY * GetFrameTime();
        } else {
            // Jump
            if (IsKeyPressed(KEY_SPACE)) {
                verticalVelocity = JUMP_FORCE;
                isGrounded = false;
            }
        }
        
        // Apply Velocity
        camera.position.y += verticalVelocity * GetFrameTime();
        
        // Floor Collision
        if (camera.position.y <= playerHeight) {
            camera.position.y = playerHeight;
            verticalVelocity = 0;
            isGrounded = true;
        } else {
            isGrounded = false;
        }

        // Box Collision (Simple AABB check for feet)
        // Player feet position: (camera.position.x, camera.position.y - playerHeight, camera.position.z)
        Vector3 playerPos = camera.position;
        float feetY = playerPos.y - playerHeight; 
        
        // Check collision with Blue Box (cubePosition) size 2
        // Box Y range: cubePosition.y - 1 to cubePosition.y + 1
        // If feet are above the box top and inside X/Z, land on it.
        float boxTop = cubePosition.y + 1.0f;
        if (playerPos.x > cubePosition.x - 1.2f && playerPos.x < cubePosition.x + 1.2f &&
            playerPos.z > cubePosition.z - 1.2f && playerPos.z < cubePosition.z + 1.2f) {
            
            // Check Landing
            if (verticalVelocity <= 0 && feetY < boxTop && feetY > boxTop - 0.5f) {
                 camera.position.y = boxTop + playerHeight;
                 verticalVelocity = 0;
                 isGrounded = true;
            }
            // Side collision (keeping simple for now, just focused on climbing ON TOP)
        }

        // Sync Camera Target Y
        camera.target.y += (camera.position.y - oldCamY);

        // Reload Logic
        Weapon *w = &weapons[currentWeapon];
        
        if (w->isReloading) {
            w->reloadTimer -= GetFrameTime();
            if (w->reloadTimer <= 0) {
                w->currentAmmo = w->maxAmmo;
                w->isReloading = false;
            }
        } else {
             // Auto-reload if empty (optional, or user press R)
             if (w->currentAmmo <= 0 || IsKeyPressed(KEY_R)) {
                 if (w->currentAmmo < w->maxAmmo) {
                    w->isReloading = true;
                    w->reloadTimer = w->reloadTime;
                 }
             }
        }
        
        // Cooldown Update
        w->timeSinceLastShot += GetFrameTime();

        // Shooting logic
        // Check input based on automatic flag
        bool shootInput = false;
        if (w->automatic) shootInput = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        else shootInput = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (shootInput && !w->isReloading && !isSwitching && w->currentAmmo > 0 && w->timeSinceLastShot >= w->cooldown) {
            // Find inactive bullet
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullets[i].active) {
                    bullets[i].active = true;
                    bullets[i].position = camera.position;
                    // Calculate forward vector
                    Vector3 forward = Vector3Subtract(camera.target, camera.position);
                    bullets[i].direction = Vector3Normalize(forward);
                    
                    w->currentAmmo--;
                    w->timeSinceLastShot = 0.0f;
                    
                    // Recoil impulse
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
               
                // Collision with Enemy
                if (enemyActive) {
                    if (bullets[i].position.x > enemyPosition.x - 1.0f && bullets[i].position.x < enemyPosition.x + 1.0f &&
                        bullets[i].position.z > enemyPosition.z - 1.0f && bullets[i].position.z < enemyPosition.z + 1.0f &&
                        bullets[i].position.y > enemyPosition.y - 1.0f && bullets[i].position.y < enemyPosition.y + 1.0f) {
                            enemyActive = false;
                            bullets[i].active = false;
                    }
                }
                
                // Despawn distance
                 if (Vector3Distance(camera.position, bullets[i].position) > 100.0f) {
                    bullets[i].active = false;
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

                // Draw projectiles
                for (int i = 0; i < MAX_BULLETS; i++) {
                    if (bullets[i].active) {
                        DrawSphere(bullets[i].position, 0.1f, YELLOW); // Smaller bullets
                    }
                }

            EndMode3D();



            // Draw Gun (Realistic Sprite)
            // Center the gun, apply sway, bob, and recoil
            // Use current weapon texture
            
            float scale = w->scale;
            int gunWidth = w->texture.width * scale;
            int gunHeight = w->texture.height * scale;
            
            // Base Position (Bottom Right-ish for standard FPS feel)
            float bobOffsetX = sinf(weaponBob) * 10.0f;
            float bobOffsetY = fabs(cosf(weaponBob)) * 10.0f; // Only bob up/down in arc
            
            float recoilScreenY = recoilOffset * 200.0f; // Convert physics recoil to screen pixels
            
            // Reload Animation (Lower gun)
            float reloadOffsetY = 0.0f;
            if (w->isReloading) {
                // Simple curve: go down then up
                // Normalized time 0 to 1
                float t = 1.0f - (w->reloadTimer / w->reloadTime); 
                if (t < 0.5f) reloadOffsetY = Lerp(0.0f, 200.0f, t * 2.0f);
                else reloadOffsetY = Lerp(200.0f, 0.0f, (t - 0.5f) * 2.0f);
            }
            
            // Switch Animation (Lower gun then raise)
            float switchOffsetY = 0.0f;
            if (isSwitching) {
                float t = switchTimer / switchDuration;
                if (t < 0.5f) {
                    // Going Down (0 to 1 in first half)
                    float halfT = t * 2.0f; 
                    switchOffsetY = Lerp(0.0f, 300.0f, halfT); 
                } else {
                    // Coming Up (0 to 1 in second half)
                    float halfT = (t - 0.5f) * 2.0f;
                    switchOffsetY = Lerp(300.0f, 0.0f, halfT);
                }
            }

            int gunX = (screenWidth / 2) + 120 - (gunWidth / 2) + (int)weaponSway.x + (int)bobOffsetX;
            int gunY = screenHeight - gunHeight + 60 + (int)weaponSway.y + (int)bobOffsetY + (int)recoilScreenY + (int)reloadOffsetY + (int)switchOffsetY; 

            // Draw gun sprite
            DrawTextureEx(w->texture, (Vector2){ (float)gunX, (float)gunY }, 0.0f, scale, WHITE);
            
            // UI: Ammo
            Color ammoColor = (w->currentAmmo <= w->maxAmmo / 4) ? RED : DARKGREEN;
            if (w->isReloading) {
                 const char *text = "RELOADING...";
                 int textWidth = MeasureText(text, 30);
                 DrawText(text, screenWidth - textWidth - 20, screenHeight - 40, 30, ORANGE);
            } else {
                 const char *text = TextFormat("AMMO: %d / %d", w->currentAmmo, w->maxAmmo);
                 int textWidth = MeasureText(text, 40);
                 DrawText(text, screenWidth - textWidth - 20, screenHeight - 50, 40, ammoColor);
            }

            // Muzzle Flash
            if (w->timeSinceLastShot < 0.1f && recoilOffset > 0.1f) {
                // Determine flash position relative to gun
                int flashX = gunX + w->flashOffsetX - (flashTexture.width * w->flashScale / 2);
                int flashY = gunY + w->flashOffsetY - (flashTexture.height * w->flashScale / 2);
                
                BeginBlendMode(BLEND_ADDITIVE);
                    DrawTextureEx(flashTexture, (Vector2){ (float)flashX, (float)flashY }, 0.0f, w->flashScale, WHITE);
                EndBlendMode();
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
    UnloadTexture(revolverTexture);
    UnloadTexture(flashTexture);

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}