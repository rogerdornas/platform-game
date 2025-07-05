//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "DrawAnimatedComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Json.h"
#include <fstream>

DrawAnimatedComponent::DrawAnimatedComponent(Actor* owner, float width, float height,
                                             const std::string &spriteSheetPath, const std::string &spriteSheetData,
                                             int drawOrder)
    :DrawSpriteComponent(owner, spriteSheetPath, width, height, drawOrder)
    ,mTransparency(255)
    ,mUseFlip(false)
    ,mFlip(SDL_FLIP_NONE)
    ,mUseRotation(false)
    ,mOffsetRotation(0.0f)
{
    LoadSpriteSheet(spriteSheetPath, spriteSheetData);
}

DrawAnimatedComponent::~DrawAnimatedComponent()
{
    for (const auto &rect: mSpriteSheetData) {
        delete rect;
    }
    mSpriteSheetData.clear();
}

void DrawAnimatedComponent::LoadSpriteSheet(const std::string &texturePath, const std::string &dataPath)
{
    // Load sprite sheet texture
    mSpriteSheetSurface = mOwner->GetGame()->LoadTexture(texturePath);

    // Load sprite sheet data
    std::ifstream spriteSheetFile(dataPath);
    nlohmann::json spriteSheetData = nlohmann::json::parse(spriteSheetFile);

    SDL_Rect* rect = nullptr;
    for (const auto &frame: spriteSheetData["frames"]) {
        int x = frame["frame"]["x"].get<int>();
        int y = frame["frame"]["y"].get<int>();
        int w = frame["frame"]["w"].get<int>();
        int h = frame["frame"]["h"].get<int>();
        rect = new SDL_Rect({x, y, w, h});

        mSpriteSheetData.emplace_back(rect);
    }
}

void DrawAnimatedComponent::Draw(SDL_Renderer* renderer)
{
    if (!mIsVisible) {
        return;
    }

    int spriteIdx = mAnimations[mAnimName][static_cast<int>(mAnimTimer)];

    SDL_Rect* srcRect = mSpriteSheetData[spriteIdx];

    // Calcula a posição na tela
    SDL_Rect dstRect;
    dstRect.h = mHeight;
    dstRect.w = mWidth;
    dstRect.x = mOwner->GetPosition().x - mWidth / 2 - GetGame()->GetCamera()->GetPosCamera().x;
    dstRect.y = mOwner->GetPosition().y - mHeight / 2 - GetGame()->GetCamera()->GetPosCamera().y;

    if (!mUseFlip) {
        mFlip = SDL_FLIP_NONE;
    }



    float angle = 0;
    if (mUseRotation) {
        angle = Math::ToDegrees(GetOwner()->GetRotation()) + mOffsetRotation;
    }
    else if (!mUseFlip) {
        // Define o flip (espelhamento) baseado na escala
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        if (GetOwner()->GetRotation() == Math::Pi) {
            flip = SDL_FLIP_HORIZONTAL;
        }
        if (mFlip == SDL_FLIP_NONE) {
            mFlip = flip;
        }
    }

    // if (mFlip == SDL_FLIP_NONE) {
    //     mFlip = flip;
    // }

    SDL_SetTextureBlendMode(mSpriteSheetSurface, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(mSpriteSheetSurface, mTransparency);
    SDL_RenderCopyEx(renderer, mSpriteSheetSurface, srcRect, &dstRect, angle, nullptr, mFlip);
}

void DrawAnimatedComponent::Update(float deltaTime) {
    if (mIsPaused) {
        return;
    }

    mAnimTimer += deltaTime * mAnimFPS;

    int numFrames = mAnimations[mAnimName].size();
    while (mAnimTimer >= numFrames) {
        mAnimTimer -= numFrames;
    }
}

void DrawAnimatedComponent::SetAnimation(const std::string &name)
{
    mAnimName = name;
    Update(0);
}

void DrawAnimatedComponent::AddAnimation(const std::string &name, const std::vector<int> &spriteNums)
{
    mAnimations.emplace(name, spriteNums);
}
