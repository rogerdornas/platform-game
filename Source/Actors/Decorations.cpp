//
// Created by roger on 03/10/2025.
//

#include "Decorations.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"

Decorations::Decorations(Game *game, float width, float height, std::string imagePath)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mDrawComponent(nullptr)
    ,mRectComponent(nullptr)
{
    mImagePath = "../Assets/" + imagePath;

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {0, 255, 0, 255});

    mDrawComponent = new AnimatorComponent(this, mImagePath, "", mWidth, mHeight, 200);
}

void Decorations::OnUpdate(float deltaTime) {

}

void Decorations::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));

    // if (mDrawSpriteComponent) {
    //     mDrawSpriteComponent->SetWidth(mWidth);
    //     mDrawSpriteComponent->SetHeight(mHeight);
    // }

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // if (mDrawPolygonComponent) {
    //     mDrawPolygonComponent->SetVertices(vertices);
    // }
}
