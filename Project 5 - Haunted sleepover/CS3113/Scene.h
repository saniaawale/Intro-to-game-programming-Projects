#include "Entity.h"
#include "Enemy.h"

#ifndef SCENE_H
#define SCENE_H

// Forward declaration
class ShaderProgram;

struct GameState
{
    Entity* Leah = nullptr;
    Map* map = nullptr;
    bool playerWon = false;
    bool playerDied = false;

    //ROOM DOORS 
    Entity* door1 = nullptr;
    Entity* door2 = nullptr;

    //ENEMIES
    Enemy* dad = nullptr;
    Enemy* mom = nullptr;
    bool playerCaughtByEnemy = false;
    bool playerTouchingEnemy = false;
    float enemyContactTimer = 0.0f;
    float enemyContactDuration = 2.0f;

    // LIVING ROOM
    Entity* maindoor = nullptr;
    Entity* couch = nullptr;
    Entity* clock = nullptr;
    Entity* piano = nullptr;
    Entity* lamp = nullptr;
    Entity* tvtable = nullptr;
    Entity* tv = nullptr;
    bool tvTableOpen = false;
    bool pianoPlayed = false;
    float pianoTimer = 0.0f;
    float pianoDuration = 15.0f;

    //PARENTS BEDROOM
    Entity* parents_bed = nullptr;
    Entity* dresser = nullptr;
    Entity* bedroomMirror = nullptr;
    Entity* parentsWardrobe = nullptr;
    Entity* bedroom_art = nullptr;
    bool dresserOpen = false;
    bool pwardrobeOpen = false;
    Texture2D goldKeyTexture;
    bool hasGoldKey = false;
    bool lightsOn = true;
    Vector2 dadOriginalPosition = { 0.0f, 0.0f };
    bool emmaReleased = false;

    //KITCHEN 
    Entity* fridge = nullptr;
    Entity* sink = nullptr;
    Entity* kitchen_table = nullptr;
    Entity* bigSpoons = nullptr;
    Entity* microwave = nullptr;
    Entity* kitchenTrash = nullptr;
    Entity* stove_oven = nullptr;
    Entity* exhaust = nullptr;
    Entity* bread = nullptr;
    bool fridgeOpen = false;
    bool microwaveOpen = false;
    bool kitchenSinkDrawerOpen = false;
    bool hasSilverKey = false;
    Texture2D silverKeyTexture;

    //BATHROOM
    Entity* toilet = nullptr;
    Entity* bathroomSink = nullptr;
    Entity* toiletPaper = nullptr;
    Entity* bathtub = nullptr;
    Entity* handwash = nullptr;
    Entity* toothbrushCup = nullptr;
    Entity* bathroomTrash = nullptr;
    Entity* bathroomMirror = nullptr;
    Entity* bathroomShelves = nullptr;
    bool toiletOpen = false;
    bool bathroomTrashOpen = false;
    bool sinkDrawerOpen = false;

    //KIDS BEDROOM 
    Entity* kids_bed = nullptr;
    Entity* kidsDresser = nullptr;
    Entity* posters = nullptr;
    Entity* kidsWardrobe = nullptr;
    Entity* ball = nullptr;
    Entity* shelf = nullptr;
    Entity* curtains = nullptr;
    Entity* emma = nullptr;
    bool kwardrobeOpen = false;
    bool kdresserOpen = false;

    //game state flags 
    bool openingPlayed = false;
    bool dialogueActive = false;
    bool noteFound = false;
    bool showNote = false;
    int dialogueStep = 0;

    Music bgm = { 0 };
    Sound jumpSound = { 0 };
    int playerLives = 3;
    Camera2D camera = { 0 };
    float gameTime = 0.0f;
    bool hasMainDoorKey = false;
    bool hasBrassKey = false;
    int nextSceneID = 0;

    bool hasFlashlight = false;
    bool flashlightActive = false;
    ShaderProgram* flashlightShader = nullptr;
};

enum SceneType
{
    SCENE_NONE,
    KIDS_BEDROOM,
    PARENTS_BEDROOM,
    LIVING_ROOM,
    KITCHEN,
    BATHROOM,
    WIN_SCREEN,
    LOSE_SCREEN
};

class Scene
{
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char* mBGColourHexCode = "#000000";
    Sound* mLoseSound = nullptr;
    Sound* mPianoSound = nullptr;

public:
    Scene();
    Scene(Vector2 origin, const char* bgHexCode);

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
    virtual void drawGameTime(float screenX, float screenY);
    void setShader(ShaderProgram* shader){mGameState.flashlightShader = shader;}
    void setLoseSound(Sound* loseSound) { mLoseSound = loseSound; }
    void setPianoSound(Sound* pianoSound) { mPianoSound = pianoSound; }
    void renderLoseOverlay();
    void renderWinOverlay();

    GameState& getState() { return mGameState; }
    Vector2     getOrigin()          const { return mOrigin; }
    const char* getBGColourHexCode() const { return mBGColourHexCode; }
};

#endif