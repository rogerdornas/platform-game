//
// Created by roger on 25/05/2025.
//

#include "DynamicGround.h"

#include <unordered_map>
#include <iomanip>
#include <sstream>
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawDynamicGroundSpritesComponent.h"

DynamicGround::DynamicGround(Game *game, float width, float height, bool isSpike, bool isMoving, float movingDuration,
               Vector2 velocity)
    : Ground(game, width, height, isSpike, isMoving, movingDuration, velocity)
    ,mMaxWidth(width)
    ,mMaxHeight(height)
    ,mGrowSpeed(Vector2::Zero)
    ,mIsGrowing(false)
    ,mIsDecreasing(false)
    ,mIsOscillating(false)
    ,mGrowthDirection(GrowthDirection::Centered)
    ,mDrawDynamicGroundSpritesComponent(nullptr)
    ,mDrawSpriteComponent(nullptr)
{
    // mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/iron beam2.png",
    //                                             static_cast<int>(mWidth),
    //                                             static_cast<int>(mHeight));
}

DynamicGround::~DynamicGround() { mGame->RemoveGround(this); }

void DynamicGround::OnUpdate(float deltaTime)
{
    if ((mWidth == 0 || mHeight == 0) && !mIsGrowing && !mIsDecreasing) {
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
        if (mWidth <= 0 || mHeight <= 0) {
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
            if (mWidth + growX >= mMaxWidth) {
                growX = mMaxWidth - mWidth;
            }
            if (mHeight + growY > mMaxHeight) {
                growY = mMaxHeight - mHeight;
            }
        }
        if (mIsDecreasing) {
            growX = -mGrowSpeed.x * deltaTime;
            growY = -mGrowSpeed.y * deltaTime;
            if (mWidth + growX <= 0) {
                growX = -mWidth;
            }
            if (mHeight + growY <= 0) {
                growY = -mHeight;
            }
        }

        mWidth += growX;
        mHeight += growY;
        if (mDrawSpriteComponent) {
            mDrawSpriteComponent->SetWidth(mWidth);
            mDrawSpriteComponent->SetHeight(mHeight);
        }

        if (mWidth >= mMaxWidth && mHeight >= mMaxHeight) {
            mIsGrowing = false;
        }
        if (mWidth <= 0 || mHeight <= 0) {
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
    if (mDrawDynamicGroundSpritesComponent) {
        RemoveComponent(mDrawDynamicGroundSpritesComponent);
        delete mDrawDynamicGroundSpritesComponent;
    }

    int rows = mMaxHeight / mGame->GetTileSize();
    int cols = mMaxWidth / mGame->GetTileSize();

    int topLeftX = mStartingPosition.x - mMaxWidth / 2;
    int topLeftY = mStartingPosition.y - mMaxHeight / 2;

    int minRow = topLeftY / mGame->GetTileSize();
    int maxRow = minRow + rows;

    int minCol = topLeftX / mGame->GetTileSize();
    int maxCol = minCol + cols;

    std::unordered_map<std::string, std::vector<Vector2> > sprite_offset_map;

    int **levelData = mGame->GetLevelData();

    for (int row = minRow; row < maxRow; ++row)
    {
        for (int col = minCol; col < maxCol; ++col)
        {
            int tile = levelData[row][col];

            int tileX = col * mGame->GetTileSize();
            int tileY = row * mGame->GetTileSize();

            Vector2 offset = Vector2(tileX, tileY) - mStartingPosition;

            if (tile >= 0)
            {
                std::ostringstream tileName;
                tileName << std::setw(2) << std::setfill('0') << tile;
                std::string file = tileName.str();
                sprite_offset_map["../Assets/Sprites/Forest/" + file + ".png"].push_back(offset);
            }
        }
    }

    mDrawDynamicGroundSpritesComponent = new DrawDynamicGroundSpritesComponent(this, sprite_offset_map, mGame->GetTileSize(),
                                                                 mGame->GetTileSize());
}


void DynamicGround::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    SetRespawPosition(Vector2(mRespawnPosition.x / oldScale * newScale, mRespawnPosition.y / oldScale * newScale));
    mStartingPosition.x = mStartingPosition.x / oldScale * newScale;
    mStartingPosition.y = mStartingPosition.y / oldScale * newScale;

    mMaxWidth = mMaxWidth / oldScale * newScale;
    mMaxHeight = mMaxHeight / oldScale * newScale;
    mGrowSpeed.x = mGrowSpeed.x / oldScale * newScale;
    mGrowSpeed.y = mGrowSpeed.y / oldScale * newScale;

    if (mDrawSpriteComponent) {
        mDrawSpriteComponent->SetWidth(mWidth);
        mDrawSpriteComponent->SetHeight(mHeight);
    }

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

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

    if (mDrawPolygonComponent)
        mDrawPolygonComponent->SetVertices(vertices);

    SetSprites();
}
