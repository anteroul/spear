#include <spear/rendering/shapes/base_cube.hh>

namespace spear::rendering
{

BaseCube::BaseCube(std::shared_ptr<rendering::BaseShader> shader, std::shared_ptr<rendering::BaseTexture> texture, const glm::vec4& color)
    : Shape(shader, color),
      m_texture(texture)
{
}

} // namespace spear::rendering
