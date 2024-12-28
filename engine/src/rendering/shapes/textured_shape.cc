#include <spear/rendering/shapes/textured_shape.hh>

namespace spear::rendering
{

TexturedShape::TexturedShape(std::shared_ptr<rendering::BaseShader> shader, std::shared_ptr<rendering::BaseTexture> texture, const glm::vec4& color)
    : Shape(shader, color),
      m_texture(texture)
{
}

} // namespace spear::rendering
