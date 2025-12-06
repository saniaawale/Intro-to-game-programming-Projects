#include "Kitchen.h"
#include "ShaderProgram.h" 

Kitchen::Kitchen() : Scene{ {0.0f}, nullptr } {}

Kitchen::Kitchen(Vector2 origin, const char* bgHexCode) : Scene{ origin, bgHexCode } {}

Kitchen::~Kitchen() { shutdown(); }

void Kitchen::initialise()
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
    mGameState.silverKeyTexture = LoadTexture("assets/game/key_silver.png");


    /*
         ---------------MAP----------------
    */
    mGameState.map = new Map(
        KITCHEN_WIDTH, KITCHEN_HEIGHT,
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
        { mOrigin.x + 900.0f, mOrigin.y + 50.0f },
        { 150.0f, 250.0f },
        "assets/game/door_tile.png",
        DOOR
    );

    /*
     ---------------furniture----------------
    */
    //fridge
    mGameState.fridge = new Entity(
        { mOrigin.x + 600.0f, mOrigin.y + 70.0f },
        { 250.0f, 250.0f },
        "assets/game/fridge.png",
        ITEM
    );


    // sink
    mGameState.sink = new Entity(
        { mOrigin.x - 100.0f, mOrigin.y + 70.0f },
        { 300.0f, 150.0f },
        "assets/game/kitchen_sink.png",
        ITEM
    );

    //kitch_table
    mGameState.kitchen_table = new Entity(
        { mOrigin.x + 350.0f, mOrigin.y + 150.0f },
        { 300.0f, 100.0f },
        "assets/game/kitchen_table.png",
        ITEM
    );

    //big spoons 
    mGameState.bigSpoons = new Entity(
        { mOrigin.x - 260.0f, mOrigin.y + 120.0f },
        { 50.0f, 50.0f },
        "assets/game/bigSpoons.png",
        ITEM
    );

    //KITCHEN TRASH
    mGameState.kitchenTrash = new Entity(
        { mOrigin.x + 120, mOrigin.y + 130.0f },
        { 100.0f, 100.0f },
        "assets/game/kitchen_trash.png",
        ITEM
    );

    //stove_oven
    mGameState.stove_oven = new Entity(
        { mOrigin.x - 400.0f, mOrigin.y + 100.0f },
        { 250.0f, 150.0f },
        "assets/game/stove_oven.png",
        ITEM
    );

   // Exhaust
    mGameState.exhaust = new Entity(
        { mOrigin.x - 500.0f, mOrigin.y + 130.0f },
        { 70.0f, 100.0f },
        "assets/game/exhaust_pipe.png",
        ITEM
    );

    //bread
    mGameState.bread = new Entity(
        { mOrigin.x - 100.0f, mOrigin.y - 20.0f },
        { 200.0f, 150.0f },
        "assets/game/bread.png",
        ITEM
    );

    //microwave
    mGameState.microwave = new Entity(
        { mOrigin.x + 300.0f, mOrigin.y + 80.0f },
        { 100.0f, 50.0f },
        "assets/game/microwave.png",
        ITEM
    );

}

void Kitchen::update(float deltaTime)
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

    float mapWidth = KITCHEN_WIDTH * TILE_DIMENSION;
    float mapHeight = KITCHEN_HEIGHT * TILE_DIMENSION;

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

    // CLICK TO OPEN/CLOSE FRIDGE
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        Vector2 worldMousePos = GetScreenToWorld2D(mousePos, mGameState.camera);

        // FRIDGE CLICK
        Vector2 fridgePos = mGameState.fridge->getPosition();
        Vector2 fridgeScale = mGameState.fridge->getScale();

        float fridgeLeft = fridgePos.x - fridgeScale.x / 2.0f;
        float fridgeRight = fridgePos.x + fridgeScale.x / 2.0f;
        float fridgeTop = fridgePos.y - fridgeScale.y / 2.0f;
        float fridgeBottom = fridgePos.y + fridgeScale.y / 2.0f;

        if (worldMousePos.x >= fridgeLeft && worldMousePos.x <= fridgeRight &&
            worldMousePos.y >= fridgeTop && worldMousePos.y <= fridgeBottom)
        {
            // Toggle fridge state
            mGameState.fridgeOpen = !mGameState.fridgeOpen;

            if (mGameState.fridgeOpen) {
                mGameState.fridge->setTexture("assets/game/fridge_open.png");


                if (!mGameState.hasSilverKey) {
                    mGameState.hasSilverKey = true;
                }
            }
            else {
                mGameState.fridge->setTexture("assets/game/fridge.png");
            }
        }
    }

    // CLICK TO OPEN/CLOSE MICROWAVE
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        Vector2 worldMousePos = GetScreenToWorld2D(mousePos, mGameState.camera);

        // MICROWAVE CLICK
        Vector2 microwavePos = mGameState.microwave->getPosition();
        Vector2 microwaveScale = mGameState.microwave->getScale();

        float microwaveLeft = microwavePos.x - microwaveScale.x / 2.0f;
        float microwaveRight = microwavePos.x + microwaveScale.x / 2.0f;
        float microwaveTop = microwavePos.y - microwaveScale.y / 2.0f;
        float microwaveBottom = microwavePos.y + microwaveScale.y / 2.0f;

        if (worldMousePos.x >= microwaveLeft && worldMousePos.x <= microwaveRight &&
            worldMousePos.y >= microwaveTop && worldMousePos.y <= microwaveBottom)
        {
            // Toggle microwave state
            mGameState.microwaveOpen = !mGameState.microwaveOpen;

            if (mGameState.microwaveOpen) {
                mGameState.microwave->setTexture("assets/game/microwave_open.png");
            }
            else {
                mGameState.microwave->setTexture("assets/game/microwave.png");
            }
        }
    }

    // CLICK TO OPEN/CLOSE SINK DRAWERS
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        Vector2 worldMousePos = GetScreenToWorld2D(mousePos, mGameState.camera);

        // SINK DRAWERS CLICK
        Vector2 sinkPos = mGameState.sink->getPosition();
        Vector2 sinkScale = mGameState.sink->getScale();

        float sinkLeft = sinkPos.x - sinkScale.x / 2.0f;
        float sinkRight = sinkPos.x + sinkScale.x / 2.0f;
        float sinkTop = sinkPos.y - sinkScale.y / 2.0f;
        float sinkBottom = sinkPos.y + sinkScale.y / 2.0f;

        if (worldMousePos.x >= sinkLeft && worldMousePos.x <= sinkRight &&
            worldMousePos.y >= sinkTop && worldMousePos.y <= sinkBottom)
        {
            // Toggle sink drawer state
            mGameState.kitchenSinkDrawerOpen = !mGameState.kitchenSinkDrawerOpen;

            if (mGameState.kitchenSinkDrawerOpen) {
                mGameState.sink->setTexture("assets/game/kitchen_sink_open.png");

            }
            else {
                mGameState.sink->setTexture("assets/game/kitchen_sink.png");
            }
        }
    }

    if (IsKeyPressed(KEY_E))
    {
        // door1 : Living Room
        if (mGameState.Leah->isColliding(mGameState.door1))
        {
            mGameState.nextSceneID = 3; 
        }

        // door2 : Kids Bedroom
        if (mGameState.Leah->isColliding(mGameState.door2))
        {
            mGameState.nextSceneID = 2;  
        }
    }


}

void Kitchen::render()
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

    // door1 : Living Room
    mGameState.door1->render();
    if (mGameState.Leah->isColliding(mGameState.door1)) {
        DrawText("Press E - Living Room",
            mGameState.door1->getPosition().x - 100,
            mGameState.door1->getPosition().y - 120,
            20, YELLOW);
    }

    // door2 : Kids Bedroom
    mGameState.door2->render();
    if (mGameState.Leah->isColliding(mGameState.door2)) {
        DrawText("Press E - Kids Bedroom",
            mGameState.door2->getPosition().x - 100,
            mGameState.door2->getPosition().y - 120,
            20, YELLOW);
    }

    mGameState.map->render();
    // render furniture
    mGameState.fridge->render();
    mGameState.sink->render();
    mGameState.kitchen_table->render();
    //mGameState.bigSpoons->render();
    mGameState.kitchenTrash->render();
    mGameState.stove_oven->render();
    //mGameState.exhaust_pipe->render();
    mGameState.microwave->render();

    mGameState.Leah->render();

    mGameState.flashlightShader->end();
    
    float screenX = mGameState.camera.target.x - (GetScreenWidth() / 2.0f);
    float screenY = mGameState.camera.target.y - (GetScreenHeight() / 2.0f);
    // draw silver key
    if (mGameState.hasSilverKey) {
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

    if (mGameState.playerWon) {
        renderWinOverlay();
    }
}

void Kitchen::shutdown()
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