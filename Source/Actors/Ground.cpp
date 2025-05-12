//
// Created by roger on 22/04/2025.
//

#include "Ground.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include <unordered_map>
#include <iomanip>
#include <sstream>
#include "../Components/DrawComponents/DrawGroundSpritesComponent.h"

Ground::Ground(Game *game, float width, float height, bool isSpike, bool isMoving, float movingDuration, Vector2 velocity)
    :Actor(game)
    ,mDrawPolygonComponent(nullptr)
    ,mDrawGroundSpritesComponent(nullptr)
    ,mDrawSpriteComponent(nullptr)

    ,mWidth(width)
    ,mHeight(height)
    ,mIsSpike(isSpike)
    ,mIsMoving(isMoving)
    ,mMovingDuration(movingDuration)
    ,mMovingTimer(movingDuration)
{
    Vector2 v1(-mWidth/2, -mHeight/2);
    Vector2 v2(mWidth/2, -mHeight/2);
    Vector2 v3(mWidth/2, mHeight/2);
    Vector2 v4(-mWidth/2, mHeight/2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    SDL_Color color;
    if (mIsSpike) {
        color = SDL_Color{255, 0, 0, 255};
    }
    else {
        color = SDL_Color{0, 255, 0, 255};
    }

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, color);
    // mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Blocks/BlockA.png", mWidth, mHeight);

    mRigidBodyComponent = new RigidBodyComponent(this, 1);
    mAABBComponent = new AABBComponent(this, v1, v3);

    mRigidBodyComponent->SetVelocity(velocity);

    game->AddGround(this);
}

Ground::~Ground() {
    mGame->RemoveGround(this);
}

void Ground::OnUpdate(float deltaTime) {
    if (mIsMoving) {
        mMovingTimer += deltaTime;
        if (mMovingTimer > mMovingDuration) {
            mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() * -1);
            mMovingTimer = 0;
        }
    }
}

void Ground::SetSprites() {

    int rows = mHeight / GetGame()->GetTileSize();
    int cols = mWidth / GetGame()->GetTileSize();

    int topLeftX = GetPosition().x - mWidth / 2;
    int topLeftY = GetPosition().y - mHeight / 2;

    int minRow = topLeftY / GetGame()->GetTileSize();
    int maxRow = minRow + rows;

    int minCol = topLeftX / GetGame()->GetTileSize();
    int maxCol = minCol + cols;

    std::unordered_map<std::string, std::vector<Vector2>> sprite_offset_map;

    int** levelData = GetGame()->GetLevelData();

    for (int row = minRow; row <= maxRow; ++row) {
        for (int col = minCol; col <= maxCol; ++col) {
            int tile = levelData[row][col];

            int tileX = col * GetGame()->GetTileSize();
            int tileY = row * GetGame()->GetTileSize();

            Vector2 offset = Vector2(tileX, tileY) - GetPosition();

            if (tile >= 0) {
                std::ostringstream tileName;
                tileName << std::setw(2) << std::setfill('0') << tile;
                std::string file = tileName.str();
                sprite_offset_map["../Assets/Sprites/Forest/" + file + ".png"].push_back(offset);
            }
        }
    }

    mDrawGroundSpritesComponent = new DrawGroundSpritesComponent(this, sprite_offset_map, GetGame()->GetTileSize(), GetGame()->GetTileSize());

}
