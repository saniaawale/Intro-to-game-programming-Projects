#include "LivingRoom.h"
#include "ShaderProgram.h" 
LivingRoom::LivingRoom() : Scene{ {0.0f}, nullptr } {}

LivingRoom::LivingRoom(Vector2 origin, const char* bgHexCode) : Scene{ origin, bgHexCode } {}

LivingRoom::~LivingRoom() { shutdown(); }

void LivingRoom::initialise()
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
        LIVING_ROOM_WIDTH, LIVING_ROOM_HEIGHT,
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
    // door to kitchen
    mGameState.door1 = new Entity(
        { mOrigin.x + 900.0f, mOrigin.y + 50.0f },
        { 150.0f, 250.0f },
        "assets/game/door_tile.png",
        DOOR
    );

    // this is the main exit door  which locked until u get key
    mGameState.maindoor = new Entity(
        { mOrigin.x - 900.0f, mOrigin.y + 50.0f },
        { 150.0f, 250.0f },
        "assets/game/door_tile.png",
        DOOR
    );
    /*
     ---------------EMMA----------------
    */
    std::map<Direction, std::vector<int>> EmmaIdleAtlas = {
   {LEFT,  { 0, 1 }},
   {RIGHT, { 0, 1 }},
    };

    mGameState.emma = new Entity(
        { mOrigin.x, mOrigin.y },  
        { 150.0f, 150.0f },
        "assets/game/emma_idle.png",
        ATLAS,
        { 1, 2 },
        EmmaIdleAtlas,
        NPC
    );
    mGameState.emma->loadAnimationTextures(
        "assets/game/emma_idle.png",
        "assets/game/emma_walk.png",
        "assets/game/idleplayer.png",
        "assets/game/idleplayer.png",
        "assets/game/sprintplayer.png"
    );
    mGameState.emma->setSpeed(200);
    mGameState.emma->setFrameSpeed(8);
    mGameState.emma->deactivate();


    /*
     ---------------furniture----------------
    */
    //couch
    mGameState.couch = new Entity(
        { mOrigin.x - 360.0f, mOrigin.y + 120.0f },
        { 300.0f, 150.0f },
        "assets/game/sofa.png",
        ITEM
    );

    mGameState.couch->setAcceleration({ 0.0f, 981.0f });

    //tv table
    mGameState.tvtable = new Entity(
        { mOrigin.x - 50.0f, mOrigin.y + 140.0f },
        { 300.0f, 100.0f },
        "assets/game/tv_table.png",
        ITEM
    );

    //lamp
    mGameState.lamp = new Entity(
        { mOrigin.x - 570.0f, mOrigin.y + 90.0f },
        { 100.0f, 200.0f },
        "assets/game/lamp.png",
        ITEM
    );

    //clock
    mGameState.clock = new Entity(
        { mOrigin.x + 200.0f, mOrigin.y },
        { 50.0f, 50.0f },
        "assets/game/clock.png",
        ITEM
    );

    //lamp
    //mGameState.lamp = new Entity(
        //{ mOrigin.x - 570.0f, mOrigin.y + 90.0f },
        //{ 100.0f, 200.0f },
        //"assets/game/lamp.png",
        //ITEM
    //);

    //piano
    mGameState.piano = new Entity(
        { mOrigin.x + 400.0f, mOrigin.y + 70.0f },
        { 350.0f, 250.0f },
        "assets/game/piano.png",
        ITEM
    );

    //piano
    //mGameState.piano = new Entity(
        //{ mOrigin.x + 400.0f, mOrigin.y + 70.0f },
        //{ 350.0f, 250.0f },
        //"assets/game/piano.png",
        //ITEM
    //);
}

    
void LivingRoom::update(float deltaTime) {

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

    float mapWidth = LIVING_ROOM_WIDTH * TILE_DIMENSION;
    float mapHeight = LIVING_ROOM_HEIGHT * TILE_DIMENSION;

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


    // CLICK HANDLER FOR TV TABLE
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();

        Vector2 worldMousePos = GetScreenToWorld2D(mousePos, mGameState.camera);

        Vector2 drawerPos = mGameState.tvtable->getPosition();
        Vector2 drawerScale = mGameState.tvtable->getScale();

        // calculate left drawer bounding box
        float drawerLeft = drawerPos.x - drawerScale.x / 2.0f;
        float drawerRight = drawerPos.x + 20.0f;
        float drawerTop = drawerPos.y - drawerScale.y / 2.0f;
        float drawerBottom = drawerPos.y + drawerScale.y / 2.0f;

        // check if click is inside drawer bounds
        if (worldMousePos.x >= drawerLeft && worldMousePos.x <= drawerRight &&
            worldMousePos.y >= drawerTop && worldMousePos.y <= drawerBottom)
        {
            if (!mGameState.tvTableOpen)
            {
                // open drawers
                UnloadTexture(mGameState.tvtable->getTexture());
                mGameState.tvtable->setTexture("assets/game/tvtable_open_right.png");
                mGameState.tvTableOpen = true;
            }
            else
            {
                // close drawers
                UnloadTexture(mGameState.tvtable->getTexture());
                mGameState.tvtable->setTexture("assets/game/tv_table.png");
                mGameState.tvTableOpen = false;
            }
        }

        // calculate right drawer bounding box
        float drawer2Left = drawerPos.x + 20.0f;
        float drawer2Right = drawerPos.x + drawerScale.x / 2.0f;
        float drawer2Top = drawerPos.y - drawerScale.y / 2.0f;
        float drawer2Bottom = drawerPos.y + drawerScale.y / 2.0f;

        // check if click is inside drawer bounds
        if (worldMousePos.x >= drawer2Left && worldMousePos.x <= drawer2Right &&
            worldMousePos.y >= drawer2Top && worldMousePos.y <= drawer2Bottom)
        {
            if (!mGameState.tvTableOpen)
            {
                // open drawers
                UnloadTexture(mGameState.tvtable->getTexture());
                mGameState.tvtable->setTexture("assets/game/tvtable_open_left.png");
                mGameState.tvTableOpen = true;
            }
            else
            {
                // close drawers
                UnloadTexture(mGameState.tvtable->getTexture());
                mGameState.tvtable->setTexture("assets/game/tv_table.png");
                mGameState.tvTableOpen = false;
            }
        }
    }

    // CLICK TO PLAY PIANO
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        Vector2 worldMousePos = GetScreenToWorld2D(mousePos, mGameState.camera);

        Vector2 pianoPos = mGameState.piano->getPosition();
        Vector2 pianoScale = mGameState.piano->getScale();

        float pianoLeft = pianoPos.x - pianoScale.x / 2.0f;
        float pianoRight = pianoPos.x + pianoScale.x / 2.0f;
        float pianoTop = pianoPos.y - pianoScale.y / 2.0f;
        float pianoBottom = pianoPos.y + pianoScale.y / 2.0f;

        if (worldMousePos.x >= pianoLeft && worldMousePos.x <= pianoRight &&
            worldMousePos.y >= pianoTop && worldMousePos.y <= pianoBottom)
        {
            // Play piano 
            if (!mGameState.pianoPlayed)
            {
                mGameState.pianoPlayed = true;
                mGameState.pianoTimer = 0.0f;

                if (mPianoSound != nullptr)
                {
                    PlaySound(*mPianoSound);
                }
            }
        }
    }

    // Update piano timer
    if (mGameState.pianoPlayed)
    {
        mGameState.pianoTimer += deltaTime;

        if (mGameState.pianoTimer >= mGameState.pianoDuration)
        {
            // Piano effect ends
            mGameState.pianoPlayed = false;
            mGameState.pianoTimer = 0.0f;
        }
    }

    if (IsKeyPressed(KEY_E))
    {
        // door1 -> kitchen
        if (mGameState.Leah->isColliding(mGameState.door1))
        {
            mGameState.nextSceneID = 4;  
        }

        // maindoor -> exit 
        if (mGameState.Leah->isColliding(mGameState.maindoor))
        {
            if (mGameState.hasGoldKey)
            {
                mGameState.playerWon = true;// win screen
            }
        }
    }

    // EMMA AI
    if (mGameState.emmaReleased && mGameState.emma != nullptr && mGameState.emma->isActive())
    {
        Vector2 emmaPos = mGameState.emma->getPosition();
        Vector2 leahPos = mGameState.Leah->getPosition();

        Vector2 direction = {
            leahPos.x - emmaPos.x,
            leahPos.y - emmaPos.y
        };

        float distanceToLeah = sqrtf(direction.x * direction.x + direction.y * direction.y);

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
            UnloadTexture(mGameState.emma->getTexture());
            mGameState.emma->setTexture("assets/game/emma_walk.png");
            mGameState.emma->setSpriteSheetDimensions({ 1, 9 });
        }
        else
        {
            mGameState.emma->setMovement({ 0.0f, 0.0f });
            UnloadTexture(mGameState.emma->getTexture());
            mGameState.emma->setTexture("assets/game/emma_idle.png");
            mGameState.emma->setSpriteSheetDimensions({ 1, 2 });
        }

        mGameState.emma->animate(deltaTime);
    }


}

void LivingRoom::render()
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

    // regular door

    mGameState.door1->render();

    if (mGameState.Leah->isColliding(mGameState.door1)) {
        DrawText("Press E - Kitchen",
            mGameState.door1->getPosition().x - 120,
            mGameState.door1->getPosition().y - 120,
            20, YELLOW);
    }

    // main door 

     mGameState.maindoor->render();

    if (mGameState.Leah->isColliding(mGameState.maindoor)) {
        if (mGameState.hasGoldKey) {
            DrawText("Press E - ESCAPE!",
            mGameState.maindoor->getPosition().x - 90,
            mGameState.maindoor->getPosition().y - 140,
            24, GREEN);
        }
        else {
            DrawText("Press E - LOCKED",
            mGameState.maindoor->getPosition().x - 80,
            mGameState.maindoor->getPosition().y - 140,
            24, RED);
        }
        
    }
    mGameState.map->render();
    // render furniture
    mGameState.couch->render();
    mGameState.tvtable->render();
    mGameState.lamp->render();
    mGameState.clock->render();
    mGameState.piano->render();
    mGameState.emma->render();
    mGameState.Leah->render();

    // Piano hover text
    Vector2 mousePos = GetMousePosition();
    Vector2 worldMousePos = GetScreenToWorld2D(mousePos, mGameState.camera);

    Vector2 pianoPos = mGameState.piano->getPosition();
    Vector2 pianoScale = mGameState.piano->getScale();

    float pianoLeft = pianoPos.x - pianoScale.x / 2.0f;
    float pianoRight = pianoPos.x + pianoScale.x / 2.0f;
    float pianoTop = pianoPos.y - pianoScale.y / 2.0f;
    float pianoBottom = pianoPos.y + pianoScale.y / 2.0f;

    if (worldMousePos.x >= pianoLeft && worldMousePos.x <= pianoRight &&
        worldMousePos.y >= pianoTop && worldMousePos.y <= pianoBottom)
    {
        if (mGameState.pianoPlayed)
        {
            float timeLeft = mGameState.pianoDuration - mGameState.pianoTimer;
            char text[50];
            DrawText(text, pianoPos.x - 100, pianoPos.y - 180, 20, GREEN);
        }
        else
        {
            // Piano not playing 
            DrawText("Click - Play Piano (Lures Dad for 15s)",
                pianoPos.x - 160,
                pianoPos.y - 180,
                18, YELLOW);
        }
    }

    if (mGameState.flashlightShader != nullptr && mGameState.flashlightShader->isLoaded())
    {
        mGameState.flashlightShader->end();
    }
    
    float screenX = mGameState.camera.target.x - (GetScreenWidth() / 2.0f);
    float screenY = mGameState.camera.target.y - (GetScreenHeight() / 2.0f);
    // draw silver key if collected
    if (mGameState.hasSilverKey) {
        float screenX = mGameState.camera.target.x - (GetScreenWidth() / 2.0f);
        float screenY = mGameState.camera.target.y - (GetScreenHeight() / 2.0f);

        DrawTexture(mGameState.silverKeyTexture, screenX + 10, screenY + 50, WHITE);
        DrawText("Silver Key", screenX + 45, screenY + 55, 16, WHITE);
    }

    // Draw gold key
    if (mGameState.hasGoldKey) {
        Color keyColor = mGameState.lightsOn ? WHITE : Fade(WHITE, 0.1f);
        DrawTexture(mGameState.goldKeyTexture, screenX + 10, screenY + 90, keyColor);  // 40 pixels below silver key
        DrawText("Gold Key", screenX + 45, screenY + 95, 16, keyColor);
    }

    // draw time at top left of screen
    drawGameTime(screenX, screenY);

    //flashlight toggle 
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

    if (mGameState.playerWon) {
        renderWinOverlay();
    }

}

void LivingRoom::shutdown()
{
    delete mGameState.Leah;
    mGameState.Leah = nullptr;
    delete mGameState.map;
    mGameState.map = nullptr;
    delete mGameState.door1;
    mGameState.door1 = nullptr;
    delete mGameState.maindoor;
    mGameState.maindoor = nullptr;
}

