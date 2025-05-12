//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once
#include "DrawComponent.h"
#include <string>
#include <unordered_map>

class DrawGroundSpritesComponent : public DrawComponent
{
public:
    // (Lower draw order corresponds with further back)
    DrawGroundSpritesComponent(class Actor* owner, std::unordered_map<std::string, std::vector<Vector2>> sprite_offset_map, int width = 0, int height = 0, int drawOrder = 100);

    void Draw(SDL_Renderer* renderer) override;

protected:
    // Map of textures loaded
    SDL_Texture* mSpriteSheetSurface = nullptr;
    std::unordered_map<SDL_Texture*, std::vector<Vector2>> mTextureOffsetMap;

    int mWidth;
    int mHeight;
    std::unordered_map<std::string, std::vector<Vector2>> mSprite_offset_map;
};