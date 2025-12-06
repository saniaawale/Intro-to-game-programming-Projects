#ifndef ENEMY_H
#define ENEMY_H

#include "Entity.h"

enum EnemyType { DAD, MOM };
enum EnemyState { ENEMY_IDLE, ENEMY_PATROLLING, ENEMY_ANGRY, ENEMY_CHASING };

class Enemy : public Entity
{
private:
    Vector2 mScale;
    EnemyType mEnemyType;
    EnemyState mEnemyState;
    Vector2 mPatrolStart;
    Vector2 mPatrolEnd;
    float mPatrolSpeed;
    bool mMovingToEnd;
    float mChaseSpeed;
    float mDetectionRadius;
    Entity* mTarget;
    float mAngerTimer;
    float mAngerDuration;
    bool mIsAngry;
    //for dad 
    Vector2 mGuardPosition;

public:
    Enemy();
    Enemy(Vector2 position, Vector2 scale, const char* textureFilepath, EnemyType type);
    Enemy(Vector2 position, Vector2 scale, const char* textureFilepath,
        TextureType textureType, Vector2 spriteSheetDimensions,
        std::map<Direction, std::vector<int>> animationAtlas, EnemyType type);
    ~Enemy();

    void updateAI(float deltaTime, Entity* player, bool flashlightOn, bool playerSprinting);
    void updateDadAI(float deltaTime, Entity* player, bool playerSprinting);
    void updateMomAI(float deltaTime, Entity* player, bool flashlightOn);

    void setPatrolPoints(Vector2 start, Vector2 end, float speed);
    void patrol(float deltaTime);

    void chasePlayer(float deltaTime, Entity* player);
    float getDistanceToPlayer(Entity* player);

    void setGuardPosition(Vector2 position);
    void guardPosition();

    // Anger system
    void triggerAnger(float duration);
    void updateAnger(float deltaTime);
    bool isAngry() const { return mIsAngry; }

    EnemyType getEnemyType() const { return mEnemyType; }
    EnemyState getEnemyState() const { return mEnemyState; }
    void setEnemyState(EnemyState state) { mEnemyState = state; }
    void setDetectionRadius(float radius) { mDetectionRadius = radius; }
    void setChaseSpeed(float speed) { mChaseSpeed = speed; }
    void setScale(Vector2 newScale){mScale = newScale;}
};

#endif // ENEMY_H