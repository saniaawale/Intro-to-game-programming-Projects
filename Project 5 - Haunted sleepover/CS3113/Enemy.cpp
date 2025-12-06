#include "Enemy.h"
#include <cmath>

Enemy::Enemy() : Entity(),
mEnemyType(DAD),
mEnemyState(ENEMY_IDLE),
mPatrolStart{ 0.0f, 0.0f },
mPatrolEnd{ 0.0f, 0.0f },
mPatrolSpeed(100.0f),
mMovingToEnd(true),
mChaseSpeed(250.0f),
mDetectionRadius(300.0f),
mTarget(nullptr),
mAngerTimer(0.0f),
mAngerDuration(5.0f),
mIsAngry(false),
mGuardPosition{ 0.0f, 0.0f }
{}

Enemy::Enemy(Vector2 position, Vector2 scale, const char* textureFilepath, EnemyType type)
    : Entity(position, scale, textureFilepath, ITEM),
    mEnemyType(type),
    mEnemyState(type == DAD ? ENEMY_IDLE : ENEMY_PATROLLING),
    mPatrolStart{ position.x - 200.0f, position.y },
    mPatrolEnd{ position.x + 200.0f, position.y },
    mPatrolSpeed(100.0f),
    mMovingToEnd(true),
    mChaseSpeed(250.0f),
    mDetectionRadius(300.0f),
    mTarget(nullptr),
    mAngerTimer(0.0f),
    mAngerDuration(5.0f),
    mIsAngry(false),
    mGuardPosition(position)
{}

Enemy::Enemy(Vector2 position, Vector2 scale, const char* textureFilepath,
    TextureType textureType, Vector2 spriteSheetDimensions,
    std::map<Direction, std::vector<int>> animationAtlas, EnemyType type)
    : Entity(position, scale, textureFilepath, textureType, spriteSheetDimensions, animationAtlas, ITEM),
    mEnemyType(type),
    mEnemyState(type == DAD ? ENEMY_IDLE : ENEMY_PATROLLING),
    mPatrolStart{ position.x - 200.0f, position.y },
    mPatrolEnd{ position.x + 200.0f, position.y },
    mPatrolSpeed(100.0f),
    mMovingToEnd(true),
    mChaseSpeed(250.0f),
    mDetectionRadius(300.0f),
    mTarget(nullptr),
    mAngerTimer(0.0f),
    mAngerDuration(5.0f),
    mIsAngry(false),
    mGuardPosition(position)
{}

Enemy::~Enemy()
{}

void Enemy::updateAI(float deltaTime, Entity* player, bool flashlightOn, bool playerSprinting)
{
    if (mEnemyType == DAD)
    {
        updateDadAI(deltaTime, player, playerSprinting);
    }
    else if (mEnemyType == MOM)
    {
        updateMomAI(deltaTime, player, flashlightOn);
    }

    updateAnger(deltaTime);
}

void Enemy::updateDadAI(float deltaTime, Entity* player, bool playerSprinting)
{
    float distanceToPlayer = getDistanceToPlayer(player);

    // Dad gets angry if player is sprinting nearby
    if (playerSprinting && distanceToPlayer < 600)
    {
        triggerAnger(3.0f);
        mEnemyState = ENEMY_CHASING;
    }

    switch (mEnemyState)
    {
    case ENEMY_IDLE:
        guardPosition();

        if (distanceToPlayer < 200.0f || (playerSprinting && distanceToPlayer < mDetectionRadius))
        {
            mEnemyState = ENEMY_CHASING;
        }
        break;

    case ENEMY_CHASING:
        chasePlayer(deltaTime, player);

        if (distanceToPlayer > mDetectionRadius * 1.5f)
        {
            mEnemyState = ENEMY_IDLE;
        }
        break;

    default:
        break;
    }
}

void Enemy::updateMomAI(float deltaTime, Entity* player, bool flashlightOn)
{
    
}

void Enemy::setPatrolPoints(Vector2 start, Vector2 end, float speed)
{
    mPatrolStart = start;
    mPatrolEnd = end;
    mPatrolSpeed = speed;
    mMovingToEnd = true;
}

void Enemy::patrol(float deltaTime)
{
    Vector2 currentPos = getPosition();
    Vector2 target = mMovingToEnd ? mPatrolEnd : mPatrolStart;

    // Calculate direction to target
    Vector2 direction = {
        target.x - currentPos.x,
        target.y - currentPos.y
    };

    float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);

    // If close to target, switch direction
    if (distance < 20.0f)
    {
        mMovingToEnd = !mMovingToEnd;
        return;
    }

    // Normalize direction
    direction.x /= distance;
    direction.y /= distance;

    // Move towards target
    Vector2 newPos = {
        currentPos.x + direction.x * mPatrolSpeed * deltaTime,
        currentPos.y + direction.y * mPatrolSpeed * deltaTime
    };

    setPosition(newPos);
}

void Enemy::chasePlayer(float deltaTime, Entity* player)
{
    Vector2 enemyPos = getPosition();
    Vector2 playerPos = player->getPosition();

    // Calculate direction to player
    Vector2 direction = {
        playerPos.x - enemyPos.x,
        playerPos.y - enemyPos.y
    };

    float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);

    if (distance > 10.0f)
    {
        direction.x /= distance;
        direction.y /= distance;

        if (direction.x > 0)
        {
            setDirection(RIGHT);
        }
        else if (direction.x < 0)
        {
            setDirection(LEFT);
        }

        // Move towards player
        Vector2 newPos = {enemyPos.x + direction.x * mChaseSpeed * deltaTime, enemyPos.y + direction.y * mChaseSpeed * deltaTime
        };

        setPosition(newPos);
    }
}

float Enemy::getDistanceToPlayer(Entity* player)
{
    Vector2 enemyPos = getPosition();
    Vector2 playerPos = player->getPosition();

    float dx = playerPos.x - enemyPos.x;
    float dy = playerPos.y - enemyPos.y;

    return sqrtf(dx * dx + dy * dy);
}

void Enemy::setGuardPosition(Vector2 position)
{
    mGuardPosition = position;
}

void Enemy::guardPosition(){
    setPosition(mGuardPosition);
}

void Enemy::triggerAnger(float duration)
{
    mIsAngry = true;
    mAngerTimer = 0.0f;
    mAngerDuration = duration;
}

void Enemy::updateAnger(float deltaTime)
{
    if (mIsAngry)
    {
        mAngerTimer += deltaTime;

        if (mAngerTimer >= mAngerDuration)
        {
            mIsAngry = false;
            mAngerTimer = 0.0f;
        }
    }
}