//
// Created by roger on 23/05/2025.
//

#include "Trigger.h"
#include "../Game.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

Trigger::Trigger(class Game *game, float width, float height)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mDrawPolygonComponent(nullptr)

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

    mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {160, 32, 240, 255});
    mAABBComponent = new AABBComponent(this, v1, v3);
}

void Trigger::SetTarget(std::string target) {
    if (target == "Camera") {
        mTarget = Target::camera;
        return;
    }
}

void Trigger::SetEvent(std::string event) {
    if (event == "Fixed") {
        mEvent = Event::fixed;
        return;
    }
    if (event == "FollowPlayer") {
        mEvent = Event::followPlayer;
        return;
    }
    if (event == "ScrollRight") {
        mEvent = Event::scrollRight;
        return;
    }
    if (event == "ScrollUp") {
        mEvent = Event::scrollUp;
        return;
    }
}


void Trigger::OnUpdate(float deltaTime) {
    Player* player = mGame->GetPlayer();
    if (mAABBComponent->Intersect(*player->GetComponent<AABBComponent>())) {
        switch (mTarget) {
            case Target::camera:
                CameraTrigger();
                break;
        }
    }
}

void Trigger::CameraTrigger() {
    Camera* camera = mGame->GetCamera();
    switch (mEvent) {
        case Event::fixed:
            camera->ChangeCameraMode(CameraMode::Fixed);
            break;
        case Event::followPlayer:
            camera->ChangeCameraMode(CameraMode::FollowPlayer);
            break;
        case Event::scrollRight:
            camera->ChangeCameraMode(CameraMode::ScrollRight);
            break;
        case Event::scrollUp:
            camera->ChangeCameraMode(CameraMode::ScrollUp);
            break;
    }
}

void Trigger::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));

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

    if (mDrawPolygonComponent)
        mDrawPolygonComponent->SetVertices(vertices);
}
