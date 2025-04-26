//
// Created by Lucas N. Ferreira on 03/08/23.
//

#pragma once
#include "Component.h"
#include "../Math.h"
#include <vector>
#include <SDL.h>

class DrawComponent : public Component
{
public:
    // (Lower draw order corresponds with further back)
    DrawComponent(class Actor* owner, std::vector<Vector2> &vertices, int drawOrder = 100);
    ~DrawComponent();

    virtual void Draw(SDL_Renderer* renderer);

    int GetDrawOrder() const { return mDrawOrder; }
    std::vector<Vector2>& GetVertices() { return mVertices; }

    void DrawPolygon(SDL_Renderer *renderer, std::vector<Vector2>& vertices);
    void DrawCircle(SDL_Renderer *renderer, const Vector2& center = Vector2::Zero, float radius = 10.0f, int numVertices = 30);

    void DrawAABB(SDL_Renderer *renderer, const Vector2& min, const Vector2& max);

protected:
    int mDrawOrder;
    std::vector<Vector2> mVertices;
};
