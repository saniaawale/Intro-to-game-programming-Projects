/**
* Author: Sania Kishor Awale
* Assignment: Lunar Lander
* Date due: 2025-10-29 11:59pm (With Accomodations)
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/Entity.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 60;

constexpr char BG_COLOUR[]    = "#000000";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr int   NUMBER_OF_PLATFORMS     = 5;
constexpr float ACCELERATION_OF_GRAVITY = 30.0f, 
                FIXED_TIMESTEP          = 1.0f / 60.0f;

constexpr float PLATFORM_WIDTH = 100.0f;
constexpr float PLATFORM_HEIGHT = 20.0f; 

constexpr float THRUST_ACCELERATION  = 100.0f;
constexpr float LANDER_SIZE = 40.0f;
constexpr float FUEL_CONSUMPTION_RATE = 0.5f;
constexpr float STARTING_FUEL = 500.0f;
constexpr float MAX_SAFE_LANDING_SPEED = 80.0f;
constexpr float MAX_SAFE_ANGLE = 15.0f;

enum GameState { PLAYING, MISSION_ACCOMPLISHED, MISSION_FAILED };
enum PlatformType { SAFE_LANDING, DANGEROUS };
PlatformType gPlatformTypes[NUMBER_OF_PLATFORMS];// array to keep track of platform types

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;
float gFuel = STARTING_FUEL;
GameState gGameState = PLAYING;

//lander properties
Vector2 gLanderPosition = { ORIGIN.x, 100.0f };
Vector2 gLanderAcceleration = { 0.0f, ACCELERATION_OF_GRAVITY };
float gLanderRotation = 0.0f;

Entity* gPlatforms = nullptr;
Entity* gLander = nullptr;
Entity* gWinScreen = nullptr;
Entity* gLoseScreen = nullptr;
Entity* fuelBarBackground = nullptr;
Entity* fuelBarFill = nullptr;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
void checkCollisions();
void addPlatform();

//platform helper
void addPlatform(Entity* platforms, int index, float x, float y, float width, float height,PlatformType type,const char* texturePath) {
    platforms[index].setPosition(Vector2{ x, y });
    platforms[index].setScale(Vector2{ width, height });
    platforms[index].setTexture(texturePath);
    platforms[index].setColliderDimensions(Vector2{ width, height });
    gPlatformTypes[index] = type;  
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lunar Lander");

    /*
       ----------- FUEL BAR -----------
   */
    //THE RED BACKGROUND
    fuelBarBackground = new Entity(
        { 100, 50 },
        { 300, 30 },
        "assets/red_tile.png"  
    );

    //THE FUEL COUNT
    fuelBarFill = new Entity(
        { 100, 50 },
        { 300, 30 },
        "assets/green_tile.png"
    );


    /*
        ----------- PLAYER -----------
    */
    gLander = new Entity(
        { ORIGIN.x + 100.0f , 50.0f },  
        { LANDER_SIZE, LANDER_SIZE * 1.2f },
        "assets/idel-001.png"
    );

    //gLander stuff
    gLander->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    gLander->setColliderDimensions({ LANDER_SIZE, LANDER_SIZE * 1.2f });
    gLander->setSpeed(0);
    gLander->setAngle(0.0f);


    /*
        ----------- Winning Screen -----------
    */
    gWinScreen = new Entity(
        { ORIGIN.x, ORIGIN.y },
        { SCREEN_WIDTH, SCREEN_HEIGHT },
        "assets/Mission_Accomplished.png"
    );


    /*
        ----------- Losing Screen -----------
    */
    gLoseScreen = new Entity(
        { ORIGIN.x, ORIGIN.y },
        { SCREEN_WIDTH, SCREEN_HEIGHT },
        "assets/Mission_Failed.png"
    );

    /*
        ----------- SIMPLE MAP -----------
    */

    gPlatforms = new Entity[NUMBER_OF_PLATFORMS];
    
    // safe landing platforms
    addPlatform(gPlatforms, 0, 200.0f, SCREEN_HEIGHT - 50.0f, PLATFORM_WIDTH * 1.5f, PLATFORM_HEIGHT, SAFE_LANDING, "assets/green_tile.png");
    addPlatform(gPlatforms, 1,SCREEN_WIDTH - 200.0f, SCREEN_HEIGHT - 50.0f,PLATFORM_WIDTH * 1.5f, PLATFORM_HEIGHT, SAFE_LANDING, "assets/green_tile.png");

    // Dangerous platforms
    addPlatform(gPlatforms, 2, 500.0f, SCREEN_HEIGHT- 200.0f, PLATFORM_WIDTH, PLATFORM_HEIGHT, DANGEROUS, "assets/red_tile.png");
    addPlatform(gPlatforms, 3, 800.0f, SCREEN_HEIGHT - 150.0f,PLATFORM_WIDTH, PLATFORM_HEIGHT, DANGEROUS, "assets/red_tile.png");
    addPlatform(gPlatforms, 4, SCREEN_WIDTH - 800, SCREEN_HEIGHT- 200, PLATFORM_WIDTH, PLATFORM_HEIGHT,DANGEROUS, "assets/red_tile.png");

    SetTargetFPS(FPS);

}

void processInput() 
{   
    bool fuelUsed = false;
    Vector2 currentAccel = { 0.0f, ACCELERATION_OF_GRAVITY };
    constexpr float ROTATION_INCREMENT = 2.0f;

    if (IsKeyDown(KEY_LEFT)) {
        float currentAngle = gLander->getAngle();
        currentAngle += ROTATION_INCREMENT;//rotate left
        if (currentAngle > 90.0f) currentAngle = 90.0f;
        gLander->setAngle(currentAngle);

    }
    if (IsKeyDown(KEY_RIGHT)) {
        float currentAngle = gLander->getAngle();
        currentAngle -= ROTATION_INCREMENT; //rotate right
        if (currentAngle < -90.0f) currentAngle = -90.0f;
        gLander->setAngle(currentAngle);

    }
    if (IsKeyDown(KEY_UP) && gFuel > 0.0f) {
        float angleDegrees = gLander->getAngle();
        float angleRadians = angleDegrees * PI / 180.0f;
        float thrustX = sin(angleRadians) * THRUST_ACCELERATION;
        float thrustY = -cos(angleRadians) * THRUST_ACCELERATION;

        currentAccel.x += thrustX;
        currentAccel.y += thrustY;
        fuelUsed = true;

    }
    gLander->setAcceleration(currentAccel);

    if (fuelUsed) {
        gFuel -= FUEL_CONSUMPTION_RATE;
        if (gFuel < 0.0f) gFuel = 0.0f;
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
   
}

void update()
{
    // Delta time
    float ticks = (float)GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    // Fixed timestep
    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        // moving platform
        static float platformDirection = 1.0f;
        static float platformSpeed = 50.0f;
        Vector2 platformPos = gPlatforms[3].getPosition();
        platformPos.x += platformDirection * platformSpeed * FIXED_TIMESTEP;

        if (platformPos.x > SCREEN_WIDTH - PLATFORM_WIDTH * 0.6f ||
            platformPos.x < PLATFORM_WIDTH * 0.6f) {
            platformDirection *= -1.0f;
        }
        gPlatforms[3].setPosition(platformPos);

        //gLander movement
        Vector2 accel = gLander->getAcceleration();
        Vector2 velocity = gLander->getVelocity();
        Vector2 pos = gLander->getPosition();
        velocity.x += accel.x * FIXED_TIMESTEP;
        velocity.y += accel.y * FIXED_TIMESTEP;
        Vector2 newPos;
        newPos.x = pos.x + velocity.x * FIXED_TIMESTEP;
        newPos.y = pos.y + velocity.y * FIXED_TIMESTEP;
        gLander->setPosition(newPos);

        //  Flags to sense collision
        bool hitPlatform = false;
        int hitPlatformIndex = -1;

        //collision condition ( i put isColliding in public) 
        for (int i = 0; i < NUMBER_OF_PLATFORMS; i++) {
            if (gLander->isColliding(&gPlatforms[i])) {
                hitPlatform = true;
                hitPlatformIndex = i;
                break;
            }
        }
        gLander->update(FIXED_TIMESTEP, gPlatforms, NUMBER_OF_PLATFORMS);

        // If detected a collision
        if (hitPlatform) {
            velocity = gLander->getVelocity();
            float landingSpeed = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y);
            float landerAngle = gLander->getAngle();


            if (gPlatformTypes[hitPlatformIndex] == SAFE_LANDING) { // safe landing
                if ( landingSpeed <= MAX_SAFE_LANDING_SPEED && landerAngle <= MAX_SAFE_ANGLE) {
                    gGameState = MISSION_ACCOMPLISHED;
                }
                else { 
                    gGameState = MISSION_FAILED;
                }
            }
            else { // any other platform 
                gGameState = MISSION_FAILED;
            }
            return;
        }

        // Boundary checking
        pos = gLander->getPosition();
        if (pos.x < 0 || pos.x > SCREEN_WIDTH || pos.y < 0 || pos.y > SCREEN_HEIGHT) {
            gGameState = MISSION_FAILED;
            return;
        }

        deltaTime -= FIXED_TIMESTEP;
    }

    // FUEL UI BAR
    float percentage = gFuel / STARTING_FUEL;
    fuelBarFill->setScale({ 300 * percentage, 30 });
    fuelBarFill->setPosition({ 100 -(300 * (1.0f -percentage) / 2.0f), 50 });

    gTimeAccumulator = deltaTime;
}


void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    // Draw platforms
    for (int i = 0; i < NUMBER_OF_PLATFORMS; i++) {
        gPlatforms[i].render();
    }

    gLander->render();

    //rendering fuel bar UI 
    fuelBarBackground->render();
    fuelBarFill->render();

    if (gGameState == MISSION_ACCOMPLISHED) {
        gWinScreen->render();
    }
    else if (gGameState == MISSION_FAILED) {
        gLoseScreen->render();
    }

    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}
