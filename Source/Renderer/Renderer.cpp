#include <GL/glew.h>
#include "Renderer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"
#include "../Game.h"
#include "../Actors/Light.h"

Renderer::Renderer(SDL_Window *window)
    :mBaseShader(nullptr)
    ,mFadeShader(nullptr)
    ,mWindow(window)
    ,mContext(nullptr)
    ,mOrthoProjection(Matrix4::Identity)
    ,mAmbientColor(Vector3(1.0f, 1.0f, 1.0f))
    ,mAmbientIntensity(0.8f)
    ,mVirtualWidth(1920.0f) // <-- Defina sua resolução virtual 16:9 aqui
    ,mVirtualHeight(1080.0f) // <-- Defina sua resolução virtual 16:9 aqui
    ,mWindowWidth(0.0f)
    ,mWindowHeight(0.0f)
    ,mZoom(1.0f) // <-- Inicializa o zoom como 1.0 (normal)
    ,mZoomedWidth(1920.0f) // <-- Inicializa com o valor base
    ,mZoomedHeight(1080.0f) // <-- Inicializa com o valor base
    ,mDrawingUI(false)
{

}

Renderer::~Renderer()
{
    delete mSpriteVerts;
    mSpriteVerts = nullptr;
}

bool Renderer::Initialize(float width, float height)
{
    // Specify version 3.3 (core profile)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Force OpenGL to use hardware acceleration
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // Create an OpenGL context
    mContext = SDL_GL_CreateContext(mWindow);

    // Turn on vsync
    SDL_GL_SetSwapInterval(1);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        SDL_Log("Failed to initialize GLEW.");
        return false;
    }

    // Armazena o tamanho inicial da janela
    // mWindowWidth = width;
    // mWindowHeight = height;

    int pixelWidth, pixelHeight;
    SDL_GL_GetDrawableSize(mWindow, &pixelWidth, &pixelHeight);
    // Armazena o tamanho inicial da janela (em pixels)
    mWindowWidth = static_cast<float>(pixelWidth);
    mWindowHeight = static_cast<float>(pixelHeight);

    mZoomedWidth = mVirtualWidth;
    mZoomedHeight = mVirtualHeight;

	// Make sure we can create/compile shaders
	if (!LoadShaders()) {
		SDL_Log("Failed to load shaders.");
		return false;
	}

    // Create quad for drawing sprites
    CreateSpriteVerts();

    InitializeFadeQuad();

    // Set the clear color to light grey
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Enable alpha blending on textures
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create orthografic projection matrix
    mOrthoProjection = Matrix4::CreateOrtho(0.0f, mZoomedWidth, mZoomedHeight, 0.0f, -1.0f, 1.0f);
    mBaseShader->SetMatrixUniform("uOrthoProj", mOrthoProjection);

    // Set texture unit to zero (all textures will be binded to this one)
    mBaseShader->SetTextureUniform("uTexture", 0);

    // Activate shader
    mBaseShader->SetActive();

    // Calcule e defina o viewport inicial
    UpdateViewport(); // <-- ADICIONADO

    return true;
}

void Renderer::InitializeFadeQuad()
{
    // Quad em tela cheia no espaço NDC (-1 a 1)
    float vertices[] = {
        -1.0f,  1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f
    };
    unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

    mFadeVAO = new VertexArray(vertices, 4, indices, 6);
    mFadeShader = new Shader();
    mFadeShader->Load("../Shaders/Fade"); // fade.vert + fade.frag
}

void Renderer::Shutdown()
{
    // Destroy textures
    for (auto i : mTextures)
    {
        // i.second->Unload();
        delete i.second;
    }
    mTextures.clear();

    mBaseShader->Unload();
    delete mBaseShader;
    mFadeShader->Unload();
    delete mFadeShader;


    SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
}

void Renderer::UnloadAllTextures()
{
    for (auto it = mTextures.begin(); it != mTextures.end(); )
    {
        if (it->first != "../Assets/Sprites/Esquilo5/Esquilo.png" &&
            it->first != "../Assets/Sprites/Rope/Rope2.png")
        {
            it->second->Unload();
            delete it->second;
            it = mTextures.erase(it); // remove do mapa e avança
        }
        else
        {
            ++it; // apenas avança
        }
    }
}

void Renderer::Clear()
{
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(RendererMode mode, const Matrix4 &modelMatrix, const Vector2 &cameraPos, VertexArray *vertices,
                    const Vector3 &color, float alpha, Texture *texture, const Vector4 &textureRect,
                    float textureFactor)
{
    // Transforma posição da cânera em int para não ter tremor
    Vector2 cameraInt(std::floor(cameraPos.x), std::floor(cameraPos.y));

    mBaseShader->SetMatrixUniform("uOrthoProj", mOrthoProjection);
    mBaseShader->SetMatrixUniform("uWorldTransform", modelMatrix);
    mBaseShader->SetVectorUniform("uColor", color);
    mBaseShader->SetVectorUniform("uTexRect", textureRect);
    mBaseShader->SetVectorUniform("uCameraPos", cameraInt);
    mBaseShader->SetFloatUniform("uAlpha", alpha);

    if (mDrawingUI) {
        mOrthoProjection = Matrix4::CreateOrtho(0.0f, mVirtualWidth, mVirtualHeight, 0.0f, -1.0f, 1.0f);
        mBaseShader->SetMatrixUniform("uOrthoProj", mOrthoProjection);
        DeactivateLighting();

    }
    else {
        mOrthoProjection = Matrix4::CreateOrtho(0.0f, mZoomedWidth, mZoomedHeight, 0.0f, -1.0f, 1.0f);
        mBaseShader->SetMatrixUniform("uOrthoProj", mOrthoProjection);
        UploadLightingUniforms();
    }

    if(vertices)
    {
        vertices->SetActive();
    }

    if(texture)
    {
        texture->SetActive();
        mBaseShader->SetFloatUniform("uTextureFactor", textureFactor);
    }
    else {
        glBindTexture(GL_TEXTURE_2D, 0); // desativa qualquer textura anterior
        mBaseShader->SetFloatUniform("uTextureFactor", 0.0f);
    }

    if (mode == RendererMode::LINES)
    {
        glDrawElements(GL_LINE_LOOP, vertices->GetNumIndices(), GL_UNSIGNED_INT,nullptr);
    }
    else if(mode == RendererMode::TRIANGLES)
    {
        glDrawElements(GL_TRIANGLES, vertices->GetNumIndices(), GL_UNSIGNED_INT,nullptr);
    }
}

void Renderer::DrawRect(const Vector2 &position, const Vector2 &size, float rotation, const Vector3 &color,
                        const Vector2 &cameraPos, RendererMode mode, float alpha)
{
    Matrix4 model = Matrix4::CreateScale(Vector3(size.x, size.y, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(Vector3(position.x, position.y, 0.0f));

    Draw(mode, model, cameraPos, mSpriteVerts, color, alpha);
}

void Renderer::DrawTexture(const Vector2 &position, const Vector2 &size, float rotation, const Vector3 &color,
                           Texture *texture, const Vector4 &textureRect, const Vector2 &cameraPos, Vector2 scale,
                           float textureFactor, float alpha, float freezeLevel)
{
    // Transforma para int a posição para não ter tremor
    Vector3 pos(std::floor(position.x), std::floor(position.y), std::floor(0.0f));

    Matrix4 model = Matrix4::CreateScale(Vector3(size.x * scale.x, size.y * scale.y, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(pos);

    mBaseShader->SetFloatUniform("uFreezeLevel", freezeLevel);

    Draw(RendererMode::TRIANGLES, model, cameraPos, mSpriteVerts, color, alpha, texture, textureRect, textureFactor);
}

void Renderer::DrawGeometry(const Vector2 &position, const Vector2 &size, float rotation, const Vector3 &color,
                            const Vector2 &cameraPos, VertexArray *vertexArray, RendererMode mode)
{
    Matrix4 model = Matrix4::CreateScale(Vector3(size.x, size.y, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(Vector3(position.x, position.y, 0.0f));

    Draw(mode, model, cameraPos, vertexArray, color);
}

void Renderer::DrawLine(const Vector2 &start, const Vector2 &end, const Vector3 &color,
                        float thickness, const Vector2 &cameraPos, float alpha)
{
    // 1. Calcular o vetor da linha e seu comprimento
    Vector2 lineVector = end - start;
    float lineLength = lineVector.Length();

    // Se a linha tem comprimento zero, não desenhe
    if (lineLength < 0.01f) {
        return;
    }

    // 2. Calcular o ponto central (posição) e a rotação da linha
    Vector2 center = (start + end) / 2.0f;
    float rotation = std::atan2(lineVector.y, lineVector.x);

    // 3. Criar a matriz de modelo
    // O tamanho 'size' será (comprimento_da_linha, espessura)
    Matrix4 model = Matrix4::CreateScale(Vector3(lineLength, thickness, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(Vector3(center.x, center.y, 0.0f));

    // 4. Chamar a função principal Draw
    // Assumimos que 'mLineVerts' é um VertexArray que representa um segmento de linha padrão
    // (por exemplo, um quadrado 1x1 ou apenas 2 vértices em (0,0) e (1,0) se você estiver desenhando apenas uma linha)
    // Se você não tiver um mLineVerts, você pode usar mSpriteVerts e desenhar um retângulo fino.

    // **NOTA:** Se você quiser desenhar a linha como um segmento fino (LINES), você precisa de
    // um VertexArray com apenas 2 vértices (por exemplo, (0,0) e (1,0)) e um índice para GL_LINE_LOOP/GL_LINES.
    // Se você usar mSpriteVerts, o modo RendererMode::LINES desenhará um retângulo *contorno*.

    // Vou usar mSpriteVerts e o modo LINES, assumindo que você quer o *contorno* de um retângulo,
    // ou se mSpriteVerts é um VBO/IBO de 2 pontos (0,0) e (1,0) para desenhar uma linha.
    // **Vou desenhar como um segmento fino de triângulos para ter espessura (DrawRect style):**

    // Para desenhar uma linha com espessura (um retângulo fino):
    Draw(RendererMode::TRIANGLES, model, cameraPos, mSpriteVerts, color, alpha);

    // Se você quiser desenhar apenas um segmento de linha de 1 pixel de espessura (e **mLineVerts** existir e
    // tiver 2 vértices), você usaria:
    // Draw(RendererMode::LINES, model, cameraPos, mLineVerts, color, alpha);
}

void Renderer::DrawFade(float alpha)
{
    if (!mFadeShader || !mFadeVAO)
        return;

    // Ativa blending (já ativo em geral, mas garantimos)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Usa o shader de fade
    mFadeShader->SetActive();
    mFadeShader->SetFloatUniform("uAlpha", alpha);

    // Desenha o quad
    mFadeVAO->SetActive();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // Restaura o shader principal
    mBaseShader->SetActive();
}

void Renderer::Present()
{
	// Swap the buffers
	SDL_GL_SwapWindow(mWindow);
}

void Renderer::SetAmbientLight(const Vector3& color, float intensity)
{
    mAmbientColor = color;
    mAmbientIntensity = intensity;
}

void Renderer::AddLight(Light* light)
{
    if (mLights.size() < 80) // mesmo limite que o shader
        mLights.push_back(light);
}

void Renderer::ClearLights()
{
    mLights.clear();
}

bool Renderer::LoadShaders()
{
	// Create sprite shader
	mBaseShader = new Shader();
	if (!mBaseShader->Load("../Shaders/Base")) {
		return false;
	}

    mBaseShader->SetActive(); // Ativa para configurar
    mBaseShader->SetMatrixUniform("uOrthoProj", mOrthoProjection);
    mBaseShader->SetTextureUniform("uTexture", 0);

    return true;
}

void Renderer::CreateSpriteVerts()
{
    // Create rect vertex data to draw sprite
    float vertexData[] = {
            -0.5f, 0.5f, 0.0f, 1.0f,
            0.5f, 0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.0f, 0.0f
    };

    unsigned int indexData[] = {0, 1, 2, 2, 3, 0};

    mSpriteVerts = new VertexArray(vertexData, 4, indexData, 6);
}

void Renderer::UploadLightingUniforms()
{
    mBaseShader->SetVectorUniform("uAmbientColor", mAmbientColor);
    mBaseShader->SetFloatUniform("uAmbientIntensity", mAmbientIntensity);
    mBaseShader->SetIntUniform("uNumLights", static_cast<int>(mLights.size()));

    for (int i = 0; i < static_cast<int>(mLights.size()); ++i)
    {
        std::string prefix = "uLights[" + std::to_string(i) + "]";
        if (mLights[i]->IsActivate()) {
            mBaseShader->SetVectorUniform((prefix + ".position").c_str(), mLights[i]->GetPosition());
            mBaseShader->SetVectorUniform((prefix + ".color").c_str(), mLights[i]->GetColor());
            mBaseShader->SetFloatUniform((prefix + ".intensity").c_str(), mLights[i]->GetIntensity());
            mBaseShader->SetFloatUniform((prefix + ".radius").c_str(), mLights[i]->GetRadius());
        }
        else {
            mBaseShader->SetFloatUniform((prefix + ".intensity").c_str(), 0);
            mBaseShader->SetFloatUniform((prefix + ".radius").c_str(), 0);
        }
    }
}

void Renderer::DeactivateLighting() {
    mBaseShader->SetVectorUniform("uAmbientColor", Vector3(1.0f, 1.0f, 1.0f));
    mBaseShader->SetFloatUniform("uAmbientIntensity", 1.0f);
    mBaseShader->SetIntUniform("uNumLights", 0);
}


Texture* Renderer::GetTexture(const std::string& fileName)
{
    Texture* tex = nullptr;
    auto iter = mTextures.find(fileName);
    if (iter != mTextures.end())
    {
        tex = iter->second;
    }
    else
    {
        tex = new Texture();
        if (tex->Load(fileName))
        {
            mTextures.emplace(fileName, tex);
            return tex;
        }
        else
        {
            delete tex;
            return nullptr;
        }
    }
    return tex;
}

void Renderer::OnWindowResize(float newWidth, float newHeight)
{
    // Atualiza as dimensões da janela
    mWindowWidth = newWidth;
    mWindowHeight = newHeight;

    // Recalcula o viewport
    UpdateViewport();
}

void Renderer::SetZoom(float zoom)
{
    // Limita o zoom para evitar valores estranhos (ex: divisão por zero)
    if (zoom < 0.01f)
    {
        zoom = 0.01f;
    }

    mZoom = zoom;

    // Recalcula as dimensões da "janela" virtual
    mZoomedWidth = mVirtualWidth / mZoom;
    mZoomedHeight = mVirtualHeight / mZoom;

    // Recalcula a matriz de projeção com base nas novas dimensões
    mOrthoProjection = Matrix4::CreateOrtho(0.0f, mZoomedWidth, mZoomedHeight, 0.0f, -1.0f, 1.0f);
}

void Renderer::UpdateViewport()
{
    if (mWindowWidth == 0.0f || mWindowHeight == 0.0f)
    {
        return;
    }

    // 1. Calcula a proporção da janela e a proporção desejada
    float windowAspect = mWindowWidth / mWindowHeight;
    float targetAspect = mVirtualWidth / mVirtualHeight; // (16.0f / 9.0f)

    int newWidth = static_cast<int>(mWindowWidth);
    int newHeight = static_cast<int>(mWindowHeight);

    int xOffset = 0;
    int yOffset = 0;

    // 2. Compara as proporções para decidir sobre letterbox (barras em cima/baixo) ou pillarbox (barras laterais)

    if (windowAspect > targetAspect)
    {
        // Pillarbox (janela mais larga que 16:9)
        newWidth = static_cast<int>(mWindowHeight * targetAspect);
        xOffset = (static_cast<int>(mWindowWidth) - newWidth) / 2;
    }
    else if (windowAspect < targetAspect)
    {
        // Letterbox (janela mais alta que 16:9)
        newHeight = static_cast<int>(mWindowWidth / targetAspect);
        yOffset = (static_cast<int>(mWindowHeight) - newHeight) / 2;
    }
    // else: A proporção é exata, xOffset e yOffset permanecem 0.

    // Armazene os valores calculados nos membros da classe
    mViewportX = xOffset;
    mViewportY = yOffset;
    mViewportWidth = newWidth;
    mViewportHeight = newHeight;

    // 3. Define o viewport do OpenGL
    // Isso diz ao OpenGL para desenhar apenas nesta área retangular da janela.
    // O `Renderer::Clear()` (com glClearColor(0,0,0,1)) limpará a janela inteira,
    // criando automaticamente as barras pretas nas áreas fora do viewport.
    glViewport(xOffset, yOffset, newWidth, newHeight);
}

Vector2 Renderer::ScreenToVirtual(const Vector2& screenPoint) const
{
    // Evita divisão por zero se a janela for minimizada
    if (mViewportWidth == 0 || mViewportHeight == 0)
    {
        return Vector2(0.0f, 0.0f);
    }

    Vector2 virtualPoint;

    // 1. Remove o offset das barras pretas (traduz para coordenadas relativas ao viewport)
    virtualPoint.x = screenPoint.x - mViewportX;
    virtualPoint.y = screenPoint.y - mViewportY;

    // 2. Escala as coordenadas do viewport para as coordenadas virtuais
    // (mouse.x / viewportWidth) * virtualWidth
    virtualPoint.x *= (mVirtualWidth / static_cast<float>(mViewportWidth));
    virtualPoint.y *= (mVirtualHeight / static_cast<float>(mViewportHeight));

    return virtualPoint;
}

void Renderer::BeginGameDraw()
{
    mDrawingUI = false;
}

void Renderer::BeginUIDraw()
{
    mDrawingUI = true;
}