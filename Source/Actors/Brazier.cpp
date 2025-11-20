//
// Created by roger on 20/11/2025.
//

#include "Brazier.h"
#include "Light.h"
#include "../Game.h"
#include "../Components/AABBComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"

Brazier::Brazier(Game *game)
    :Actor(game)
    ,mWidth(100)
    ,mHeight(100)
    ,mLight(nullptr)
    ,mRedLight(nullptr)
    ,mBrazierState(BrazierState::LightOff)
    ,mAABBComponent(nullptr)
    ,mRectComponent(nullptr)
    ,mDrawComponent(nullptr)
{
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

    mAABBComponent = new AABBComponent(this, v1, v3);

    // mRectComponent = new RectComponent(this, mWidth, mHeight, RendererMode::LINES);
    // mRectComponent->SetColor(Vector3(255, 0, 0));

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Brazier/Brazier.png",
                                                "../Assets/Sprites/Brazier/Brazier.json",
                                                mWidth * 1.5f, mWidth * 1.82f * 1.5f);

    std::vector lightOff = {8};
    mDrawComponent->AddAnimation("lightOff", lightOff);

    std::vector lightOn = {0, 1, 2, 3, 4, 5, 6, 7};
    mDrawComponent->AddAnimation("lightOn", lightOn);

    mDrawComponent->SetAnimation("lightOff");
    mDrawComponent->SetAnimFPS(12.0f);
}

void Brazier::OnUpdate(float deltaTime) {
    if (mBrazierState == BrazierState::LightOff) {
        std::vector<FireBall* > fireBalls = mGame->GetFireBalls();
        for (FireBall* f: fireBalls) {
            if (!f->GetIsFromEnemy()) {
                if (mAABBComponent->Intersect(*f->GetComponent<ColliderComponent>())) {
                    mBrazierState = BrazierState::LightOn;
                    InitLight();
                    f->ExplodeFireball();
                    break;
                }
            }
        }
    }

    if (mLight) {
        mLight->SetPosition(GetPosition() - Vector2(0, mHeight / 2));
    }
    if (mRedLight) {
        mRedLight->SetPosition(GetPosition() - Vector2(0, mHeight / 2));
    }

    ManageAnimations();
}

void Brazier::InitLight() {
    mLight = new Light(mGame);
    mLight->SetRadius(1000.0f);
    mLight->SetMaxIntensity(0.7f);
    mLight->SetColor(Vector3(1.0f, 1.0f, 1.0f));
    mLight->Activate(0.4f);

    mRedLight = new Light(mGame);
    mRedLight->SetRadius(200.0f);
    mRedLight->SetMaxIntensity(0.85f);
    mRedLight->SetColor(Vector3(0.92f, 0.37f, 0.37f));
    mRedLight->Activate(0.4f);
}

void Brazier::ManageAnimations() {
    if (mBrazierState == BrazierState::LightOff) {
        mDrawComponent->SetAnimation("lightOff");
    }
    else if (mBrazierState == BrazierState::LightOn) {
        mDrawComponent->SetAnimation("lightOn");
    }
}
