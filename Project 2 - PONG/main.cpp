#include "CS3113/cs3113.h"

constexpr int SCREEN_WIDTH  = 1600 / 2,
              SCREEN_HEIGHT = 900 / 2,
              FPS           = 60,
              SIZE          = 25,
              SPEED         = 200,
              MAX_BALLS = 3;

constexpr char    BG_COLOUR[]    = "#F8F1C8";
constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
BASE_SIZE = { (float)SIZE, (float)SIZE*5 },
P1_INIT_POS = { SIZE / 2 , SCREEN_HEIGHT / 2 },
P2_INIT_POS = { SCREEN_WIDTH - (SIZE/2) , SCREEN_HEIGHT / 2 };

 

constexpr char PLAYER_ONE[]  = "assets/Computer.png";
constexpr char PLAYER_TWO[] = "assets/Player.png";
constexpr char BG_TEXTURE[] = "assets/Board.png";
constexpr char BALL_PATH[] = "assets/ball.png";

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gAngle         = 0.0f,
          gPreviousTicks = 0.0f;
bool singlePlayerGame = false;


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
Vector2 gBallPositions[MAX_BALLS];
Vector2 gBallMovements[MAX_BALLS];
Vector2 gBallScale = { (float)SIZE * 0.8f , (float)SIZE * 0.8f };
int gActiveBalls = 1;  // Start with 1 ball active


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

void resetBall(int ballIndex)
{
    gBallPositions[ballIndex] = ORIGIN;

    // Vary the initial direction slightly for each ball
    float xDir = (ballIndex % 2 == 0) ? 1.0f : -1.0f;
    float yDir = (ballIndex == 0) ? 1.0f : (ballIndex == 1) ? -1.0f : 0.5f;

    gBallMovements[ballIndex] = { xDir, yDir };
}

void setActiveBalls(int count)
{
    int previousCount = gActiveBalls;
    gActiveBalls = count;

    if (count > previousCount) {
        for (int i = previousCount; i < count; i++) {
            resetBall(i);
        }
    }
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

    for (int i = 0; i < MAX_BALLS; i++) {
        resetBall(i);
    }
    

    SetTargetFPS(FPS);
}

void processInput() 
{
 

    gp1Movement = { 0.0f, 0.0f };
    gp2Movement = { 0.0f , 0.0f };

    //Player 1 controls 
    if (IsKeyDown(KEY_W)) gp1Movement.y = -1;
    else if (IsKeyDown(KEY_S)) gp1Movement.y = 1;

    //Switch to single player 
    if (IsKeyPressed(KEY_T)) singlePlayerGame = true;

    // set ball count
    if (IsKeyPressed(KEY_ONE)) setActiveBalls(1);
    if (IsKeyPressed(KEY_TWO)) setActiveBalls(2);
    if (IsKeyPressed(KEY_THREE)) setActiveBalls(3);

    if (singlePlayerGame == false) {
        //Player 2 controls 
        if (IsKeyDown(KEY_UP))    gp2Movement.y = -1;
        else if (IsKeyDown(KEY_DOWN))  gp2Movement.y = 1;
    }
    else {
        int targetBall = -1;
        for (int i = 0; i < gActiveBalls; i++) {
            if (gBallPositions[i].x > SCREEN_WIDTH / 2) {
                targetBall = i;
                break;// Follow the first ball found in the court
            }
        }

        // If a ball is in right court 
        if (targetBall != -1) {
            if (gBallPositions[targetBall].y < gp2Position.y) {
                gp2Movement.y = -1;
            }
            else if (gBallPositions[targetBall].y > gp2Position.y) {
                gp2Movement.y = 1;
            }
        }
    }
    

    if (GetLength(&gp1Movement) > 1.0f) Normalise(&gp1Movement);
    if (GetLength(&gp2Movement) > 1.0f) Normalise(&gp2Movement);

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

    
}

void update()
{
    // Delta time
    float ticks = (float)GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    gp1Position = {
        gp1Position.x + SPEED * gp1Movement.x * deltaTime,
        gp1Position.y + SPEED * gp1Movement.y * deltaTime
    };

    // keep player1 in bounds 
    if (gp1Position.y < gp1Scale.y / 2) gp1Position.y = gp1Scale.y / 2;
    if (gp1Position.y > SCREEN_HEIGHT - gp1Scale.y / 2) gp1Position.y = SCREEN_HEIGHT - gp1Scale.y / 2;


    gp2Position = {
        gp2Position.x + SPEED * gp2Movement.x * deltaTime,
        gp2Position.y + SPEED * gp2Movement.y * deltaTime
    };

    // Player 2 in bounds
    if (gp2Position.y < gp2Scale.y / 2) gp2Position.y = gp2Scale.y / 2;
    if (gp2Position.y > SCREEN_HEIGHT - gp2Scale.y / 2) gp2Position.y = SCREEN_HEIGHT - gp2Scale.y / 2;

    
    float ballSpeed = 300.0f;
    //BALL MOVEMENT 
    for (int i = 0; i < gActiveBalls; i++) {
        // Normalize
        if (GetLength(&gBallMovements[i]) > 1.0f) Normalise(&gBallMovements[i]);

        gBallPositions[i] = {
            gBallPositions[i].x + ballSpeed * gBallMovements[i].x * deltaTime,
            gBallPositions[i].y + ballSpeed * gBallMovements[i].y * deltaTime
        };

        // collision with top and bottom walls
        if (gBallPositions[i].y <= gBallScale.y / 2 ||
            gBallPositions[i].y >= SCREEN_HEIGHT - gBallScale.y / 2) {
            gBallMovements[i].y *= -1;  // reverse direction vertically 
        }

        //collision with player 1 
        if (isColliding(&gBallPositions[i], &gBallScale, &gp1Position, &gp1Scale)) {
            if (gBallMovements[i].x < 0) {  // reverse if moving toward P1
                gBallMovements[i].x *= -1;
            }
        }

        // collision with Player 2 
        if (isColliding(&gBallPositions[i], &gBallScale, &gp2Position, &gp2Scale)) {
            if (gBallMovements[i].x > 0) {  // reverse if moving toward P2
                gBallMovements[i].x *= -1;
            }
        }

        // P2 winning condition 
        if (gBallPositions[i].x < 0) {
            gp2Score++;
            printf("Player 2 wins!");
            gAppStatus = TERMINATED;  //end game
        }

        // P1 winning condition
        if (gBallPositions[i].x > SCREEN_WIDTH) {
            p1Score++;
            printf("Player 1 wins!");
            gAppStatus = TERMINATED; // end the game
        }
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

    // Render only active balls
    for (int i = 0; i < gActiveBalls; i++) {
        renderObject(&gBallTexture, &gBallPositions[i], &gBallScale);
    }

    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow(); 
    UnloadTexture(gPlayer1Texture);
    UnloadTexture(gPlayer2Texture);
    UnloadTexture(gBallTexture);
    UnloadTexture(gBGTexture);
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