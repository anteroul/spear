#ifndef SPEAR_RENDERING_SHADER_TYPE_HH
#define SPEAR_RENDERING_SHADER_TYPE_HH

#include <spear/rendering/api.hh>
#include <spear/rendering/base_shader.hh>

namespace spear::rendering
{

enum class ShaderType
{
    basic,
    lighting,
    sprite3D,
    quad
};

struct ShaderFileData
{
    std::string vertex_shader, fragment_shader;
};

ShaderFileData getShaderFiles(ShaderType type, API api);

} // namespace spear::rendering

#endif
