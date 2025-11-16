//
// Created by roger on 03/10/2025.
//

#pragma once
#include <string>
#include "Actor.h"

class Decorations : public  Actor
{
public:
    Decorations(Game *game, float width, float height, std::string imagePath);

    void OnUpdate(float deltaTime) override;
    void ChangeResolution(float oldScale, float newScale) override;

private:
    float mWidth;
    float mHeight;
    std::string mImagePath;

    class RectComponent* mRectComponent;
    class AnimatorComponent* mDrawComponent;
};

