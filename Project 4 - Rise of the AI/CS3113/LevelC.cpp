#include "LevelC.h"

LevelC::LevelC() : Scene{ {0.0f}, nullptr } {}
LevelC::LevelC(Vector2 origin, const char* bgHexCode) : Scene{ origin, bgHexCode } {}

LevelC::~LevelC() { shutdown(); }

void LevelC::initialise()
{
    hasWon = false;
    hasLost = false;

    playerStartPosition = { mOrigin.x - 850.0f, mOrigin.y - 250.0f };

    mGameState.bgm = LoadMusicStream("assets/game/groovy_vibe.mp3");
    SetMusicVolume(mGameState.bgm, 0.33f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/game/cartoon_jump.mp3");
    

    /*
       ----------- MAP -----------
    */
    mGameState.map = new Map(
        LEVEL_C_WIDTH, LEVEL_C_HEIGHT,   // map grid cols & rows
        (unsigned int*)mLevelData, // grid data
        "assets/game/tileset.png",   // texture filepath
        TILE_DIMENSION,              // tile size
        3, 3,                        // texture cols & rows
        mOrigin                      // in-game origin
    );

    /*
    ----------- DOOR ENTITY -----------
     */
    mGameState.door = new Entity(
        { mOrigin.x + 900.0f, mOrigin.y + 50 },  // position
        { 100.0f, 150.0f },                       // size    
        "assets/game/door.png",
        NONE
    );

    /*
    ----------- BULLETS -----------
    */
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i] = new Entity(
            { 0.0f, 0.0f },           
            { 20.0f, 10.0f },         
            nullptr,                
            NONE                   
        );
        bullets[i]->deactivate();   
    }


    /*
       ----------- FLYING ENEMY -----------
    */
    std::map<Direction, std::vector<int>> flyerAtlas = {
       {LEFT,  { 0 }},
       {RIGHT, { 0 }},
    };
    enemies[0] = new Entity(
        { mOrigin.x, mOrigin.y },  
        { 150.0f, 150.0f },                           
        "assets/game/flyer/Flight.png",                 
        ATLAS,                                        
        { 8, 1 },                                     
        flyerAtlas,                                  
        NPC                                          
    );

    enemies[0]->setAIType(FLYER);                     
    enemies[0]->setAIState(WALKING);                  
    enemies[0]->setSpeed(120);                        // Flying speed
    enemies[0]->setFrameSpeed(12);                    // Animation speed
    enemies[0]->setAcceleration({ 0.0f, 0.0f });      // no gravity for the flyer 
    enemies[0]->setColliderDimensions({ 60.0f, 60.0f });
    enemies[0]->setSpriteSheetDimensions({ 8, 1 });

    // Load animation textures
    enemies[0]->loadAnimationTextures(
        "assets/game/flyer/Flight.png",      
        "assets/game/flyer/Flight.png",   
        "assets/game/flyer/Flight.png",    
        "assets/game/flyer/Flight.png"     
    );

    enemies[0]->activate();  

    /*
       ----------- PROTAGONIST -----------
    */
    std::map<Direction, std::vector<int>> xochitlAnimationAtlas = {
       {DOWN,  {  0,  1,  2,  3,  4,  5,  6,  7 }},
       {LEFT,  {  8,  9, 10, 11, 12, 13, 14, 15 }},
       {UP,    { 24, 25, 26, 27, 28, 29, 30, 31 }},
       {RIGHT, { 40, 41, 42, 43, 44, 45, 46, 47 }},
    };

    float spriteWidth = 200.0f;
    float spriteHeight = 200.0f;

    mGameState.xochitl = new Entity(
        { mOrigin.x - 850.0f, mOrigin.y - 250.0f }, // position
        { spriteWidth, spriteHeight },             // scale
        "assets/game/idle_platformer.png",        // texture file address
        ATLAS,                                    
        { 7, 1 },                                 // atlas dimensions
        xochitlAnimationAtlas,                    // actual atlas
        PLAYER                                    // entity type
    );

    mGameState.xochitl->loadAnimationTextures(
        "assets/game/idle_platformer.png",
        "assets/game/run_platformer.png",
        "assets/game/jump_platformer.png",
        "assets/game/attack_platformer.png"
    );


    mGameState.xochitl->setJumpingPower(550.0f);
    mGameState.xochitl->setColliderDimensions({
        spriteWidth * 0.25f,
        spriteHeight * 0.35f
        });
    mGameState.xochitl->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    /*
       ----------- CAMERA -----------
    */
    mGameState.camera = { 0 };                                    // zero initialize
    mGameState.camera.target = mGameState.xochitl->getPosition(); // camera follows player
    mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
    mGameState.camera.rotation = 0.0f;                            // no rotation
    mGameState.camera.zoom = 1.0f;                                // default zoom
}

void LevelC::shootBullet() {
    if (shootCooldownTimer > 0.0f) {
        return;
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i]->isActive()) {
            // Get player position
            Vector2 spawnPos = mGameState.xochitl->getPosition();

            // Check facing direction
            Direction playerDir = mGameState.xochitl->getDirection();
            bool facingRight = (playerDir == RIGHT);

            // Position bullet slightly in front of player
            spawnPos.x += facingRight ? 10.0f : -10.0f;

            // Set up the bullet
            bullets[i]->setPosition(spawnPos);
            bullets[i]->setMovement({ facingRight ? 1.0f : -1.0f, 0.0f });
            bullets[i]->setSpeed(500);
            bullets[i]->setAcceleration({ 0.0f, 0.0f });
            bullets[i]->activate();
            shootCooldownTimer = shootCooldown;
            break;
        }
    }
}

void LevelC::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    // Update shoot cooldown
    if (shootCooldownTimer > 0.0f) {
        shootCooldownTimer -= deltaTime;
    }

    // Update bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i]->isActive()) {
            bullets[i]->update(deltaTime, nullptr, nullptr, nullptr, 0);

            Vector2 bulletPos = bullets[i]->getPosition();
            float xOverlap = 0.0f;
            float yOverlap = 0.0f;

            // Deactivate if hits wall
            if (mGameState.map->isSolidTileAt(bulletPos, &xOverlap, &yOverlap)) {
                bullets[i]->deactivate();
                continue;
            }

            if (enemies[0]->isActive() && bullets[i]->isColliding(enemies[0])) {
                bullets[i]->deactivate();
                enemies[0]->deactivate();
            }
           
        }
    }

    mGameState.xochitl->update(
        deltaTime,      // delta time / fixed timestep
        nullptr,        // player
        mGameState.map, // map
        nullptr,        // collidable entities
        0               
    );

    // Update flying enemy
    if (enemies[0]->isActive()) {
        enemies[0]->update(deltaTime, mGameState.xochitl, mGameState.map, nullptr, 0);

        if (mGameState.xochitl->isColliding(enemies[0]) && !mGameState.xochitl->isInvincible()) {
            mGameState.playerLives--;
            mGameState.xochitl->makeInvincible(1.0f);

            if (mGameState.playerLives <= 0) {
                hasLost = true;
            }
            else {
                // Reset player position if they still have lives
                mGameState.xochitl->setPosition(playerStartPosition);
            }
        }
    }

    if (mGameState.xochitl->isColliding(mGameState.door)) {
        hasWon = true;
    }

    if (hasWon && IsKeyPressed(KEY_ENTER)) {
        mGameState.nextSceneID = 0;  // Go to menu
        mGameState.playerLives = 3;
        hasWon = false;
        return;
    }

    if (hasLost && IsKeyPressed(KEY_ENTER)) {
        mGameState.nextSceneID = 0;  // Go to menu
        mGameState.playerLives = 3;
        hasLost = false;
        return;
    }

    // CAMERA
    Vector2 currentPlayerPosition = { mGameState.xochitl->getPosition().x, mOrigin.y };

    panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelC::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.xochitl->render();

    mGameState.door->render();
    mGameState.map->render();

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i]->isActive()) {
            Vector2 pos = bullets[i]->getPosition();
            Vector2 size = bullets[i]->getScale();

            DrawRectangle(
                (int)(pos.x - size.x / 2),
                (int)(pos.y - size.y / 2),
                (int)size.x,
                (int)size.y,
                WHITE
            );
        }
    }


    // render enenmy 
    if (enemies[0]->isActive()) {
        enemies[0]->render();
    }


    float screenX = mGameState.camera.target.x - (GetScreenWidth() / 2.0f);
    float screenY = mGameState.camera.target.y - (GetScreenHeight() / 2.0f);

    DrawText(TextFormat("Lives: %d", mGameState.playerLives),
        screenX + 20,
        screenY + 20,
        30,
        RED);

    // Display win message if player reached the door
    if (hasWon) {
        const char* winText = "YOU WIN!";
        int textWidth = MeasureText(winText, 80);
        DrawText(winText,
            (mGameState.camera.target.x - (GetScreenWidth() / 2.0f)) + (GetScreenWidth() / 2) - (textWidth / 2),
            (mGameState.camera.target.y - (GetScreenHeight() / 2.0f)) + (GetScreenHeight() / 2) - 40,
            80,
            GOLD);

        const char* subText = "Press ENTER to go back to menu";
        int subTextWidth = MeasureText(subText, 30);
        DrawText(subText,
            (mGameState.camera.target.x - (GetScreenWidth() / 2.0f)) + (GetScreenWidth() / 2) - (subTextWidth / 2),
            (mGameState.camera.target.y - (GetScreenHeight() / 2.0f)) + (GetScreenHeight() / 2) + 60,
            30,
            WHITE);
    }

    // Display lose message if player died
    if (hasLost) {
        const char* loseText = "YOU LOSE!";
        int textWidth = MeasureText(loseText, 80);
        DrawText(loseText,
            (mGameState.camera.target.x - (GetScreenWidth() / 2.0f)) + (GetScreenWidth() / 2) - (textWidth / 2),
            (mGameState.camera.target.y - (GetScreenHeight() / 2.0f)) + (GetScreenHeight() / 2) - 40,
            80,
            RED);

        const char* subText = "Press ENTER to return to menu";
        int subTextWidth = MeasureText(subText, 30);
        DrawText(subText,
            (mGameState.camera.target.x - (GetScreenWidth() / 2.0f)) + (GetScreenWidth() / 2) - (subTextWidth / 2),
            (mGameState.camera.target.y - (GetScreenHeight() / 2.0f)) + (GetScreenHeight() / 2) + 60,
            30,
            WHITE);
    }
}

void LevelC::shutdown()
{
    delete mGameState.xochitl;
    delete mGameState.map;

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
}