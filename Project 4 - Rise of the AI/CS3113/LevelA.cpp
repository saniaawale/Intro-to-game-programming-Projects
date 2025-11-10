#include "LevelA.h"
LevelA::LevelA() : Scene{ {0.0f}, nullptr } {}
LevelA::LevelA(Vector2 origin, const char* bgHexCode) : Scene{ origin, bgHexCode } {}

LevelA::~LevelA() {}

void LevelA::initialise()
{
    mGameState.nextSceneID = 0;

    // Initialize shooting variables
    shootCooldown = 0.3f;
    shootCooldownTimer = 0.0f;

    // Store the starting position for respawning
    playerStartPosition = { mOrigin.x - 850.0f, mOrigin.y - 250.0f };

    mGameState.bgm = LoadMusicStream("assets/game/groovy_vibe.mp3");
    SetMusicVolume(mGameState.bgm, 0.33f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/game/cartoon_jump.mp3");

    /*
       ----------- MAP -----------
    */
    mGameState.map = new Map(
        LEVEL_A_WIDTH, LEVEL_A_HEIGHT,   // map grid cols & rows
        (unsigned int*)mLevelData, // grid data
        "assets/game/tileset.png",   // texture filepath
        TILE_DIMENSION,              // tile size
        3, 3,                        // texture cols & rows
        mOrigin                      // in-game origin
    );
    /*
       ----------- ENEMy -----------
    */

    std::map<Direction, std::vector<int>> emptyAtlas = {
       {LEFT,  { 0 }},
       {RIGHT, { 0 }},
    };

    enemies[0] = new Entity(
        { mOrigin.x + 400.0f, mOrigin.y - 250.0f },  // position
        { 200.0f, 200.0f },                           // size
        "assets/game/mushroom/Run.png",               // walking sprite sheet
        ATLAS,                                        // using sprite atlas
        { 8, 1 },                                     // 8 columns, 1 row
        emptyAtlas,                                    // animation atlas
        NPC                                           // entity type
    );
    enemies[0]->setAIType(WANDERER);
    enemies[0]->setAIState(WALKING);
    enemies[0]->setSpeed(100);
    enemies[0]->setFrameSpeed(12);  // Animation speed
    enemies[0]->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    enemies[0]->setColliderDimensions({ 70.0f, 70.0f });
    enemies[0]->setSpriteSheetDimensions({ 8, 1 });

    enemies[0]->loadAnimationTextures(
        "assets/game/mushroom/Idle.png",      // idle (4 frames)
        "assets/game/mushroom/Run.png",       // run (8 frames)
        "assets/game/mushroom/Run.png",       // jump (reuse run)
        "assets/game/mushroom/Attack.png"     // attack (8 frames)
    );

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
        "assets/game/idle_platformer.png",         // texture file address
        ATLAS,                                    // single image or atlas?
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
            { 0.0f, 0.0f },           // position (will be set when fired)
            { 20.0f, 10.0f },         // size (small bullet)
            nullptr,                // no texture, we'll draw a rectangle
            NONE                    // texture type
        );
        bullets[i]->deactivate();   // Start inactive
    }



    /*
       ----------- CAMERA -----------
    */
    mGameState.camera = { 0 };                                    // zero initialize
    mGameState.camera.target = mGameState.xochitl->getPosition(); // camera follows player
    mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
    mGameState.camera.rotation = 0.0f;                            // no rotation
    mGameState.camera.zoom = 1.0f;                                // default zoom
}

void LevelA::shootBullet() {

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

void LevelA::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);


    mGameState.xochitl->update(
        deltaTime,
        nullptr,
        mGameState.map,
        enemies[0],      // Pass enemies for collision
        ENEMY_COUNT   // Pass enemy count
    );

    // UPDATE WANDERER ENEMY
    enemies[0]->update(
        deltaTime,
        mGameState.xochitl, 
        mGameState.map,
        nullptr,
        0
    );

    // Set enemy animation based on collision
    if (mGameState.xochitl->isColliding(enemies[0])) {
        enemies[0]->setAIState(ATTACKING);
    }
    else {
        enemies[0]->setAIState(WALKING);
    }

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

 

    Vector2 playerPos = mGameState.xochitl->getPosition();
    Vector2 cameraTarget = {
       playerPos.x + 200.0f,
       playerPos.y - 150.0f
    };

    int currentTile = mGameState.map->getTileAt(playerPos);

    if (mGameState.xochitl->isColliding(mGameState.door)) {
        mGameState.nextSceneID = 2;  // Trigger scene transition!!!!
    }

    panCamera(&mGameState.camera, &cameraTarget);

    float mapWidth = LEVEL_A_WIDTH * TILE_DIMENSION;
    float mapHeight = LEVEL_A_HEIGHT * TILE_DIMENSION;

    float halfScreenWidth = 640.0f;
    float halfScreenHeight = 360.0f;

    // Calculate map boundaries
    float mapLeft = mOrigin.x - mapWidth / 2.0f;
    float mapRight = mOrigin.x + mapWidth / 2.0f;
    float mapTop = mOrigin.y - mapHeight / 2.0f;
    float mapBottom = mOrigin.y + mapHeight / 2.0f;

    // Constrain camera horizontally
    if (mGameState.camera.target.x - halfScreenWidth < mapLeft) {
        mGameState.camera.target.x = mapLeft + halfScreenWidth;
    }
    if (mGameState.camera.target.x + halfScreenWidth > mapRight) {
        mGameState.camera.target.x = mapRight - halfScreenWidth;
    }

    // Constrain camera vertically
    if (mGameState.camera.target.y - halfScreenHeight < mapTop) {
        mGameState.camera.target.y = mapTop + halfScreenHeight;
    }
    if (mGameState.camera.target.y + halfScreenHeight > mapBottom) {
        mGameState.camera.target.y = mapBottom - halfScreenHeight;
    }


    if (hasLost && IsKeyPressed(KEY_ENTER)) {
        mGameState.nextSceneID = 0;  // Go to menu
        mGameState.playerLives = 3;
        hasLost = false;
        return;
    }

    // update shoot cooldown
    if (shootCooldownTimer > 0.0f) {
        shootCooldownTimer -= deltaTime;
    }

    // update bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i]->isActive()) {
            bullets[i]->update(deltaTime, nullptr, nullptr, nullptr, 0);

            Vector2 bulletPos = bullets[i]->getPosition();
            float xOverlap = 0.0f;
            float yOverlap = 0.0f;

            if (mGameState.map->isSolidTileAt(bulletPos, &xOverlap, &yOverlap)) {
                bullets[i]->deactivate();
                continue;
            }

            // Check if bullet hits the wanderer enemy
            if (bullets[i]->isColliding(enemies[0])) {
                bullets[i]->deactivate();
                enemies[0]->deactivate();  // Kill mushroom
            }

            // Deactivate bullets off screen
            if (bulletPos.x < mapLeft - 200.0f || bulletPos.x > mapRight + 200.0f ||
                bulletPos.y < mapTop - 200.0f || bulletPos.y > mapBottom + 200.0f) {
                bullets[i]->deactivate();
            }
        }
    }
}

void LevelA::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    mGameState.door->render();
    mGameState.xochitl->render();

    int renderedBullets = 0;
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

            renderedBullets++;
        }
    }

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

void LevelA::shutdown()
{
    delete mGameState.xochitl;
    delete mGameState.map;
    delete mGameState.door;

    for (int i = 0; i < MAX_BULLETS; i++) {
        delete bullets[i];
    }

    delete enemies[0]; 

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
}