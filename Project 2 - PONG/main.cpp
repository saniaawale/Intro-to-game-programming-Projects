#include "CS3113/cs3113.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1600 / 2,
              SCREEN_HEIGHT = 900 / 2,
              FPS           = 60,
              SIZE          = 25,
              SPEED         = 200;

constexpr char    BG_COLOUR[]    = "#F8F1C8";
constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
BASE_SIZE = { (float)SIZE, (float)SIZE*5 },
P1_INIT_POS = { SIZE / 2 , SCREEN_HEIGHT / 2 },
P2_INIT_POS = { SCREEN_WIDTH - (SIZE/2) , SCREEN_HEIGHT / 2 };
 

// Images owned by Nintendo — please don't sue me.
constexpr char PLAYER_ONE[]  = "assets/Computer.png";
constexpr char PLAYER_TWO[] = "assets/Player.png";
constexpr char BG_TEXTURE[] = "assets/Board.png";
constexpr char BALL_PATH[] = "assets/ball.png";

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gAngle         = 0.0f,
          gPreviousTicks = 0.0f;


Texture2D gPlayer1Texture;
Texture2D gPlayer2Texture;
Texture2D gBGTexture;
Texture2D gBallTexture;


unsigned int startTime;

// Player 1 variables
Vector2 gp1Position = P1_INIT_POS,
        gp1Movement = { 0.0f, 0.0f },
        gp1Scale = BASE_SIZE;
        int p1Score = 0;

// Player 2 variables
Vector2 gp2Position = P2_INIT_POS,
        gp2Movement = { 0.0f, 0.0f },
        gp2Scale = BASE_SIZE;
        int gp2Score = 0;

// Ball Variables 
Vector2 gBallPosition = ORIGIN,
        gBallMovement = {1.0f, 1.0f},
        gBallScale = { (float)SIZE * 0.8f , (float)SIZE * 0.8f };



// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);

// Function Definitions


bool isColliding(const Vector2 *positionA,  const Vector2 *scaleA, 
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(positionA->x - positionB->x) - ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) - ((scaleA->y + scaleB->y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

void renderObject(const Texture2D *texture, const Vector2 *position, 
                  const Vector2 *scale)
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        *texture, 
        textureArea, destinationArea, originOffset,
        gAngle, WHITE
    );
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 2 - Pong");

    startTime = time(NULL);

    gPlayer1Texture  = LoadTexture(PLAYER_ONE);
    gPlayer2Texture = LoadTexture(PLAYER_TWO);
    gBGTexture = LoadTexture(BG_TEXTURE);
    gBallTexture = LoadTexture(BALL_PATH);

    SetTargetFPS(FPS);
}

void processInput() 
{
    /*

    if      (IsKeyDown(KEY_A)) gMovement.x = -1;
    else if (IsKeyDown(KEY_D)) gMovement.x =  1;
    if      (IsKeyDown(KEY_W)) gMovement.y = -1;
    else if (IsKeyDown(KEY_S)) gMovement.y =  1;

    /*
    This system will cause quite a bit of "shaking" once the game object
    reaches the mouse position. Ideally, we'd stop checking for this
    once the object reaches a general area AROUND the mouse position.
    
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if (gPosition.x < gMousePosition.x) gMovement.x =  1;
        if (gPosition.x > gMousePosition.x) gMovement.x = -1;
        if (gPosition.y < gMousePosition.y) gMovement.y =  1;
        if (gPosition.y > gMousePosition.y) gMovement.y = -1;
    }

    // to avoid faster diagonal speed
    if (GetLength(&gMovement) > 1.0f) Normalise(&gMovement);
    */

    gp1Movement = { 0.0f, 0.0f };
    gp2Movement = { 0.0f , 0.0f };

    //Player 1 controls 
    if (IsKeyDown(KEY_W)) gp1Movement.y = -1;
    else if (IsKeyDown(KEY_S)) gp1Movement.y = 1;

    //Player 2 controls 
    if (IsKeyDown(KEY_UP))    gp2Movement.y = -1;
    else if (IsKeyDown(KEY_DOWN))  gp2Movement.y = 1;

    if (GetLength(&gp1Movement) > 1.0f) Normalise(&gp1Movement);
    if (GetLength(&gp2Movement) > 1.0f) Normalise(&gp2Movement);

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

    
}

void update() 
{
    // Delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    /*gMousePosition = GetMousePosition();

    gPosition = {
        gPosition.x + SPEED * gMovement.x * deltaTime,
        gPosition.y + SPEED * gMovement.y * deltaTime
    };

    if (isColliding(
        &gPosition,  &gScale,
        &gRupeePosition, &gRupeeScale
    )) printf("Collision @ %us in game time.\n", (unsigned) time(NULL) - startTime);
    */

    gp1Position = {
        gp1Position.x + SPEED * gp1Movement.x * deltaTime,
        gp1Position.y + SPEED * gp1Movement.y * deltaTime
    };

    gp2Position = {
        gp2Position.x + SPEED * gp2Movement.x * deltaTime,
        gp2Position.y + SPEED * gp2Movement.y * deltaTime
    };

    // keep player in bounds 
    if (gp1Position.y < gp1Scale.y / 2) gp1Position.y = gp1Scale.y / 2;
    if (gp1Position.y > SCREEN_HEIGHT - gp1Scale.y / 2) gp1Position.y = SCREEN_HEIGHT - gp1Scale.y / 2;

    if (gp2Position.y < gp2Scale.y / 2) gp2Position.y = gp2Scale.y / 2;
    if (gp2Position.y > SCREEN_HEIGHT - gp2Scale.y / 2) gp2Position.y = SCREEN_HEIGHT - gp2Scale.y / 2;

    // Normalize ball movement 
    if (GetLength(&gBallMovement) > 1.0f) Normalise(&gBallMovement);

    float ballSpeed = 300.0f;  // ball speed 
    gBallPosition = {
        gBallPosition.x + ballSpeed * gBallMovement.x * deltaTime,
        gBallPosition.y + ballSpeed * gBallMovement.y * deltaTime
    };

    // Ball collision with top and bottom walls
    if (gBallPosition.y <= gBallScale.y / 2 ||
        gBallPosition.y >= SCREEN_HEIGHT - gBallScale.y / 2) {
        gBallMovement.y *= -1;  // Reverse vertical direction
    }

    // Ball collision with Player 1 paddle
    if (isColliding(&gBallPosition, &gBallScale, &gp1Position, &gp1Scale)) {
        if (gBallMovement.x < 0) {  // Only reverse if moving toward P1
            gBallMovement.x *= -1;
        }
    }

    // Ball collision with Player 2 paddle
    if (isColliding(&gBallPosition, &gBallScale, &gp2Position, &gp2Scale)) {
        if (gBallMovement.x > 0) {  // Only reverse if moving toward P2
            gBallMovement.x *= -1;
        }
    }

    // Scoring - player 2 scores
    if (gBallPosition.x < 0) {
        gp2Score++;
        printf("Player 2 scores! Score: %d - %d\n", p1Score, gp2Score);

        // Reset ball to center
        gBallPosition = ORIGIN;
        gBallMovement = { -1.0f, 1.0f };  // send toward P1
    }

    // Scoring - Player 1 scores
    if (gBallPosition.x > SCREEN_WIDTH) {
        p1Score++;
        printf("Player 1 scores! Score: %d - %d\n", p1Score, gp2Score);

        // reset to center
        gBallPosition = ORIGIN;
        gBallMovement = { 1.0f, 1.0f };  // Send toward P2
    }


    
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));
    DrawTexturePro(
        gBGTexture,
        { 0, 0, (float)gBGTexture.width, (float)gBGTexture.height },  // source
        { 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT },          // destination
        { 0, 0 },                                                      // origin
        0.0f,                                                        // rotation
        WHITE                                                        
    );

    renderObject(&gPlayer1Texture, &gp1Position, &gp1Scale);
    renderObject(&gPlayer2Texture, &gp2Position, &gp2Scale);

    renderObject(&gBallTexture, &gBallPosition, &gBallScale);

    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow(); 
    UnloadTexture(gPlayer1Texture);
    UnloadTexture(gPlayer2Texture);
    UnloadTexture(gBallTexture);
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}