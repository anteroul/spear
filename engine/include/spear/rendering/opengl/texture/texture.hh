#ifndef SPEAR_RENDERING_OPENGL_TEXTURE_TEXTURE_HH
#define SPEAR_RENDERING_OPENGL_TEXTURE_TEXTURE_HH

#include <spear/rendering/base_texture.hh>

namespace spear::rendering::opengl
{

class Texture : public BaseTexture
{
public:
    /// Constructor.
    Texture();

    /// Move constructor.
    Texture(Texture&& other);

    /// Move assignment operator.
    Texture& operator=(Texture&& other);

    /// Copy constructor.
    Texture(const Texture& other) = delete;

    /// Copy assignment operator.
    Texture& operator=(const Texture& other) = delete;

    ~Texture() override
    {
    }

    /// BaseTexture::bind implementation.
    void bind(uint32_t unit = 0) const override;

    /// BaseTexture::unbind implementation.
    void unbind(uint32_t unit = 0) override;

protected:
    uint32_t m_texture;
};

} // namespace spear::rendering::opengl

#endif
