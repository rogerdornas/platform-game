#include "Texture.h"

Texture::Texture()
: mTextureID(0)
, mWidth(0)
, mHeight(0)
{
}

Texture::~Texture()
{
    Unload();
}

bool Texture::Load(const std::string &filePath)
{
    // 1. Carrega a imagem original
    SDL_Surface* loadedSurface = IMG_Load(filePath.c_str());
    if (!loadedSurface) {
        SDL_Log("Failed to load image %s: %s", filePath.c_str(), IMG_GetError());
        return false;
    }

    // 2. Define o formato de pixel ideal para OpenGL (R, G, B, A na memória)
    // SDL_PIXELFORMAT_ABGR8888 em Little Endian (PCs) garante a ordem de bytes R-G-B-A.
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        const Uint32 targetFormat = SDL_PIXELFORMAT_RGBA8888;
    #else
        const Uint32 targetFormat = SDL_PIXELFORMAT_ABGR8888;
    #endif

    // 3. Converte para o formato padrão (RGBA 32-bit)
    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, targetFormat, 0);

    // Podemos liberar a superfície original carregada, pois já temos a convertida
    SDL_FreeSurface(loadedSurface);

    if (!formattedSurface) {
        SDL_Log("Unable to convert surface format: %s", SDL_GetError());
        return false;
    }

    mWidth = formattedSurface->w;
    mHeight = formattedSurface->h;

    // 4. Gera a textura OpenGL
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    // 5. Envia para a GPU
    // Como convertemos a surface, SABEMOS que é GL_RGBA e GL_UNSIGNED_BYTE
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, formattedSurface->pixels);

    // Configura filtros (Pixel Art fica melhor com GL_NEAREST)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Configura para a textura repetir ou clamar (opcional, mas bom para garantir)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 6. Libera a superfície convertida (já está na memória da GPU)
    SDL_FreeSurface(formattedSurface);

    return true;
}

bool Texture::LoadFromSurface(SDL_Surface *surface) {
    if (!surface)
    {
        SDL_Log("Falha ao carregar textura: SDL_Surface está nula.");
        return false;
    }

    mWidth = surface->w;
    mHeight = surface->h;

    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    GLenum format = GL_RGBA;
    GLint bytesPerPixel = surface->format->BytesPerPixel;

    if (bytesPerPixel == 3)
    {
        format = (surface->format->Rmask == 0x000000ff) ? GL_RGB : GL_BGR;
    }
    else if (bytesPerPixel == 4)
    {
        format = (surface->format->Rmask == 0x000000ff) ? GL_RGBA : GL_BGRA;
    }
    else
    {
        SDL_Log("Formato de textura desconhecido! BytesPerPixel: %d", bytesPerPixel);
        glDeleteTextures(1, &mTextureID); // Limpa o ID gerado
        mTextureID = 0;
        return false;
    }

    // Salva os valores antigos para restaurá-los depois
    GLint oldAlignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldAlignment);
    GLint oldRowLength;
    glGetIntegerv(GL_UNPACK_ROW_LENGTH, &oldRowLength);

    // Define os valores necessários para carregar o SDL_Surface
    // GL_UNPACK_ALIGNMENT, 1 é seguro para qualquer surface.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // GL_UNPACK_ROW_LENGTH informa ao OpenGL o "pitch" em pixels.
    glPixelStorei(GL_UNPACK_ROW_LENGTH, surface->pitch / bytesPerPixel);


    // Usamos GL_RGBA como formato *interno* para consistência.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0,
                 format, GL_UNSIGNED_BYTE, surface->pixels);

    // Isso evita que o carregamento de *outras* texturas quebre.
    glPixelStorei(GL_UNPACK_ALIGNMENT, oldAlignment);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, oldRowLength);

    // 5. Configurar filtros (LINEAR é melhor para fontes)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Adicionar Clamp to Edge é bom para fontes, evita "sangramento"
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Desvincular textura (boa prática)
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}


void Texture::Unload()
{
	glDeleteTextures(1, &mTextureID);
    mTextureID = 0;
}

void Texture::SetActive(int index) const
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}

void Texture::SetFromGLTexture(GLuint texID, int width, int height) {
    if (mTextureID != 0 && mTextureID != texID)
        Unload();

    mTextureID = texID;
    mWidth = width;
    mHeight = height;
}
