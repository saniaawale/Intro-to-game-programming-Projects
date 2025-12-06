#include "Bathroom.h"
#include "ShaderProgram.h" 

Bathroom::Bathroom() : Scene{ {0.0f}, nullptr } {}

Bathroom::Bathroom(Vector2 origin, const char* bgHexCode) : Scene{ origin, bgHexCode } {}

Bathroom::~Bathroom() { shutdown(); }

void Bathroom::initialise()
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
        BATHROOM_WIDTH, BATHROOM_HEIGHT,
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
    // door1 
    mGameState.door1 = new Entity(
        { mOrigin.x - 700.0f, mOrigin.y + 50.0f },
        { 150.0f, 250.0f },
        "assets/game/door_tile.png",
        DOOR
    );

    // door2 
    mGameState.door2 = new Entity(
        { mOrigin.x + 800.0f, mOrigin.y + 50.0f },
        { 150.0f, 250.0f },
        "assets/game/door_tile.png",
        DOOR
    );

    /*
         ---------------furniture----------------
        */
        //toilet
    mGameState.toilet = new Entity(
        { mOrigin.x - 360.0f, mOrigin.y + 120.0f },
        { 80.0f, 150.0f },
        "assets/game/toilet.png",
        ITEM
    );

    //bathroom sink
    mGameState.bathroomSink = new Entity(
        { mOrigin.x - 100.0f, mOrigin.y + 70.0f },
        { 150.0f, 100.0f },
        "assets/game/bathroom_sink.png",
        ITEM
    );

    //bathtub
    mGameState.bathtub = new Entity(
        { mOrigin.x + 500.0f, mOrigin.y + 100.0f },
        { 300.0f, 200.0f },
        "assets/game/bathtub.png",
        ITEM
    );

    //toiletPaper
    mGameState.toiletPaper = new Entity(
        { mOrigin.x - 260.0f, mOrigin.y + 120.0f },
        { 50.0f, 50.0f },
        "assets/game/toiletPaper.png",
        ITEM
    );

    //trash
    mGameState.bathroomTrash = new Entity(
        { mOrigin.x - 500.0f, mOrigin.y + 130.0f },
        { 70.0f, 100.0f },
        "assets/game/bathroom_trash.png",
        ITEM
    );

    //mirror
    mGameState.bathroomMirror = new Entity(
        { mOrigin.x - 100.0f, mOrigin.y - 20.0f },
        { 200.0f, 150.0f },
        "assets/game/bathroom_mirror.png",
        ITEM
    );

    //shelves
    mGameState.bathroomShelves = new Entity(
        { mOrigin.x + 200.0f, mOrigin.y + 30.0f },
        { 250.0f, 300.0f },
        "assets/game/bathroom_shelves.png",
        ITEM
    );
}

void Bathroom::update(float deltaTime)
{
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

    float mapWidth = BATHROOM_WIDTH * TILE_DIMENSION;
    float mapHeight = BATHROOM_HEIGHT * TILE_DIMENSION;

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

    // CLICK TO OPEN/CLOSE TOILET
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        Vector2 worldMousePos = GetScreenToWorld2D(mousePos, mGameState.camera);

        // TOILET CLICK
        Vector2 toiletPos = mGameState.toilet->getPosition();
        Vector2 toiletScale = mGameState.toilet->getScale();

        float toiletLeft = toiletPos.x - toiletScale.x / 2.0f;
        float toiletRight = toiletPos.x + toiletScale.x / 2.0f;
        float toiletTop = toiletPos.y - toiletScale.y / 2.0f;
        float toiletBottom = toiletPos.y + toiletScale.y / 2.0f;

        if (worldMousePos.x >= toiletLeft && worldMousePos.x <= toiletRight &&
            worldMousePos.y >= toiletTop && worldMousePos.y <= toiletBottom)
        {
            // Toggle toilet state
            mGameState.toiletOpen = !mGameState.toiletOpen;

            if (mGameState.toiletOpen) {
                mGameState.toilet->setTexture("assets/game/toilet_open.png");
            }
            else {
                mGameState.toilet->setTexture("assets/game/toilet.png");
            }
        }
        
    }  

    // CLICK TO OPEN/CLOSE TRASH
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        Vector2 worldMousePos = GetScreenToWorld2D(mousePos, mGameState.camera);

        // TRASH CLICK
        Vector2 trashPos = mGameState.bathroomTrash->getPosition();
        Vector2 trashScale = mGameState.bathroomTrash->getScale();

        float trashLeft = trashPos.x - trashScale.x / 2.0f;
        float trashRight = trashPos.x + trashScale.x / 2.0f;
        float trashTop = trashPos.y - trashScale.y / 2.0f;
        float trashBottom = trashPos.y + trashScale.y / 2.0f;

        if (worldMousePos.x >= trashLeft && worldMousePos.x <= trashRight &&
            worldMousePos.y >= trashTop && worldMousePos.y <= trashBottom)
        {
            // Toggle trash state
            mGameState.bathroomTrashOpen = !mGameState.bathroomTrashOpen;

            if (mGameState.bathroomTrashOpen) {
                mGameState.bathroomTrash->setTexture("assets/game/bathroom_trash_open.png");
            }
            else {
                mGameState.bathroomTrash->setTexture("assets/game/bathroom_trash.png");
            }
        }

    }

    // CLICK TO OPEN/CLOSE SINK DRAWERS
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        Vector2 worldMousePos = GetScreenToWorld2D(mousePos, mGameState.camera);

        // SINK DRAWERS CLICK
        Vector2 sinkDrawerPos = mGameState.bathroomSink->getPosition();
        Vector2 sinkDrawerScale = mGameState.bathroomSink->getScale();

        float bathroomSinkLeft = sinkDrawerPos.x - sinkDrawerScale.x / 2.0f;
        float bathroomSinkRight = sinkDrawerPos.x + sinkDrawerScale.x / 2.0f;
        float bathroomSinkTop = sinkDrawerPos.y - sinkDrawerScale.y / 2.0f;
        float bathroomSinkBottom = sinkDrawerPos.y + sinkDrawerScale.y / 2.0f;

        if (worldMousePos.x >= bathroomSinkLeft && worldMousePos.x <= bathroomSinkRight &&
            worldMousePos.y >= bathroomSinkTop && worldMousePos.y <= bathroomSinkBottom)
        {
            // Toggle sink state
            mGameState.sinkDrawerOpen = !mGameState.sinkDrawerOpen;

            if (mGameState.sinkDrawerOpen) {
                mGameState.bathroomSink->setTexture("assets/game/bathroom_sink_open.png");
            }
            else {
                mGameState.bathroomSink->setTexture("assets/game/bathroom_sink.png");
            }
        }

    }
 

    
    // door checks
  

    if (IsKeyPressed(KEY_E))
    {
        // door1 -> Kids Bedroom
        if (mGameState.Leah->isColliding(mGameState.door1))
        {
            mGameState.nextSceneID = 2;
        }

        // door2 -> Parents Bedroom
        if (mGameState.Leah->isColliding(mGameState.door2))
        {
            mGameState.nextSceneID = 5;
        }
    }
}

void Bathroom::render()
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

    mGameState.map->render();
    mGameState.door1->render();
    mGameState.door2->render();

    // render furniture
    mGameState.toilet->render();
    mGameState.toiletPaper->render();
    mGameState.bathroomMirror->render();
    mGameState.bathroomSink->render();
    mGameState.bathtub->render();
    mGameState.bathroomTrash->render();
    mGameState.bathroomShelves->render();

    mGameState.Leah->render();

    if (mGameState.flashlightShader != nullptr && mGameState.flashlightShader->isLoaded())
    {
        mGameState.flashlightShader->end();
    }

    // UI ELEMENTS
    float screenX = mGameState.camera.target.x - (GetScreenWidth() / 2.0f);
    float screenY = mGameState.camera.target.y - (GetScreenHeight() / 2.0f);


    // door prompts
    if (mGameState.Leah->isColliding(mGameState.door1)) {
        DrawText("Press E - Kids Bedroom",mGameState.door1->getPosition().x - 100,
            mGameState.door1->getPosition().y - 120,
            20, YELLOW);
    }

    if (mGameState.Leah->isColliding(mGameState.door2)) {
        DrawText("Press E - Parents Bedroom",
            mGameState.door2->getPosition().x - 120,
            mGameState.door2->getPosition().y - 120,
            20, YELLOW);
    }

    // Draw silver key in corner if collected
    if (mGameState.hasSilverKey) {
        float screenX = mGameState.camera.target.x - (GetScreenWidth() / 2.0f);
        float screenY = mGameState.camera.target.y - (GetScreenHeight() / 2.0f);

        DrawTexture(mGameState.silverKeyTexture, screenX + 10, screenY + 50, WHITE);
        DrawText("Silver Key", screenX + 45, screenY + 55, 16, WHITE);
    }

    // Draw gold key
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

void Bathroom::shutdown()
{
    if (mGameState.Leah != nullptr) {
        delete mGameState.Leah;
        mGameState.Leah = nullptr;
    }
    if (mGameState.map != nullptr) {
        delete mGameState.map;
        mGameState.map = nullptr;
    }
    if (mGameState.door1 != nullptr) {
        delete mGameState.door1;
        mGameState.door1 = nullptr;
    }
    if (mGameState.door2 != nullptr) {
        delete mGameState.door2;
        mGameState.door2 = nullptr;
    }
}