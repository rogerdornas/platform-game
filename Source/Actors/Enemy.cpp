//
// Created by roger on 02/05/2025.
//

#include "Enemy.h"

#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

Enemy::Enemy(Game *game, float width, float height, float moveSpeed, float heathPoints, float contactDamage)
    :Actor(game)
    ,mDrawPolygonComponent(nullptr)
    ,mDrawSpriteComponent(nullptr)
    ,mDrawAnimatedComponent(nullptr)

    ,mWidth(width * mGame->GetScale())
    ,mHeight(height * mGame->GetScale())
    ,mMoveSpeed(moveSpeed * mGame->GetScale())
    ,mHealthPoints(heathPoints)
    ,mContactDamage(contactDamage)
    ,mKnockBackSpeed(0.0f)
    ,mKnockBackTimer(0.0f)
    ,mKnockBackDuration(0.0f)
    ,mIsFlashing(false)
    ,mFlashDuration(0.05f)
    ,mFlashTimer(mFlashDuration)
    ,mPlayerSpotted(false)
{
    Vector2 v1(-mWidth/2, -mHeight/2);
    Vector2 v2(mWidth/2, -mHeight/2);
    Vector2 v3(mWidth/2, mHeight/2);
    Vector2 v4(-mWidth/2, mHeight/2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {245, 154, 25, 255});
    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000 * mGame->GetScale(), 40000 * mGame->GetScale());
    mAABBComponent = new AABBComponent(this, v1, v3);

    game->AddEnemy(this);
}

Enemy::~Enemy() {
    mGame->RemoveEnemy(this);
}

void Enemy::ReceiveHit(float damage, Vector2 knockBackDirection) {
    mHealthPoints -= damage;
    // mRigidBodyComponent->SetVelocity(knockBackDirection * mKnockBack);
    mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() + knockBackDirection * mKnockBackSpeed);
    mKnockBackTimer = 0;
    mIsFlashing = true;
    mFlashTimer = 0;
    mPlayerSpotted = true;
}

bool Enemy::Died() {
    return mHealthPoints <= 0;
}
