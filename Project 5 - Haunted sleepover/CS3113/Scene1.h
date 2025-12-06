#ifndef SCENE1_H
#define SCENE1_H

#include "Scene.h"

class Scene1 : public Scene {
private:
    // Dialogue system
    struct DialogueLine {
        const char* speaker;
        const char* text;
        Color speakerColor;
    };

    static const int DIALOGUE_COUNT = 4;
    DialogueLine dialogue[DIALOGUE_COUNT];
    int currentDialogueIndex;
    bool dialogueComplete;

    // Text display
    float textDisplayTimer;
    int charactersToShow;
    const float TEXT_SPEED = 0.03f; // seconds per character

    // Visual elements
    Texture2D backgroundTexture;

    Entity* mother;
    Entity* daughter;

    // Entrance animation
    bool leahWalkingIn;
    Vector2 leahTargetPosition;
    Vector2 leahStartPosition;

public:
    Scene1();
    Scene1(Vector2 origin, const char* bgHexCode);
    ~Scene1();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;

private:
    void advanceDialogue();
    void renderDialogueBox();
};

#endif