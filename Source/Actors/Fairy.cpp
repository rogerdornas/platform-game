//
// Created by roger on 11/06/2025.
//

#include "Fairy.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"


Fairy::Fairy(Game *game, float width, float height)
    :Actor(game)
    ,mWidth(width * mGame->GetScale())
    ,mHeight(height * mGame->GetScale())
    ,mSpeed(400.0f * mGame->GetScale())
    ,mLerpSpeed(2.0f * mGame->GetScale())
    ,mOffsetPosition(Vector2(-70, -70) * mGame->GetScale())
    ,mDrawPolygonComponent(nullptr)
    ,mDrawSpriteComponent(nullptr)
    ,mDrawAnimatedComponent(nullptr)
{
    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {255, 255, 255, 255});
    // mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Fairy/Fairy1.png",
    //                                                 static_cast<int>(mWidth * 1.8),
    //                                                 static_cast<int>(mHeight * 1.8));
    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 2.0f, mHeight * 2.0f, "../Assets/Sprites/FinalBoss/FinalBoss.png", "../Assets/Sprites/FinalBoss/FinalBoss.json", 500);
    std::vector idle = {53, 54, 55, 56, 57, 58, 59, 60};
    mDrawAnimatedComponent->AddAnimation("idle", idle);

    mDrawAnimatedComponent->SetAnimation("idle");
    mDrawAnimatedComponent->SetAnimFPS(10.0f);

    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000 * mGame->GetScale(), 1600 * mGame->GetScale());

}

void Fairy::OnUpdate(float deltaTime) {
    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 targetPos(playerPos + mOffsetPosition);
    mRigidBodyComponent->SetVelocity((targetPos - GetPosition()) * mLerpSpeed);
    // SetPosition(Vector2(GetPosition().x + (targetPos.x - GetPosition().x) * mLerpSpeed * deltaTime,
    //            GetPosition().y + (targetPos.y - GetPosition().y) * mLerpSpeed * deltaTime));

    if (mRigidBodyComponent->GetVelocity().x >= 0) {
        SetRotation(0);
    }
    else {
        SetRotation(Math::Pi);
    }
}

void Fairy::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mSpeed = mSpeed / oldScale * newScale;
    mLerpSpeed = mLerpSpeed / oldScale * newScale;
    mOffsetPosition.x = mOffsetPosition.x / oldScale * newScale;
    mOffsetPosition.y = mOffsetPosition.y / oldScale * newScale;
}
