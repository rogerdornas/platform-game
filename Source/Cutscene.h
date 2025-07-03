//
// Created by roger on 01/07/2025.
//

#pragma once

#include <string>
#include <vector>
#include "Json.h"
#include "Math.h"

class Action {
public:
    Action(class Game* game, const nlohmann::json& actionJson);

    void Start();
    void Update(float deltaTime);
    bool IsComplete() const { return mIsComplete; }

private:
    Game* mGame;
    std::string mType;
    nlohmann::json mParams;

    bool mStarted;
    bool mIsComplete;

    float mDuration;
    float mTimer;

    Vector2 mCameraVelocity;
    Vector2 mPosCameraStart;

    Vector2 mMoveDirection;
    float mMoveSpeed;
    float mMoveElapsed;

    std::string mDialoguePath;
    class DialogueSystem* mDialogue;
};



class Cutscene
{
public:
    Cutscene(class Game* game, const std::string& cutsceneId, const std::string& jsonFilePath);

    void Start();
    void Update(float deltaTime);
    bool IsComplete() const { return mIsComplete; }

private:
    Game* mGame;
    std::string mId;
    std::vector<Action*> mActions;
    size_t mCurrentActionIndex;
    bool mIsComplete;

    // Utilitários internos
    nlohmann::json LoadJsonFromFile(const std::string& filePath);
    nlohmann::json FindCutsceneById(const nlohmann::json& cutscenesJson, const std::string& cutsceneId);
};
