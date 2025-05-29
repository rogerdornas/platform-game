//
// Created by roger on 27/05/2025.
//

#pragma once
#include "DrawComponent.h"
#include <string>
#include <unordered_map>
#include "DrawGroundSpritesComponent.h"

class DrawDynamicGroundSpritesComponent : public DrawGroundSpritesComponent
{
public:
    // (Lower draw order corresponds with further back)
    DrawDynamicGroundSpritesComponent(Actor *owner,
                               std::unordered_map<std::string, std::vector<Vector2> > sprite_offset_map, int width = 0,
                               int height = 0, int drawOrder = 100);

    void Draw(SDL_Renderer *renderer) override;

protected:
    class DynamicGround* mOwnerDynamicGround;
};
