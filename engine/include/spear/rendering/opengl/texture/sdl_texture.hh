#ifndef SPEAR_RENDERING_OPENGL_TEXTURE_SDL_TEXTURE_HH
#define SPEAR_RENDERING_OPENGL_TEXTURE_SDL_TEXTURE_HH

#include <spear/rendering/opengl/texture/texture.hh>
#include <string>

namespace spear::rendering::opengl
{

class SDLTexture : public Texture, public std::enable_shared_from_this<SDLTexture>
{
public:
    /// Constructor.
    SDLTexture(const std::string& path, bool asset_path = true);

    /// Destructor.
    ~SDLTexture() override;

    /// Move constructor.
    SDLTexture(SDLTexture&& other);

    /// Move assignment operator.
    SDLTexture& operator=(SDLTexture&& other);

    /// Deleted copy constructor.
    SDLTexture(const SDLTexture&) = delete;

    /// Deleted copy assignment.
    SDLTexture& operator=(const SDLTexture&) = delete;

    // Free texture
    void free();
};

} // namespace spear::rendering::opengl

#endif
