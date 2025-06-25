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
    ,mMoneyType(type)

    ,mFlySpeed(500.0f * mGame->GetScale())
    ,mHomingSpeed(1000.0f * mGame->GetScale())

    ,mFlyDuration(0.2f)
    ,mFlyTimer(0.0f)
    ,mHoverDuration(0.5f)
    ,mHoverTimer(0.0f)

    ,mDrawSpriteComponent(nullptr)
    ,mDrawPolygonComponent(nullptr)
{

    switch (mMoneyType) {
        case MoneyType::Large:
            mValue = 10;
            mWidth = 36.0f * mGame->GetScale();
            mHeight = 63.0f * mGame->GetScale();
            mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Money/CristalLarge.png", mWidth, mHeight, 5000);
            break;

        case MoneyType::Medium:
            mValue = 5;
            mWidth = 26.0f * mGame->GetScale();
            mHeight = 45.5f * mGame->GetScale();
            mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Money/CristalMedium.png", mWidth, mHeight, 5000);
            break;

        case MoneyType::Small:
            mValue = 1;
            mWidth = 16.0f * mGame->GetScale();
            mHeight = 28.0f * mGame->GetScale();
            mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Money/CristalSmall.png", mWidth, mHeight, 5000);
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

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, SDL_Color{255, 255, 0, 255}, 5000);

    SetRotation(Random::GetFloatRange(Math::Pi, Math::TwoPi));

    mRigidBodyComponent = new RigidBodyComponent(this, 0.1);
    mRigidBodyComponent->SetVelocity(GetForward() * mFlySpeed);

    mAABBComponent = new AABBComponent(this, v1, v3);

    Deactivate();

    mGame->AddMoney(this);
}

Money::~Money() {
    mGame->RemoveMoney(this);
}

void Money::OnUpdate(float deltaTime) {
    Activate();
    // Colisão entre moneys
    std::vector<Money*> moneys = mGame->GetMoneys();
    if (!moneys.empty()) {
        for (Money* m: moneys) {
            if (m != this) {
                if (mAABBComponent->Intersect(*m->GetComponent<AABBComponent>())) {
                    mAABBComponent->ResolveCollision(*m->GetComponent<AABBComponent>());
                }
            }
        }
    }

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
        mGame->GetAudio()->PlaySound("Money/Money.wav");
        Deactivate();
    }
}

void Money::Activate() {
    mAABBComponent->SetActive(true); // reativa colisão
    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetIsVisible(true);
    }
    if (mDrawSpriteComponent) {
        mDrawSpriteComponent->SetIsVisible(true);
    }
}

void Money::Deactivate() {
    SetState(ActorState::Paused);
    mMoneyState = State::FlyingOut;
    SetRotation(Random::GetFloatRange(Math::Pi, Math::TwoPi));
    mRigidBodyComponent->SetVelocity(GetForward() * mFlySpeed);
    mAABBComponent->SetActive(false); // desativa colisão
    mFlyTimer = 0;
    mHoverTimer = 0;

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetIsVisible(false);
    }
    if (mDrawSpriteComponent) {
        mDrawSpriteComponent->SetIsVisible(false);
    }
}



void Money::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mFlySpeed = mFlySpeed / oldScale * newScale;
    mHomingSpeed = mHomingSpeed / oldScale * newScale;

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    if (mDrawSpriteComponent) {
        mDrawSpriteComponent->SetWidth(mWidth);
        mDrawSpriteComponent->SetHeight(mHeight);
    }

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
