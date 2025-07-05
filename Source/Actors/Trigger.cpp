//
// Created by roger on 23/05/2025.
//

#include "Trigger.h"
#include "../Camera.h"
#include "../DialogueSystem.h"
#include "../Game.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Actors/Ground.h"
#include "../Actors/DynamicGround.h"

Trigger::Trigger(class Game *game, float width, float height)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mTarget(Target::Nothing)
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
        mTarget = Target::Camera;
        return;
    }
    if (target == "DynamicGround") {
        mTarget = Target::DynamicGround;
        return;
    }
    if (target == "Ground") {
        mTarget = Target::Ground;
        return;
    }
    if (target == "Game") {
        mTarget = Target::Game;
        return;
    }
    if (target == "Enemy") {
        mTarget = Target::Enemy;
        return;
    }
    if (target == "Dialogue") {
        mTarget = Target::Dialogue;
        return;
    }
    if (target == "Cutscene") {
        mTarget = Target::Cutscene;
        return;
    }
}

void Trigger::SetEvent(std::string event) {
    if (event == "Fixed") {
        mEvent = Event::Fixed;
        return;
    }
    if (event == "FollowPlayer") {
        mEvent = Event::FollowPlayer;
        return;
    }
    if (event == "FollowPlayerHorizontally") {
        mEvent = Event::FollowPlayerHorizontally;
        return;
    }
    if (event == "FollowPlayerLimitRight") {
        mEvent = Event::FollowPlayerLimitRight;
        return;
    }
    if (event == "FollowPlayerLimitLeft") {
        mEvent = Event::FollowPlayerLimitLeft;
        return;
    }
    if (event == "FollowPlayerLimitRightHorizontally") {
        mEvent = Event::FollowPlayerLimitRightHorizontally;
        return;
    }
    if (event == "ScrollRight") {
        mEvent = Event::ScrollRight;
        return;
    }
    if (event == "ScrollUp") {
        mEvent = Event::ScrollUp;
        return;
    }

    if (event == "SetIsGrowing") {
        mEvent = Event::SetIsGrowing;
        return;
    }
    if (event == "SetIsDecreasing") {
        mEvent = Event::SetIsDecreasing;
        return;
    }
    if (event == "SetIsDecreasingAfterKillingEnemies") {
        mEvent = Event::SetIsDecreasingAfterKillingEnemies;
        return;
    }

    if (event == "SetIsMoving") {
        mEvent = Event::SetIsMoving;
        return;
    }

    if (event == "ChangeScene") {
        mEvent = Event::ChangeScene;
        return;
    }

    if (event == "SpotPlayer") {
        mEvent = Event::SpotPlayer;
        return;
    }
    if (event == "GolemVulnerable") {
        mEvent = Event::GolemVulnerable;
        return;
    }

    if (event == "StartDialogue") {
        mEvent = Event::StartDialogue;
        return;
    }

    if (event == "StartCutscene") {
        mEvent = Event::StarCutscene;
        return;
    }
}

void Trigger::SetScene(std::string scene) {
    if (scene == "LevelTeste") {
        mScene = Game::GameScene::LevelTeste;
        return;
    }
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
    if (scene == "Level5") {
        mScene = Game::GameScene::Level5;
        return;
    }
}



void Trigger::OnUpdate(float deltaTime) {
    Player* player = mGame->GetPlayer();
    if (mAABBComponent->Intersect(*player->GetComponent<AABBComponent>())) {
        switch (mTarget) {
            case Target::Camera:
                CameraTrigger();
                break;

            case Target::DynamicGround:
                DynamicGroundTrigger();
                break;

            case Target::Ground:
                GroundTrigger();
                break;

            case Target::Game:
                GameTrigger();
                break;

            case Target::Enemy:
                EnemyTrigger();
                break;

            case Target::Dialogue:
                DialogueTrigger();
                break;

            case Target::Cutscene:
                CutsceneTrigger();
                break;

            default:
                break;
        }
    }
}

void Trigger::CameraTrigger() {
    Camera* camera = mGame->GetCamera();
    switch (mEvent) {
        case Event::Fixed:
            camera->SetFixedCameraPosition(mFixedCameraPosition);
            camera->ChangeCameraMode(CameraMode::Fixed);
            SetState(ActorState::Destroy);
            break;

        case Event::FollowPlayer:
            camera->ChangeCameraMode(CameraMode::FollowPlayer);
            break;

        case Event::FollowPlayerHorizontally:
            camera->SetFixedCameraPosition(mFixedCameraPosition);
            camera->ChangeCameraMode(CameraMode::FollowPlayerHorizontally);
            break;

        case Event::FollowPlayerLimitRight:
            camera->SetFixedCameraPosition(mFixedCameraPosition);
            camera->ChangeCameraMode(CameraMode::FollowPlayerLimitRight);
            break;

        case Event::FollowPlayerLimitLeft:
            camera->SetFixedCameraPosition(mFixedCameraPosition);
            camera->ChangeCameraMode(CameraMode::FollowPlayerLimitLeft);
            break;

        case Event::FollowPlayerLimitRightHorizontally:
            camera->SetFixedCameraPosition(mFixedCameraPosition);
            camera->ChangeCameraMode(CameraMode::FollowPlayerLimitRightHorizontally);
            break;

        case Event::ScrollRight:
            camera->ChangeCameraMode(CameraMode::ScrollRight);
            SetState(ActorState::Destroy);
            break;

        case Event::ScrollUp:
            camera->ChangeCameraMode(CameraMode::ScrollUp);
            SetState(ActorState::Destroy);
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
        case Event::SetIsGrowing:
            for (int id : mGroundsIds) {
                Ground *g = mGame->GetGroundById(id);
                DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
                if (dynamicGround) {
                    dynamicGround->SetIsGrowing(true);
                }
            }
            SetState(ActorState::Destroy);
            break;

        case Event::SetIsDecreasing:
            for (int id : mGroundsIds) {
                Ground *g = mGame->GetGroundById(id);
                DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
                if (dynamicGround) {
                    dynamicGround->SetIsDecreasing(true);
                }
            }
            SetState(ActorState::Destroy);
            break;

        case Event::SetIsDecreasingAfterKillingEnemies:
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
        case Event::SetIsMoving:
            for (int id : mGroundsIds) {
                Ground *g = mGame->GetGroundById(id);
                if (g) {
                    g->SetIsMoving(true);
                }
            }
            SetState(ActorState::Destroy);
            break;
        default:
            break;
    }
}

void Trigger::GameTrigger() {
    switch (mEvent) {
        case Event::ChangeScene:
            mGame->GetAudio()->StopAllSounds();
            mGame->SetGameScene(mScene, 2.0f);
            mGame->SetGoingToNextLevel();
            SetState(ActorState::Destroy);
            break;

        default:
            break;
    }
}

void Trigger::EnemyTrigger() {
    std::vector<Enemy *> enemies = mGame->GetEnemies();
    switch (mEvent) {
        case Event::SpotPlayer:
            for (int id : mEnemiesIds) {
                Enemy *e = mGame->GetEnemyById(id);
                if (e) {
                    e->SetSpottedPlayer(true);
                }
            }
            SetState(ActorState::Destroy);
            break;

        default:
            break;
    }
}

void Trigger::DialogueTrigger() {
    auto* dialogue = new DialogueSystem(mGame, "../Assets/Fonts/K2D-Bold.ttf", mDialoguePath);
    SetState(ActorState::Destroy);
}

void Trigger::CutsceneTrigger() {
    auto* cutscene = new Cutscene(mGame, mCutsceneId, "../Assets/Cutscenes/Cutscenes.json");
    cutscene->Start();
    mGame->SetCurrentCutscene(cutscene);
    mGame->SetGamePlayState(Game::GamePlayState::Cutscene);
    SetState(ActorState::Destroy);
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
