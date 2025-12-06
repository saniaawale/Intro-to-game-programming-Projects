#include "Scene1.h"

Scene1::Scene1() : Scene{ {0.0f}, nullptr } {
    currentDialogueIndex = 0;
    dialogueComplete = false;
    textDisplayTimer = 0.0f;
    charactersToShow = 0;
    daughter = nullptr;
    leahWalkingIn = true;
}

Scene1::Scene1(Vector2 origin, const char* bgHexCode) : Scene{ origin, bgHexCode } {
    currentDialogueIndex = 0;
    dialogueComplete = false;
    textDisplayTimer = 0.0f;
    charactersToShow = 0;
    daughter = nullptr;
    leahWalkingIn = true;
}

Scene1::~Scene1() {
    shutdown();
}

void Scene1::initialise()
{
    mGameState.nextSceneID = -1;

    currentDialogueIndex = 0;
    dialogueComplete = false;
    textDisplayTimer = 0.0f;
    charactersToShow = 0;
    leahWalkingIn = true;

    // Initialize dialogue
    dialogue[0] = {
        "Leah",
        "Mom! Can I sleep over at Emma's house tonight? Please?",
        SKYBLUE
    };

    dialogue[1] = {
        "Leah's Mother",
        "Emma's house? Hmm... I don't know, sweetheart.",
        PINK
    };

    dialogue[2] = {
        "Leah's Mother",
        "Her father is... a little strange. Are you sure about this?",
        PINK
    };

    dialogue[3] = {
        "Leah",
        "*LIES* He's not going to be home!!",
        SKYBLUE
    };

    // Load background image
    backgroundTexture = LoadTexture("assets/game/living_room_scene.png");

    /*
       ----------- LEAH CHARACTER -----------
    */
    std::map<Direction, std::vector<int>> leahWalkAnimationAtlas = {
       {LEFT,  { 0, 1, 2, 3, 4, 5, 6, 7, 8 }},
       {RIGHT, { 0, 1, 2, 3, 4, 5, 6, 7, 8 }},
       {UP,    { 0, 1, 2, 3, 4, 5, 6, 7, 8 }},
       {DOWN,  { 0, 1, 2, 3, 4, 5, 6, 7, 8 }}
    };

    leahTargetPosition = { mOrigin.x - 150.0f, mOrigin.y + 100.0f };
    leahStartPosition = { mOrigin.x - 600.0f, mOrigin.y + 100.0f };

    daughter = new Entity(
        leahStartPosition,
        { 200.0f, 150.0f },                      
        "assets/game/walkPlayer.png",
        ATLAS,
        { 1, 9 },                                 
        leahWalkAnimationAtlas,
        PLAYER
    );

    daughter->setFrameSpeed(12);
    daughter->setAcceleration({ 0.0f, 0.0f });
    daughter->setDirection(RIGHT);
    daughter->setSpeed(0);

    /*
       ----------- CAMERA -----------
    */
    mGameState.camera = { 0 };
    mGameState.camera.target = mOrigin;
    mGameState.camera.offset = mOrigin;
    mGameState.camera.rotation = 0.0f;
    mGameState.camera.zoom = 1.0f;
}

void Scene1::update(float deltaTime)
{
    if (leahWalkingIn) {
        // walking animation
        daughter->update(deltaTime, nullptr, nullptr, nullptr, 0);

        Vector2 currentPos = daughter->getPosition();
        currentPos.x += 150.0f * deltaTime;
        daughter->setPosition(currentPos);

        // Check if reached target
        if (currentPos.x >= leahTargetPosition.x) {
            daughter->setPosition(leahTargetPosition);

            // Switch to idle sprite
            daughter->setTexture("assets/game/idleplayer.png");
            daughter->setSpriteSheetDimensions({ 1, 2 }); 
            daughter->setFrameSpeed(8);

            leahWalkingIn = false;
        }
        return;
    }

    // Update idle animation
    daughter->update(deltaTime, nullptr, nullptr, nullptr, 0);

    if (!dialogueComplete && currentDialogueIndex < DIALOGUE_COUNT) {
        // Get text length
        int textLength = 0;
        const char* currentText = dialogue[currentDialogueIndex].text;
        while (currentText[textLength] != '\0') {
            textLength++;
        }

        if (charactersToShow < textLength) {
            textDisplayTimer += deltaTime;
            if (textDisplayTimer >= TEXT_SPEED) {
                charactersToShow++;
                textDisplayTimer = 0.0f;
            }
        }
    }

    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        if (dialogueComplete) {
            mGameState.nextSceneID = 2; 
        }
        else {
            advanceDialogue();
        }
    }
}

void Scene1::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    if (backgroundTexture.id != 0) {
        Rectangle source = { 0, 0, (float)backgroundTexture.width, (float)backgroundTexture.height };
        Rectangle dest = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
        DrawTexturePro(backgroundTexture, source, dest, { 0, 0 }, 0.0f, WHITE);
    }

    daughter->render();
    if (leahWalkingIn) {
        return;
    }
    renderDialogueBox();

    const char* prompt;
    if (dialogueComplete) {
        prompt = "Press SPACE or ENTER to begin...";
    }
    else {
        int textLength = 0;
        const char* currentText = dialogue[currentDialogueIndex].text;
        while (currentText[textLength] != '\0') {
            textLength++;
        }

        if (charactersToShow >= textLength) {
            prompt = "Press SPACE or ENTER to continue...";
        }
        else {
            prompt = "";
        }
    }

    int promptWidth = MeasureText(prompt, 20);
    DrawText(prompt, GetScreenWidth() / 2 - promptWidth / 2, GetScreenHeight() - 40, 20, LIGHTGRAY);

    // Title
    const char* title = "THE SLEEPOVER";
    int titleWidth = MeasureText(title, 40);
    DrawText(title, GetScreenWidth() / 2 - titleWidth / 2, 30, 40, RED);
}

void Scene1::renderDialogueBox()
{
    if (dialogueComplete) {
        int boxX = 100;
        int boxY = GetScreenHeight() - 180;
        int boxWidth = GetScreenWidth() - 200;
        int boxHeight = 120;

        DrawRectangle(boxX, boxY, boxWidth, boxHeight, (Color) { 26, 26, 26, 80 });
        DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, WHITE);

        const char* endText = "And so, the sleepover begins...";
        int textWidth = MeasureText(endText, 30);
        DrawText(endText, boxX + boxWidth / 2 - textWidth / 2, boxY + boxHeight / 2 - 15, 30, WHITE);
        return;
    }

    int boxX = 100;
    int boxY = GetScreenHeight() - 180;
    int boxWidth = GetScreenWidth() - 200;
    int boxHeight = 120;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, (Color) { 26, 26, 26, 80 });
    DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, WHITE);

    // Speaker name
    if (currentDialogueIndex < DIALOGUE_COUNT) {
        DialogueLine currentLine = dialogue[currentDialogueIndex];
        DrawText(currentLine.speaker, boxX + 20, boxY + 15, 25, currentLine.speakerColor);

        // Dialogue text with typewriter effect
        char displayText[256];
        const char* fullText = currentLine.text;

        int i;
        for (i = 0; i < charactersToShow && i < 255 && fullText[i] != '\0'; i++) {
            displayText[i] = fullText[i];
        }
        displayText[i] = '\0';

        DrawText(displayText, boxX + 20, boxY + 55, 20, WHITE);
    }
}

void Scene1::advanceDialogue()
{
    // Safety check
    if (currentDialogueIndex >= DIALOGUE_COUNT) {
        dialogueComplete = true;
        return;
    }

    int textLength = 0;
    const char* currentText = dialogue[currentDialogueIndex].text;
    while (currentText[textLength] != '\0') {
        textLength++;
    }

    if (charactersToShow < textLength) {
        charactersToShow = textLength;
        return;
    }
    currentDialogueIndex++;
    charactersToShow = 0;
    textDisplayTimer = 0.0f;
    if (currentDialogueIndex >= DIALOGUE_COUNT) {
        dialogueComplete = true;
    }
}

void Scene1::shutdown()
{
    if (daughter != nullptr) {
        delete daughter;
        daughter = nullptr;
    }

    UnloadTexture(backgroundTexture);
}