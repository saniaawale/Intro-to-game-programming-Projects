#include "kids_bedroom.h"
#include "ShaderProgram.h" 

Kids_bedroom::Kids_bedroom() : Scene{ {0.0f}, nullptr } {}

Kids_bedroom::Kids_bedroom(Vector2 origin, const char* bgHexCode) : Scene{ origin, bgHexCode } {}

Kids_bedroom::~Kids_bedroom() { shutdown(); }

void Kids_bedroom::initialise()
{
    mGameState.silverKeyTexture = LoadTexture("assets/game/key_silver.png");
    mGameState.goldKeyTexture = LoadTexture("assets/game/key_gold.png");
    playerStartPosition = { mOrigin.x, mOrigin.y + 120.0f };
    mGameState.nextSceneID = -1;

    mGameState.camera = { 0 };
    mGameState.camera.target = mOrigin;
    mGameState.camera.offset = mOrigin;
    mGameState.camera.rotation = 0.0f;
    mGameState.camera.zoom = 1.0f;
    mGameState.kwardrobeOpen = false;
    mGameState.hasMainDoorKey = false;
    mGameState.hasBrassKey = false;
    mGameState.gameTime = 0.0f;
    
    if (!mGameState.openingPlayed)
    {
        mGameState.dialogueActive = true;
        mGameState.dialogueStep = 0;
        mGameState.noteFound = false;
        mGameState.showNote = false;
    }
    // if returning to room dialogue stays off

    /*
         ---------------MAP----------------
    */
    mGameState.map = new Map(
        KIDS_BEDROOM_WIDTH, KIDS_BEDROOM_HEIGHT,
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
    mGameState.Leah->setColliderDimensions({spriteWidth * 0.25f, spriteHeight * 0.35f });
    mGameState.Leah->setAcceleration({ 0.0f, 981.0f });

    /*
     ---------------DOORs----------------
    */
    // door to bathroom
    mGameState.door1 = new Entity(
        { mOrigin.x + 700.0f, mOrigin.y + 50.0f },
        { 150.0f, 250.0f },
        "assets/game/door_tile.png",
        DOOR
    );

    // door to parents bedroom
    mGameState.door2 = new Entity(
        { mOrigin.x - 800.0f, mOrigin.y + 50.0f },
        { 150.0f, 250.0f },
        "assets/game/door_tile.png",
        DOOR
    );

    /*
         ---------------furniture----------------
    */
    //kids bed
    mGameState.kids_bed = new Entity(
        { mOrigin.x - 360.0f, mOrigin.y + 120.0f },
        { 300.0f, 150.0f },
        "assets/game/kids_bed.png",
        ITEM
    );


    //kids dresser
    mGameState.kidsDresser = new Entity(
        { mOrigin.x - 50.0f, mOrigin.y + 140.0f },
        { 300.0f, 100.0f },
        "assets/game/kids_dresser.png",
        ITEM
    );

    //posters
    mGameState.posters = new Entity(
        { mOrigin.x - 470.0f, mOrigin.y - 100  },
        { 200.0f, 100.0f },
        "assets/game/posters.png",
        ITEM
    );

    //wardrobe
    mGameState.kidsWardrobe = new Entity(
        { mOrigin.x + 400.0f, mOrigin.y  },
        { 350.0f, 400.0f },
        "assets/game/kids_wardrobe.png",
        ITEM
    );

    //curtains
    mGameState.curtains = new Entity(
        { mOrigin.x - 100.0f, mOrigin.y - 100.0f },
        { 350.0f, 250.0f },
        "assets/game/curtains_kids_bedroom.png",
        ITEM
    );

    //shelf
    mGameState.shelf = new Entity(
        { mOrigin.x + 400.0f, mOrigin.y + 70.0f },
        { 350.0f, 250.0f },
        "assets/game/kids_shelf.png",
        ITEM
    );

    /*
     ---------------EMMA - only spawn on first visit----------------
    */
    if (!mGameState.openingPlayed)
    {
        mGameState.emma = new Entity(
            { mOrigin.x + 300.0f, mOrigin.y + 150.0f },
            { spriteWidth, spriteHeight },
            "assets/game/emma_idle.png", 
            ATLAS,
            { 1, 2 },
            LeahAnimationAtlas,
            NPC
        );


        mGameState.emma->setSpeed(200);
        mGameState.emma->setFrameSpeed(8);
        mGameState.emma->setJumpingPower(550.0f);
        mGameState.emma->setColliderDimensions({spriteWidth * 0.25f, spriteHeight * 0.35f});

        mGameState.emma->setAcceleration({ 0.0f, 981.0f });
    }
    else
    {
        mGameState.emma = nullptr;
    }
}


void Kids_bedroom::advanceDialogue()
{
    mGameState.dialogueStep++;

    if (mGameState.dialogueStep >= 6)
    {
        // dialogue ends and Emma leaves
        mGameState.dialogueActive = false;
        mGameState.openingPlayed = true;
        // set per position to be far because simply deleting her did not work 
        if (mGameState.emma != nullptr) {
            mGameState.emma->setPosition({ -10000.0f, -10000.0f });
        }
    }
}


void Kids_bedroom::update(float deltaTime)
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

    // DIALOGUE SYSTEM 
    if (mGameState.dialogueActive)
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            advanceDialogue();
        }
    }

    if (mGameState.showNote)
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            mGameState.showNote = false;
        }

    }
    //update emma
    if (mGameState.emma != nullptr) {
        //mGameState.emma->resetMovement();
        //mGameState.emma->updateAnimationState();
        //mGameState.emma->update(deltaTime, nullptr, mGameState.map, nullptr, 0);

        mGameState.emma->setVelocity({ 0.0f, 0.0f });
        mGameState.emma->setMovement({ 0.0f, 0.0f });

        // Only update animation
        mGameState.emma->animate(deltaTime);
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

    float mapWidth = KIDS_BEDROOM_WIDTH * TILE_DIMENSION;
    float mapHeight = KIDS_BEDROOM_HEIGHT * TILE_DIMENSION;

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

    // CLICK EMMA BED TO FIND NOTE 
    if ( !mGameState.dialogueActive)
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mousePos = GetMousePosition();
            Vector2 worldMousePos = GetScreenToWorld2D(mousePos, mGameState.camera);

            Vector2 bedPos = mGameState.kids_bed->getPosition();
            Vector2 bedScale = mGameState.kids_bed->getScale();

            float bedLeft = bedPos.x - bedScale.x / 2.0f;
            float bedRight = bedPos.x + bedScale.x / 2.0f;
            float bedTop = bedPos.y - bedScale.y / 2.0f;
            float bedBottom = bedPos.y + bedScale.y / 2.0f;

            if (worldMousePos.x >= bedLeft && worldMousePos.x <= bedRight &&
                worldMousePos.y >= bedTop && worldMousePos.y <= bedBottom)
            {
                // found the note!
                mGameState.noteFound = true;
                mGameState.showNote = true;
            }

        }
    }

    if (mGameState.noteFound && !mGameState.dialogueActive)
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mousePos = GetMousePosition();
            Vector2 worldMousePos = GetScreenToWorld2D(mousePos, mGameState.camera);

            Vector2 wardrobePos = mGameState.kidsWardrobe->getPosition();
            Vector2 wardrobeScale = mGameState.kidsWardrobe->getScale();

            float wardrobeLeft = wardrobePos.x - wardrobeScale.x / 2.0f;
            float wardrobeRight = wardrobePos.x + wardrobeScale.x / 2.0f;
            float wardrobeTop = wardrobePos.y - wardrobeScale.y / 2.0f;
            float wardrobeBottom = wardrobePos.y + wardrobeScale.y / 2.0f;

            if (worldMousePos.x >= wardrobeLeft && worldMousePos.x <= wardrobeRight &&
                worldMousePos.y >= wardrobeTop && worldMousePos.y <= wardrobeBottom)
            {
                mGameState.kwardrobeOpen = !mGameState.kwardrobeOpen;

                if (mGameState.kwardrobeOpen && !mGameState.hasFlashlight)
                {
                    mGameState.hasFlashlight = true;
                    mGameState.flashlightActive = true;
                }

                if (mGameState.kwardrobeOpen) {
                    mGameState.kidsWardrobe->setTexture("assets/game/kids_wardrobe_open.png");
                }
                else {
                    mGameState.kidsWardrobe->setTexture("assets/game/kids_wardrobe.png");
                }
            }
            else 
            {
                Vector2 dresserPos = mGameState.kidsDresser->getPosition();
                Vector2 dresserScale = mGameState.kidsDresser->getScale();

                float dresserLeft = dresserPos.x - dresserScale.x / 2.0f;
                float dresserRight = dresserPos.x + dresserScale.x / 2.0f;
                float dresserTop = dresserPos.y - dresserScale.y / 2.0f;
                float dresserBottom = dresserPos.y + dresserScale.y / 2.0f;

                if (worldMousePos.x >= dresserLeft && worldMousePos.x <= dresserRight &&
                    worldMousePos.y >= dresserTop && worldMousePos.y <= dresserBottom)
                {
                    mGameState.kdresserOpen = !mGameState.kdresserOpen;
                    if (mGameState.kdresserOpen) {
                        mGameState.kidsDresser->setTexture("assets/game/kids_dresser_open.png");
                    }
                    else {
                        mGameState.kidsDresser->setTexture("assets/game/kids_dresser.png");
                    }
                }
            }
        }
    }


    // door interaction checks
    if (IsKeyPressed(KEY_E))
    {
        // door1 -> bathroom
        if (mGameState.Leah->isColliding(mGameState.door1))
        {
            mGameState.nextSceneID = 6 ; 
        }

        // door2 -> parents bedroom
        if (mGameState.Leah->isColliding(mGameState.door2))
        {
            mGameState.nextSceneID = 4; 
        }

        //if (mGameState.Leah->isColliding(mGameState.door2))
        //{
           // mGameState.nextSceneID = 3;  
        //}
    }

  
}

void Kids_bedroom::renderDialogue()
{
    float screenX = mGameState.camera.target.x - (GetScreenWidth() / 2.0f);
    float screenY = mGameState.camera.target.y - (GetScreenHeight() / 2.0f);


    // dialogue box background
    int boxHeight = 150;
    int boxY = GetScreenHeight() - boxHeight;

    DrawRectangle(
        screenX,
        screenY + boxY,
        GetScreenWidth(),
        boxHeight,
        Fade(BLACK, 0.5f)
    );

    // border
    DrawRectangleLines(
        screenX,
        screenY + boxY,
        GetScreenWidth(),
        boxHeight,
        WHITE
    );

    // dialogue text based on step
    const char* speaker = "";
    const char* text = "";

    switch (mGameState.dialogueStep)
    {
    case 0:
        speaker = "You";
        text = "Emma? What time is it?";
        break;
    case 1:
        speaker = "Emma";
        text = "Oh, you're awake! It's midnight. Sorry, did I wake you?";
        break;
    case 2:
        speaker = "You";
        text = "No, I just... the house feels weird. Is everything okay?";
        break;
    case 3:
        speaker = "Emma";
        text = "Everything's perfect. My mom is not home and my dad is sleeping. We have the whole night.\nWant to play a game?";
        break;
    case 4:
        speaker = "You";
        text = "Maybe later? I'm still kind of tired...";
        break;
    case 5:
        speaker = "Emma";
        text = "Of course! I'll get us some snacks from the kitchen.\nMake yourself comfortable. Oh, and... don't go in my parents' room.";
        break;
    }

    // render speaker name
    DrawText(speaker,screenX + 30,screenY + boxY + 20,24, YELLOW);

    // render dialogue text
    DrawText(text,screenX + 30, screenY + boxY + 55, 20, WHITE);

    DrawText(
        "Later that night at Emmas, you wake up feeling quite uneasy...", screenX + 30, screenY + 10, 20, WHITE);

    DrawText("Press SPACE to continue", screenX + GetScreenWidth() - 250, screenY + boxY + boxHeight - 30, 16,  GRAY);
}

void Kids_bedroom::renderNote()
{
    float screenX = mGameState.camera.target.x - (GetScreenWidth() / 2.0f);
    float screenY = mGameState.camera.target.y - (GetScreenHeight() / 2.0f);

    DrawRectangle(
        screenX,
        screenY,
        GetScreenWidth(),
        GetScreenHeight(),
        Fade(BLACK, 0.7f)
    );

    int noteWidth = 700;
    int noteHeight = 500;
    int noteX = screenX + (GetScreenWidth() - noteWidth) / 2;
    int noteY = screenY + (GetScreenHeight() - noteHeight) / 2;

    DrawRectangle(noteX, noteY, noteWidth, noteHeight, Color{ 240, 230, 210, 255 });
    DrawRectangleLines(noteX, noteY, noteWidth, noteHeight, BLACK);

    DrawText("HELP ME!",noteX + noteWidth / 2 - 120, noteY + 30, 24, RED);

    const char* noteText =
        "REAL EMMA HERE - I'M TRAPPED SOMEWHERE IN THIS HOUSE!\n\n"
        "ESCAPE:\n"
        " SILVER KEY - unlocks the lock on my parents wardrobe \n"
        "I'm not sure where they keep it!\n"
        " GOLD KEY - Parents Bedroom wardrobe\n"
        "the gold key unlocks the Main door - RUN!\n\n"
        "My dad has been doing witchcraft and has killed my mother and imprisoned me\n"
        "HE HUNTS AT MIDNIGHT!\n\n"
        "I have a FLASHLIGHT in my room!\n"
        "DAD:\n"
        "HATES LOUD NOISES - will chase you if you sprint!\n"
        "Weakness: PLAYING THE PIANO lures him to the living room for 20 seconds\n"
        "Get keys and ESCAPE or be KILLED once the clock hits 3am\n\n"
        "- REAL Emma";

    DrawText(noteText,noteX + 40, noteY + 80,16,DARKGRAY);

    DrawText("Press SPACE to close", noteX + noteWidth / 2 - 100, noteY + noteHeight + 20, 20, WHITE);
}

void Kids_bedroom::render()
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


    // RENDER ALL WORLD OBJECTS
    mGameState.map->render();
    mGameState.posters->render();


    mGameState.kids_bed->render();

    if (!mGameState.noteFound && !mGameState.dialogueActive)
    {
        Vector2 mousePos = GetMousePosition();
        Vector2 worldMousePos = GetScreenToWorld2D(mousePos, mGameState.camera);
        Vector2 bedPos = mGameState.kids_bed->getPosition();
        Vector2 bedScale = mGameState.kids_bed->getScale();
        float bedLeft = bedPos.x - bedScale.x / 2.0f;
        float bedRight = bedPos.x + bedScale.x / 2.0f;
        float bedTop = bedPos.y - bedScale.y / 2.0f;
        float bedBottom = bedPos.y + bedScale.y / 2.0f;

        if (worldMousePos.x >= bedLeft && worldMousePos.x <= bedRight &&
            worldMousePos.y >= bedTop && worldMousePos.y <= bedBottom)
        {
            DrawText("Click - Search pillow", bedPos.x - 90, bedPos.y - 100, 18, YELLOW);
        }
    }

    mGameState.door1->render();
    if (mGameState.noteFound && mGameState.Leah->isColliding(mGameState.door1)) {
        DrawText("Press E - Bathroom",mGameState.door1->getPosition().x - 80, mGameState.door1->getPosition().y - 120, 20, YELLOW);
    }
    else if (!mGameState.noteFound && mGameState.Leah->isColliding(mGameState.door1)) {
        DrawText("Locked", mGameState.door1->getPosition().x - 30, mGameState.door1->getPosition().y - 120, 20, RED);
    }

    mGameState.door2->render();
    if (mGameState.noteFound && mGameState.Leah->isColliding(mGameState.door2)) {
        DrawText("Press E - Kitchen", mGameState.door2->getPosition().x - 120, mGameState.door2->getPosition().y - 120, 20, YELLOW);
    }
    else if (!mGameState.noteFound && mGameState.Leah->isColliding(mGameState.door2)) {
        DrawText("Locked",
            mGameState.door2->getPosition().x - 30,
            mGameState.door2->getPosition().y - 120,
            20, RED);
    }

    mGameState.kidsDresser->render();
    mGameState.kidsWardrobe->render();
    mGameState.curtains->render();
    mGameState.Leah->render();

    if (mGameState.emma != nullptr) {
        Vector2 emmaPos = mGameState.emma->getPosition();
        if (emmaPos.x > -5000.0f) {
            mGameState.emma->render();
        }
    }
    //if (mGameState.dialogueActive) {
        //renderDialogue();
    //}
    // END SHADER
    if (mGameState.flashlightShader != nullptr && mGameState.flashlightShader->isLoaded())
    {
        mGameState.flashlightShader->end();
    }
    float screenX = mGameState.camera.target.x - (GetScreenWidth() / 2.0f);
    float screenY = mGameState.camera.target.y - (GetScreenHeight() / 2.0f);

    if (mGameState.hasSilverKey) {
        Color keyColor = mGameState.lightsOn ? WHITE : Fade(WHITE, 0.1f);
        DrawTexture(mGameState.silverKeyTexture, screenX + 10, screenY + 50, WHITE);
        DrawText("Silver Key", screenX + 45, screenY + 55, 16, WHITE);
    }

    // Draw gold key
    if (mGameState.hasGoldKey) {
        Color keyColor = mGameState.lightsOn ? WHITE : Fade(WHITE, 0.1f);
        DrawTexture(mGameState.goldKeyTexture, screenX + 10, screenY + 90, keyColor); 
        DrawText("Gold Key", screenX + 45, screenY + 95, 16, keyColor);
    }

    
    if (mGameState.noteFound) {
        float screenX = mGameState.camera.target.x - (GetScreenWidth() / 2.0f);
        float screenY = mGameState.camera.target.y - (GetScreenHeight() / 2.0f);
        drawGameTime(screenX, screenY);

        if (mGameState.hasFlashlight) {
            if (mGameState.flashlightActive) {
                DrawText("FLASHLIGHT ON (SPACE)", screenX + GetScreenWidth() - 280, screenY + 10, 20, YELLOW);
            }
            else {
                DrawText("FLASHLIGHT OFF (SPACE)", screenX + GetScreenWidth() - 280, screenY + 10, 20, GRAY);
            }
        }
    }

    if (mGameState.dialogueActive) {
        renderDialogue();
    }

    if (mGameState.showNote) {
        renderNote();
    }

    if (mGameState.playerDied) {
        renderLoseOverlay();
    }
}


void Kids_bedroom::shutdown()
{
    delete mGameState.Leah;
    mGameState.Leah = nullptr;
    delete mGameState.map;
    mGameState.map = nullptr;
    delete mGameState.door1;
    mGameState.door1 = nullptr;
    delete mGameState.door2;
    mGameState.door2 = nullptr;
    delete mGameState.kids_bed;
    mGameState.kids_bed = nullptr;
}