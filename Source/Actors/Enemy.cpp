//
// Created by roger on 02/05/2025.
//

#include "Enemy.h"

#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/DrawComponent.h"
#include "../Components/AABBComponent.h"

Enemy::Enemy(Game *game, float width, float height, float movespeed, float heathPoints)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mMoveSpeed(movespeed)
    ,mHealthPoints(heathPoints)
    ,mKnockBack(0.0f)
    ,mKnockBackTimer(0.0f)
    ,mKnockBackDuration(0.0f)
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

    mDrawComponent = new DrawComponent(this, vertices);
    mRigidBodyComponent = new RigidBodyComponent(this, 1, true, 40000, 1300);
    mAABBComponent = new AABBComponent(this, v1, v3, {245, 154, 25, 255});

    game->AddEnemy(this);

}

Enemy::~Enemy() {
    mGame->RemoveEnemy(this);
}

void Enemy::ReceiveHit(float damage, Vector2 knockBackDirection) {
    mHealthPoints -= damage;
    mRigidBodyComponent->SetVelocity(knockBackDirection * mKnockBack);
    mKnockBackTimer = 0;
}

bool Enemy::Died() {
    return mHealthPoints <= 0;
}
