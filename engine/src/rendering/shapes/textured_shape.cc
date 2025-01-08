#include <spear/rendering/shapes/textured_shape.hh>

namespace spear::rendering
{

TexturedShape::TexturedShape(std::shared_ptr<rendering::BaseShader> shader, std::shared_ptr<rendering::BaseTexture> texture, physics::bullet::ObjectData&& object_data, const glm::vec4& color)
    : Shape(shader, std::move(object_data), color),
      m_texture(texture)
{
}

} // namespace spear::rendering
