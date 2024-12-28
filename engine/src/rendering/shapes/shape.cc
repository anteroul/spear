#include <spear/rendering/shapes/shape.hh>

namespace spear::rendering
{

Shape::Shape(std::shared_ptr<rendering::BaseShader> shader, const glm::vec4& color)
    : Mesh(std::move(shader)),
      m_color(color)
{
}

} // namespace spear::rendering
