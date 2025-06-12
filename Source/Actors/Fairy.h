//
// Created by roger on 11/06/2025.
//

#pragma once
#include "Actor.h"
#include "../AudioSystem.h"

class Fairy : public  Actor
{
public:
    Fairy(Game *game, float width, float height);

    void OnUpdate(float deltaTime) override;
    void ChangeResolution(float oldScale, float newScale) override;

private:
    float mHeight;
    float mWidth;

    class DrawPolygonComponent *mDrawPolygonComponent;
    class DrawSpriteComponent *mDrawSpriteComponent;
    class DrawAnimatedComponent *mDrawAnimatedComponent;

};
