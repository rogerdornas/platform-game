//
// Created by roger on 26/04/2025.
//

#include "Sword.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/DrawComponent.h"
#include "../Components/AABBComponent.h"

Sword::Sword(class Game *game, float width, float height, float duration, float damage)
    :Actor(game)
    ,mWidthHorizontal(width)
    ,mHeightHorizontal(height)
    ,mWidthVertical(height)
    ,mHeightVertical(width)
    ,mDuration(duration)
    ,mDamage(damage)
{
    mDurationTimer = mDuration;

    Vector2 v1(-mWidthHorizontal/2, -mHeightHorizontal/2);
    Vector2 v2(mWidthHorizontal/2, -mHeightHorizontal/2);
    Vector2 v3(mWidthHorizontal/2, mHeightHorizontal/2);
    Vector2 v4(-mWidthHorizontal/2, mHeightHorizontal/2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    mDrawComponent = new DrawComponent(this, vertices);
    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 1800);
    mAABBComponent = new AABBComponent(this, v1, v3, {37, 218, 255, 255});
}

void Sword::OnUpdate(float deltaTime) {
    mDurationTimer += deltaTime;
    if (mDurationTimer >= mDuration) {
        SetState(ActorState::Paused);
        mAABBComponent->SetActive(false); // desativa colisão
        mDurationTimer = 0;
    }
    else {
        mAABBComponent->SetActive(true); // reativa colisão
        class Player* player = GetGame()->GetPlayer();

        float epsilon = 0.001f;
        bool isHorizontal = true;
        if ((Math::Abs(GetRotation() - 0) < epsilon) || (Math::Abs(GetRotation() - Math::Pi) < epsilon)) {
            isHorizontal = true;
        }

        if ((Math::Abs(GetRotation() - Math::Pi / 2) < epsilon) || (Math::Abs(GetRotation() - 3 * Math::Pi / 2) < epsilon)) {
            isHorizontal = false;
        }

        float offset = 0;
        if (isHorizontal) {
            Vector2 v1(-mWidthHorizontal/2, -mHeightHorizontal/2);
            Vector2 v3(mWidthHorizontal/2, mHeightHorizontal/2);
            mAABBComponent->SetMin(v1);
            mAABBComponent->SetMax(v3);

            offset = mWidthHorizontal / 2;
        }
        else {
            Vector2 v1(-mWidthVertical/2, -mHeightVertical/2);
            Vector2 v3(mWidthVertical/2, mHeightVertical/2);
            mAABBComponent->SetMin(v1);
            mAABBComponent->SetMax(v3);

            offset = mHeightVertical / 2;
        }
        SetPosition(Vector2(player->GetPosition() + GetForward() * offset));
    }
}



