#ifndef SPEAR_RENDERING_OPENGL_TEXTURE_STB_TEXTURE_HH
#define SPEAR_RENDERING_OPENGL_TEXTURE_STB_TEXTURE_HH

#include <spear/rendering/opengl/texture/texture.hh>

#include <cstdint>
#include <string>

namespace spear::rendering::opengl
{

class STBTexture : public Texture
{
public:
    STBTexture(const std::string& path, bool asset_path = true);
};

} // namespace spear::rendering::opengl

#endif
