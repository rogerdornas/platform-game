//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "TileMapComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Actors/DynamicGround.h"
#include "../../Renderer/Shader.h"

TileMapComponent::TileMapComponent(Actor* owner, int drawOrder)
    :DrawComponent(owner, drawOrder)
    , mBakedTexture(nullptr)
    , mBakedWidth(0.0f)
    , mBakedHeight(0.0f)
    , mCurrentWidth(0.0f)
    , mCurrentHeight(0.0f)
    , mGrowthDirection(GrowthDirection::Right)
    , mIsBaked(false)
{
}

TileMapComponent::~TileMapComponent()
{
    mTilesIndex.clear();

    if (mBakedTexture)
    {
        mBakedTexture->Unload();
        delete mBakedTexture;
        mBakedTexture = nullptr;
    }
}

void TileMapComponent::SetTilesIndex(const std::vector<std::vector<int>>& tiles)
{
    mTilesIndex = tiles;
}


void TileMapComponent::BakeTilesToTexture(Renderer* renderer)
{
    if (mIsBaked || mTilesIndex.empty())
        return;

    if (mBakedTexture)
    {
        delete mBakedTexture;
        mBakedTexture = nullptr;
    }

    auto game = mOwner->GetGame();
    Texture* tileSheet = game->GetTileSheet();
    const auto& tileSheetData = game->GetTileSheetData();

    int numRows = static_cast<int>(mTilesIndex.size());
    int numCols = static_cast<int>(mTilesIndex[0].size());
    int tileSize = game->GetTileSize();

    mBakedWidth = numCols * tileSize;
    mBakedHeight = numRows * tileSize;

    // --- Criar e bindar FBO ---
    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // --- Criar textura destino (baked) ---
    GLuint texId = 0;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    // IMPORTANT: use NEAREST e CLAMP_TO_EDGE para evitar bleeding entre tiles
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Cria a textura vazia com o tamanho do tilemap
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)mBakedWidth, (GLsizei)mBakedHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        SDL_Log("Erro ao criar FBO para Bake de TileMap! status=%u", status);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteTextures(1, &texId);
        glDeleteFramebuffers(1, &fbo);
        return;
    }

    // --- Salva viewport atual (opcional) e seta viewport do bake ---
    int oldViewport[4];
    glGetIntegerv(GL_VIEWPORT, oldViewport);
    glViewport(0, 0, (GLsizei)mBakedWidth, (GLsizei)mBakedHeight);

    // --- Limpa FBO ---
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // --- Ajusta projection do shader para o espaço da textura (0..width, 0..height) ---
    auto shader = renderer->GetBaseShader();
    // Guarda a ortho atual da janela (recria após o bake)
    Matrix4 bakeOrtho = Matrix4::CreateOrtho(0, mBakedWidth, 0, mBakedHeight, -1, 1);
    shader->SetMatrixUniform("uOrthoProj", bakeOrtho);
    shader->SetActive(); // garante que o shader atualizado seja usado

    shader->SetIntUniform("uEnableLighting", 0); // 0 = false

    // --- Pega os vértices do sprite ---
    VertexArray* spriteVerts = renderer->GetSpriteVerts();
    if (!spriteVerts) {
        SDL_Log("SpriteVerts nulo no bake!");
        // (Restaurar estado e sair...)
        return;
    }
    spriteVerts->SetActive(); // Ativa o VAO do sprite uma vez

    // --- Renderiza tiles para o FBO ---
    // observe: DrawTexture usa coordenadas do mundo; aqui, passamos posições no espaço do bake
    for (int row = 0; row < numRows; ++row)
    {
        for (int col = 0; col < numCols; ++col)
        {
            int tileIndex = mTilesIndex[row][col];
            if (tileSheetData.find(tileIndex) == tileSheetData.end())
                continue;

            Vector4 texRect = tileSheetData.at(tileIndex); // deve ser normalizado (0..1)
            // pos central do tile no espaço do baked texture
            Vector2 pos(col * tileSize + tileSize / 2.0f, row * tileSize + tileSize / 2.0f);

            // renderer->DrawTexture(
            //     pos,
            //     Vector2((float)tileSize, (float)tileSize),
            //     0.0f,
            //     Vector3(255, 255, 255),
            //     tileSheet,
            //     texRect,
            //     Vector2(0, 0), // câmera não importa no bake
            //     Vector2(1, 1),
            //     1.0f
            // );

            // ===============================================================
            // SUBSTITUA A CHAMADA DrawTexture POR ESTE BLOCO:
            // --- INÍCIO DO DESENHO MANUAL ---
            
            // 1. Cria a matriz de modelo
            Matrix4 model = Matrix4::CreateScale(Vector3((float)tileSize, (float)tileSize, 1.0f)) *
                            Matrix4::CreateTranslation(Vector3(pos.x, pos.y, 0.0f));

            // 2. Define os uniforms (exceto uOrthoProj, que já está correto)
            shader->SetMatrixUniform("uWorldTransform", model);
            shader->SetVectorUniform("uColor", Vector3(1.0f, 1.0f, 1.0f)); // Cor branca
            shader->SetVectorUniform("uTexRect", texRect);
            shader->SetVectorUniform("uCameraPos", Vector2(0, 0)); // Câmera não se aplica
            shader->SetFloatUniform("uAlpha", 1.0f);

            // 3. Define a textura
            tileSheet->SetActive();
            shader->SetFloatUniform("uTextureFactor", 1.0f); // 1.0f = 100% textura

            // 4. Desenha!
            glDrawElements(GL_TRIANGLES, spriteVerts->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
            
            // --- FIM DO DESENHO MANUAL ---
            // ===============================================================
        }
    }

    // --- Finaliza bake: desbind FBO e restaura viewport e shader ortho para a janela ---
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDeleteFramebuffers(1, &fbo);

    // Restaura viewport da janela (usando game->GetLogicalWindowWidth/Height)
    // int w = game->GetLogicalWindowWidth();
    // int h = game->GetLogicalWindowHeight();
    // glViewport(0, 0, w, h);
    renderer->UpdateViewport();

    // Restaura uOrthoProj para a ortho da janela (o renderer define originalmente essa ortho)
    // Matrix4 windowOrtho = Matrix4::CreateOrtho(0, static_cast<float>(w), static_cast<float>(h), 0, -1, 1);
    // shader->SetMatrixUniform("uOrthoProj", windowOrtho);
    renderer->SetZoom(renderer->GetZoom());
    
    shader->SetIntUniform("uEnableLighting", 1);
    shader->SetActive();

    // --- Cria wrapper Texture a partir do GL texture id (sua função SetFromGLTexture) ---
    mBakedTexture = new Texture();
    mBakedTexture->SetFromGLTexture(texId, (int)mBakedWidth, (int)mBakedHeight);

    mIsBaked = true;
    mCurrentWidth = mBakedWidth;
    mCurrentHeight = mBakedHeight;
}

void TileMapComponent::Draw(Renderer* renderer)
{
    if (!mIsBaked || !mBakedTexture || !mIsVisible)
        return;

    float uMin = 0.0f, vMin = 0.0f;
    float uMax = 1.0f, vMax = 1.0f;

    // Fatores de recorte normalizados
    float uCrop = mCurrentWidth / mBakedWidth;
    float vCrop = mCurrentHeight / mBakedHeight;

    // Define o retângulo de textura com base na direção de crescimento
    switch (mGrowthDirection)
    {
        case GrowthDirection::Right:
            // cresce para direita → corta da esquerda
            uMin = 0.0f;
            uMax = uCrop;
            vMin = 0.0f;
            vMax = vCrop;
            break;

        case GrowthDirection::Left:
            // cresce para esquerda → corta da direita
            uMin = 1.0f - uCrop;
            uMax = 1.0f;
            vMin = 0.0f;
            vMax = vCrop;
            break;

        case GrowthDirection::Up:
            // cresce para cima → corta de baixo
            uMin = 0.0f;
            uMax = uCrop;
            vMin = 1.0f - vCrop;
            vMax = 1.0f;
            break;

        case GrowthDirection::Down:
            // cresce para baixo → corta de cima
            uMin = 0.0f;
            uMax = uCrop;
            vMin = 0.0f;
            vMax = vCrop;
            break;

        default:
            uMin = 0.0f;
            uMax = uCrop;
            vMin = 0.0f;
            vMax = vCrop;
            break;
    }

    // Vector4 texRect(uMin, vMin, uMax, vMax);
    Vector4 texRect(uMin, vMin, uMax - uMin, vMax - vMin);

    // Matrix4 orthoProjection = Matrix4::CreateOrtho(0.0f, mOwner->GetGame()->GetRenderer()->GetZoomedWidth(), mOwner->GetGame()->GetRenderer()->GetZoomedHeight(), 0.0f, -1.0f, 1.0f);
    // renderer->GetBaseShader()->SetMatrixUniform("uOrthoProj", orthoProjection);

    renderer->DrawTexture(
        mOwner->GetPosition(),
        Vector2(mCurrentWidth, mCurrentHeight),
        0.0f,
        Vector3(255, 255, 255),
        mBakedTexture,
        texRect,
        mOwner->GetGame()->GetCamera()->GetPosCamera(),
        Vector2(1.0f, 1.0f)
    );
}