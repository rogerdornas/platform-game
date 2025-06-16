//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once

#include "DrawComponent.h"

class DrawPolygonComponent : public DrawComponent
{
public:
    // (Lower draw order corresponds with further back)
    DrawPolygonComponent(Actor* owner, std::vector<Vector2>& vertices, SDL_Color color = {255, 255, 255, 255},
                         int drawOrder = 100);

    void Draw(SDL_Renderer* renderer) override;

    void SetVertices(std::vector<Vector2> vertices) { mVertices = vertices; }
    std::vector<Vector2>& GetVertices() { return mVertices; }

protected:
    int mDrawOrder;
    std::vector<Vector2> mVertices;
    SDL_Color mColor;
};
