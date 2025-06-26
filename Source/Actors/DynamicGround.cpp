//
// Created by roger on 25/05/2025.
//

#include "DynamicGround.h"
#include <unordered_map>
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawDynamicGroundSpritesComponent.h"

DynamicGround::DynamicGround(Game* game, float width, float height, bool isSpike, bool isMoving,
                             float movingDuration, Vector2 velocity)
    :Ground(game, width, height, isSpike, isMoving, movingDuration, velocity)
    ,mMaxWidth(width)
    ,mMaxHeight(height)
    ,mGrowSpeed(Vector2::Zero)
    ,mIsGrowing(false)
    ,mIsDecreasing(false)
    ,mIsOscillating(false)
    ,mGrowthDirection(GrowthDirection::Centered)
    ,mDrawDynamicGroundSpritesComponent(nullptr)
{
    mDrawDynamicGroundSpritesComponent = new DrawDynamicGroundSpritesComponent(this, mGame->GetTileSize(), mGame->GetTileSize());
}

void DynamicGround::OnUpdate(float deltaTime) {
    if ((mWidth == 0 || mHeight == 0)) {
        if (mDrawPolygonComponent) {
            mDrawPolygonComponent->SetIsVisible(false);
        }
        if (mAABBComponent) {
            mAABBComponent->SetActive(false);
        }
    }
    else {
        if (mDrawPolygonComponent) {
            mDrawPolygonComponent->SetIsVisible(true);
        }
        if (mAABBComponent) {
            mAABBComponent->SetActive(true);
        }
    }
    if (mIsMoving) {
        mMovingTimer += deltaTime;
        if (mMovingTimer > mMovingDuration) {
            mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() * -1);
            mMovingTimer = 0;
        }
    }
    if (mIsOscillating) {
        if (mWidth >= mMaxWidth && mHeight >= mMaxHeight) {
            mIsGrowing = false;
            mIsDecreasing = true;
        }
        if (mWidth <= mMinWidth && mHeight <= mMinHeight) {
            mIsDecreasing = false;
            mIsGrowing = true;
        }
    }
    if (mIsGrowing || mIsDecreasing) {
        float growX = 0;
        float growY = 0;
        if (mIsGrowing) {
            growX = mGrowSpeed.x * deltaTime;
            growY = mGrowSpeed.y * deltaTime;
            if (mWidth + growX > mMaxWidth) {
                growX = mMaxWidth - mWidth;
            }
            if (mHeight + growY > mMaxHeight) {
                growY = mMaxHeight - mHeight;
            }
        }
        if (mIsDecreasing) {
            growX = -mGrowSpeed.x * deltaTime;
            growY = -mGrowSpeed.y * deltaTime;
            if (mWidth + growX < mMinWidth) {
                growX = mMinWidth - mWidth;
            }
            if (mHeight + growY < mMinHeight) {
                growY = mMinHeight - mHeight;
            }
        }

        mWidth += growX;
        mHeight += growY;

        if (mWidth >= mMaxWidth && mHeight >= mMaxHeight) {
            mIsGrowing = false;
        }
        if (mWidth <= mMinWidth && mHeight <= mMinHeight) {
            mIsDecreasing = false;
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

        switch (mGrowthDirection) {
            case GrowthDirection::Right:
                SetPosition(Vector2(GetPosition().x + growX / 2.0f, GetPosition().y));
                break;
            case GrowthDirection::Left:
                SetPosition(Vector2(GetPosition().x - growX / 2.0f, GetPosition().y));
                break;
            case GrowthDirection::Up:
                SetPosition(Vector2(GetPosition().x, GetPosition().y - growY / 2.0f));
                break;
            case GrowthDirection::Down:
                SetPosition(Vector2(GetPosition().x, GetPosition().y + growY / 2.0f));
                break;
            default:
                break;
        }
    }
}

void DynamicGround::SetSprites() {
    int rows = mMaxHeight / mGame->GetTileSize();
    int cols = mMaxWidth / mGame->GetTileSize();

    int topLeftX = mStartingPosition.x - mMaxWidth / 2;
    int topLeftY = mStartingPosition.y - mMaxHeight / 2;

    int minRow = topLeftY / mGame->GetTileSize();
    int maxRow = minRow + rows;

    int minCol = topLeftX / mGame->GetTileSize();
    int maxCol = minCol + cols;

    std::unordered_map<int, std::vector<Vector2> > spriteOffsetMap;

    int** levelData = mGame->GetLevelDataDynamicGrounds();

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
    mDrawDynamicGroundSpritesComponent->SetSpriteOffsetMap(spriteOffsetMap);
    mDrawDynamicGroundSpritesComponent->SetWidth(mGame->GetTileSize());
    mDrawDynamicGroundSpritesComponent->SetHeight(mGame->GetTileSize());
}


void DynamicGround::ChangeResolution(float oldScale, float newScale) {
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

    mMaxWidth = mMaxWidth / oldScale * newScale;
    mMaxHeight = mMaxHeight / oldScale * newScale;
    mMinWidth = mMinWidth / oldScale * newScale;
    mMinHeight = mMinHeight / oldScale * newScale;
    mGrowSpeed.x = mGrowSpeed.x / oldScale * newScale;
    mGrowSpeed.y = mGrowSpeed.y / oldScale * newScale;

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
