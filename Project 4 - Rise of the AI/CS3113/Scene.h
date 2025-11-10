#include "Entity.h"

#ifndef SCENE_H
#define SCENE_H

struct GameState
{
    Entity* xochitl = nullptr;
    Map* map = nullptr;
    Entity* door = nullptr;
    Music bgm = { 0 };
    Sound jumpSound = { 0 };
    int playerLives = 3;
    Camera2D camera = { 0 };

    int nextSceneID = 0;
};

class Scene
{
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char* mBGColourHexCode = "#000000";

public:
    Scene();
    Scene(Vector2 origin, const char* bgHexCode);

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
    virtual void shootBullet() {}

    GameState& getState() { return mGameState; } 
    Vector2     getOrigin()          const { return mOrigin; }
    const char* getBGColourHexCode() const { return mBGColourHexCode; }
};

#endif