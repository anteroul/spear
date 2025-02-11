#ifndef SPEAR_RENDERING_BASE_TEXTURE_HH
#define SPEAR_RENDERING_BASE_TEXTURE_HH

#include <cstdint>
#include <memory>

namespace spear::rendering
{

class BaseTexture
{
public:
    /// Constructor.
    BaseTexture();

    /// Move constructor.
    BaseTexture(BaseTexture&& other);

    /// Move assignment operator.
    BaseTexture& operator=(BaseTexture&& other);

    /// Copy constructor.
    BaseTexture(const BaseTexture& other);

    /// Copy assignment operator.
    BaseTexture& operator=(const BaseTexture& other);

    virtual ~BaseTexture()
    {
    }

    virtual void bind(uint32_t unit = 0) const = 0;
    virtual void unbind(uint32_t unit = 0) = 0;

    int getWidth() const
    {
        return m_width;
    }

    int getHeight() const
    {
        return m_height;
    }

    void setHeight(float height)
    {
        m_height = height;
    }

    void setWidth(float width)
    {
        m_width = width;
    }

protected:
    float m_width;
    float m_height;
};

} // namespace spear::rendering

#endif
