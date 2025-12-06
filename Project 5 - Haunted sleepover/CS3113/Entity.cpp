#include "Entity.h"

Entity::Entity() : mPosition{ 0.0f, 0.0f }, mMovement{ 0.0f, 0.0f },
mVelocity{ 0.0f, 0.0f }, mAcceleration{ 0.0f, 0.0f },
mScale{ DEFAULT_SIZE, DEFAULT_SIZE },
mColliderDimensions{ DEFAULT_SIZE, DEFAULT_SIZE },
mTexture{ NULL }, mTextureType{ SINGLE }, mAngle{ 0.0f },
mSpriteSheetDimensions{}, mDirection{ RIGHT },
mAnimationAtlas{ {} }, mAnimationIndices{}, mFrameSpeed{ 0 },
mEntityType{ NONE } { }

Entity::Entity(Vector2 position, Vector2 scale, const char* textureFilepath,
    EntityType entityType) : mPosition{ position }, mVelocity{ 0.0f, 0.0f },
    mAcceleration{ 0.0f, 0.0f }, mScale{ scale }, mMovement{ 0.0f, 0.0f },
    mColliderDimensions{ scale }, mTexture{ LoadTexture(textureFilepath) },
    mTextureType{ SINGLE }, mDirection{ RIGHT }, mAnimationAtlas{ {} },
    mAnimationIndices{}, mFrameSpeed{ 0 }, mSpeed{ DEFAULT_SPEED },
    mAngle{ 0.0f }, mEntityType{ entityType } { }

Entity::Entity(Vector2 position, Vector2 scale, const char* textureFilepath,
    TextureType textureType, Vector2 spriteSheetDimensions, std::map<Direction,
    std::vector<int>> animationAtlas, EntityType entityType) :
    mPosition{ position }, mVelocity{ 0.0f, 0.0f },
    mAcceleration{ 0.0f, 0.0f }, mMovement{ 0.0f, 0.0f }, mScale{ scale },
    mColliderDimensions{ scale }, mTexture{ LoadTexture(textureFilepath) },
    mTextureType{ ATLAS }, mSpriteSheetDimensions{ spriteSheetDimensions },
    mAnimationAtlas{ animationAtlas }, mDirection{ RIGHT },
    mAnimationIndices{ animationAtlas.find(RIGHT) != animationAtlas.end() ? 
                       animationAtlas.at(RIGHT) : std::vector<int>() }, 
    mFrameSpeed{ DEFAULT_FRAME_SPEED }, mAngle{ 0.0f },
    mSpeed{ DEFAULT_SPEED }, mEntityType{ entityType } { }

Entity::~Entity() { UnloadTexture(mTexture); };

void Entity::checkCollisionY(Entity* collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity* collidableEntity = &collidableEntities[i];

        if (isColliding(collidableEntity))
        {
            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap = fabs(yDistance - (mColliderDimensions.y / 2.0f) -
                (collidableEntity->mColliderDimensions.y / 2.0f));

            if (mVelocity.y > 0)
            {
                mPosition.y -= yOverlap;
                mVelocity.y = 0;
                mIsCollidingBottom = true;
            }
            else if (mVelocity.y < 0)
            {
                mPosition.y += yOverlap;
                mVelocity.y = 0;
                mIsCollidingTop = true;

                if (collidableEntity->mEntityType == BLOCK)
                    collidableEntity->deactivate();
            }
        }
    }
}

void Entity::checkCollisionX(Entity* collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity* collidableEntity = &collidableEntities[i];

        if (isColliding(collidableEntity))
        {
            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap = fabs(yDistance - (mColliderDimensions.y / 2.0f) - (collidableEntity->mColliderDimensions.y / 2.0f));

            if (yOverlap < Y_COLLISION_THRESHOLD) continue;

            float xDistance = fabs(mPosition.x - collidableEntity->mPosition.x);
            float xOverlap = fabs(xDistance - (mColliderDimensions.x / 2.0f) - (collidableEntity->mColliderDimensions.x / 2.0f));

            if (mVelocity.x > 0) {
                mPosition.x -= xOverlap;
                mVelocity.x = 0;

                // Collision!
                mIsCollidingRight = true;
            }
            else if (mVelocity.x < 0) {
                mPosition.x += xOverlap;
                mVelocity.x = 0;

                // Collision!
                mIsCollidingLeft = true;
            }
        }
    }
}

void Entity::checkCollisionY(Map* map)
{
    if (map == nullptr) return;

    Vector2 topCentreProbe = { mPosition.x, mPosition.y - (mColliderDimensions.y / 2.0f) };
    Vector2 topLeftProbe = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y - (mColliderDimensions.y / 2.0f) };
    Vector2 topRightProbe = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y - (mColliderDimensions.y / 2.0f) };

    Vector2 bottomCentreProbe = { mPosition.x, mPosition.y + (mColliderDimensions.y / 2.0f) };
    Vector2 bottomLeftProbe = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y + (mColliderDimensions.y / 2.0f) };
    Vector2 bottomRightProbe = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y + (mColliderDimensions.y / 2.0f) };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    // COLLISION ABOVE (jumping upward)
    if ((map->isSolidTileAt(topCentreProbe, &xOverlap, &yOverlap) ||
        map->isSolidTileAt(topLeftProbe, &xOverlap, &yOverlap) ||
        map->isSolidTileAt(topRightProbe, &xOverlap, &yOverlap)) && mVelocity.y < 0.0f)
    {
        mPosition.y += yOverlap;   // push down
        mVelocity.y = 0.0f;
        mIsCollidingTop = true;
    }

    // COLLISION BELOW (falling downward)
    if ((map->isSolidTileAt(bottomCentreProbe, &xOverlap, &yOverlap) ||
        map->isSolidTileAt(bottomLeftProbe, &xOverlap, &yOverlap) ||
        map->isSolidTileAt(bottomRightProbe, &xOverlap, &yOverlap)) && mVelocity.y > 0.0f)
    {
        mPosition.y -= yOverlap;   // push up
        mVelocity.y = 0.0f;
        mIsCollidingBottom = true;
    }
}


void Entity::checkCollisionX(Map* map)
{
    if (map == nullptr) return;

    // Left side probes 
    Vector2 leftTopProbe = {
        mPosition.x - (mColliderDimensions.x / 2.0f),
        mPosition.y - (mColliderDimensions.y / 2.0f)
    };
    Vector2 leftCentreProbe = {
        mPosition.x - (mColliderDimensions.x / 2.0f),
        mPosition.y
    };

    // Right side probes 
    Vector2 rightTopProbe = {
        mPosition.x + (mColliderDimensions.x / 2.0f),
        mPosition.y - (mColliderDimensions.y / 2.0f)
    };
    Vector2 rightCentreProbe = {
        mPosition.x + (mColliderDimensions.x / 2.0f),
        mPosition.y
    };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    // COLLISION ON RIGHT 
    if ((map->isSolidTileAt(rightTopProbe, &xOverlap, &yOverlap) ||
        map->isSolidTileAt(rightCentreProbe, &xOverlap, &yOverlap))
        && mVelocity.x > 0.0f && yOverlap >= 0.5f)
    {
        mPosition.x -= xOverlap * 1.01f;
        mVelocity.x = 0.0f;
        mIsCollidingRight = true;
    }

    // COLLISION ON LEFT 
    if ((map->isSolidTileAt(leftTopProbe, &xOverlap, &yOverlap) ||
        map->isSolidTileAt(leftCentreProbe, &xOverlap, &yOverlap))
        && mVelocity.x < 0.0f && yOverlap >= 0.5f)
    {
        mPosition.x += xOverlap * 1.01f;
        mVelocity.x = 0.0f;
        mIsCollidingLeft = true;
    }
}

bool Entity::isColliding(Entity* other) const
{
    if (!other->isActive() || other == this) return false;

    float xDistance = fabs(mPosition.x - other->getPosition().x) -
        ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);
    float yDistance = fabs(mPosition.y - other->getPosition().y) -
        ((mColliderDimensions.y + other->getColliderDimensions().y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

void Entity::animate(float deltaTime)
{
    if (!mTexturesLoaded) {
        if (mAnimationAtlas.find(mDirection) != mAnimationAtlas.end()) {
            mAnimationIndices = mAnimationAtlas.at(mDirection);
        }
        else {
            return;
        }
    }
    
    mAnimationTime += deltaTime;
    float framesPerSecond = 1.0f / mFrameSpeed;
    if (mAnimationTime >= framesPerSecond)
    {
        mAnimationTime = 0.0f;
        mCurrentFrameIndex++;
        if (mEntityType == PLAYER && mTexture.id == mJumpTexture.id && mCurrentFrameIndex >= 4) {
            mCurrentFrameIndex = 3; 
        }
        else {
            if (mCurrentFrameIndex >= mAnimationIndices.size()) {
                mCurrentFrameIndex = 0; 
            }
        }
    }
}

void Entity::AIWander()
{
    if (!mPatrolInitialized) {
        mPatrolStart = mPosition;
        mPatrolEnd = { mPosition.x - 1000, mPosition.y };
        mPatrolInitialized = true;
        mWalkingLeft = true; 
    }
    if (mWalkingLeft) {
        moveLeft();
    }
    else {
        moveRight();
    }
    mDirectionChangeTimer += 0.016f;
    if (mDirectionChangeTimer >= 0.5f) {

        // Turn around if hitting walls
        if (mIsCollidingLeft && mWalkingLeft) {
            mWalkingLeft = false;  // Now walk right
            mPatrolEnd = mPosition;
            mDirectionChangeTimer = 0.0f;
        }
        else if (mIsCollidingRight && !mWalkingLeft) {
            mWalkingLeft = true;  // Now walk left
            mPatrolStart = mPosition;
            mDirectionChangeTimer = 0.0f;
        }
        // Turn around at patrol distance boundaries
        else if (mWalkingLeft && mPosition.x <= mPatrolEnd.x) {
            mWalkingLeft = false;  // Now walk right
            mDirectionChangeTimer = 0.0f;
        }
        else if (!mWalkingLeft && mPosition.x >= mPatrolStart.x) {
            mWalkingLeft = true;  // Now walk left
            mDirectionChangeTimer = 0.0f;
        }
    }
}

void Entity::AIFly(Entity* target)
{
    if (!mPatrolInitialized) {
        mPatrolStart = mPosition;
        mPatrolEnd = { mPosition.x + 800.0f, mPosition.y };  // Fly 800 units to the right
        mPatrolInitialized = true;
        mWalkingLeft = false;  // Start moving right
        mDirectionChangeTimer = 0.0f;
    }

    // Horizontal movement
    if (mWalkingLeft) {
        moveLeft();
        mDirection = LEFT;
    }
    else {
        moveRight();
        mDirection = RIGHT;
    }
    // Turn around at patrol boundaries
    if (!mWalkingLeft && mPosition.x >= mPatrolEnd.x) {
        mWalkingLeft = true;  // Start moving left
    }
    else if (mWalkingLeft && mPosition.x <= mPatrolStart.x) {
        mWalkingLeft = false;  // Start moving right
    }

    mAIState = WALKING;
}


void Entity::AIFollow(Entity* target)
{
    if (!target) return;

    float distanceToPlayer = Vector2Distance(mPosition, target->getPosition());

    if (distanceToPlayer >= 400.0f) {
        mAIState = IDLE;
    }
    // If player is nearby, chase them
    else {
        mAIState = WALKING; 

        if (mPosition.x > target->getPosition().x) {
            moveLeft();
        }
        else {
            moveRight();
        }
    }
}

void Entity::AIActivate(Entity* target)
{
    switch (mAIType)
    {
    case WANDERER:
        AIWander();
        break;

    case FOLLOWER:
        AIFollow(target);
        break;

    case FLYER:
        AIFly(target);
        break;

    default:
        break;
    }
}


void Entity::update(float deltaTime, Entity* player, Map* map,
    Entity* collidableEntities, int collisionCheckCount)
{
    if (mEntityStatus == INACTIVE) return;

    if (mEntityType == NPC) AIActivate(player);

    resetColliderFlags();

    float currentSpeed = mSpeed;
    if (mIsSprinting && mEntityType == PLAYER)
    {
        currentSpeed *= 2.0f;
    }
    mVelocity.x = mMovement.x * currentSpeed;

    // Handle invincibility timer
    if (mIsInvincible)
    {
        mInvincibleTimer -= deltaTime;
        if (mInvincibleTimer <= 0.0f)
        {
            mIsInvincible = false;
            mInvincibleTimer = 0.0f;
        }
    }


    mVelocity.x += mAcceleration.x * deltaTime;
    mVelocity.y += mAcceleration.y * deltaTime;

    // ––––– JUMPING ––––– //
    if (mIsJumping)
    {
        mIsJumping = false;
        mVelocity.y -= mJumpingPower;
    }

    mPosition.y += mVelocity.y * deltaTime;
    checkCollisionY(collidableEntities, collisionCheckCount);
    checkCollisionY(map);

    mPosition.x += mVelocity.x * deltaTime;
    checkCollisionX(collidableEntities, collisionCheckCount);
    checkCollisionX(map);

    if (mEntityType == PLAYER) {
        updateAnimationState();  // Player uses this
    }
 

    if (mTextureType == ATLAS)
    {
        animate(deltaTime);
    }

}

void Entity::render()
{
    if (mEntityStatus == INACTIVE) return;

    Rectangle textureArea;

    switch (mTextureType)
    {
    case SINGLE:
        textureArea = { 0.0f, 0.0f,
            (mDirection == LEFT ? -mTexture.width : mTexture.width),
            (float)mTexture.height
        };
        break;;
    case ATLAS:
        textureArea = getUVRectangle(
            &mTexture,
            mAnimationIndices[mCurrentFrameIndex],
            (int)mSpriteSheetDimensions.x,
            (int)mSpriteSheetDimensions.y
        );
        if (mDirection == LEFT)
            textureArea.width = -textureArea.width;
        break;

    default: break;
    }
    Rectangle destinationArea = {
        mPosition.x,
        mPosition.y,
        static_cast<float>(mScale.x),
        static_cast<float>(mScale.y)
    };
    Vector2 originOffset = {
        static_cast<float>(mScale.x) / 2.0f,
        static_cast<float>(mScale.y) / 2.0f
    };
    DrawTexturePro(
        mTexture,
        textureArea, destinationArea, originOffset,
        mAngle, WHITE
    );

    //displayCollider();
}

void Entity::displayCollider()
{
    // draw the collision box
    Rectangle colliderBox = {
        mPosition.x - mColliderDimensions.x / 2.0f,
        mPosition.y - mColliderDimensions.y / 2.0f,
        mColliderDimensions.x,
        mColliderDimensions.y
    };

    DrawRectangleLines(
        colliderBox.x,      // Top-left X
        colliderBox.y,      // Top-left Y
        colliderBox.width,  // Width
        colliderBox.height, // Height
        GREEN               // Color
    );
}

void Entity::loadAnimationTextures(const char* idlePath, const char* runPath,
    const char* jumpPath, const char* attackPath, const char* sprintPath)
{
    mIdleTexture = LoadTexture(idlePath);
    mRunTexture = LoadTexture(runPath);
    mJumpTexture = LoadTexture(jumpPath);
    mAttackTexture = LoadTexture(attackPath);
    mSprintTexture = LoadTexture(sprintPath);
    mTexturesLoaded = true;
    mTexture = mIdleTexture;  // Start with idle
    
    
    mAnimationAtlas.clear();
}

void Entity::updateAnimationState()
{
    if (!mTexturesLoaded) return;

    Texture2D previous = mTexture;

    if (!mIsCollidingBottom || mVelocity.y < -50.0f)  
    { // in da air 
        if (mMovement.x < 0) mDirection = LEFT;
        else if (mMovement.x > 0) mDirection = RIGHT;

        mTexture = mJumpTexture;
        mSpriteSheetDimensions = { 1, 2 };
    }
    else
    {
        // on ground
        if (GetLength(mMovement) > 0.0f)
        {
            if (mIsSprinting)
            {
                mTexture = mSprintTexture;
                mSpriteSheetDimensions = { 1, 8  };
            }
            else
            {
                mTexture = mRunTexture;
                mSpriteSheetDimensions = { 1, 9 };
            }
        }
        else
        {
            mTexture = mIdleTexture;
            mSpriteSheetDimensions = { 1, 2 };
        }
    }
     // checking if animation actually changed 
    mAnimationJustChanged = (mTexture.id != previous.id);

    if (mAnimationJustChanged)
    {
        mAnimationIndices.clear();
        for (int i = 0; i < (int)mSpriteSheetDimensions.y; i++) {
            mAnimationIndices.push_back(i);
        }                
        mCurrentFrameIndex = 0;
        mAnimationTime = 0.0f;
    }
}

void Entity::takeDamage(int amount)
{
    if (mIsInvincible) return; // prevents repeated hits in one frame
    mHealth -= amount;
    if (mHealth < 0) mHealth = 0;
    // invincibility for 1 second after being hit
    mIsInvincible = true;
    mInvincibleTimer = 1.0f; 

}
