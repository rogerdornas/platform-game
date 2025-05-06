//
// Created by roger on 22/04/2025.
//

#include "Ground.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/DrawComponent.h"
#include "../Components/AABBComponent.h"

Ground::Ground(Game *game, float width, float height, bool isSpine, bool isMoving, float movingDuration, Vector2 velocity)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mIsSpine(isSpine)
    ,mIsMoving(isMoving)
    ,mMovingDuration(movingDuration)
    ,mMovingTimer(movingDuration)
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

    SDL_Color color;
    if (mIsSpine) {
        color = SDL_Color{255, 0, 0, 255};
    }
    else {
        color = SDL_Color{0, 255, 0, 255};
    }

    mDrawComponent = new DrawComponent(this, vertices);
    mRigidBodyComponent = new RigidBodyComponent(this, 1, false);
    mAABBComponent = new AABBComponent(this, v1, v3, color);

    mRigidBodyComponent->SetVelocity(velocity);

    game->AddGround(this);
}

Ground::~Ground() {
    mGame->RemoveGround(this);
}

void Ground::OnUpdate(float deltaTime) {
    if (mIsMoving) {
        mMovingTimer += deltaTime;
        if (mMovingTimer > mMovingDuration) {
            mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() * -1);
            mMovingTimer = 0;
        }
    }
}
