

/**
* Author: Sania Kishor Awale
* Assignment: Rise of the AI
* Date due: 2025-11-09, 11:59pm (With Accomodations)
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/



#include "CS3113/cs3113.h"
#include "CS3113/Menu.h"
#include "CS3113/LevelA.h"
#include "CS3113/LevelB.h"
#include "CS3113/LevelC.h"  

// Global Constants
constexpr int SCREEN_WIDTH     = 1000,
              SCREEN_HEIGHT    = 600,
              FPS              = 120,
              NUMBER_OF_LEVELS = 4;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
int gPlayerLives = 3;
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

// Global scene pointers
Menu* gMenu = nullptr;
LevelA* gLevelA = nullptr;
LevelB* gLevelB = nullptr;
LevelC* gLevelC = nullptr;  

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene)
{   
    gCurrentScene = scene;
    gCurrentScene->initialise();
    gCurrentScene->getState().playerLives = gPlayerLives;
    gCurrentScene->getState().nextSceneID = -1;
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scenes");
    InitAudioDevice();

    gMenu = new Menu(ORIGIN, "#C0897E");
    gLevelA = new LevelA(ORIGIN, "#396C73");
    gLevelB = new LevelB(ORIGIN, "#011627");
    gLevelC = new LevelC(ORIGIN, "#573380");

    gLevels.push_back(gMenu);
    gLevels.push_back(gLevelA);
    gLevels.push_back(gLevelB);
    gLevels.push_back(gLevelC);

    switchToScene(gLevels[0]);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (gCurrentScene->getState().xochitl != nullptr)
    {
        gCurrentScene->getState().xochitl->resetMovement();

        if      (IsKeyDown(KEY_A)) gCurrentScene->getState().xochitl->moveLeft();
        else if (IsKeyDown(KEY_D)) gCurrentScene->getState().xochitl->moveRight();

        if (IsKeyPressed(KEY_W) && gCurrentScene->getState().xochitl->isCollidingBottom())
        {
            gCurrentScene->getState().xochitl->jump();
            PlaySound(gCurrentScene->getState().jumpSound);
        }

        if (IsKeyPressed(KEY_SPACE)) {
            gCurrentScene->shootBullet();  
        }

        if (GetLength(gCurrentScene->getState().xochitl->getMovement()) > 1.0f) 
            gCurrentScene->getState().xochitl->normaliseMovement();
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

    
}

void update() 
{
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);

        gPlayerLives = gCurrentScene->getState().playerLives;


        deltaTime -= FIXED_TIMESTEP;
    }
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCurrentScene->getState().camera);

    gCurrentScene->render();

    EndMode2D();
    EndDrawing();
}

void shutdown() 
{
    delete gLevelA;
    delete gLevelB;

    for (int i = 0; i < NUMBER_OF_LEVELS; i++) gLevels[i] = nullptr;

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();

        if (gCurrentScene->getState().nextSceneID == -2) {
            gAppStatus = TERMINATED;
            break;
        }

        if (gCurrentScene->getState().nextSceneID >= 0)
        {
            int id = gCurrentScene->getState().nextSceneID;
            switchToScene(gLevels[id]);
        }

        render();
    }

    shutdown();

    return 0;
}