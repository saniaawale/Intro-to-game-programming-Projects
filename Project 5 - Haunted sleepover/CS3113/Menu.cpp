#include "Menu.h"
#include "cs3113.h"

Menu::Menu() : Scene()
{
    mSelectedOption = 0;
    mTotalOptions = 2;  // Start Game, Quit
}

Menu::Menu(Vector2 origin, const char* bgHexCode)
    : Scene(origin, bgHexCode)
{
    mSelectedOption = 0;
    mTotalOptions = 2;  // Start Game, Quit
}

Menu::~Menu() {}

void Menu::initialise()
{
    mGameState.nextSceneID = 0;    
    mGameState.Leah = nullptr;
    mGameState.map = nullptr;
    
    mGameState.camera = { 0 };
    mGameState.camera.target = mOrigin;
    mGameState.camera.offset = mOrigin;
    mGameState.camera.rotation = 0.0f;
    mGameState.camera.zoom = 1.0f;

}

void Menu::update(float deltaTime)
{
    
    mGameState.nextSceneID = 0;

    // Navigate menu with UP/DOWN or W/S
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        mSelectedOption--;
        if (mSelectedOption < 0) {
            mSelectedOption = mTotalOptions - 1;  // Wrap to bottom
        }
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        mSelectedOption++;
        if (mSelectedOption >= mTotalOptions) {
            mSelectedOption = 0;  // Wrap to top
        }
    }

    // Select option with ENTER or SPACE
    if (IsKeyPressed(KEY_ENTER)) {
        switch (mSelectedOption) {
        case 0:  // Start Game
            mGameState.nextSceneID = 1; 
            break;

        case 1:  // Quit
            mGameState.nextSceneID = -2; 
            break;
        }
    }
}

void Menu::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    // Draw title
    const char* title = "Haunted Sleepover";
    int titleFontSize = 80;
    int titleWidth = MeasureText(title, titleFontSize);
    DrawText(
        title,
        (int)(mOrigin.x - titleWidth / 2),
        (int)(mOrigin.y - 300),
        titleFontSize,
        WHITE
    );

    const char* otherTitle = "Can YOU survive?";
    int titleFontSize1 = 40;
    int titleWidth1 = MeasureText(otherTitle, titleFontSize1);
    DrawText(
        otherTitle,
        (int)(mOrigin.x - titleWidth1 / 2),
        (int)(mOrigin.y - 200),
        titleFontSize1,
        WHITE
    );



    // Draw menu options
    const char* options[2] = {
        "Yes",
        "No, I quit"
    };

    int fontSize = 40;
    int spacing = 80;
    int startY = (int)mOrigin.y - 50;

    for (int i = 0; i < mTotalOptions; i++) {
        Color textColor = (i == mSelectedOption) ? YELLOW : WHITE;
        
        // Draw option text
        int textWidth = MeasureText(options[i], fontSize);
        DrawText(
            options[i],
            (int)(mOrigin.x - textWidth / 2),
            startY + (i * spacing),
            fontSize,
            textColor
        );
    }


}

void Menu::shutdown()
{
}