
/**
* Author: Sania Kishor Awale
* Assignment: The Haunted Sleepover
* Date due: 12/05/2025, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#include "CS3113/cs3113.h"
#include "CS3113/Menu.h"
#include "CS3113/Scene1.h"
#include "CS3113/LivingRoom.h"
#include "CS3113/Kitchen.h"
#include "CS3113/parents_bedroom.h"
#include "CS3113/kids_bedroom.h"
#include "CS3113/Bathroom.h"
#include "CS3113/ShaderProgram.h"

// Global Constants
constexpr int SCREEN_WIDTH = 1000,
SCREEN_HEIGHT = 600,
FPS = 120,
NUMBER_OF_SCENES = 7;

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f,
gTimeAccumulator = 0.0f;
Music gBackgroundMusic = { 0 };
Sound gLoseSound = { 0 };
Sound gWinSound = { 0 };
Sound gPianoSound = { 0 };

ShaderProgram* gFlashlightShader = nullptr;
Texture2D gLoseTexture = { 0 };
Texture2D gWinTexture = { 0 };

Scene* gCurrentScene = nullptr;
std::vector<Scene*> gScenes = {};

// Scene pointers
Menu* gMenu = nullptr;
Scene1* gScene1 = nullptr;
LivingRoom* gLivingRoom = nullptr;
Kitchen* gKitchen = nullptr;
Parents_bedroom* gParentsBedroom = nullptr;
Kids_bedroom* gKidsBedroom = nullptr;
Bathroom* gBathroom = nullptr;

// Function Declarations
void switchToScene(Scene* scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene* scene)
{
    // saving state before switching
    float currentGameTime = 0.0f;
    //bool hasBrassKey = false;
    bool hasSilverKey = false;
    bool hasGoldKey = false;
    bool tvTableOpen = false;
    bool hasFlashlight = false;
    bool flashlightActive = false;
    bool pianoPlayed = false;
    float pianoTimer = 0.0f;
    Vector2 dadOriginalPosition = { 0.0f, 0.0f };
    bool playerDied = false;
    bool playerWon = false;
    bool openingPlayed = false;
    bool noteFound = false;
    bool emmaReleased = false;
    Vector2 emmaPosition = { 0.0f, 0.0f };
    bool emmaActive = false;

    if (gCurrentScene != nullptr) {
        currentGameTime = gCurrentScene->getState().gameTime;
        hasSilverKey = gCurrentScene->getState().hasSilverKey;
        hasGoldKey = gCurrentScene->getState().hasGoldKey;
        tvTableOpen = gCurrentScene->getState().tvTableOpen;
        hasFlashlight = gCurrentScene->getState().hasFlashlight;
        flashlightActive = gCurrentScene->getState().flashlightActive;
        pianoPlayed = gCurrentScene->getState().pianoPlayed;
        pianoTimer = gCurrentScene->getState().pianoTimer;
        dadOriginalPosition = gCurrentScene->getState().dadOriginalPosition;
        playerDied = gCurrentScene->getState().playerDied;
        playerWon = gCurrentScene->getState().playerWon;  
        openingPlayed = gCurrentScene->getState().openingPlayed;
        noteFound = gCurrentScene->getState().noteFound;
        emmaReleased = gCurrentScene->getState().emmaReleased;
        if (gCurrentScene->getState().emma != nullptr) {
            emmaPosition = gCurrentScene->getState().emma->getPosition();
            emmaActive = gCurrentScene->getState().emma->isActive();
        }
    }

    // switching to a new scene
    gCurrentScene = scene;
    gCurrentScene->initialise();
    gCurrentScene->getState().nextSceneID = -1;

    // bring it back
    gCurrentScene->getState().gameTime = currentGameTime;
    gCurrentScene->getState().hasSilverKey = hasSilverKey;
    gCurrentScene->getState().hasGoldKey = hasGoldKey;
    gCurrentScene->getState().tvTableOpen = tvTableOpen;
    gCurrentScene->getState().hasFlashlight = hasFlashlight;
    gCurrentScene->getState().flashlightActive = flashlightActive;
    gCurrentScene->getState().pianoPlayed = pianoPlayed;
    gCurrentScene->getState().pianoTimer = pianoTimer;
    gCurrentScene->getState().dadOriginalPosition = dadOriginalPosition;
    gCurrentScene->getState().playerDied = playerDied;
    gCurrentScene->getState().playerWon = playerWon;
    gCurrentScene->getState().openingPlayed = openingPlayed;
    gCurrentScene->getState().noteFound = noteFound;
    gCurrentScene->getState().emmaReleased = emmaReleased;
    if (emmaReleased && gCurrentScene->getState().emma != nullptr) {
        gCurrentScene->getState().emma->setPosition(emmaPosition);
        if (emmaActive) {
            gCurrentScene->getState().emma->activate();
        }
    }
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "The Sleepover");
    InitAudioDevice();

    gBackgroundMusic = LoadMusicStream("assets/game/ominous_soundscape.mp3");
    gLoseSound = LoadSound("assets/game/girl_scream.mp3"); 
    //gWinSound = LoadSound("assets/audio/victory_chime.wav"); 
    gPianoSound = LoadSound("assets/game/piano.mp3");

    //load flashlight shader 
    gFlashlightShader = new ShaderProgram();
    gFlashlightShader->load("shaders/vertex.glsl", "shaders/fragment.glsl");

    SetMusicVolume(gBackgroundMusic, 1.0f);
    PlayMusicStream(gBackgroundMusic);
    SetSoundVolume(gLoseSound, 1.0f);
    SetSoundVolume(gPianoSound, 0.8f);

    gMenu = new Menu(ORIGIN, "#2C1817");
    gScene1 = new Scene1(ORIGIN, "#2C1810");
    gLivingRoom = new LivingRoom(ORIGIN, "#3D2825");
    gKitchen = new Kitchen(ORIGIN, "#5B518F");
    gParentsBedroom = new Parents_bedroom(ORIGIN, "#59964Bw");
    gKidsBedroom = new Kids_bedroom(ORIGIN, "#CC9562");
    gBathroom = new Bathroom(ORIGIN, "#A3CFD4");

    gMenu->setShader(gFlashlightShader);
    gScene1->setShader(gFlashlightShader);
    gLivingRoom->setShader(gFlashlightShader);
    gKitchen->setShader(gFlashlightShader);
    gParentsBedroom->setShader(gFlashlightShader);
    gKidsBedroom->setShader(gFlashlightShader);
    gBathroom->setShader(gFlashlightShader);
    
    gParentsBedroom->setLoseSound(&gLoseSound);
    gLivingRoom->setPianoSound(&gPianoSound);
    gKitchen->setPianoSound(&gPianoSound);
    gParentsBedroom->setPianoSound(&gPianoSound);
    gBathroom->setPianoSound(&gPianoSound);
    gKidsBedroom->setPianoSound(&gPianoSound);

    gScenes.push_back(gMenu);               //0
    gScenes.push_back(gScene1);             //1
    gScenes.push_back(gKidsBedroom);        //2
    gScenes.push_back(gLivingRoom);         //3
    gScenes.push_back(gKitchen);            //4
    gScenes.push_back(gParentsBedroom);     //5
    gScenes.push_back(gBathroom);           //6


    switchToScene(gScenes[0]);

    SetTargetFPS(FPS);
}

void processInput()
{
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) {
        gAppStatus = TERMINATED;
    }

    // Don't process input if player died or won
    if (gCurrentScene != nullptr && (gCurrentScene->getState().playerDied || gCurrentScene->getState().playerWon))
    {
        // Only allow restart
        if (IsKeyPressed(KEY_ENTER))
        {
            switchToScene(gScenes[0]);  // Return to menu
        }
        if (IsKeyPressed(KEY_ESCAPE))
        {
            gAppStatus = TERMINATED;
        }
        return; 
    }

    if (gCurrentScene != gScene1 && gCurrentScene->getState().Leah != nullptr) {
        gCurrentScene->getState().Leah->resetMovement();

        if (IsKeyDown(KEY_A)) gCurrentScene->getState().Leah->moveLeft();
        else if (IsKeyDown(KEY_D)) gCurrentScene->getState().Leah->moveRight();

        if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
            gCurrentScene->getState().Leah->setSprinting(true);
        }
        else {
            gCurrentScene->getState().Leah->setSprinting(false);
        }

        if (GetLength(gCurrentScene->getState().Leah->getMovement()) > 1.0f) {
            gCurrentScene->getState().Leah->normaliseMovement();
        }
    }

    // Debug scene switching
    if (IsKeyPressed(KEY_J)) {
        switchToScene(gScenes[2]);
    }

    if (IsKeyPressed(KEY_K)) {
        switchToScene(gScenes[3]);
    }

    if (IsKeyPressed(KEY_L)) {
        switchToScene(gScenes[4]);
    }

    if (IsKeyPressed(KEY_M)) {
        switchToScene(gScenes[5]);
    }

    if (IsKeyPressed(KEY_N)) {
        switchToScene(gScenes[6]);
    }
}

void update()
{
    float ticks = (float)GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        UpdateMusicStream(gBackgroundMusic);

        // CHECK DEATH TIMER - DIE AT 3:00 AM (180 minutes)
        if (gCurrentScene != nullptr &&
            !gCurrentScene->getState().playerDied &&
            !gCurrentScene->getState().playerWon)
        {
            float gameTime = gCurrentScene->getState().gameTime;

            if (gameTime >= 180.0f)
            {
                // Player DIED
                gCurrentScene->getState().playerDied = true;
                PlaySound(gLoseSound);
                SetMusicVolume(gBackgroundMusic, 0.1f);
            }
        }

        // Only update if player hasn't died or won
        if (gCurrentScene != nullptr &&
            !gCurrentScene->getState().playerDied &&
            !gCurrentScene->getState().playerWon)
        {
            gCurrentScene->update(FIXED_TIMESTEP);
        }

        deltaTime -= FIXED_TIMESTEP;
    }

    gTimeAccumulator = deltaTime;
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
    delete gMenu;
    delete gScene1;
    delete gLivingRoom;
    delete gKitchen;
    delete gParentsBedroom;
    delete gKidsBedroom;
    delete gBathroom;

    for (int i = 0; i < NUMBER_OF_SCENES; i++) {
        gScenes[i] = nullptr;
    }

    if (gFlashlightShader != nullptr) {
        delete gFlashlightShader;
        gFlashlightShader = nullptr;
    }
    StopMusicStream(gBackgroundMusic);
    UnloadMusicStream(gBackgroundMusic);

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

        // Scene transitions
        if (gCurrentScene->getState().nextSceneID >= 0 &&
            !gCurrentScene->getState().playerDied &&
            !gCurrentScene->getState().playerWon)
        {
            int id = gCurrentScene->getState().nextSceneID;
            switchToScene(gScenes[id]);
        }

        render();
    }

    shutdown();

    return 0;
}