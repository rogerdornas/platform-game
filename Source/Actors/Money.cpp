//
// Created by roger on 17/06/2025.
//

#include "Money.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Random.h"

Money::Money(class Game *game, MoneyType type)
    :Actor(game)
    ,mWidth(15.0f * mGame->GetScale())
    ,mHeight(15.0f * mGame->GetScale())
    ,mMoneyState(State::FlyingOut)

    ,mFlySpeed(500.0f * mGame->GetScale())
    ,mHomingSpeed(1000.0f * mGame->GetScale())

    ,mFlyDuration(0.2f)
    ,mFlyTimer(0.0f)
    ,mHoverDuration(0.5f)
    ,mHoverTimer(0.0f)
{

    switch (type) {
        case MoneyType::Large:
            mValue = 10;
            mWidth = 35.0f * mGame->GetScale();
            mHeight = 35.0f * mGame->GetScale();
            break;

        case MoneyType::Medium:
            mValue = 5;
            mWidth = 25.0f * mGame->GetScale();
            mHeight = 25.0f * mGame->GetScale();
            break;

        case MoneyType::Small:
            mValue = 1;
            mWidth = 15.0f * mGame->GetScale();
            mHeight = 15.0f * mGame->GetScale();
            break;
    }

    // Componente visual
    Vector2 v1(-mWidth/2, -mHeight/2);
    Vector2 v2(mWidth/2, -mHeight/2);
    Vector2 v3(mWidth/2, mHeight/2);
    Vector2 v4(-mWidth/2, mHeight/2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, SDL_Color{255, 255, 0, 255}, 5000);

    SetRotation(Random::GetFloatRange(Math::Pi, Math::TwoPi));

    mRigidBodyComponent = new RigidBodyComponent(this, 0.1);
    mRigidBodyComponent->SetVelocity(GetForward() * mFlySpeed);

    mAABBComponent = new AABBComponent(this, v1, v3);
}

void Money::OnUpdate(float deltaTime) {
    switch (mMoneyState) {
        case State::FlyingOut:
            mFlyTimer += deltaTime;
            if (mFlyTimer >= mFlyDuration) {
                mFlyTimer = 0;
                mMoneyState = State::Hovering;
                mRigidBodyComponent->SetVelocity(Vector2::Zero);
            }
            break;

        case State::Hovering:
            mHoverTimer += deltaTime;
            if (mHoverTimer >= mHoverDuration) {
                mHoverTimer = 0;
                mMoneyState = State::Homing;
            }
            break;

        case State::Homing:
            {
                Vector2 dist = mGame->GetPlayer()->GetPosition() - GetPosition();
                float angle = Math::Atan2(dist.y, dist.x);
                // Ajustar para intervalo [0, 2*pi)
                if (angle < 0) {
                    angle += 2 * Math::Pi;
                }

                SetRotation(angle);
                mRigidBodyComponent->SetVelocity(Vector2(GetForward() * mHomingSpeed));
            }
            break;
    }

    if (mAABBComponent->Intersect(*mGame->GetPlayer()->GetComponent<AABBComponent>())) {
        mGame->GetPlayer()->IncreaseMoney(mValue);
        SetState(ActorState::Destroy);
    }
}

void Money::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mFlySpeed = mFlySpeed / oldScale * newScale;
    mHomingSpeed = mHomingSpeed / oldScale * newScale;

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    mAABBComponent->SetMin(v1);
    mAABBComponent->SetMax(v3);

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetVertices(vertices);
    }
}
