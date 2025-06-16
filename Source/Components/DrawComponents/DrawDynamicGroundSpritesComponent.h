//
// Created by roger on 27/05/2025.
//

#pragma once

#include "DrawComponent.h"
#include "DrawGroundSpritesComponent.h"

class DrawDynamicGroundSpritesComponent : public DrawGroundSpritesComponent
{
public:
    // (Lower draw order corresponds with further back)
    DrawDynamicGroundSpritesComponent(Actor* owner, int width = 0, int height = 0, int drawOrder = 101);

    void Draw(SDL_Renderer* renderer) override;

protected:
    class DynamicGround* mOwnerDynamicGround;
};
