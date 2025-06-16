//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once

#include "DrawComponent.h"
#include <unordered_map>

class DrawGroundSpritesComponent : public DrawComponent
{
public:
    // (Lower draw order corresponds with further back)
    DrawGroundSpritesComponent(Actor* owner, int width = 32, int height = 32, int drawOrder = 100);

    void Draw(SDL_Renderer* renderer) override;
    void SetSpriteOffsetMap(std::unordered_map<int, std::vector<Vector2> > spriteOffsetMap) {mSpriteOffsetMap = spriteOffsetMap; }
    void SetWidth(float width) { mWidth = width; }
    void SetHeight(float height) { mHeight = height; }

protected:
    std::unordered_map<int, std::vector<Vector2> > mSpriteOffsetMap;

    int mWidth;
    int mHeight;
};
