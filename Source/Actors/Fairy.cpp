//
// Created by roger on 11/06/2025.
//

#include "Fairy.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"


Fairy::Fairy(Game *game, float width, float height)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mDrawPolygonComponent(nullptr)
    ,mDrawSpriteComponent(nullptr)
    ,mDrawAnimatedComponent(nullptr)
{
    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {255, 255, 255, 255});
    // mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Fairy/Fairy1.png",
    //                                                 static_cast<int>(mWidth * 1.8),
    //                                                 static_cast<int>(mHeight * 1.8));
    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 1.8, mHeight * 1.8, "../Assets/Sprites/Fairy/Fairy.png", "../Assets/Sprites/Fairy/Fairy.json");
    std::vector<int> idle = {0, 1, 2, 3};
    mDrawAnimatedComponent->AddAnimation("idle", idle);

    mDrawAnimatedComponent->SetAnimation("idle");
    mDrawAnimatedComponent->SetAnimFPS(8.0f);
}

void Fairy::OnUpdate(float deltaTime) {

}

void Fairy::ChangeResolution(float oldScale, float newScale) {

}
