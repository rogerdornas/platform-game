//
// Created by roger on 01/07/2025.
//

#include "Cutscene.h"
#include "Game.h"
#include "Actors/Player.h"
#include "Camera.h"
#include "AudioSystem.h"
#include "DialogueSystem.h"
#include <fstream>
#include <iostream>
#include "Components/RigidBodyComponent.h"
#include "Components/DrawComponents/DrawAnimatedComponent.h"

Action::Action(Game* game, const nlohmann::json& actionJson)
    :mGame(game)
    ,mStarted(false)
    ,mIsComplete(false)
    ,mDuration(0.0f)
    ,mTimer(0.0f)

    ,mCameraVelocity(Vector2::Zero)
    ,mPosCameraStart(Vector2::Zero)

    ,mMoveDirection(Vector2::Zero)
    ,mMoveSpeed(0.0f)
    ,mMoveElapsed(0.0f)
    ,mDialogue(nullptr)
{
    mType = actionJson["type"];
    mParams = actionJson["params"];
}

void Action::Start() {
    mStarted = true;

    if (mType == "movePlayer") {
        Player* player = mGame->GetPlayer();
        std::string direction = mParams["direction"];
        mMoveSpeed = mParams["speed"];
        mMoveSpeed *= mGame->GetScale();
        mDuration = mParams["duration"];
        mMoveElapsed = 0.0f;
        mTimer = 0.0f;

        if (direction == "right") {
            player->SetRotation(0);
            // mMoveDirection = Vector2(1.0f, 0.0f);
        } else if (direction == "left") {
            player->SetRotation(Math::Pi);
        }
            // mMoveDirection = Vector2(-1.0f, 0.0f);
        // } else if (direction == "up") {
        //     mMoveDirection = Vector2(0.0f, -1.0f);
        // } else if (direction == "down") {
        //     mMoveDirection = Vector2(0.0f, 1.0f);
        // } else {
        //     mMoveDirection = Vector2::Zero;
        // }
    }

    else if (mType == "playerLookLeft") {
        mDuration = mParams["duration"];
        mTimer = 0.0f;
        mGame->GetPlayer()->SetRotation(Math::Pi);
    }

    else if (mType == "playerLookRight") {
        mDuration = mParams["duration"];
        mTimer = 0.0f;
        mGame->GetPlayer()->SetRotation(0);
    }

    else if (mType == "wait" || mType == "playerStop") {
        mDuration = mParams["duration"];
        mTimer = 0.0f;
        mGame->GetPlayer()->SetIsRunning(false);
    }

    else if (mType == "show_dialogue") {
        if (mParams.contains("dialoguePath")) {
            mDialoguePath = mParams["dialoguePath"];
            mDialogue = new DialogueSystem(mGame, "../Assets/Fonts/K2D-Bold.ttf", mDialoguePath);
        }
    }

    else if (mType == "panCamera") {
        mDuration = mParams["duration"];
        mTimer = 0.0f;
        mCameraVelocity.x = mParams["speedX"];
        mCameraVelocity.x *= mGame->GetScale();
        mCameraVelocity.y = mParams["speedY"];
        mCameraVelocity.y *= mGame->GetScale();
        mPosCameraStart.x = mParams["posStartX"];
        mPosCameraStart.x *= mGame->GetScale();
        mPosCameraStart.y = mParams["posStartY"];
        mPosCameraStart.y *= mGame->GetScale();
        mGame->GetCamera()->ChangeCameraMode(CameraMode::PanoramicCamera);
        mGame->GetCamera()->SetCameraVelocity(mCameraVelocity);
        mGame->GetCamera()->SetPosition(mPosCameraStart);
    }
}

void Action::Update(float deltaTime) {
    if (!mStarted) {
        Start();
    }

    if (mIsComplete) {
        return;
    }

    if (mType == "movePlayer") {
        if (mTimer < mDuration) {
            mGame->GetPlayer()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(mGame->GetPlayer()->GetForward().x * mMoveSpeed,
                                                                                mGame->GetPlayer()->GetComponent<RigidBodyComponent>()->GetVelocity().y));
            if (mGame->GetPlayer()->GetIsOnGround()) {
                mGame->GetPlayer()->SetIsRunning(true);
            }
            mTimer += deltaTime;
        } else {
            mIsComplete = true;
        }
    }

    else if (mType == "wait" || mType == "playerStop") {
        mGame->GetPlayer()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(0, mGame->GetPlayer()->GetComponent<RigidBodyComponent>()->GetVelocity().y));
        mTimer += deltaTime;
        if (mTimer >= mDuration) {
            mIsComplete = true;
        }
    }

    else if (mType == "playerLookLeft" || mType == "playerLookRight") {
        mGame->GetPlayer()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(0, mGame->GetPlayer()->GetComponent<RigidBodyComponent>()->GetVelocity().y));
        mTimer += deltaTime;
        if (mTimer >= mDuration) {
            mIsComplete = true;
        }
    }

    else if (mType == "show_dialogue") {
        if (mDialogue->GetState() == UIScreen::UIState::Closing) {
            mIsComplete = true;
        }
    }

    else if (mType == "panCamera") {
        mTimer += deltaTime;
        if (mTimer >= mDuration) {
            mIsComplete = true;
        }
    }
}




Cutscene::Cutscene(Game* game, const std::string& cutsceneId, const std::string& jsonFilePath)
    :mGame(game)
    ,mCurrentActionIndex(0)
    ,mIsComplete(false)
{
    nlohmann::json cutscenesJson = LoadJsonFromFile(jsonFilePath);
    nlohmann::json cutsceneJson = FindCutsceneById(cutscenesJson, cutsceneId);

    if (cutsceneJson.is_null()) {
        std::cerr << "Cutscene with id '" << cutsceneId << "' not found. Cutscene will be marked complete.\n";
        mIsComplete = true;
        return;
    }

    mId = cutsceneJson["id"];

    const auto& actionsArray = cutsceneJson["actions"];
    for (const auto& actionJson : actionsArray) {
        auto* action = new Action(mGame, actionJson);
        mActions.emplace_back(action);
    }
}

void Cutscene::Start() {
    if (!mActions.empty()) {
        mActions[mCurrentActionIndex]->Start();
    } else {
        mIsComplete = true;
    }
}

void Cutscene::Update(float deltaTime) {
    if (mIsComplete) {
        return;
    }

    if (mCurrentActionIndex >= mActions.size()) {
        mIsComplete = true;
        return;
    }

    auto& currentAction = mActions[mCurrentActionIndex];
    currentAction->Update(deltaTime);

    if (currentAction->IsComplete()) {
        ++mCurrentActionIndex;
        if (mCurrentActionIndex < mActions.size()) {
            mActions[mCurrentActionIndex]->Start();
        } else {
            mIsComplete = true;
        }
    }
}

nlohmann::json Cutscene::LoadJsonFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open JSON file: " << filePath << std::endl;
        return nlohmann::json();
    }

    nlohmann::json jsonData;
    file >> jsonData;
    return jsonData;
}

nlohmann::json Cutscene::FindCutsceneById(const nlohmann::json& cutscenesJson, const std::string& cutsceneId) {
    if (!cutscenesJson.contains("cutscenes")) {
        std::cerr << "JSON does not contain 'cutscenes' array.\n";
        return nlohmann::json();
    }

    const auto& cutsceneArray = cutscenesJson["cutscenes"];
    for (const auto& cutscene : cutsceneArray) {
        if (cutscene["id"] == cutsceneId) {
            return cutscene;
        }
    }

    std::cerr << "Cutscene with id '" << cutsceneId << "' not found in JSON.\n";
    return nlohmann::json();
}
