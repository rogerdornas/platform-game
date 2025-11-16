//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "AnimatorComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Json.h"
#include "../../Renderer/Texture.h"
#include <fstream>

AnimatorComponent::AnimatorComponent(class Actor* owner, const std::string &texPath, const std::string &dataPath,
                                     int width, int height, int drawOrder)
        :DrawComponent(owner,  drawOrder)
        ,mAnimTimer(0.0f)
        ,mIsPaused(false)
        ,mWidth(width)
        ,mHeight(height)
        ,mTextureFactor(1.0f)
{
    // Load texture
    mSpriteTexture = GetGame()->GetRenderer()->GetTexture(texPath);

    // Load sprite sheet data
    LoadSpriteSheetData(dataPath);
}

AnimatorComponent::~AnimatorComponent()
{
    mAnimations.clear();
    mSpriteSheetData.clear();
    // if (mSpriteTexture) {
    //     mSpriteTexture->Unload();
    //     delete mSpriteTexture;
    //     mSpriteTexture = nullptr;
    // }
}

bool AnimatorComponent::LoadSpriteSheetData(const std::string& dataPath)
{
    // Load sprite sheet data and return false if it fails
    std::ifstream spriteSheetFile(dataPath);

    if (!spriteSheetFile.is_open()) {
        SDL_Log("Failed to open sprite sheet data file: %s", dataPath.c_str());
        return false;
    }

    nlohmann::json spriteSheetData = nlohmann::json::parse(spriteSheetFile);

    if (spriteSheetData.is_null()) {
        SDL_Log("Failed to parse sprite sheet data file: %s", dataPath.c_str());
        return false;
    }

    auto textureWidth = static_cast<float>(spriteSheetData["meta"]["size"]["w"].get<int>());
    auto textureHeight = static_cast<float>(spriteSheetData["meta"]["size"]["h"].get<int>());

    for(const auto& frame : spriteSheetData["frames"]) {

        int x = frame["frame"]["x"].get<int>();
        int y = frame["frame"]["y"].get<int>();
        int w = frame["frame"]["w"].get<int>();
        int h = frame["frame"]["h"].get<int>();

        mSpriteSheetData.emplace_back(static_cast<float>(x)/textureWidth, static_cast<float>(y)/textureHeight,
                                      static_cast<float>(w)/textureWidth, static_cast<float>(h)/textureHeight);
    }

    return true;
}

void AnimatorComponent::Draw(Renderer* renderer)
{
    if (mIsVisible)
    {
        // Init with default full texture rect
        Vector4 texRect = Vector4::UnitRect;

        // If we have sprite sheet data, get the current sprite rect
        if (mAnimations.find(mAnimName) != mAnimations.end())
        {
            int spriteIdx = mAnimations[mAnimName][static_cast<int>(mAnimTimer)];
            texRect = mSpriteSheetData[spriteIdx];
        }

        renderer->DrawTexture(mOwner->GetPosition(), Vector2(mWidth, mHeight), mOwner->GetTransformRotation(),
                              mColor, mSpriteTexture, texRect, GetGame()->GetCamera()->GetPosCamera(), mOwner->GetScale(), mTextureFactor, mAlpha);
    }
}

void AnimatorComponent::Update(float deltaTime)
{
    if (mIsPaused || mAnimations.find(mAnimName) == mAnimations.end()) {
        return;
    }

    mAnimTimer += mAnimFPS * deltaTime;
    if (mAnimTimer >= mAnimations[mAnimName].size()) {
        while (mAnimTimer >= mAnimations[mAnimName].size()) {
            mAnimTimer -= mAnimations[mAnimName].size();
        }
    }
}

void AnimatorComponent::SetAnimation(const std::string& name)
{
    if (mAnimName != name) {
        mAnimTimer = 0;
    }
    mAnimName = name;
    Update(0.0f);
}

void AnimatorComponent::AddAnimation(const std::string& name, const std::vector<int>& spriteNums)
{
    mAnimations.emplace(name, spriteNums);
}