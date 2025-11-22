//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once

#include <unordered_map>
#include "DrawComponent.h"

class AnimatorComponent : public DrawComponent {
public:
    // (Lower draw order corresponds with further back)
    AnimatorComponent(class Actor* owner, const std::string &texturePath, const std::string &dataPath,
            int width, int height, int drawOrder = 100);
    ~AnimatorComponent() override;

    void Draw(Renderer* renderer) override;
    void Update(float deltaTime) override;

    void SetWidth(float width) { mWidth = static_cast<int>(width); }
    void SetHeight(float height) { mHeight = static_cast<int>(height); }

    // Use to change the FPS of the animation
    void SetAnimFPS(float fps) { mAnimFPS = fps; }

    // Set the current active animation
    void SetAnimation(const std::string& name);

    // Use to pause/unpause the animation
    void SetIsPaused(bool pause) { mIsPaused = pause; }

    // Add an animation of the corresponding name to the animation map
    void AddAnimation(const std::string& name, const std::vector<int>& images);

    void SetTextureFactor(float textureFactor) { mTextureFactor = textureFactor; }

    void SetFreezeLevel(float freezeLevel) { mFreezeLevel = freezeLevel; }

private:
    bool LoadSpriteSheetData(const std::string& dataPath);

    // Sprite sheet texture
    class Texture* mSpriteTexture;

    // Vector of sprites
    std::vector<Vector4> mSpriteSheetData;

    // Map of animation name to vector of textures corresponding to the animation
    std::unordered_map<std::string, std::vector<int>> mAnimations;

    // Name of current animation
    std::string mAnimName;

    // Tracks current elapsed time in animation
    float mAnimTimer;

    // The frames per second the animation should run at
    float mAnimFPS;

    // Whether or not the animation is paused (defaults to false)
    bool mIsPaused;

    // Size
    int mWidth;
    int mHeight;

    float mTextureFactor;

    // Congelamento
    float mFreezeLevel;
};

