#include "spear/rendering/base_texture.hh"
#include <spear/rendering/opengl/texture/texture.hh>

#include <GL/glew.h>
#include <iostream>

namespace spear::rendering::opengl
{

Texture::Texture()
    : BaseTexture()
{
}

Texture::Texture(Texture&& other)
    : BaseTexture(std::move(other)),
      m_texture(std::move(other.m_texture))
{
}

Texture& Texture::operator=(Texture&& other)
{
    if (this != &other)
    {
        BaseTexture::operator=(std::move(other));
        m_texture = std::move(other.m_texture);
    }
    return *this;
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

void Texture::unbind(uint32_t unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

} // namespace spear::rendering::opengl
