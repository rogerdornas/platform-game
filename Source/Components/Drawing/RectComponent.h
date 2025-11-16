//
// Created by Lucas N. Ferreira on 09/10/25.
//

#pragma once

#include "DrawComponent.h"
#include "../../Renderer/VertexArray.h"

class RectComponent : public DrawComponent
{
public:
    RectComponent(class Actor* owner, int width, int height, RendererMode mode, int drawOrder = 100000);
    ~RectComponent();

    void SetWidth(float width) { mWidth = static_cast<int>(width); }
    void SetHeight(float height) { mHeight = static_cast<int>(height); }

    void Draw(class Renderer* renderer) override;

private:
    int mWidth;
    int mHeight;
    RendererMode mMode;
};
