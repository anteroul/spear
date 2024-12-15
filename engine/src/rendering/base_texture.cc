#include <spear/rendering/base_texture.hh>

#include <utility>

namespace spear::rendering
{

BaseTexture::BaseTexture()
    : m_width(0),
      m_height(0)
{
}

BaseTexture::BaseTexture(BaseTexture&& other)
    : m_width(std::move(other.m_width)),
      m_height(std::move(other.m_height))
{
}

BaseTexture& BaseTexture::operator=(BaseTexture&& other)
{
    if (this != &other)
    {
        m_width = std::move(other.m_width);
        m_height = std::move(other.m_height);
    }
    return *this;
}

BaseTexture::BaseTexture(const BaseTexture& other)
    : m_width(other.m_width),
      m_height(other.m_height)
{
}

BaseTexture& BaseTexture::operator=(const BaseTexture& other)
{
    if (this != &other)
    {
        m_width = other.m_width;
        m_height = other.m_height;
    }
    return *this;
}

} // namespace spear::rendering
