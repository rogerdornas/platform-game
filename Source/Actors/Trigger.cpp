//
// Created by roger on 23/05/2025.
//

#include "Trigger.h"
#include "../Game.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Actors/Ground.h"
#include "../Actors/DynamicGround.h"

Trigger::Trigger(class Game *game, float width, float height)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mTarget(Target::nothing)
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

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {160, 32, 240, 255});
    mAABBComponent = new AABBComponent(this, v1, v3);
}

void Trigger::SetTarget(std::string target) {
    if (target == "Camera") {
        mTarget = Target::camera;
        return;
    }
    if (target == "DynamicGround") {
        mTarget = Target::dynamicGround;
        return;
    }
    if (target == "Ground") {
        mTarget = Target::ground;
        return;
    }
    if (target == "Game") {
        mTarget = Target::game;
        return;
    }
    if (target == "Enemy") {
        mTarget = Target::enemy;
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
    if (event == "FollowPlayerHorizontally") {
        mEvent = Event::followPlayerHorizontally;
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

    if (event == "SetIsGrowing") {
        mEvent = Event::setIsGrowing;
        return;
    }
    if (event == "SetIsDecreasing") {
        mEvent = Event::setIsDecreasing;
        return;
    }
    if (event == "SetIsDecreasingAfterKillingEnemies") {
        mEvent = Event::setIsDecreasingAfterKillingEnemies;
        return;
    }

    if (event == "SetIsMoving") {
        mEvent = Event::setIsMoving;
        return;
    }

    if (event == "ChangeScene") {
        mEvent = Event::changeScene;
        return;
    }

    if (event == "SpotPlayer") {
        mEvent = Event::spotPlayer;
        return;
    }
}

void Trigger::SetScene(std::string scene) {
    if (scene == "Level1") {
        mScene = Game::GameScene::Level1;
        return;
    }
    if (scene == "Level2") {
        mScene = Game::GameScene::Level2;
        return;
    }
    if (scene == "Level3") {
        mScene = Game::GameScene::Level3;
        return;
    }
    if (scene == "Level4") {
        mScene = Game::GameScene::Level4;
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
            case Target::dynamicGround:
                DynamicGroundTrigger();
            break;
            case Target::ground:
                GroundTrigger();
            break;
            case Target::game:
                GameTrigger();
            break;
            case Target::enemy:
                EnemyTrigger();
            break;
            default:
            break;
        }
    }
}

void Trigger::CameraTrigger() {
    Camera* camera = mGame->GetCamera();
    switch (mEvent) {
        case Event::fixed:
            camera->SetFixedCameraPosition(mFixedCameraPosition);
            camera->ChangeCameraMode(CameraMode::Fixed);
            SetState(ActorState::Destroy);
            break;
        case Event::followPlayer:
            camera->ChangeCameraMode(CameraMode::FollowPlayer);
            break;
        case Event::followPlayerHorizontally:
            camera->SetFixedCameraPosition(mFixedCameraPosition);
            camera->ChangeCameraMode(CameraMode::FollowPlayerHorizontally);
            break;
        case Event::scrollRight:
            camera->ChangeCameraMode(CameraMode::ScrollRight);
            break;
        case Event::scrollUp:
            camera->ChangeCameraMode(CameraMode::ScrollUp);
            break;
        default:
            break;
    }
}

void Trigger::DynamicGroundTrigger() {
    std::vector<Ground *> grounds = mGame->GetGrounds();
    std::vector<Enemy *> enemies = mGame->GetEnemies();
    bool allEnemiesDie = true;
    switch (mEvent) {
        case Event::setIsGrowing:
            for (int id : mGroundsIds) {
                Ground *g = mGame->GetGroundById(id);
                DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
                if (dynamicGround) {
                    dynamicGround->SetIsGrowing(true);
                }
            }
            SetState(ActorState::Destroy);
            break;
        case Event::setIsDecreasing:
            for (int id : mGroundsIds) {
                Ground *g = mGame->GetGroundById(id);
                DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
                if (dynamicGround) {
                    dynamicGround->SetIsDecreasing(true);
                }
            }
            SetState(ActorState::Destroy);
            break;
        case Event::setIsDecreasingAfterKillingEnemies:
            for (int id : mEnemiesIds) {
                Enemy *e = mGame->GetEnemyById(id);
                if (e != nullptr) {
                    allEnemiesDie = false;
                }
            }
            if (allEnemiesDie) {
                for (int id : mGroundsIds) {
                    Ground *g = mGame->GetGroundById(id);
                    DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
                    if (dynamicGround) {
                        dynamicGround->SetIsDecreasing(true);
                    }
                }
                SetState(ActorState::Destroy);
            }
            break;
        default:
            break;
    }
}

void Trigger::GroundTrigger() {
    std::vector<Ground *> grounds = mGame->GetGrounds();
    switch (mEvent) {
        case Event::setIsMoving:
            for (int id : mGroundsIds) {
                Ground *g = mGame->GetGroundById(id);
                g->SetIsMoving(true);
            }
        break;
    }
}

void Trigger::GameTrigger() {
    switch (mEvent) {
        case Event::changeScene:
            mGame->GetAudio()->StopAllSounds();
            mGame->SetGameScene(mScene, 2.0f);
            SetState(ActorState::Destroy);
        break;
        default:
        break;
    }
}

void Trigger::EnemyTrigger() {
    std::vector<Enemy *> enemies = mGame->GetEnemies();
    switch (mEvent) {
        case Event::spotPlayer:
            for (int id : mEnemiesIds) {
                Enemy *e = mGame->GetEnemyById(id);
                e->SetSpottedPlayer(true);
            }
            SetState(ActorState::Destroy);
        break;
        default:
        break;
    }
}



void Trigger::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mFixedCameraPosition.x = mFixedCameraPosition.x / oldScale * newScale;
    mFixedCameraPosition.y = mFixedCameraPosition.y / oldScale * newScale;

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
