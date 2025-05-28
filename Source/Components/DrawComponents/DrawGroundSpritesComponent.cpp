//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "DrawGroundSpritesComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

DrawGroundSpritesComponent::DrawGroundSpritesComponent(Actor *owner,
                                                       std::unordered_map<std::string, std::vector<Vector2>> sprite_offset_map,
                                                       int width, int height, const int drawOrder)
    : DrawComponent(owner, drawOrder),
      mWidth(width),
      mHeight(height),
      mSprite_offset_map(sprite_offset_map)
{
    for (const auto &pair: sprite_offset_map)
    {
        const std::string &filename = pair.first;
        const std::vector<Vector2> &offsets = pair.second;

        SDL_Texture *texture = GetGame()->LoadTexture(filename);

        if (texture)
            mTextureOffsetMap[texture] = offsets;

        else
            SDL_Log("Erro ao carregar textura '%s'", filename.c_str());

    }
}

void DrawGroundSpritesComponent::Draw(SDL_Renderer *renderer)
{
    if (!mIsVisible)
    {
        return;
    }
    // TODO 1.2 (~5 linhas): Utilize a função SDL_RenderCopyEx para desenhar a textura armazenada
    //  no atributo mSpriteSheetSurface. Você terá que criar um SDL_Rect para definir a região
    //  da tela onde será desenhado o sprite. Para que o objeto seja desenhado em relação a posição da câmera,
    //  subtraia a posição da câmera da posição do objeto. Além disso, você terá que criar uma flag do tipo
    //  SDL_RendererFlip para definir se o sprite será desenhado virado à direita ou à
    //  esquerda. A orientação do sprite (esquerda ou direita) depende da rotação do objeto dono do sprite.
    //  Se a rotação for zero, o sprite deve ser desenhado virado à direita. Se for igual a MAth::Pi, deve
    //  ser desenhado à esquerda.

    for (const auto &pair: mTextureOffsetMap)
    {
        SDL_Texture *texture = pair.first;
        const std::vector<Vector2> &offsets = pair.second;

        for (const Vector2 &offset: offsets)
        {
            SDL_Rect region;
            region.h = mHeight;
            region.w = mWidth;
            region.x = mOwner->GetPosition().x - GetGame()->GetCamera()->GetPosCamera().x + offset.x;
            region.y = mOwner->GetPosition().y - GetGame()->GetCamera()->GetPosCamera().y + offset.y;

            SDL_RendererFlip flip = SDL_FLIP_NONE;
            if (GetOwner()->GetRotation() == Math::Pi)
                flip = SDL_FLIP_HORIZONTAL;

            SDL_RenderCopyEx(renderer, texture, nullptr, &region, 0.0f, nullptr, flip);
        }
    }
}
