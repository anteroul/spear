#include "spear/rendering/opengl/error.hh"
#include <spear/rendering/opengl/texture.hh>
#include <spear/spear_root.hh>

#include <SDL3_image/SDL_image.h>
#include <GL/glew.h>

#include <iostream>

namespace spear::rendering::opengl
{

Texture::Texture()
    : BaseTexture()
{
    if (IMG_Init(IMG_INIT_PNG) == 0)
    {
        std::cerr << "IMG_Init failed: " << SDL_GetError() << std::endl;
    }
}

Texture::~Texture()
{
    free();
    IMG_Quit();
}

Texture::Texture(Texture&& other)
    : BaseTexture(std::move(other))
{
}

Texture& Texture::operator=(Texture&& other)
{
    if (this != &other)
    {
        BaseTexture::operator=(std::move(other));
    }
    return *this;
}

GLenum SDLFormatToOpenGLFormat(SDL_PixelFormat sdlFormat)
{
    switch (sdlFormat)
    {
        case SDL_PIXELFORMAT_RGBA8888:
            return GL_RGBA;
        case SDL_PIXELFORMAT_XRGB8888:
            return GL_RGB;
        case SDL_PIXELFORMAT_XBGR8888:
            return GL_BGR;
        case SDL_PIXELFORMAT_BGRA8888:
            return GL_BGRA;
        case SDL_PIXELFORMAT_RGB24:
            return GL_RGB;
        default:
            std::cerr << "SDL_PIXELFORMAT_" << SDL_GetPixelFormatName(sdlFormat) << std::endl;
            return sdlFormat;
    }
}

// Load texture from file (expects an external renderer setup)
bool Texture::loadFile(const std::string& path, bool asset_path)
{
    rendering::opengl::openglError("before Texture loadfile");

    free();
    std::string used_path = asset_path ? spearRoot() + "/assets/" + path : path;

    std::cout << "Loading image: " << used_path << std::endl;

    // Load image using SDL
    SDL_Surface* surface = IMG_Load(used_path.c_str());
    if (!surface)
    {
        std::cerr << "Failed to load image: " << path 
                  << " | SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Determine the format.
    GLenum format = SDLFormatToOpenGLFormat(surface->format);

    // Generate OpenGL texture.
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // Set texture parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture data to GPU.
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        //format,
        GL_RGBA,
        surface->w,
        surface->h,
        0,
        format,
        GL_UNSIGNED_BYTE,
        surface->pixels
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    // Store dimensions.
    m_width = surface->w;
    m_height = surface->h;

    // Unbind texture and free image data.
    SDL_DestroySurface(surface);
    glBindTexture(GL_TEXTURE_2D, 0);

    rendering::opengl::openglError("Texture loadfile");
    return true;
}

void Texture::bind(uint32_t unit) const
{
    if (m_texture == 0)
    {
        std::cerr << "Attempted to bind an uninitialized texture." << std::endl;
        return;
    }
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void unbind(uint32_t unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::free()
{
    if (m_texture)
    {
        glDeleteTextures(1, &m_texture);
        m_texture = 0;
        m_width = 0;
        m_height = 0;
    }
}

}
