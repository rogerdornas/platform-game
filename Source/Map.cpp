//
// Created by roger on 18/08/2025.
//

#include "Map.h"

#include "Game.h"

Map::Map(class Game *game, int **mapData, int width, int height)
    :mGame(game)
    ,mCompleteMap(mapData)
    ,mMapWidth(width)
    ,mMapHeight(height)
    ,mRadius(17)
    ,mMapTexture(nullptr)
    ,mPlayerTexture(nullptr)
{
    mTileSize = (mGame->GetLogicalWindowWidth() * 0.75f) / mMapWidth;

    // cria a textura onde o mapa será desenhado
    mMapTexture = SDL_CreateTexture(
        mGame->GetRenderer(),
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        mMapWidth * mTileSize,
        mMapHeight * mTileSize
    );

    SDL_SetTextureBlendMode(mMapTexture, SDL_BLENDMODE_BLEND);

    mVisibleMap = new bool*[height];
    for (int i = 0; i < height; ++i) {
        mVisibleMap[i] = new bool[width];
        for (int j = 0; j < width; ++j) {
            mVisibleMap[i][j] = false;
        }
    }

    // desenha fundo inicial (tudo bloqueado)
    SDL_SetRenderTarget(mGame->GetRenderer(), mMapTexture);
    SDL_SetRenderDrawColor(mGame->GetRenderer(), 50, 50, 50, 120);
    SDL_RenderClear(mGame->GetRenderer());
    SDL_SetRenderTarget(mGame->GetRenderer(), nullptr);

    mPlayerTexture = mGame->LoadTexture("../Assets/Sprites/PlayerIconMap/Esquilo.png");
}

Map::~Map() {
    for (int i = 0; i < mMapHeight; ++i) {
        if (mCompleteMap[i] != nullptr)
            delete[] mCompleteMap[i];
    }
    delete[] mCompleteMap;
    mCompleteMap = nullptr;

    for (int i = 0; i < mMapHeight; ++i) {
        if (mVisibleMap[i] != nullptr)
            delete[] mVisibleMap[i];
    }
    delete[] mVisibleMap;
    mVisibleMap = nullptr;

    SDL_DestroyTexture(mMapTexture);
    mMapTexture = nullptr;
}


SDL_Color Map::GetTileColor(int tile) {
    switch (tile) {
        case Empty:     return { 173, 216, 230, 255 }; // azul claro
        case Blocked:   return { 50, 50, 50, 120 }; // cinza
        case Nothing:   return { 173, 216, 230, 255 }; // azul claro
        case Spike:     return { 220, 0, 0, 255 };     // vermelho
        default:        return { 0, 0, 0, 255 };       // fallback preto
    }
}


void Map::Update(float deltaTime) {
    int playerTileX = mGame->GetPlayer()->GetPosition().x / mGame->GetTileSize();
    int playerTileY = mGame->GetPlayer()->GetPosition().y / mGame->GetTileSize();

    SDL_SetRenderTarget(mGame->GetRenderer(), mMapTexture);

    for (int dy = -mRadius; dy <= mRadius; dy++) {
        for (int dx = -mRadius; dx <= mRadius; dx++) {
            int nx = playerTileX + dx;
            int ny = playerTileY + dy;

            if (nx >= 0 && ny >= 0 && nx < mMapWidth && ny < mMapHeight) {
                mVisibleMap[ny][nx] = true;
            }
        }
    }

    for (int y = 0; y < mMapHeight; y++) {
        for (int x = 0; x < mMapWidth; x++) {
            if (mVisibleMap[y][x]) {
                int tile = mCompleteMap[y][x];

                SDL_Color c = GetTileColor(tile);
                SDL_SetRenderDrawColor(mGame->GetRenderer(), c.r, c.g, c.b, c.a);

                SDL_Rect rect = {
                    static_cast<int>(x * mTileSize),
                    static_cast<int>(y * mTileSize),
                    static_cast<int>(mTileSize),
                    static_cast<int>(mTileSize)
                };
                SDL_RenderFillRect(mGame->GetRenderer(), &rect);

                if (tile == Blocked) {
                    SDL_SetRenderDrawColor(mGame->GetRenderer(), 0, 255, 0, 255);

                    int border = 1; // espessura da borda

                    // cima
                    if (y > 0 && (mCompleteMap[y-1][x] == Empty || mCompleteMap[y-1][x] == Spike) && mVisibleMap[y-1][x] == true) {
                        SDL_Rect edge = { rect.x, rect.y, rect.w, border };
                        SDL_RenderFillRect(mGame->GetRenderer(), &edge);
                    }
                    // baixo
                    if (y < mMapHeight-1 && (mCompleteMap[y+1][x] == Empty || mCompleteMap[y+1][x] == Spike) && mVisibleMap[y+1][x] == true) {
                        SDL_Rect edge = { rect.x, rect.y + rect.h - border, rect.w, border };
                        SDL_RenderFillRect(mGame->GetRenderer(), &edge);
                    }
                    // esquerda
                    if (x > 0 && (mCompleteMap[y][x-1] == Empty || mCompleteMap[y][x-1] == Spike) && mVisibleMap[y][x-1] == true) {
                        SDL_Rect edge = { rect.x, rect.y, border, rect.h };
                        SDL_RenderFillRect(mGame->GetRenderer(), &edge);
                    }
                    // direita
                    if (x < mMapWidth-1 && (mCompleteMap[y][x+1] == Empty || mCompleteMap[y][x+1] == Spike) && mVisibleMap[y][x+1] == true) {
                        SDL_Rect edge = { rect.x + rect.w - border, rect.y, border, rect.h };
                        SDL_RenderFillRect(mGame->GetRenderer(), &edge);
                    }
                }
            }
        }
    }

    // desenha o player como um quadrado verde
    // SDL_SetRenderDrawColor(mGame->GetRenderer(), 0, 255, 0, 255);
    // SDL_Rect playerRect = {
    //     static_cast<int>(playerTileX * mTileSize),
    //     static_cast<int>(playerTileY * mTileSize),
    //     static_cast<int>(mTileSize),
    //     static_cast<int>(mTileSize * 2)
    // };
    // SDL_RenderFillRect(mGame->GetRenderer(), &playerRect);

    SDL_SetRenderTarget(mGame->GetRenderer(), nullptr);
}

void Map::CreateMap(SDL_Renderer *renderer) {
    // muda o alvo de renderização para a textura
    SDL_SetRenderTarget(renderer, mMapTexture);

    // limpar a textura antes de desenhar
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // totalmente transparente
    SDL_RenderClear(renderer);

    for (int y = 0; y < mMapHeight; y++) {
        for (int x = 0; x < mMapWidth; x++) {
            if (!mVisibleMap[y][x]) {
                // área não descoberta → cor escura
                SDL_Color c = GetTileColor(Blocked);
                SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
                // SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
                SDL_Rect rect = {
                    static_cast<int>(x * mTileSize),
                    static_cast<int>(y * mTileSize),
                    static_cast<int>(mTileSize),
                    static_cast<int>(mTileSize)
                };
                SDL_RenderFillRect(renderer, &rect);
            }
            else {
                int tile = mCompleteMap[y][x];

                SDL_Color c = GetTileColor(tile);
                SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);

                SDL_Rect rect = {
                    static_cast<int>(x * mTileSize),
                    static_cast<int>(y * mTileSize),
                    static_cast<int>(mTileSize),
                    static_cast<int>(mTileSize)
                };
                SDL_RenderFillRect(renderer, &rect);

                if (tile == Blocked) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

                    int border = 1; // espessura da borda

                    // cima
                    if (y > 0 && (mCompleteMap[y-1][x] == Empty || mCompleteMap[y-1][x] == Spike) && mVisibleMap[y-1][x] == true) {
                        SDL_Rect edge = { rect.x, rect.y, rect.w, border };
                        SDL_RenderFillRect(renderer, &edge);
                    }
                    // baixo
                    if (y < mMapHeight-1 && (mCompleteMap[y+1][x] == Empty || mCompleteMap[y+1][x] == Spike) && mVisibleMap[y+1][x] == true) {
                        SDL_Rect edge = { rect.x, rect.y + rect.h - border, rect.w, border };
                        SDL_RenderFillRect(renderer, &edge);
                    }
                    // esquerda
                    if (x > 0 && (mCompleteMap[y][x-1] == Empty || mCompleteMap[y][x-1] == Spike) && mVisibleMap[y][x-1] == true) {
                        SDL_Rect edge = { rect.x, rect.y, border, rect.h };
                        SDL_RenderFillRect(renderer, &edge);
                    }
                    // direita
                    if (x < mMapWidth-1 && (mCompleteMap[y][x+1] == Empty || mCompleteMap[y][x+1] == Spike) && mVisibleMap[y][x+1] == true) {
                        SDL_Rect edge = { rect.x + rect.w - border, rect.y, border, rect.h };
                        SDL_RenderFillRect(renderer, &edge);
                    }
                }
            }
        }
    }

    int playerTileX = mGame->GetPlayer()->GetPosition().x / mGame->GetTileSize();
    int playerTileY = mGame->GetPlayer()->GetPosition().y / mGame->GetTileSize();

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // verde
    SDL_Rect playerRect = {
        static_cast<int>(playerTileX * mTileSize),
        static_cast<int>(playerTileY * mTileSize),
        static_cast<int>(mTileSize),
        static_cast<int>(mTileSize * 2)
    };
    SDL_RenderFillRect(renderer, &playerRect);

    SDL_SetRenderTarget(renderer, nullptr);
}

void Map::Draw(SDL_Renderer *renderer) {
    // tamanho original do mapa em pixels
    int mapPixelWidth  = mMapWidth * mTileSize;
    int mapPixelHeight = mMapHeight * mTileSize;

    // queremos que a largura final seja 3/4 da largura da tela
    int dstW = mGame->GetLogicalWindowWidth() * 0.75f;
    // mantém a proporção
    float scale = (float)dstW / (float)mapPixelWidth;
    int dstH = (int)(mapPixelHeight * scale);

    // posição: 1/3 da tela em X
    int dstX = mGame->GetLogicalWindowWidth() * 0.125f;
    int dstY = (mGame->GetLogicalWindowHeight() - dstH) / 2;

    SDL_Rect dst = { dstX, dstY, dstW, dstH };

    SDL_RenderCopy(renderer, mMapTexture, nullptr, &dst);

    // desenha o player por cima (verde)
    int playerTileX = mGame->GetPlayer()->GetPosition().x / mGame->GetTileSize();
    int playerTileY = mGame->GetPlayer()->GetPosition().y / mGame->GetTileSize();

    SDL_Rect dstRect = {
        dstX + static_cast<int>(playerTileX * mTileSize * scale - (mTileSize * 4 * scale)),
        dstY + static_cast<int>(playerTileY * mTileSize * scale - (mTileSize * 4 * scale)),
        static_cast<int>(mTileSize * 8 * scale),
        static_cast<int>(mTileSize * 8 * scale)
    };

    SDL_RenderCopyEx(renderer, mPlayerTexture, nullptr, &dstRect, 0.0f, nullptr, SDL_FLIP_NONE);


    // SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    // SDL_Rect playerRect = {
    //     dstX + static_cast<int>(playerTileX * mTileSize * scale),
    //     dstY + static_cast<int>(playerTileY * mTileSize * scale - (mTileSize * 4 * scale)),
    //     static_cast<int>(mTileSize * 4 * scale),
    //     static_cast<int>(mTileSize * 8 * scale)
    // };
    // SDL_RenderFillRect(renderer, &playerRect);

    // Desenha a textura pronta
    // SDL_Rect dst = { 200, 200, mMapWidth * mTileSize, mMapHeight * mTileSize };
    // SDL_RenderCopy(renderer, mMapTexture, nullptr, &dst);
}

void Map::ChangeResolution(float oldScale, float newScale) {
    // CreateMap(mGame->GetRenderer());
    // mTileSize = (mGame->GetLogicalWindowWidth() * 0.75f) / mMapWidth;

    // desenha fundo inicial (tudo bloqueado)
    SDL_SetRenderTarget(mGame->GetRenderer(), mMapTexture);
    SDL_SetRenderDrawColor(mGame->GetRenderer(), 50, 50, 50, 120);
    SDL_RenderClear(mGame->GetRenderer());
    SDL_SetRenderTarget(mGame->GetRenderer(), nullptr);
}
