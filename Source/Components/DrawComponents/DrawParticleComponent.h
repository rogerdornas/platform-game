//
// Created by roger on 29/05/2025.
//

#pragma once

#include <string>
#include "DrawSpriteComponent.h"

class DrawParticleComponent : public DrawSpriteComponent
{
public:
    // (Lower draw order corresponds with further back)
    DrawParticleComponent(Actor* owner, const std::string& texturePath, int width, int height,
                        SDL_Color color, int drawOrder = 5000);

    void SetColor(SDL_Color color) { mColor = color; }
    void Draw(SDL_Renderer* renderer) override;

protected:
    SDL_Color mColor;
};
