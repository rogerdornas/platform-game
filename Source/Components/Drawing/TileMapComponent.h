//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once

#include "DrawComponent.h"
#include <unordered_map>

enum class GrowthDirection;

class TileMapComponent : public DrawComponent
{
public:
    // (Lower draw order corresponds with further back)
    TileMapComponent(Actor* owner, int width = 32, int height = 32, int drawOrder = 99);
    ~TileMapComponent();

    void SetTilesIndex(const std::vector<std::vector<int>>& tiles);
    void Draw(class Renderer* renderer) override;

    // === Controle de recorte dinâmico ===
    void SetCurrentWidth(float width) { mCurrentWidth = width; }
    float GetCurrentWidth() const { return mCurrentWidth; }
    void SetCurrentHeight(float height) { mCurrentHeight = height; }
    float GetCurrentHeight() const { return mCurrentHeight; }
    void SetGrowDirection(GrowthDirection growDirection) { mGrowthDirection = growDirection; }

    // === Bake (gera textura única a partir dos tiles) ===
    void BakeTilesToTexture(Renderer* renderer);

protected:
    std::vector<std::vector<int>> mTilesIndex;
    class Texture* mBakedTexture; // textura única gerada
    float mBakedWidth;
    float mBakedHeight;

    float mCurrentWidth; // largura atual visível (recorte dinâmico)
    float mCurrentHeight;
    GrowthDirection mGrowthDirection;
    bool mIsBaked;
};
