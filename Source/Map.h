//
// Created by roger on 18/08/2025.
//

#pragma once
#include <SDL.h>

class Map
{
public:
    enum TileType {
        Empty,
        Blocked,
        Nothing,
        Spike
    };
    Map(class Game* game, int** mapData, int width, int height);
    ~Map();

    SDL_Color GetTileColor(int tile);

    void Update(float deltaTime);

    void CreateMap(SDL_Renderer* renderer);
    void Draw(SDL_Renderer* renderer);
    void ChangeResolution(float oldScale, float newScale);


private:
    class Game *mGame;
    int** mCompleteMap;
    bool** mVisibleMap;
    int mMapWidth;
    int mMapHeight;
    int mTileSize;
    int mRadius;
    float mPlayerAmplitude;
    float mFrequency;
    SDL_Texture* mMapTexture;
    SDL_Texture* mPlayerTexture;
};
