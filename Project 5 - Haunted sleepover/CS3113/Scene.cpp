#include "Scene.h"

Scene::Scene() : mOrigin{{}} {}

Scene::Scene(Vector2 origin, const char *bgHexCode) : mOrigin{origin}, mBGColourHexCode {bgHexCode} 
{
    ClearBackground(ColorFromHex(bgHexCode));
}

void Scene::drawGameTime(float screenX, float screenY)
{
    int startMinutes = 24 * 60; 
    int gameMinutes = (int)mGameState.gameTime;
    int totalMinutes = (startMinutes + gameMinutes) % 1440;

    int hours = totalMinutes / 60;
    int minutes = totalMinutes % 60;
    
    bool isPM = (hours >= 12);
    int displayHours = (hours % 12);
    if (displayHours == 0) displayHours = 12;
    const char* period = isPM ? "PM" : "AM";

    DrawText(TextFormat("TIME: %d:%02d %s", displayHours, minutes, period),
        screenX + 10,
        screenY + 10,
        30, WHITE);

}

void Scene::renderLoseOverlay()
{
    float screenX = mGameState.camera.target.x - (GetScreenWidth() / 2.0f);
    float screenY = mGameState.camera.target.y - (GetScreenHeight() / 2.0f);

    // RED BACKGROUND
    DrawRectangle(screenX, screenY, GetScreenWidth(), GetScreenHeight(), RED);

    // Black text
    const char* message = "TOO LATE...";
    int msgFontSize = 60;
    int msgWidth = MeasureText(message, msgFontSize);
    DrawText(message,
        screenX + (GetScreenWidth() - msgWidth) / 2,
        screenY + GetScreenHeight() / 2 - 50,
        msgFontSize,
        BLACK);

    const char* restart = "Press ENTER to try again";
    int restartFontSize = 24;
    int restartWidth = MeasureText(restart, restartFontSize);
    DrawText(restart,
        screenX + (GetScreenWidth() - restartWidth) / 2,
        screenY + GetScreenHeight() / 2 + 50,
        restartFontSize,
        BLACK);
}

void Scene::renderWinOverlay()
{
    float screenX = mGameState.camera.target.x - (GetScreenWidth() / 2.0f);
    float screenY = mGameState.camera.target.y - (GetScreenHeight() / 2.0f);

    // GREEN BACKGROUND
    DrawRectangle(screenX, screenY, GetScreenWidth(), GetScreenHeight(), GREEN);

    // Black text
    const char* message = "YOU ESCAPED!";
    int msgFontSize = 60;
    int msgWidth = MeasureText(message, msgFontSize);
    DrawText(message,
        screenX + (GetScreenWidth() - msgWidth) / 2,
        screenY + GetScreenHeight() / 2 - 50,
        msgFontSize,
        BLACK);

    const char* restart = "Press ENTER to play again";
    int restartFontSize = 24;
    int restartWidth = MeasureText(restart, restartFontSize);
    DrawText(restart,
        screenX + (GetScreenWidth() - restartWidth) / 2,
        screenY + GetScreenHeight() / 2 + 50,
        restartFontSize,
        BLACK);
}