//
// Created by roger on 22/04/2025.
//

#include "Ground.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawGroundSpritesComponent.h"
#include <unordered_map>

Ground::Ground(Game* game, float width, float height, bool isSpike, bool isMoving, float movingDuration, Vector2 velocity)
    :Actor(game)
    ,mStartingPosition(Vector2::Zero)
    ,mRespawnPosition(Vector2::Zero)
    ,mHeight(height)
    ,mWidth(width)
    ,mIsSpike(isSpike)
    ,mIsMoving(isMoving)
    ,mMovingTimer(movingDuration)
    ,mMovingDuration(movingDuration)
    ,mVelocity(velocity * mGame->GetScale())
    ,mDrawPolygonComponent(nullptr)
    ,mDrawGroundSpritesComponent(nullptr)
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

    SDL_Color color;
    if (mIsSpike)
        color = SDL_Color{255, 0, 0, 255};

    else
        color = SDL_Color{0, 255, 0, 255};

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, color);

    mRigidBodyComponent = new RigidBodyComponent(this, 1);
    mAABBComponent = new AABBComponent(this, v1, v3);

    mDrawGroundSpritesComponent = new DrawGroundSpritesComponent(this, mGame->GetTileSize(), mGame->GetTileSize());
    if (mIsMoving) {
        mRigidBodyComponent->SetVelocity(mVelocity);
    }

    mGame->AddGround(this);
}

Ground::~Ground() { mGame->RemoveGround(this); }

void Ground::OnUpdate(float deltaTime) {
    if (mIsMoving) {
        mMovingTimer += deltaTime;
        if (mMovingTimer > mMovingDuration) {
            mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() * -1);
            mMovingTimer = 0;
        }
    }
}

void Ground::SetIsMoving(bool isMoving) {
    mIsMoving = isMoving;
    if (mIsMoving == true) {
        mRigidBodyComponent->SetVelocity(mVelocity);
    }
    else {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }
}

void Ground::SetSprites() {
    int rows = mHeight / mGame->GetTileSize();
    int cols = mWidth / mGame->GetTileSize();

    int topLeftX = mStartingPosition.x - mWidth / 2;
    int topLeftY = mStartingPosition.y - mHeight / 2;

    int minRow = topLeftY / mGame->GetTileSize();
    int maxRow = minRow + rows;

    int minCol = topLeftX / mGame->GetTileSize();
    int maxCol = minCol + cols;

    std::unordered_map<int, std::vector<Vector2> > spriteOffsetMap;

    int** levelData = mGame->GetLevelData();

    for (int row = minRow; row < maxRow; ++row) {
        for (int col = minCol; col < maxCol; ++col) {
            int tile = levelData[row][col];

            int tileX = col * mGame->GetTileSize();
            int tileY = row * mGame->GetTileSize();

            Vector2 offset = Vector2(tileX, tileY) - mStartingPosition;

            if (tile >= 0) {
                spriteOffsetMap[tile].emplace_back(offset);
            }
        }
    }
    mDrawGroundSpritesComponent->SetSpriteOffsetMap(spriteOffsetMap);
    mDrawGroundSpritesComponent->SetWidth(mGame->GetTileSize());
    mDrawGroundSpritesComponent->SetHeight(mGame->GetTileSize());
}

void Ground::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    SetRespawPosition(Vector2(mRespawnPosition.x / oldScale * newScale, mRespawnPosition.y / oldScale * newScale));
    mStartingPosition.x = mStartingPosition.x / oldScale * newScale;
    mStartingPosition.y = mStartingPosition.y / oldScale * newScale;
    mVelocity.x = mVelocity.x / oldScale * newScale;
    mVelocity.y = mVelocity.y / oldScale * newScale;

    if (mIsMoving) {
        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));
    }

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    mAABBComponent->SetMin(v1);
    mAABBComponent->SetMax(v3);

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetVertices(vertices);
    }
    SetSprites();
}
