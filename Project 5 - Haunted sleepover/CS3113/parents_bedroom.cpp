#include "parents_bedroom.h"
#include "ShaderProgram.h" 

Parents_bedroom::Parents_bedroom() : Scene{ {0.0f}, nullptr } {}

Parents_bedroom::Parents_bedroom(Vector2 origin, const char* bgHexCode) : Scene{ origin, bgHexCode } {}

Parents_bedroom::~Parents_bedroom() { shutdown(); }

void Parents_bedroom::initialise()
{
    mGameState.silverKeyTexture = LoadTexture("assets/game/key_silver.png");
    mGameState.goldKeyTexture = LoadTexture("assets/game/key_gold.png");
    playerStartPosition = { mOrigin.x, mOrigin.y - 50.0f };
    mGameState.nextSceneID = -1;

    mGameState.camera = { 0 };
    mGameState.camera.target = mOrigin;
    mGameState.camera.offset = mOrigin;
    mGameState.camera.rotation = 0.0f;
    mGameState.camera.zoom = 1.0f;

    /*
         ---------------MAP----------------
    */
    mGameState.map = new Map(
        PARENTS_BEDROOM_WIDTH, PARENTS_BEDROOM_HEIGHT,
        (unsigned int*)mLevelData,
        "assets/game/tileset.png",
        TILE_DIMENSION,
        3, 3,
        mOrigin
    );

    /*
     ---------------LEAH----------------
    */
    std::map<Direction, std::vector<int>> LeahAnimationAtlas = {
       {LEFT,  { 0, 1 }},
       {RIGHT, { 0, 1 }},
    };

    float spriteWidth = 200.0f;
    float spriteHeight = 150.0f;

    mGameState.Leah = new Entity(
        { mOrigin.x, mOrigin.y - 50.0f },
        { spriteWidth, spriteHeight },
        "assets/game/idleplayer.png",
        ATLAS,
        { 1, 2 },
        LeahAnimationAtlas,
        PLAYER
    );

    mGameState.Leah->loadAnimationTextures(
        "assets/game/idleplayer.png",
        "assets/game/walkPlayer.png",
        "assets/game/idleplayer.png",
        "assets/game/idleplayer.png",
        "assets/game/sprintplayer.png"
    );

    mGameState.Leah->setSpeed(200);
    mGameState.Leah->setFrameSpeed(8);
    mGameState.Leah->setJumpingPower(550.0f);
    mGameState.Leah->setColliderDimensions({
        spriteWidth * 0.25f,
        spriteHeight * 0.35f
        });
    mGameState.Leah->setAcceleration({ 0.0f, 981.0f });

    /*
     ---------------DOORs----------------
    */
    // door1 -> Living Room 
    mGameState.door1 = new Entity(
        { mOrigin.x - 700.0f, mOrigin.y + 50.0f },
        { 150.0f, 250.0f },
        "assets/game/door_tile.png",
        DOOR
    );

    // door2 -> Bathroom 
    mGameState.door2 = new Entity(
        { mOrigin.x + 900.0f, mOrigin.y + 50.0f },
        { 150.0f, 250.0f },
        "assets/game/door_tile.png",
        DOOR
    );


    /*
     ---------------furniture----------------
    */
    //parents_bed
    mGameState.parents_bed = new Entity(
        { mOrigin.x - 60.0f, mOrigin.y + 120.0f },
        { 300.0f, 150.0f },
        "assets/game/parents_bed.png",
        ITEM
    );

    //parents wardrobe
    mGameState.parentsWardrobe = new Entity(
        { mOrigin.x + 500.0f , mOrigin.y + 50.0f },
        { 250.0f, 300.0f },
        "assets/game/parents_wardrobe.png",
        ITEM
    );

    //Mirror
    mGameState.bedroomMirror = new Entity(
        { mOrigin.x - 300.0f, mOrigin.y + 90.0f },
        { 100.0f, 200.0f },
        "assets/game/parents_bedroom_mirror.png",
        ITEM
    );

    // Dresser
    mGameState.dresser = new Entity(
        { mOrigin.x + 200.0f, mOrigin.y + 120.0f },
        { 200.0f, 150.0f },
        "assets/game/parents_dresser.png",
        ITEM
    );

    //bedroom art
    mGameState.bedroom_art = new Entity(
        { mOrigin.x - 60.0f, mOrigin.y - 100.0f },
        { 250.0f, 150.0f },
        "assets/game/bedroom_art.png",
        ITEM
    );

    //lamp
    mGameState.lamp = new Entity(
        { mOrigin.x - 470.0f, mOrigin.y + 90.0f },
        { 100.0f, 200.0f },
        "assets/game/lamp.png",
        ITEM
    );

    /*
------------------EMMA real----------------------
*/
    std::map<Direction, std::vector<int>> EmmaIdleAtlas = {
       {LEFT,  { 0, 1 }},
       {RIGHT, { 0, 1 }},
    };

    mGameState.emma = new Entity(
        { mOrigin.x + 500.0f, mOrigin.y + 100.0f },  
        { 150.0f, 150.0f },
        "assets/game/emma_idle.png",  
        ATLAS,
        { 1, 2 }, 
        EmmaIdleAtlas,
        NPC
    );

    mGameState.emma->setSpeed(200);  
    mGameState.emma->setFrameSpeed(8);
    mGameState.emma->deactivate();

    /*
    ------------------ENEMIES----------------------
    */

    std::map<Direction, std::vector<int>> DadIdleAtlas = {
   {LEFT,  { 0, 1 }},      
   {RIGHT, { 0, 1 }},
    };

    mGameState.dad = new Enemy(
        { mOrigin.x + 350.0f, mOrigin.y + 100.0f },
        { 250.0f, 250.0f },                     
        "assets/game/dad_idle.png",
        ATLAS,                                    
        { 2, 1 },                                
        DadIdleAtlas,
        DAD
    );

    
    //mGameState.dad->loadAnimationTextures(
    //    "assets/game/dad_idle.png",
    //   "assets/game/dad_walk.png",
    //    "assets/game/dad_idle.png",
    //    "assets/game/dad_idle.png",
    //    "assets/game/dad_run.png"  // Sprint texture 
   // );

    mGameState.dad->setSpriteSheetDimensions({ 2, 1 });
    mGameState.dad->setSpeed(100);
    mGameState.dad->setFrameSpeed(8);
    mGameState.dad->setGuardPosition({ mOrigin.x + 350.0f, mOrigin.y + 150.0f });
    //mGameState.dad->setScale({ 200,200 });
    mGameState.dadOriginalPosition = { mOrigin.x + 350.0f, mOrigin.y + 150.0f };

}

void Parents_bedroom::update(float deltaTime)
{

    // Track if piano was playing last frame
    static bool wasPianoPlayingLastFrame = false;

    if (IsKeyPressed(KEY_ENTER)) {
        mGameState.nextSceneID = 0;
    }

    if (IsKeyPressed(KEY_SPACE))
    {
        if (mGameState.hasFlashlight)
        {
            mGameState.flashlightActive = !mGameState.flashlightActive;
        }
    }
    mGameState.Leah->updateAnimationState();

    mGameState.Leah->update(
        deltaTime,
        nullptr,
        mGameState.map,
        nullptr,
        0
    );

    Vector2 playerPos = mGameState.Leah->getPosition();
    Vector2 cameraTarget = {
        playerPos.x,
        playerPos.y - 150.0f
    };
    panCamera(&mGameState.camera, &cameraTarget);

    float mapWidth = PARENTS_BEDROOM_WIDTH * TILE_DIMENSION;
    float mapHeight = PARENTS_BEDROOM_HEIGHT * TILE_DIMENSION;

    float halfScreenWidth = GetScreenWidth() / 2.0f;
    float halfScreenHeight = GetScreenHeight() / 2.0f;

    float mapLeft = mOrigin.x - mapWidth / 2.0f;
    float mapRight = mOrigin.x + mapWidth / 2.0f;
    float mapTop = mOrigin.y - mapHeight / 2.0f;
    float mapBottom = mOrigin.y + mapHeight / 2.0f;

    if (mGameState.camera.target.x - halfScreenWidth < mapLeft) {
        mGameState.camera.target.x = mapLeft + halfScreenWidth;
    }
    if (mGameState.camera.target.x + halfScreenWidth > mapRight) {
        mGameState.camera.target.x = mapRight - halfScreenWidth;
    }

    if (mGameState.camera.target.y - halfScreenHeight < mapTop) {
        mGameState.camera.target.y = mapTop + halfScreenHeight;
    }
    if (mGameState.camera.target.y + halfScreenHeight > mapBottom) {
        mGameState.camera.target.y = mapBottom - halfScreenHeight;
    }

    mGameState.gameTime += deltaTime;

    // WARDROBE INTERACTION 
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        Vector2 worldMousePos = GetScreenToWorld2D(mousePos, mGameState.camera);

        Vector2 wardrobePos = mGameState.parentsWardrobe->getPosition();
        Vector2 wardrobeScale = mGameState.parentsWardrobe->getScale();

        float wardrobeLeft = wardrobePos.x - wardrobeScale.x / 2.0f;
        float wardrobeRight = wardrobePos.x + wardrobeScale.x / 2.0f;
        float wardrobeTop = wardrobePos.y - wardrobeScale.y / 2.0f;
        float wardrobeBottom = wardrobePos.y + wardrobeScale.y / 2.0f;

        if (worldMousePos.x >= wardrobeLeft && worldMousePos.x <= wardrobeRight &&
            worldMousePos.y >= wardrobeTop && worldMousePos.y <= wardrobeBottom)
        {
            if (mGameState.hasSilverKey)
            {
                mGameState.pwardrobeOpen = !mGameState.pwardrobeOpen;

                if (mGameState.pwardrobeOpen)
                {
                    // Open wardrobe and give gold key
                    mGameState.parentsWardrobe->setTexture("assets/game/parents_wardrobe_open.png");

                    if (!mGameState.hasGoldKey)
                    {
                        mGameState.hasGoldKey = true;
                    }

                    if (!mGameState.emmaReleased)
                    {
                        mGameState.emmaReleased = true;
                        mGameState.emma->activate();
                        mGameState.emma->setPosition({mGameState.parentsWardrobe->getPosition().x,mGameState.parentsWardrobe->getPosition().y + 100 });
                    }
                }


                else
                {
                    mGameState.parentsWardrobe->setTexture("assets/game/parents_wardrobe.png");
                }
            }
        }
    }

    if (IsKeyPressed(KEY_E))
    {
        // door1 -> Living Room
        if (mGameState.Leah->isColliding(mGameState.door1))
        {
            mGameState.nextSceneID = 3;
        }

        // door2 -> Bathroom
        if (mGameState.Leah->isColliding(mGameState.door2))
        {
            mGameState.nextSceneID = 6;
        }
    }

    // player sprint check
    bool playerSprinting = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);

    //dad and piano system
    if (mGameState.pianoPlayed)
    {
        // target position
        Vector2 pianoLurePos = { mOrigin.x + 100000.0f, mOrigin.y + 100.0f };

        Vector2 dadPos = mGameState.dad->getPosition();
        Vector2 direction = {
            pianoLurePos.x - dadPos.x,
            pianoLurePos.y - dadPos.y
        };

        float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);

        if (distance > 20.0f)
        {
            direction.x /= distance;
            direction.y /= distance;

            Vector2 newPos = {
                dadPos.x + direction.x * 150.0f * deltaTime,
                dadPos.y + direction.y * deltaTime * 150.0f
            };

            mGameState.dad->setPosition(newPos);
        }

        // Set walking animation while being lured
        UnloadTexture(mGameState.dad->getTexture());
        mGameState.dad->setTexture("assets/game/dad_walk.png");
        mGameState.dad->setSpriteSheetDimensions({ 1, 9 });
        mGameState.dad->setMovement({ 1.0f, 0.0f });
        mGameState.dad->animate(deltaTime);
    }
    else
    {
        
        if (wasPianoPlayingLastFrame && !mGameState.pianoPlayed)
        {
            //teleport Dad back to guard position
            mGameState.dad->setPosition(mGameState.dadOriginalPosition);
            mGameState.dad->setEnemyState(ENEMY_IDLE);
        }

        mGameState.dad->updateAI(
            deltaTime,
            mGameState.Leah,
            mGameState.flashlightActive,
            playerSprinting
        );

        // Update Dad animation based on his state
        if (mGameState.dad->getEnemyState() == ENEMY_CHASING) {

            UnloadTexture(mGameState.dad->getTexture());
            mGameState.dad->setTexture("assets/game/dad_run.png");
            mGameState.dad->setSpriteSheetDimensions({ 1, 8 });
            mGameState.dad->setMovement({ 1.0f, 0.0f });
        }
        else {
            UnloadTexture(mGameState.dad->getTexture());
            mGameState.dad->setTexture("assets/game/dad_idle.png");
            mGameState.dad->setSpriteSheetDimensions({ 1, 2 });
            mGameState.dad->setMovement({ 0.0f, 0.0f });
        }

        mGameState.dad->animate(deltaTime);
    }

    //COLLISION WITH ENEMY / LOSING CONDITION
    if (!mGameState.pianoPlayed && mGameState.Leah->isColliding(mGameState.dad))
    {
        if (!mGameState.playerTouchingEnemy)
        {
            // start timer
            mGameState.playerTouchingEnemy = true;
            mGameState.enemyContactTimer = 0.0f;
        }
        else
        {
            mGameState.enemyContactTimer += deltaTime;
            if (mGameState.enemyContactTimer >= mGameState.enemyContactDuration)
            {
                mGameState.playerCaughtByEnemy = true;
                mGameState.playerDied = true;
            }

            PlaySound(*mLoseSound);
        }
    }
    else
    {
        //  reset timer 
        mGameState.playerTouchingEnemy = false;
        mGameState.enemyContactTimer = 0.0f;
    }


    // EMMA AI
    if (mGameState.emmaReleased && mGameState.emma != nullptr && mGameState.emma->isActive())
    {
        Vector2 emmaPos = mGameState.emma->getPosition();
        Vector2 leahPos = mGameState.Leah->getPosition();

        // Calculate direction to Leah
        Vector2 direction = {
            leahPos.x - emmaPos.x,
            leahPos.y - emmaPos.y
        };
        //distance formula 
        float distanceToLeah = sqrtf(direction.x * direction.x + direction.y * direction.y);

        // Follow Leah
        if (distanceToLeah > 100.0f)
        {
            direction.x /= distanceToLeah;
            direction.y /= distanceToLeah;

            Vector2 newPos = {
                emmaPos.x + direction.x * 150.0f * deltaTime,
                emmaPos.y + direction.y * 150.0f * deltaTime
            };

            mGameState.emma->setPosition(newPos);
            mGameState.emma->setMovement({ 1.0f, 0.0f });
        }
        else
        {
            mGameState.emma->setMovement({ 0.0f, 0.0f });
        }

        mGameState.emma->animate(deltaTime);
    }
    //update piano tracking
    wasPianoPlayingLastFrame = mGameState.pianoPlayed;
}

void Parents_bedroom::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.flashlightShader->begin();

    if (mGameState.hasFlashlight && mGameState.flashlightActive)
    {
        Vector2 playerWorldPos = mGameState.Leah->getPosition();
        Vector2 screenPos;
        screenPos.x = playerWorldPos.x - mGameState.camera.target.x + (GetScreenWidth() / 2.0f);
        screenPos.y = playerWorldPos.y - mGameState.camera.target.y + (GetScreenHeight() / 2.0f);

        mGameState.flashlightShader->setInt("flashlightActive", 1);
        mGameState.flashlightShader->setVector2("flashlightPos", screenPos);
        mGameState.flashlightShader->setFloat("flashlightRadius", 400.0f);
    }
    else
    {
        mGameState.flashlightShader->setInt("flashlightActive", 0);
    }

    mGameState.door1->render();
    if (mGameState.Leah->isColliding(mGameState.door1)) {
        DrawText("Press E - Living Room",
            mGameState.door1->getPosition().x - 100,
            mGameState.door1->getPosition().y - 120,
            20, YELLOW);
    }

    mGameState.door2->render();
    if (mGameState.Leah->isColliding(mGameState.door2)) {
        DrawText("Press E - Bathroom",
            mGameState.door2->getPosition().x - 90,
            mGameState.door2->getPosition().y - 120,
            20, YELLOW);
    }

    mGameState.map->render();

    // render furniture
    mGameState.parents_bed->render();
    mGameState.dresser->render();
    mGameState.bedroomMirror->render();
    mGameState.parentsWardrobe->render();
    mGameState.bedroom_art->render();
    mGameState.lamp->render();
    mGameState.dad->render();

    if (mGameState.emmaReleased && mGameState.emma != nullptr && mGameState.emma->isActive())
    {
        mGameState.emma->render();
    }

    mGameState.Leah->render();

    if (!mGameState.hasSilverKey)
    {
        Vector2 mousePos = GetMousePosition();
        Vector2 worldMousePos = GetScreenToWorld2D(mousePos, mGameState.camera);

        Vector2 wardrobePos = mGameState.parentsWardrobe->getPosition();
        Vector2 wardrobeScale = mGameState.parentsWardrobe->getScale();

        float wardrobeLeft = wardrobePos.x - wardrobeScale.x / 2.0f;
        float wardrobeRight = wardrobePos.x + wardrobeScale.x / 2.0f;
        float wardrobeTop = wardrobePos.y - wardrobeScale.y / 2.0f;
        float wardrobeBottom = wardrobePos.y + wardrobeScale.y / 2.0f;

        if (worldMousePos.x >= wardrobeLeft && worldMousePos.x <= wardrobeRight &&
            worldMousePos.y >= wardrobeTop && worldMousePos.y <= wardrobeBottom)
        {
            DrawText("Locked - Need Silver Key",
                wardrobePos.x - 100,
                wardrobePos.y - 180,
                20, RED);
        }
    }
    else if (mGameState.hasSilverKey)
    {
        Vector2 mousePos = GetMousePosition();
        Vector2 worldMousePos = GetScreenToWorld2D(mousePos, mGameState.camera);

        Vector2 wardrobePos = mGameState.parentsWardrobe->getPosition();
        Vector2 wardrobeScale = mGameState.parentsWardrobe->getScale();

        float wardrobeLeft = wardrobePos.x - wardrobeScale.x / 2.0f;
        float wardrobeRight = wardrobePos.x + wardrobeScale.x / 2.0f;
        float wardrobeTop = wardrobePos.y - wardrobeScale.y / 2.0f;
        float wardrobeBottom = wardrobePos.y + wardrobeScale.y / 2.0f;

        if (worldMousePos.x >= wardrobeLeft && worldMousePos.x <= wardrobeRight &&
            worldMousePos.y >= wardrobeTop && worldMousePos.y <= wardrobeBottom)
        {
            DrawText("Click to open",
                wardrobePos.x - 60,
                wardrobePos.y - 180,
                20, YELLOW);
        }
    }

    if (mGameState.flashlightShader != nullptr && mGameState.flashlightShader->isLoaded())
    {
        mGameState.flashlightShader->end();
    }

    float screenX = mGameState.camera.target.x - (GetScreenWidth() / 2.0f);
    float screenY = mGameState.camera.target.y - (GetScreenHeight() / 2.0f);

    // Draw silver key
    if (mGameState.hasSilverKey) {
        Color keyColor = mGameState.lightsOn ? WHITE : Fade(WHITE, 0.1f);
        DrawTexture(mGameState.silverKeyTexture, screenX + 10, screenY + 50, keyColor);
        DrawText("Silver Key", screenX + 45, screenY + 55, 16, keyColor);
    }

    //Draw gold key
    if (mGameState.hasGoldKey) {
        Color keyColor = mGameState.lightsOn ? WHITE : Fade(WHITE, 0.1f);
        DrawTexture(mGameState.goldKeyTexture, screenX + 10, screenY + 90, keyColor);
        DrawText("Gold Key", screenX + 45, screenY + 95, 16, keyColor);
    }

    drawGameTime(screenX, screenY);

    if (mGameState.hasFlashlight) {
        if (mGameState.flashlightActive) {
            DrawText("FLASHLIGHT ON (SPACE)", screenX + GetScreenWidth() - 280, screenY + 10, 20, YELLOW);
        }
        else {
            DrawText("FLASHLIGHT OFF (SPACE)", screenX + GetScreenWidth() - 280, screenY + 10, 20, GRAY);
        }
    }

    if (mGameState.playerDied) {
        renderLoseOverlay();
    }

}

void Parents_bedroom::shutdown()
{

    delete mGameState.Leah;
    mGameState.Leah = nullptr;

    delete mGameState.map;
    mGameState.map = nullptr;

    delete mGameState.door1;
    mGameState.door1 = nullptr;

    delete mGameState.door2;
    mGameState.door2 = nullptr;


}