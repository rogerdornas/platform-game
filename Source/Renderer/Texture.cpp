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
    SDL_Surface* surface = IMG_Load(filePath.c_str());

    if (!surface) {
        SDL_Log("Failed to load image %s: %s", filePath.c_str(), IMG_GetError());
        return false;
    }

    mWidth = surface->w;
    mHeight = surface->h;

    // Generate a GL texture
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    GLenum format = GL_RGBA;

    // Detecta formato do SDL_Surface
    if (surface->format->BytesPerPixel == 3)
    {
        format = (surface->format->Rmask == 0x000000ff) ? GL_RGB : GL_BGR;
    }
    else if (surface->format->BytesPerPixel == 4)
    {
        format = (surface->format->Rmask == 0x000000ff) ? GL_RGBA : GL_BGRA;
    }
    else
    {
        SDL_Log("Formato de textura desconhecido!");
        return false;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight,
                 0, format, GL_UNSIGNED_BYTE, surface->pixels);

    // Delete the SDL surface
    SDL_FreeSurface(surface);

    // Use linear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return true;
}

bool Texture::LoadFromSurface(SDL_Surface *surface) {
    if (!surface)
        return false;

    mWidth = surface->w;
    mHeight = surface->h;

    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    GLenum format = GL_RGBA;

    // Detecta formato do SDL_Surface
    if (surface->format->BytesPerPixel == 3)
    {
        format = (surface->format->Rmask == 0x000000ff) ? GL_RGB : GL_BGR;
    }
    else if (surface->format->BytesPerPixel == 4)
    {
        format = (surface->format->Rmask == 0x000000ff) ? GL_RGBA : GL_BGRA;
    }
    else
    {
        SDL_Log("Formato de textura desconhecido!");
        return false;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Envia pixels para GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0,
                 format, GL_UNSIGNED_BYTE, surface->pixels);

    // Configura filtros
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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
